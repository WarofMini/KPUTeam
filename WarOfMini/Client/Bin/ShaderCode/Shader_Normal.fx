Texture2D txDiffuse : register(t0);
SamplerState BaseSampler : register(s0);

cbuffer ConstantBuffer : register(b0)
{
	matrix matWorld;
	matrix matView;
	matrix matProj;
};


cbuffer ConstantLightBuffer : register(b1)
{
	float4   Light_Ambient;
	float4   Light_Diffuse;
	float4   Light_Specular;
	float4   Light_Direction;
	float	 Light_m_fPad;
};

cbuffer ConstantMaterialBuffer : register(b2)
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;
	float4 Reflect;
	float4 Eye;
};

struct VS_OUTPUT
{
	float4 vPos		: SV_POSITION;
	float2 vTexUV	: TEXCOORD0;
	float fShade	: COLOR0;
	float4 vPosW	: TEXCOORD1;
	float3 fNormal  : NORMAL;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(float4 vPos : POSITION, float2 vTexUV : TEXCOORD0, float3 vNormal : NORMAL)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	output.vPos = mul(vPos, matWorld);
	output.vPosW = output.vPos;
	output.vPos = mul(output.vPos, matView);
	output.vPos = mul(output.vPos, matProj);

	vector vWorld_Normal = normalize(mul(vector(vNormal.xyz, 0.f), matWorld));
	output.fNormal = vWorld_Normal;
	output.vTexUV = vTexUV;

	return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_Target
{
	vector vColor = txDiffuse.Sample(BaseSampler, input.vTexUV);

	input.fNormal = normalize(input.fNormal);

	float3 toEye = Eye - input.vPosW;
	float distToEye = length(toEye);
	// Normalize.
	toEye /= distToEye;

	//½ºÆåÅ§·¯ ÇØÁ¦
	toEye = float3(0.0f, 0.0f, 0.0f);

	float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

	float3 lightVec = -Light_Direction;

	// The light vector aims opposite the direction the light rays travel.
	ambient = Ambient * Light_Ambient;


	// Add ambient term.
	float diffuseFactor = dot(lightVec, input.fNormal);


	// Flatten to avoid dynamic branching.
	if (diffuseFactor > 0.0f)
	{
		float3 v = reflect(-lightVec, input.fNormal);
		float specFactor = pow(max(dot(v, toEye), 0.0f), Specular.w);

		diffuse = diffuseFactor * Diffuse * Light_Diffuse;
		spec = specFactor * Specular * Light_Specular;
	}

	float4 litColor = ambient + diffuse;

	vColor.rgb *= litColor.rgb;

	vColor.rgba += spec;


	clip(vColor.a);

	return vColor;
}






/*
float4 PS(VS_OUTPUT input) : SV_Target
{
vector vColor = txDiffuse.Sample(BaseSampler, input.vTexUV);

vColor.rgb *= input.fShade;
clip(vColor.a);

return vColor;
}

*/