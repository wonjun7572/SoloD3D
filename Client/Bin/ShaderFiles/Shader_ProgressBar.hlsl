
#include "Shader_Define.h"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D		g_AlbedoTexture;
texture2D		g_FillTexture;
texture2D		g_BackTexture;

float			g_fAmount;

sampler AlbedoSampler = sampler_state
{
	filter = min_mag_mip_linear;
	AddressU = wrap;
	AddressV = wrap;
};

sampler FillSampler = sampler_state
{
	filter = min_mag_mip_linear;
	AddressU = wrap;
	AddressV = wrap;
};

sampler BackSampler = sampler_state
{
	filter = min_mag_mip_linear;
	AddressU = wrap;
	AddressV = wrap;
};

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

PS_OUT ProgressPS(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 albedo = g_AlbedoTexture.Sample(AlbedoSampler, In.vTexUV);

	if (In.vTexUV.x > g_fAmount)
		discard;

	Out.vColor = albedo;
	
	return Out;
}

PS_OUT BillboardBarPS(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 fill = g_FillTexture.Sample(FillSampler, In.vTexUV);
	float4 back = g_BackTexture.Sample(BackSampler, In.vTexUV);

	if (In.vTexUV.x > g_fAmount)
		Out.vColor = back;
	else
		Out.vColor = fill;

	return Out;
}

technique11 DefaultTechnique
{
	pass Progress
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Test, 0);
		SetBlendState(BS_AlphaBlending,float4(0.f,0.f,0.f,1.f), 0xffffffff);
		
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 ProgressPS();
	}

	pass BillboardBar
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Test, 0);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 BillboardBarPS();
	}
}
