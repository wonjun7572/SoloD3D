
#include "Shader_Define.h"

/* 9 : �׸�, ����, ���� �׸��� ���� ���̴���. */
/* 11 : Ư�� ������ ���� ������� �̷���� ���� �׸������� ���̴���. */

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D		g_Texture;

struct VS_IN
{
	float3		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	/* ������ �ʿ��� ��ȯ�� ��� ���ƴ�. (����, ��, ����) */
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
};

/* ���� �׸��µ� ����ϴ� ������ ������ŭ. (�ε����� ������ŭ) */

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	/* �⺻���� ��ȯ�� ��ģ��. (����, ��, ����) */
	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	return Out;
}

/* TriangleList : ���������� ��� vs�� �����Ų ��, �� ������ ��ġ�� �� ��ġ������ w������ ������. (����)  */
/* ����Ʈ ��ȯ. */
/* ������ ����� �����Ѵ�.(���������� ������� �Ͽ� �ȼ��� ������ �����Ѵ�. )*/


struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	/*SV_TARGET0 : ��� ������ ������ �ȼ��̴�. AND 0��° ����Ÿ�ٿ� �׸������� �����̴�. */
	float4		vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexUV);
	
	return Out;
}

technique11 DefaultTechnique
{
	pass Rect
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlending,float4(0.f,0.f,0.f,1.f), 0xffffffff);
		
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}
