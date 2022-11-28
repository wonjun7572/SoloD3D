
#include "Shader_Define.hpp"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

matrix			g_ViewInverseMatrix, g_ProjInverseMatrix;

vector          g_vCamPosition;

/* 빛 정보 */
vector			g_vLightDir;
vector			g_vLightDiffuse;
vector			g_vLightAmbient;
vector			g_vLightSpecular;

/* 포인트 라이트를 위한 */
vector			g_vLightPos;
float			g_fRange;

/* 재질 정보 */
texture2D		g_DiffuseTexture;
texture2D		g_NormalTexture;
vector			g_vMtrlAmbient = vector(0.4f, 0.4f, 0.4f, 1.f);
vector			g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);

/* 높이 맵*/
texture2D		g_HeightTexture;

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
	float3		vTangent : TANGENT;
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

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix));
	Out.vTexUV = In.vTexUV;
	Out.vTangent = (vector)0.f;

	return Out;
}

VS_OUT VS_MAIN_UVANIMATION(VS_IN In)
{
	VS_OUT      Out = (VS_OUT)0;

	float2 vHeight = g_HeightTexture.SampleLevel(LinearSampler, In.vPosition.xz, 0);
	In.vPosition.y = vHeight.x;

	//float cosTime = g_WaveHeight * cos(g_Time * g_Speed + In.vTexUV.x * g_WaveFrequency);
	//In.vPosition.y += cosTime;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vNormal   = mul(float4(In.vNormal, 0.f), g_WorldMatrix);
	Out.vTexUV    = In.vTexUV + float2(g_Time * g_UVSpeed, 0);
	Out.vTangent  = (vector)0.f;

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
	/*SV_TARGET0 : 모든 정보가 결정된 픽셀이다. AND 0번째 렌더타겟에 그리기위한 색상이다. */
	float4		vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	return Out;
}

PS_OUT PS_MAIN_UVANIMATION(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector		vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	vector		vDiffuse = (g_vLightDiffuse * vMtrlDiffuse);

	float		fShade = saturate(dot(normalize(g_vLightDir) * -1.f, normalize(In.vNormal)));

	vector		vReflect = reflect(normalize(g_vLightDir), normalize(In.vNormal));

	vector		vPos = mul(In.vPosition, g_ProjInverseMatrix);
	vPos = mul(vPos, g_ViewInverseMatrix);

	vector		vLook	 = vPos - g_vCamPosition;

	/* 두 벡터 : 빛의 반사벡터, 정점을 바라보는 시선벡터 */
	float		fSpecular = pow(saturate(dot(normalize(vReflect) * -1.f, normalize(vLook))), 30.f);

	Out.vColor = vDiffuse * saturate(fShade + (g_vLightAmbient * g_vMtrlAmbient))
		+ fSpecular * (g_vLightSpecular * g_vMtrlSpecular);

	return Out;
}

technique11 DefaultTechnique
{
	pass Default
	{
		SetRasterizerState(rsSolidframe);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
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
