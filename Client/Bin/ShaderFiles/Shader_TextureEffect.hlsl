
#include "Shader_Define.h"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D		g_AlbedoTexture;
texture2D		g_MaskTexture;
texture2D		g_DepthTexture;

float			g_fAlpha;
float2			g_UVMoveFactor;

sampler AlbedoSampler = sampler_state
{
	filter = min_mag_mip_linear;
	AddressU = Border;
	AddressV = Border;
};

sampler AlphaMaskSampler = sampler_state
{
	filter = min_mag_mip_linear;
	AddressU = Border;
	AddressV = Border;
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
	float4		vProjPos : TEXCOORD1;
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
	Out.vProjPos = Out.vPosition;

	return Out;
}

/* TriangleList : ���������� ��� vs�� �����Ų ��, �� ������ ��ġ�� �� ��ġ������ w������ ������. (����)  */
/* ����Ʈ ��ȯ. */
/* ������ ����� �����Ѵ�.(���������� ������� �Ͽ� �ȼ��� ������ �����Ѵ�. )*/

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
};

struct PS_OUT
{
	/*SV_TARGET0 : ��� ������ ������ �ȼ��̴�. AND 0��° ����Ÿ�ٿ� �׸������� �����̴�. */
	float4		vColor : SV_TARGET0;
};

PS_OUT Texture_EFFECT(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	In.vTexUV += g_UVMoveFactor;
	float4 albedo = g_AlbedoTexture.Sample(AlbedoSampler, In.vTexUV);

	float2		vTexUV;
	vTexUV.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
	vTexUV.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

	vector		vDepthDesc = g_DepthTexture.Sample(LinearSampler, vTexUV);

	float		fOldViewZ = vDepthDesc.y * 300.f; // ī�޶��� far
	float		fViewZ = In.vProjPos.w;

	Out.vColor = albedo * g_fAlpha;
	Out.vColor.a = Out.vColor.a * (saturate(fOldViewZ - fViewZ) * 2.5f);
	
	return Out;
}

PS_OUT Texture_EFFECTwithAlphaMask(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	In.vTexUV += g_UVMoveFactor;

	float4 albedo = g_AlbedoTexture.Sample(AlbedoSampler, In.vTexUV);
	float4 alphaMask = g_MaskTexture.Sample(AlphaMaskSampler, In.vTexUV);

	Out.vColor = albedo * alphaMask.r * g_fAlpha;

	return Out;
}

PS_OUT Texture_EFFECTSimple(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	In.vTexUV += g_UVMoveFactor;

	float4 albedo = g_AlbedoTexture.Sample(AlbedoSampler, In.vTexUV);

	Out.vColor = albedo * g_fAlpha;

	return Out;
}

technique11 DefaultTechnique
{
	pass Tex_Effect
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Test, 0);
		SetBlendState(BS_Trail,float4(0.f,0.f,0.f,1.f), 0xffffffff);
		
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 Texture_EFFECT();
	}

	pass Tex_AlphaMaks_Effect
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Test, 0);
		SetBlendState(BS_Trail, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 Texture_EFFECTwithAlphaMask();
	}

	pass Tex_EffectSimple
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Test, 0);
		SetBlendState(BS_Trail, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 Texture_EFFECTSimple();
	}
}
