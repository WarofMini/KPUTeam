Texture2D txColor : register(t0);
Texture2D txDepth : register(t1);
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

	return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_OUTPUT input) :SV_Target
{
	float4 vColor[2];
	
	vColor[0] = txColor.Sample(BaseSampler, input.vTexUV);
	vColor[1] = txDepth.Sample(BaseSampler, input.vTexUV);

	vColor[1].rgb = min(vColor[1].rgb, 0.0f);

	float4 vFogColor = vector(1.f, 1.f, 1.f, 1.f);

	return vColor[0] * (1.f - vColor[1]) + vFogColor * (vColor[1]);
}