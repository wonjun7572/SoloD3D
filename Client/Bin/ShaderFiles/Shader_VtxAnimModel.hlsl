#include "Shader_Define.h"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

/* 1. ���� ��ü�� ���� �޾ƿ´�. */
/* 2. �� ��, ���� �׸����� �ϴ� �޽ÿ� ���� �޾ƿ´�. */
matrix			g_BoneMatrices[256];

texture2D		g_DiffuseTexture;
texture2D		g_NormalTexture;
texture2D		g_SpecularTexture;

bool			g_HasSpecular;

vector			g_vCamPosition;
vector			g_vRimColor;

struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float3		vTangent : TANGENT;

	/* ���� �������� ���������� ����(�ִ� 4��)�� ����� �����. */
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
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
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

	Out.vPosition = mul(vPosition, matWVP);
	Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));
	Out.vTexUV = In.vTexUV;
	Out.vTangent = (vector)0.f;
	Out.vProjPos = Out.vPosition;

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
};

struct PS_OUT
{
	/*SV_TARGET0 : ��� ������ ������ �ȼ��̴�. AND 0��° ����Ÿ�ٿ� �׸������� �����̴�. */
	float4		vDiffuse  : SV_TARGET0;
	float4		vNormal	  : SV_TARGET1;
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

	vector		vSpecular = (vector)0.2f;
	if (g_HasSpecular == true)
	{
		vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexUV);
	}
	else
	{
		vSpecular = (vector)0.2f;
	}

	float rim = pow(1 - saturate(dot(In.vNormal.xyz, -In.vViewDir)), 5.0f);
	vector rimColor = g_vRimColor;
	rimColor = rim * rimColor;

	Out.vDiffuse = vDiffuse;
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 300.f, 0.f, 0.f);
	Out.vSpecular = vSpecular;
	Out.vRimColor = rimColor;

	return Out;
}

PS_OUT PS_MAIN_OUTLINE(PS_IN In)
{
	PS_OUT         Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	Out.vDiffuse.a = Out.vDiffuse.a * 0.5f;
	
	vector		vSpecular = (vector)0.2f;

	if (g_HasSpecular == true)
	{
		vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexUV);
	}
	else
	{
		vSpecular = (vector)0.2f;
	}

	float		 Lx = 0;
	float		 Ly = 0;

	for (int y = -1; y <= 1; ++y)
	{
		for (int x = -1; x <= 1; ++x)
		{
			float2 offset = float2(x, y) * float2(1 / 1600.f, 1 / 900.f);
			float3 tex = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV + offset).rgb;
			float luminance = dot(tex, float3(0.3, 0.59, 0.11));

			Lx += luminance * Kx[y + 1][x + 1];
			Ly += luminance * Ky[y + 1][x + 1];
		}
	}
	float L = sqrt((Lx*Lx) + (Ly*Ly));

	if (L < 0.3) // �� ���� ���� �β��� �ܰ����� ����°� 
	{
		Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	}
	else
	{
		Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV) * 0.3f;
	}
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 300.f, 0.f, 0.f);
	Out.vSpecular = vSpecular;
	vector rimColor = vector(0.f, 0.f, 0.f, 1.f);
	Out.vRimColor = rimColor;

	return Out;
}

PS_OUT PS_MAIN_BLOOM(PS_IN In)
{
	PS_OUT         Out = (PS_OUT)0;

	vector		vSpecular = (vector)0.f;

	if (g_HasSpecular == true)
	{
		vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexUV);
	}
	else
	{
		vSpecular = (vector)0.f;
	}

	Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	float2 arr[8] = 
	{
		float2(-1, -1), float2(0, -1), float2(1, -1),
		float2(-1, 0), float2(1, 0), float2(-1, 1), 
		float2(0, 1), float2(1, 1)
	};

	int colorWeight = 1;
	float x;
	float y;
	float2 uv;

	for (int blur = 1; blur < 8; blur++)
	{
		for (int i = 0; i < 8; i++)
		{
			x = arr[i].x * blur / 1600.f;
			y = arr[i].y * blur / 900.f;

			uv = In.vTexUV + float2(x, y);

			Out.vDiffuse = Out.vDiffuse + g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

			colorWeight++;
		}
	}

	Out.vDiffuse /= colorWeight;
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 300.f, 0.f, 0.f);
	Out.vSpecular = vSpecular;
	vector rimColor = vector(0.f, 0.f, 0.f, 1.f);
	Out.vRimColor = rimColor;

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
