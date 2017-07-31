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
	int   iSizeX;
	int   iSizeY;
	int	  iFrame;
	float fAlpha;
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

	int iX = iFrame % iSizeX;
	int iY = iFrame / iSizeX;

	float fX = vTexUV.x / (float)iSizeX;
	float fY = vTexUV.y / (float)iSizeY;

	output.vTexUV.x = fX + (1.f / (float)iSizeX * iX);
	output.vTexUV.y = fY + (1.f / (float)iSizeY * iY);

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


	output.vDepth = input.vProjPos.z * 0.005f;
	output.vDepth.a = output.vColor.a;

	return output;
}