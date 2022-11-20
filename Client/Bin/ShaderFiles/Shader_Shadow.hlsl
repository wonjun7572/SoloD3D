
#include "Shader_Define.hpp"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

struct VS_IN
{
	float3 pos : POSITION;
};

struct VS_OUT
{
	float4 pos : SV_Position;
	float4 clipPos : POSITION;
};

VS_OUT VS_Main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0.f;
	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	output.pos = mul(float4(input.pos, 1.f), matWVP);
	output.clipPos = output.pos;

	return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
	return float4(input.clipPos.z / input.clipPos.w, 0.f, 0.f, 0.f);
}