#include "Shader_Define.h"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D		g_DiffuseTexture;
texture2D		g_MaskTexture;

float			g_fAlpha = 1.f;
float2			g_UVMoveFactor;

sampler AlbedoSampler = sampler_state
{
	filter = min_mag_mip_linear;
	AddressU = Border;
	AddressV = Border;
};

sampler AlbedoWrapSampler = sampler_state
{
	filter = min_mag_mip_linear;
	AddressU = wrap;
	AddressV = wrap;
};

sampler NormalSampler_ME = sampler_state
{
	AddressU = Border;
	AddressV = Border;
};

sampler AlphaMaskSampler = sampler_state
{
	filter = min_mag_mip_linear;
	AddressU = Border;
	AddressV = Border;
};

sampler AlphaMaskWrapSampler = sampler_state
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
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;
	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	float4		vColor : SV_TARGET0;
};

PS_OUT MeshEffectPS(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float2 texcoord2 = In.vTexUV;
	texcoord2 += g_UVMoveFactor;

	float4 albedo = g_DiffuseTexture.Sample(AlbedoSampler, texcoord2);
	float4 maskTex = g_MaskTexture.Sample(AlphaMaskSampler, texcoord2);

	Out.vColor = albedo * g_fAlpha;

	return Out;
}

PS_OUT MeshEffectWrapPS(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float2 texcoord2 = In.vTexUV;
	texcoord2 += g_UVMoveFactor;

	float4 albedo = g_DiffuseTexture.Sample(AlbedoWrapSampler, texcoord2);
	float4 maskTex = g_MaskTexture.Sample(AlphaMaskWrapSampler, texcoord2);

	Out.vColor = albedo * g_fAlpha;

	if (Out.vColor.a == 0)
		discard;

	return Out;
}

PS_OUT MeshEffectWithAlphaMaskPS(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float2 texcoord2 = In.vTexUV;
	texcoord2 += g_UVMoveFactor;
	
	float4 albedo = g_DiffuseTexture.Sample(AlbedoSampler, texcoord2);
	float4 maskTex = g_MaskTexture.Sample(AlphaMaskSampler, texcoord2);

	if (maskTex.r == 0)
		discard;

	Out.vColor = albedo * maskTex.r * g_fAlpha;

	return Out;
}

PS_OUT MeshEffectWithAlphaMaskWrapPS(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float2 texcoord2 = In.vTexUV;
	texcoord2 += g_UVMoveFactor;

	float4 albedo = g_DiffuseTexture.Sample(AlbedoSampler, texcoord2);
	float4 maskTex = g_MaskTexture.Sample(AlphaMaskWrapSampler, texcoord2);

	if (maskTex.r == 0)
		discard;

	Out.vColor = albedo * maskTex.r * g_fAlpha;

	return Out;
}

technique11 DefaultTechnique
{
	pass P0
	{
		SetRasterizerState(RS_None);
		SetDepthStencilState(DS_Test, 0);
		SetBlendState(BS_Trail, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 MeshEffectPS();
	}

	pass P1
	{
		SetRasterizerState(RS_None);
		SetDepthStencilState(DS_Test, 0);
		SetBlendState(BS_Trail, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 MeshEffectWrapPS();
	}

	pass P2
	{
		SetRasterizerState(RS_None);
		SetDepthStencilState(DS_Test, 0);
		SetBlendState(BS_Trail, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 MeshEffectWithAlphaMaskPS();
	}

	pass P3
	{
		SetRasterizerState(RS_None);
		SetDepthStencilState(DS_Test, 0);
		SetBlendState(BS_Trail, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 MeshEffectWithAlphaMaskWrapPS();
	}
}
