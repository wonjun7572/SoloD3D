
#include "Shader_Define.hpp"

matrix         g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D	   g_Texture;

vector         g_vWorldLightPosition = { 500.f, 500.f,-500.f, 1.f };

vector         g_vCamPosition;

float			gTime;

float			gWaveHeight;
float			gSpeed;
float			gWaveFrequency;
float			gUVSpeed;

struct VS_IN
{
	float3      vPosition : POSITION;
	float3      vNormal   : NORMAL;
	float2		vTexUV    : TEXCOORD0;
};

struct VS_OUT
{
	float4      vPosition	: SV_POSITION;
	float2		vTexUV		: TEXCOORD0;
	float3		vDiffuse	: TEXCOORD1;
	float3      vViewDir	: TEXCOORD2;
	float3		vReflection : TEXCOORD3;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT      Out = (VS_OUT)0;

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

struct PS_IN
{
	float4      vPosition	: SV_POSITION;
	float2		vTexUV		: TEXCOORD0;
	float3		vDiffuse	: TEXCOORD1;
	float3      vViewDir	: TEXCOORD2;
	float3		vReflection : TEXCOORD3;
};

struct PS_OUT
{
	float4      vColor : SV_TARGET0;
};

float3 gLightColor = { 1.f, 1.f, 1.f };

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT         Out = (PS_OUT)0;

	float4 albedo = g_Texture.Sample(DiffuseSampler, In.vTexUV * 200.f);
	float3 diffuse = gLightColor * albedo.rgb * saturate(In.vDiffuse);

	float3 reflection = normalize(In.vReflection);
	float3 viewDir = normalize(In.vViewDir);
	float3 specular = 0;

	if (diffuse.x > 0)
	{
		specular = saturate(dot(reflection, -viewDir));
		specular = pow(specular, 20.f);

		float4 specularIntensity = g_Texture.Sample(SpecularSampler, In.vTexUV * 200.f);
		specular *= specularIntensity.rgb * gLightColor;
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

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}
