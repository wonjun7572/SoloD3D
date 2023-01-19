
#include "Shader_Define.h"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

matrix			g_ViewInverseMatrix, g_ProjInverseMatrix;

matrix			g_SocketMatrix;

bool			g_HasSpecular;
bool			g_HasNormal;

/* 재질 정보 */
texture2D		g_SpecularTexture;
texture2D		g_DiffuseTexture;
texture2D		g_NormalTexture;

vector			g_vCamPosition;
vector			g_vRimColor;

struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float3		vTangent : TANGENT;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float4		vTangent : TANGENT;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
	float3		vBinormal : BINORMAL;
};

struct WEAPON_VS_OUT
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

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix));
	Out.vTexUV = In.vTexUV;
	Out.vProjPos = Out.vPosition;
	Out.vTangent = normalize(mul(float4(In.vTangent, 0.f), g_WorldMatrix));
	Out.vBinormal = normalize(cross(Out.vNormal.xyz, Out.vTangent.xyz));

	return Out;
}

VS_OUT VS_MAIN_SOCKET(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matVP = mul(g_ViewMatrix, g_ProjMatrix);

	vector		vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	vPosition = mul(vPosition, g_SocketMatrix);

	vector		vNormal = mul(float4(In.vNormal, 0.f), g_WorldMatrix);
	vNormal = mul(vNormal, g_SocketMatrix);

	Out.vPosition = mul(vPosition, matVP);
	Out.vNormal = normalize(vNormal);
	Out.vTexUV = In.vTexUV;
	Out.vProjPos = Out.vPosition;
	Out.vTangent = normalize(mul(float4(In.vTangent, 0.f), g_WorldMatrix));
	Out.vBinormal = normalize(cross(Out.vNormal.xyz, Out.vTangent.xyz));

	return Out;
}

WEAPON_VS_OUT VS_MAIN_WEAPON(VS_IN In)
{
	WEAPON_VS_OUT		Out = (WEAPON_VS_OUT)0;

	matrix		matVP = mul(g_ViewMatrix, g_ProjMatrix);

	vector		vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	vPosition = mul(vPosition, g_SocketMatrix);

	float4	worldPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	Out.vViewDir = normalize(worldPosition.xyz - g_vCamPosition.xyz);

	vector		vNormal = mul(float4(In.vNormal, 0.f), g_WorldMatrix);
	vNormal = mul(vNormal, g_SocketMatrix);

	Out.vPosition = mul(vPosition, matVP);
	Out.vNormal = normalize(vNormal);
	Out.vTexUV = In.vTexUV;
	Out.vProjPos = Out.vPosition;
	Out.vTangent = normalize(mul(float4(In.vTangent, 0.f), g_WorldMatrix));
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
	float3		vBinormal : BINORMAL;
};

struct PS_WEAPON_IN
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
	float4		vDiffuse  : SV_TARGET0;
	float4		vNormal   : SV_TARGET1;
	float4		vDepth	  : SV_TARGET2;
	float4		vSpecular : SV_TARGET3;
};

struct PS_OUT_WEAPON
{
	/*SV_TARGET0 : 모든 정보가 결정된 픽셀이다. AND 0번째 렌더타겟에 그리기위한 색상이다. */
	float4		vDiffuse  : SV_TARGET0;
	float4		vNormal   : SV_TARGET1;
	float4		vDepth	  : SV_TARGET2;
	float4		vSpecular : SV_TARGET3;
	float4		vRimColor : SV_TARGET4;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	if (0.1f > vDiffuse.a)
		discard;

	// 스펙큘러 연산
	vector		vSpecular = (vector)0.2f;

	if (g_HasSpecular == true)
		vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexUV);
	else
		vSpecular = (vector)0.2f;

	/* 탄젠트스페이스 */
	float3	vNormal = In.vNormal.xyz;
	if (g_HasNormal == true)
	{
		vector		vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
		vNormal = vNormalDesc.xyz * 2.f - 1.f;
		float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
		vNormal = normalize(mul(vNormal, WorldMatrix));
	}

	Out.vDiffuse = vDiffuse;
	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 300.f, 0.f, 0.f);
	Out.vSpecular = vSpecular;

	return Out;
}

PS_OUT_WEAPON PS_MAIN_WEAPON(PS_WEAPON_IN In)
{
	PS_OUT_WEAPON			Out = (PS_OUT_WEAPON)0;

	vector		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	if (0.1f > vDiffuse.a)
		discard;

	// 스펙큘러
	vector		vSpecular = (vector)0.2f;
	if (g_HasSpecular == true)
		vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexUV);
	else
		vSpecular = (vector)1.f;

	
	/* 탄젠트스페이스 */
	float3	vNormal = In.vNormal.xyz;
	if (g_HasNormal == true)
	{
		vector		vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
		vNormal = vNormalDesc.xyz * 2.f - 1.f;
		float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
		vNormal = normalize(mul(vNormal, WorldMatrix));
	}

	float rim = pow(1 - saturate(dot(In.vNormal.xyz, -In.vViewDir)), 5.0f);
	vector rimColor = g_vRimColor;
	rimColor = rim * rimColor;

	Out.vDiffuse = vDiffuse;
	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 300.f, 0.f, 0.f);
	Out.vSpecular = vSpecular;
	Out.vRimColor = rimColor;

	return Out;
}

technique11 DefaultTechnique
{
	pass Default//0
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

	pass Socket//1
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN_SOCKET();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass Weapon //2
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN_WEAPON();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_WEAPON();
	}
}
