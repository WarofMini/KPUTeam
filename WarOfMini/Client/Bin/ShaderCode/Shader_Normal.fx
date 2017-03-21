Texture2D txDiffuse : register(t0);
SamplerState BaseSampler : register(s0);

cbuffer ConstantBuffer : register(b0)
{
matrix matWorld;
matrix matView;
matrix matProj;
}

struct VS_OUTPUT
{
	float4 vPos		: SV_POSITION;
	float2 vTexUV	: TEXCOORD0;
	float fShade	: COLOR0;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(float4 vPos : POSITION, float2 vTexUV : TEXCOORD0, float3 vNormal : NORMAL)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	output.vPos = mul(vPos, matWorld);
	output.vPos = mul(output.vPos, matView);
	output.vPos = mul(output.vPos, matProj);

	output.vTexUV = vTexUV;

	vector vLightDir = vector(0.f, -0.5f, -0.5f, 0.f);

	vector vWorld_Normal = normalize(mul(vector(vNormal.xyz, 0.f), matWorld));
	vector vWorld_LightDirInv = normalize(vLightDir) * -1.f;

	output.fShade = max(dot(vWorld_Normal, vWorld_LightDirInv), 0.f) + 0.3f;
	output.fShade = min(output.fShade, 1.f);

	return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_Target
{
	vector vColor = txDiffuse.Sample(BaseSampler, input.vTexUV);

	vColor.rgb *= input.fShade;
	clip(vColor.a);

	return vColor;
}
