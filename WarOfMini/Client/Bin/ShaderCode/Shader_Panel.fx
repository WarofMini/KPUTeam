Texture2D txDiffuse : register(t0);
SamplerState BaseSampler : register(s0);

cbuffer ConstantBuffer : register(b0)
{
	matrix matWorld;
	matrix matView;
	matrix matProj;
}

cbuffer ConstantBuffer : register(b1)
{
	float	 fAlpha;
	float4   vPadding;
}

struct VS_OUTPUT
{
	float4 vPos		: SV_POSITION;
	float2 vTexUV	: TEXCOORD0;
	float4 vProjPos : TEXCOORD1;
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

	output.vProjPos = output.vPos;

	output.vTexUV = vTexUV;

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

	output.vColor = txDiffuse.Sample(BaseSampler, input.vTexUV);

	float fCenter = 0.5f;

	if((input.vTexUV.x >= (fCenter + fAlpha)) || (input.vTexUV.x <= (fCenter - fAlpha)))
	  output.vColor.a *= 0.0f;

	return output;
}