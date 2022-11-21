
#include "Shader_Define.hpp"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

vector          g_vCamPosition;

/* 빛 정보 */
vector			g_vLightDir;
vector			g_vLightDiffuse;
vector			g_vLightAmbient;
vector			g_vLightSpecular;

/* 재질 정보 */
texture2D		g_DiffuseTexture;
vector			g_vMtrlAmbient  = vector(0.4f, 0.4f, 0.4f, 1.f);
vector			g_vMtrlSpecular = vector(0.5f,0.5f,0.5f,1.f);

/* UV 애니메이션을 위한 */
float			g_Time;
float			g_WaveHeight;
float			g_Speed;
float			g_WaveFrequency;
float			g_UVSpeed;

struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float		fShade	  : COLOR0;
	float		fSpecular : COLOR1;
	float2		vTexUV	  : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	vector		vWorldPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	vector		vWorldNormal = mul(float4(In.vNormal, 0.f), g_WorldMatrix);

	Out.fShade = saturate(dot(normalize(g_vLightDir) * -1.f, normalize(vWorldNormal)));

	vector		vReflect = reflect(normalize(g_vLightDir), normalize(vWorldNormal));
	vector		vLook = normalize(vWorldPosition - g_vCamPosition);

	Out.fSpecular = saturate(dot(normalize(vReflect) * -1.f, normalize(vLook)));

	return Out;
}

VS_OUT VS_MAIN_UVANIMATION(VS_IN In)
{
	VS_OUT      Out = (VS_OUT)0;

	float cosTime = g_WaveHeight * cos(g_Time * g_Speed + In.vTexUV.x * g_WaveFrequency);
	In.vPosition.y += cosTime;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);

	vector		vWorldPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	vector		vWorldNormal = mul(float4(In.vNormal, 0.f), g_WorldMatrix);

	Out.fShade = saturate(dot(normalize(g_vLightDir) * -1.f, normalize(vWorldNormal)));

	vector		vReflect = reflect(normalize(g_vLightDir), normalize(vWorldNormal));
	vector		vLook = normalize(vWorldPosition - g_vCamPosition);

	Out.fSpecular = saturate(dot(normalize(vReflect) * -1.f, normalize(vLook)));

	Out.vTexUV = In.vTexUV + float2(g_Time * g_UVSpeed, 0);

	return Out;
}


struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float		fShade : COLOR0;
	float		fSpecular : COLOR1;
	float2		vTexUV	  : TEXCOORD0;
};

struct PS_OUT
{
	/*SV_TARGET0 : 모든 정보가 결정된 픽셀이다. AND 0번째 렌더타겟에 그리기위한 색상이다. */
	float4		vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector		vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV * 2.f);

	vector		vDiffuse = g_vLightDiffuse * vMtrlDiffuse;

	Out.vColor = vDiffuse * saturate(In.fShade + (g_vLightAmbient * g_vMtrlAmbient))
		+ In.fSpecular * (g_vLightSpecular * g_vMtrlSpecular);

	return Out;
}

PS_OUT PS_MAIN_UVANIMATION(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector		vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV * 2.f);

	vector		vDiffuse = g_vLightDiffuse * vMtrlDiffuse;

	Out.vColor = vDiffuse * saturate(In.fShade + (g_vLightAmbient * g_vMtrlAmbient))
		+ In.fSpecular * (g_vLightSpecular * g_vMtrlSpecular);

	return Out;
}

technique11 DefaultTechnique
{
	pass SPECULAMAPPING
	{
		SetRasterizerState(rsSolidframe);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass UVANIMATION
	{
		SetRasterizerState(rsSolidframe);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN_UVANIMATION();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_UVANIMATION();
	}

}
