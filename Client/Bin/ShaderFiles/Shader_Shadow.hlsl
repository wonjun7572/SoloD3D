//
//#include "Shader_Define.hpp"
//
//matrix			g_WorldMatrix, g_LightViewMatrix, g_LightProjMatrix;
//
//// 광원에서 바라본 뷰공간으로 변환해야함, 투영도 달라지기 때문에 라이트 행렬들을 추가해준다.
//
//vector          g_vWorldLightPosition = { 0.f, 500.f, 500.f, 1.f };
//
//texture2D		g_Texture;
//
//vector          g_vCamPosition;
//
//float			gTime;
//float			gWaveHeight;
//float			gSpeed;
//float			gWaveFrequency;
//float			gUVSpeed;
//
//struct VS_IN
//{
//	float3		vPosition : POSITION;
//};
//
//struct VS_IN_TORUS
//{
//	float3		vPosition : POSITION;
//};
//
//struct VS_OUT
//{
//	float4		vPosition : SV_POSITION;
//	float4		vClipPosition : TEXCOORD1;
//};
//
//VS_OUT VS_MAIN(VS_IN In)
//{
//	VS_OUT		Out = (VS_OUT)0;
//
//	float4x4 lightViewMatrix = g_LightViewMatrix;
//
//	float3 dirZ = -normalize(g_vWorldLightPosition.xyz);
//	float3 up = float3(0, 1, 0);
//	float3 dirX = cross(up, dirZ);
//	float3 dirY = cross(dirZ, dirX);
//
//	lightViewMatrix = float4x4(float4(dirX, -dot(g_vWorldLightPosition.xyz, dirX), float4(dirY, -dot(g_vWorldLightPosition.xyz, dirY), 
//		float4(dirZ, -dot(g_vWorldLightPosition.xyz, dirZ), float4(0, 0, 0, 1)));
//	
//	lightViewMatrix = transpose(lightViewMatrix);
//
//	Out.vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
//	Out.vPosition = mul(Out.vPosition, lightViewMatrix);
//	Out.vPosition = mul(Out.vPosition, g_LightProjMatrix);
//
//	Out.vClipPosition = Out.vPosition;
//
//	return Out;
//}
//
//struct PS_IN
//{
//	float4		vPosition : SV_POSITION;
//	float4		vClipPosition : TEXCOORD1;
//};
//
//struct PS_OUT
//{
//	/*SV_TARGET0 : 모든 정보가 결정된 픽셀이다. AND 0번째 렌더타겟에 그리기위한 색상이다. */
//	float4		vColor : SV_TARGET0;
//};
//
//PS_OUT PS_MAIN(PS_IN In)
//{
//	PS_OUT			Out = (PS_OUT)0;
//
//	float depth = In.vClipPosition.z / In.vClipPosition.w;
//	Out.vColor = float4(depth.xxx, 1);
//	return Out;
//}
//
//technique11 DefaultTechnique
//{
//	pass ShadowMapping
//	{
//		VertexShader = compile vs_5_0 VS_MAIN();
//		GeometryShader = NULL;
//		HullShader = NULL;
//		DomainShader = NULL;
//		PixelShader = compile ps_5_0 PS_MAIN();
//	}
//
//
//}
