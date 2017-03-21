Texture2D txBlendInfo : register(t0);
Texture2D txDiffuse : register(t1);
Texture2D txBlend1: register(t2);
Texture2D txBlend2: register(t3);
Texture2D txBlend3 : register(t4);

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
	float4 vProjPos : TEXCOORD1;
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

	output.vTexUV = vTexUV * 25.f;

	vector vLightDir = vector(0.f, -0.5f, -0.5f, 0.f);

	vector vWorld_Normal = normalize(mul(vector(vNormal.xyz, 0.f), matWorld));
	vector vWorld_LightDirInv = normalize(vLightDir) * -1.f;

	output.fShade = max(dot(vWorld_Normal, vWorld_LightDirInv), 0.f) + 0.5f;
	//output.fShade = min(output.fShade, 1.2f);

	output.vProjPos = output.vPos;

	return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
struct PS_OUTPUT
{
	float4 vColor	: SV_Target0;
	float4 vDepth	: SV_Target1;
};

PS_OUTPUT PS(VS_OUTPUT input)
{
	PS_OUTPUT output = (PS_OUTPUT)0;

	float4 vBlendInfo = txBlendInfo.Sample(BaseSampler, input.vTexUV / 25.f);

	float4 vTexColor[4];
	vTexColor[0] = txDiffuse.Sample(BaseSampler, input.vTexUV);
	vTexColor[1] = txBlend1.Sample(BaseSampler, input.vTexUV);
	vTexColor[2] = txBlend2.Sample(BaseSampler, input.vTexUV);
	vTexColor[3] = txBlend3.Sample(BaseSampler, input.vTexUV);

	output.vColor = ((vTexColor[0] * (1.f - vBlendInfo.r - vBlendInfo.g - vBlendInfo.b))
						+ (vTexColor[1] * vBlendInfo.r) + (vTexColor[2] * vBlendInfo.g) + (vTexColor[3] * vBlendInfo.b)) * input.fShade;

	output.vDepth = input.vProjPos.z * 0.005f;

	return output;
}
