#include "Shader_Define.h"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

/* 1. ���� ��ü�� ���� �޾ƿ´�. */
/* 2. �� ��, ���� �׸����� �ϴ� �޽ÿ� ���� �޾ƿ´�. */
matrix			g_BoneMatrices[256];

texture2D		g_DiffuseTexture;
texture2D		g_NormalTexture;

matrix			g_SocketMatrix;

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
	float2		vTexUV : TEXCOORD0;
	float4		vTangent : TANGENT;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

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

	return Out;
}

VS_OUT VS_MAIN_SOCKET(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matVP = mul(g_ViewMatrix, g_ProjMatrix);

	float			fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);

	matrix			BoneMatrix = g_BoneMatrices[In.vBlendIndex.x] * In.vBlendWeight.x +
		g_BoneMatrices[In.vBlendIndex.y] * In.vBlendWeight.y +
		g_BoneMatrices[In.vBlendIndex.z] * In.vBlendWeight.z +
		g_BoneMatrices[In.vBlendIndex.w] * fWeightW;

	vector		vPosition = mul(float4(In.vPosition, 1.f), BoneMatrix);
	vector		vNormal = mul(float4(In.vNormal, 0.f), BoneMatrix);

	vPosition = mul(vPosition, g_WorldMatrix);
	vPosition = mul(vPosition, g_SocketMatrix);

	vNormal = mul(vNormal, g_WorldMatrix);
	vNormal = mul(vNormal, g_SocketMatrix);

	Out.vPosition = mul(vPosition, matVP);
	Out.vNormal = normalize(vNormal);
	Out.vTexUV = In.vTexUV;
	Out.vTangent = (vector)0.f;

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vTangent : TANGENT;
};

struct PS_OUT
{
	/*SV_TARGET0 : ��� ������ ������ �ȼ��̴�. AND 0��° ����Ÿ�ٿ� �׸������� �����̴�. */
	float4		vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	
	if (0.1f > Out.vColor.a)
		discard;

	return Out;
}

PS_OUT PS_MAIN_OUTLINE(PS_IN In)
{
	PS_OUT         Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	Out.vColor.a = Out.vColor.a * 0.5f;

	float Lx = 0;
	float Ly = 0;

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
		Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	}
	else
	{
		Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV) * 0.3f;
	}

	return Out;
}

PS_OUT PS_MAIN_BLOOM(PS_IN In)
{
	PS_OUT         Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

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

			Out.vColor = Out.vColor + g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

			colorWeight++;
		}
	}

	Out.vColor /= colorWeight;

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

	pass Socket
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

	pass Outline
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_OUTLINE();
	}

	pass Bloom
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BLOOM();
	}
}
