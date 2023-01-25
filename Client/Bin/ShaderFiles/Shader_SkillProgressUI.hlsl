
#include "Shader_Define.h"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D		g_Texture;

float			g_fAmount;
float			g_fAlpha;

sampler AlbedoSampler = sampler_state
{
	filter = min_mag_mip_linear;
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

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 fillColor = g_Texture.Sample(AlbedoSampler, In.vTexUV);

	float3 center = float3(0.5f, 0.5f, 0);
	float3 top = float3(0.5f, 0, 0);
	float3 curUV = float3(In.vTexUV.xy, 0);
	float angle = 0;

	float3 centerToTop = top - center;
	float3 centerToCurUV = curUV - center;

	centerToTop = normalize(centerToTop);
	centerToCurUV = normalize(centerToCurUV);

	angle = acos(dot(centerToTop, centerToCurUV));
	angle = angle * (180.0f / 3.141592654f); // radian to degree

	angle = (centerToTop.x * centerToCurUV.x - centerToTop.y * centerToCurUV.x > 0.0f) ? angle : (-angle) + 360.0f;

	float condition = 360 * g_fAmount;

	if (angle >= condition)
		discard;

	Out.vColor = fillColor * g_fAlpha;
	
	return Out;
}

PS_OUT Texture_EFFECTSimple(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 albedo = g_Texture.Sample(AlbedoSampler, In.vTexUV);

	Out.vColor = albedo * g_fAlpha;

	return Out;
}

technique11 DefaultTechnique
{
	pass ProgressUI
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Test, 0);
		SetBlendState(BS_Trail,float4(0.f,0.f,0.f,1.f), 0xffffffff);
		
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass ProgressUI_Back
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Test, 0);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 Texture_EFFECTSimple();
	}
}
