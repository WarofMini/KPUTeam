
#define MAX_BONE_MATRICES 64

Texture2D txDiffuse : register(t0);
SamplerState BaseSampler : register(s0);

cbuffer ConstantBuffer : register(b0)
{
	matrix matWorld;
	matrix matView;
	matrix matProj;
};

cbuffer ConstantBoneWorld :register(b1)
{
	matrix matBoneWorld[MAX_BONE_MATRICES];
};

cbuffer ConstantLightBuffer : register(b2)
{
	float4   vPad;
	float	 fLightPower;

};



struct VS_OUTPUT
{
	float4 vPos		: SV_POSITION;
	float2 vTexUV	: TEXCOORD0;
	float4 vProjPos : TEXCOORD1;
	float4 vPosW	: TEXCOORD2;
	float  fShade	: COLOR0;
	float3 fNormal  : NORMAL;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(float4 vPos : POSITION, float2 vTexUV : TEXCOORD0, float3 vNormal : NORMAL, uint4 uiBones : BONES, float4 vWeights : WEIGHTS)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	matrix matTrans = (matrix)0;

	
	matTrans += matBoneWorld[uiBones.x] * vWeights.x;
	matTrans += matBoneWorld[uiBones.y] * vWeights.y;
	matTrans += matBoneWorld[uiBones.z] * vWeights.z;
	matTrans += matBoneWorld[uiBones.w] * vWeights.w;
	

	matTrans = mul(matTrans, matWorld);
	output.vPos = mul(vPos, matTrans);
	output.vPosW = output.vPos;

	output.vPos = mul(output.vPos, matView);
	output.vPos = mul(output.vPos, matProj);

	output.vTexUV = vTexUV;


	vector vLightDir = vector(0.f, -0.5f, -0.5f, 0.f);

	vector vWorld_Normal = normalize(mul(vector(vNormal.xyz, 0.f), matTrans));
	vector vWorld_LightDirInv = normalize(vLightDir) * -1.f;

	output.fShade = max(dot(vWorld_Normal, vWorld_LightDirInv), 0.f) + (fLightPower * 0.3f);
	output.fShade = min(output.fShade, 1.f);

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

	output.vColor = txDiffuse.Sample(BaseSampler, input.vTexUV);

	output.vColor.rgb *= input.fShade;

	output.vDepth = input.vProjPos.z * 0.005f;
	
	//input.fNormal = normalize(input.fNormal);
	//float3 toEye = Eye - input.vPosW;
	//float distToEye = length(toEye);
	//// Normalize.
	//toEye /= distToEye;

	////½ºÆåÅ§·¯ ÇØÁ¦
	//toEye = float3(0.0f, 0.0f, 0.0f);
	//
	//float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	//float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	//float4 spec = float4(0.0f, 0.0f, 0.0f, 0.0f);
	//

	//float3 lightVec = -Light_Direction;
	//
	//// The light vector aims opposite the direction the light rays travel.
	//ambient = Ambient * Light_Ambient;

	//
	//// Add ambient term.
	//float diffuseFactor = dot(lightVec, input.fNormal);

	//
	//// Flatten to avoid dynamic branching.	
	//if (diffuseFactor > 0.0f)
	//{
	//	float3 v = reflect(-lightVec, input.fNormal);
	//	float specFactor = pow(max(dot(v, toEye), 0.0f), Specular.w);

	//	diffuse = diffuseFactor * Diffuse * Light_Diffuse;
	//	spec = specFactor * Specular * Light_Specular;
	//}
	//
	//float4 litColor = ambient + diffuse;


	//output.vColor.rgb *= litColor.rgb;
	//
	//output.vColor.rgba += spec;

	return output;
}
















/*
VS_OUTPUT VS(float4 vPos : POSITION, float2 vTexUV : TEXCOORD0, float3 vNormal : NORMAL, uint4 uiBones : BONES, float4 vWeights : WEIGHTS)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	matrix matTrans = (matrix)0;


	matTrans += matBoneWorld[uiBones.x] * vWeights.x;
	matTrans += matBoneWorld[uiBones.y] * vWeights.y;
	matTrans += matBoneWorld[uiBones.z] * vWeights.z;
	matTrans += matBoneWorld[uiBones.w] * vWeights.w;


	matTrans = mul(matTrans, matWorld);
	output.vPos = mul(vPos, matTrans);
	output.vPos = mul(output.vPos, matView);
	output.vPos = mul(output.vPos, matProj);

	output.vTexUV = vTexUV;



	vector vLightDir = vector(0.f, -0.5f, -0.5f, 0.f);

	vector vWorld_Normal = normalize(mul(vector(vNormal.xyz, 0.f), matTrans));
	vector vWorld_LightDirInv = normalize(vLightDir) * -1.f;

	output.fShade = max(dot(vWorld_Normal, vWorld_LightDirInv), 0.f) + 0.3f;
	output.fShade = min(output.fShade, 1.f);

	output.vProjPos = output.vPos;

	return output;
}

struct PS_OUTPUT
{
	float4 vColor	: SV_Target0;
	float4 vDepth	: SV_Target1;
};

PS_OUTPUT PS(VS_OUTPUT input)
{
	PS_OUTPUT output = (PS_OUTPUT)0;

	output.vColor = txDiffuse.Sample(BaseSampler, input.vTexUV);
	output.vColor.rgb *= input.fShade;

	output.vDepth = input.vProjPos.z * 0.005f;

	return output;
}
*/