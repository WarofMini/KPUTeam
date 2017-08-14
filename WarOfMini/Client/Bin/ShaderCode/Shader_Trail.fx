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


	for (int i = 0; i < 30; ++i)
	{
		float fAlpha = ((float)(i))/30.f;

		if ((input.vTexUV.y >= fAlpha) && (input.vTexUV.y <= fAlpha + 0.1f))
		{
			output.vColor.a *= (1.f - fAlpha);
		}
	}

	return output;
}