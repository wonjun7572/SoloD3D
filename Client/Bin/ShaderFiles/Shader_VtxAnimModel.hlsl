#include "Shader_Define.h"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

/* 1. 모델의 전체의 뼈를 받아온다. */
/* 2. 모델 중, 현재 그릴려고 하는 메시에 뼈를 받아온다. */
matrix			g_BoneMatrices[256];

texture2D		g_DiffuseTexture;
texture2D		g_NormalTexture;
texture2D		g_SpecularTexture;
texture2D		g_DissolveTexture;

bool			g_HasSpecular;
bool			g_HasNormal;
bool			g_bDissolve = false;

vector			g_vCamPosition;
vector			g_vRimColor;

float			fDissolveAmount;
float			fFringeAmount;

struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float3		vTangent : TANGENT;

	/* 현재 정점에게 곱해져야할 뼈들(최대 4개)의 행렬을 만든다. */
	uint4		vBlendIndex : BLENDINDEX;
	float4		vBlendWeight : BLENDWEIGHT;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float4		vTangent : TANGENT;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
	float3		vViewDir : TEXCOORD2;
	float3		vBinormal : BINORMAL;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;
	matWV  = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	float4 worldPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	Out.vViewDir = normalize(worldPosition.xyz - g_vCamPosition.xyz);

	float			fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);

	matrix			BoneMatrix = g_BoneMatrices[In.vBlendIndex.x] * In.vBlendWeight.x +
		g_BoneMatrices[In.vBlendIndex.y] * In.vBlendWeight.y +
		g_BoneMatrices[In.vBlendIndex.z] * In.vBlendWeight.z +
		g_BoneMatrices[In.vBlendIndex.w] * fWeightW;

	vector			vPosition = mul(float4(In.vPosition, 1.f), BoneMatrix);
	vector			vNormal = mul(float4(In.vNormal, 0.f), BoneMatrix);
	vector			vTangent = mul(float4(In.vTangent, 0.f), BoneMatrix);

	Out.vPosition = mul(vPosition, matWVP);
	Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));
	Out.vTexUV = In.vTexUV;
	Out.vTangent = normalize(mul(vTangent, g_WorldMatrix));
	Out.vProjPos = Out.vPosition;
	Out.vBinormal = normalize(cross(Out.vNormal.xyz, Out.vTangent.xyz));

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float4		vTangent : TANGENT;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
	float3		vViewDir : TEXCOORD2;
	float3		vBinormal : BINORMAL;
};

struct PS_OUT
{
	/*SV_TARGET0 : 모든 정보가 결정된 픽셀이다. AND 0번째 렌더타겟에 그리기위한 색상이다. */
	float4		vDiffuse	 : SV_TARGET0;
	float4		vNormal		 : SV_TARGET1;
	float4		vDepth		 : SV_TARGET2;
	float4		vSpecular	 : SV_TARGET3;
	float4		vRimColor	 : SV_TARGET4;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	
	if (0.1f > vDiffuse.a)
		discard;

	vector		vSpecular = (vector)0.2f;
	if (g_HasSpecular == true)
		vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexUV);
	else
		vSpecular = (vector)0.2f;

	float rim = pow(1 - saturate(dot(In.vNormal.xyz, -In.vViewDir)), 25.0f);
	vector rimColor = g_vRimColor;
	rimColor = rim * rimColor;

	/* 탄젠트스페이스 */
	float3	vNormal = In.vNormal.xyz;
	if (g_HasNormal == true)
	{
		vector		vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
		vNormal = vNormalDesc.xyz * 2.f - 1.f;
		float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
		vNormal = normalize(mul(vNormal, WorldMatrix));
	}

	if (g_vRimColor.x >= 0.95f)
		vDiffuse.r += g_vRimColor.x;
		
	Out.vDiffuse = vDiffuse;
	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 300.f, 0.f, 0.f);
	Out.vSpecular = vSpecular;
	Out.vRimColor = rimColor;

	if (g_bDissolve)
	{
		float4 DissolveMap = g_DissolveTexture.Sample(LinearSampler, In.vTexUV);
		float  DissolveValue = DissolveMap.x;

		if (DissolveValue <= fDissolveAmount)
			discard;
		
		else if (DissolveValue <= fDissolveAmount + fFringeAmount && fDissolveAmount != 0)
		{
			if (Out.vDiffuse.a != 0.0f)
				Out.vDiffuse = Out.vDiffuse + float4(10.f, 0.f, 0.f, DissolveMap.x);
		}
	}

	return Out;
}

technique11 DefaultTechnique
{
	pass Default
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}
