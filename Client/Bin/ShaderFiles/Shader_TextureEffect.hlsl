
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
	/* 정점에 필요한 변환을 모두 거쳤다. (월드, 뷰, 투영) */
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
};

/* 내가 그리는데 사용하는 정점의 갯수만큼. (인덱스의 갯수만큼) */

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	/* 기본적인 변환을 거친다. (월드, 뷰, 투영) */
	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;
	Out.vProjPos = Out.vPosition;

	return Out;
}

/* TriangleList : 정점세개를 모두 vs를 통과시킨 후, 세 정점의 위치를 각 위치벡터의 w값으로 나눈다. (투영)  */
/* 뷰포트 변환. */
/* 래스터 라이즈를 수행한다.(정점정보를 기반으로 하여 픽셀의 정보를 생성한다. )*/

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
};

struct PS_OUT
{
	/*SV_TARGET0 : 모든 정보가 결정된 픽셀이다. AND 0번째 렌더타겟에 그리기위한 색상이다. */
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

	float		fOldViewZ = vDepthDesc.y * 300.f; // 카메라의 far
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
