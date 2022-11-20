
#include "Shader_Define.hpp"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

matrix			g_LightViewMatrix, g_LightProjMatrix;

texture2D		g_Texture;
texture2D     g_ShadowTexture;

vector          g_vWorldLightPosition = { 0.f, 500.f, 500.f, 1.f };

vector          g_vCamPosition;

float			gTime;
float			gWaveHeight;
float			gSpeed;
float			gWaveFrequency;
float			gUVSpeed;

struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct VS_OUT_DVR
{
	float4      vPosition	: SV_POSITION;
	float2		vTexUV		: TEXCOORD0;
	float3		vDiffuse	: TEXCOORD1;
	float3      vViewDir	: TEXCOORD2;
	float3		vReflection : TEXCOORD3;
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

float3			g_LightColor = { 1.f, 1.f, 1.f };

VS_OUT_DVR VS_MAIN_UVANIMATION(VS_IN In)
{
	VS_OUT_DVR      Out = (VS_OUT_DVR)0;

	float cosTime = gWaveHeight * cos(gTime * gSpeed + In.vTexUV.x * gWaveFrequency);
	In.vPosition.y += cosTime;
	
	Out.vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	
	float3 lightDir = Out.vPosition.xyz - g_vWorldLightPosition.xyz;
	lightDir = normalize(lightDir);
	
	float3 viewDir = normalize(Out.vPosition.xyz - g_vCamPosition.xyz);
	Out.vViewDir = viewDir;
	
	Out.vPosition = mul(Out.vPosition, g_ViewMatrix);
	Out.vPosition = mul(Out.vPosition, g_ProjMatrix);
	
	float3 worldNormal = mul(In.vNormal, (float3x3)g_WorldMatrix);
	worldNormal = normalize(worldNormal);
	
	Out.vDiffuse = dot(-lightDir, worldNormal);
	Out.vReflection = reflect(lightDir, worldNormal);
	
	Out.vTexUV = In.vTexUV + float2(gTime * gUVSpeed, 0);
	
	return Out;
}

VS_OUT_DVR VS_MAIN_SPECULARMAPPING(VS_IN In)
{
	VS_OUT_DVR      Out = (VS_OUT_DVR)0;

	Out.vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);

	float3 lightDir = Out.vPosition.xyz - g_vWorldLightPosition.xyz;
	lightDir = normalize(lightDir);

	float3 viewDir = Out.vPosition.xyz - g_vCamPosition.xyz;
	Out.vViewDir = normalize(viewDir);

	Out.vPosition = mul(Out.vPosition, g_ViewMatrix);
	Out.vPosition = mul(Out.vPosition, g_ProjMatrix);

	float3 worldNormal = mul(In.vNormal, (float3x3)g_WorldMatrix);
	worldNormal = normalize(worldNormal);

	Out.vDiffuse = dot(-lightDir, worldNormal);
	Out.vReflection = reflect(lightDir, worldNormal);
	Out.vTexUV = In.vTexUV;
	return Out;
}

VS_OUT_DVR VS_MAIN_SHADOW(VS_IN In)
{
	VS_OUT_DVR      Out = (VS_OUT_DVR)0;

	Out.vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);

	float3 lightDir = Out.vPosition.xyz - g_vWorldLightPosition.xyz;
	lightDir = normalize(lightDir);

	float3 viewDir = Out.vPosition.xyz - g_vCamPosition.xyz;
	Out.vViewDir = normalize(viewDir);

	Out.vPosition = mul(Out.vPosition, g_ViewMatrix);
	Out.vPosition = mul(Out.vPosition, g_ProjMatrix);

	float3 worldNormal = mul(In.vNormal, (float3x3)g_WorldMatrix);
	worldNormal = normalize(worldNormal);

	Out.vDiffuse = dot(-lightDir, worldNormal);
	Out.vReflection = reflect(lightDir, worldNormal);
	Out.vTexUV = In.vTexUV;
	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
};


struct PS_IN_DVR
{
	float4      vPosition	: SV_POSITION;
	float2		vTexUV		: TEXCOORD0;
	float3		vDiffuse	: TEXCOORD1;
	float3      vViewDir	: TEXCOORD2;
	float3		vReflection : TEXCOORD3;
};

struct PS_OUT
{
	/*SV_TARGET0 : 모든 정보가 결정된 픽셀이다. AND 0번째 렌더타겟에 그리기위한 색상이다. */
	float4		vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexUV * 2.f);
	
	return Out;
}

PS_OUT PS_MAIN_UVANIMATION(PS_IN_DVR In)
{
	PS_OUT         Out = (PS_OUT)0;

	float4 albedo = g_Texture.Sample(DiffuseSampler, In.vTexUV);
	float3 diffuse = g_LightColor * albedo.rgb * saturate(In.vDiffuse);

	float3 reflection = normalize(In.vReflection);
	float3 viewDir = normalize(In.vViewDir);
	float3 specular = 0;

	if (diffuse.x > 0)
	{
		specular = saturate(dot(reflection, -viewDir));
		specular = pow(specular, 20.f);

		float4 specularIntensity = g_Texture.Sample(SpecularSampler, In.vTexUV);
		specular *= specularIntensity.rgb * g_LightColor;
	}

	float3 ambient = float3(0.1f, 0.1f, 0.1f) * albedo;
	Out.vColor = float4(ambient + diffuse + specular, 1);
	return Out;
}

PS_OUT PS_MAIN_SPECULARMAPPING(PS_IN_DVR In)
{
	PS_OUT         Out = (PS_OUT)0;

	float4 albedo = g_Texture.Sample(DiffuseSampler, In.vTexUV);
	float3 diffuse = g_LightColor * albedo.rgb * saturate(In.vDiffuse);

	float3 reflection = normalize(In.vReflection);
	float3 viewDir = normalize(In.vViewDir);
	float3 specular = 0;

	if (diffuse.x > 0)
	{
		specular = saturate(dot(reflection, -viewDir));
		specular = pow(specular, 20.f);

		float4 specularIntensity = g_Texture.Sample(SpecularSampler, In.vTexUV);
		specular *= specularIntensity.rgb * g_LightColor;
	}

	float3 ambient = float3(0.1f, 0.1f, 0.1f) * albedo;

	Out.vColor = float4(ambient + diffuse + specular, 1);

	return Out;
}

PS_OUT PS_MAIN_SHADOW(PS_IN_DVR In)
{
	PS_OUT         Out = (PS_OUT)0;

	float4 albedo = g_Texture.Sample(DiffuseSampler, In.vTexUV);
	float3 diffuse = g_LightColor * albedo.rgb * saturate(In.vDiffuse);

	float3 reflection = normalize(In.vReflection);
	float3 viewDir = normalize(In.vViewDir);
	float3 specular = 0;

	if (diffuse.x > 0)
	{
		specular = saturate(dot(reflection, -viewDir));
		specular = pow(specular, 20.f);

		float4 specularIntensity = g_Texture.Sample(SpecularSampler, In.vTexUV);
		specular *= specularIntensity.rgb * g_LightColor;
	}

	float3 ambient = float3(0.1f, 0.1f, 0.1f) * albedo;

	Out.vColor = float4(ambient + diffuse + specular, 1);

	return Out;
}

technique11 DefaultTechnique
{
	pass Default
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

	pass SPECULAMAPPING
	{
		SetRasterizerState(rsSolidframe);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN_SPECULARMAPPING();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_SPECULARMAPPING();
	}

	pass SHADOW
	{
		SetRasterizerState(rsSolidframe);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN_SHADOW();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_SHADOW();
	}
}
