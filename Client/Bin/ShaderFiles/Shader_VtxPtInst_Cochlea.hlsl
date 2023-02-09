
#include "Shader_Define.h"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
vector			g_vCamPosition;
texture2D		g_Texture;

float g_fSizeX_UV = 1.f, g_fSizeY_UV = 1.f;
uint g_iSpriteCountX = 1, g_iSpriteCountY = 1;

struct VS_IN
{
	float3		vPosition : POSITION;
	float2		vPSize : PSIZE;

	row_major float4x4	Matrix : WORLD;
};

struct VS_OUT
{
	float3		vPosition : POSITION;
	float2		vPSize : PSIZE;

	float		fPSizeOffset : TEXCOORD0;
	uint		iSpriteIndex : TEXCOORD1;
};

/* 추가적으로 정점을 생성할 수 있는 상황을 준비해준다. */
VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	Out.fPSizeOffset = In.Matrix._m03;
	In.Matrix._m03 = 0.f;
	Out.iSpriteIndex = In.Matrix._m13;
	In.Matrix._m13 = 0.f;

	vector		vPosition = mul(float4(In.vPosition, 1.f), In.Matrix);

	Out.vPosition = mul(vPosition, g_WorldMatrix).xyz;
	Out.vPSize = In.vPSize;

	return Out;
}

struct GS_IN
{
	float3		vPosition : POSITION;
	float2		vPSize : PSIZE;

	float		fPSizeOffset : TEXCOORD0;
	uint		iSpriteIndex : TEXCOORD1;
};

struct GS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
};

[maxvertexcount(6)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> Vertices)
{
	GS_OUT		Out[4];

	uint index = In[0].iSpriteIndex;
	uint x = index % g_iSpriteCountX;
	uint y = index / g_iSpriteCountY;

	float3		vLook = g_vCamPosition.xyz - In[0].vPosition;
	float3		vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook)) * (In[0].vPSize.x * In[0].fPSizeOffset) * 0.5f;
	float3		vUp = normalize(cross(vLook, vRight)) * (In[0].vPSize.y * In[0].fPSizeOffset) * 0.5f;

	matrix		matVP = mul(g_ViewMatrix, g_ProjMatrix);
	float3		vPosition;

	vPosition = In[0].vPosition + vRight + vUp;
	Out[0].vPosition = mul(vector(vPosition, 1.f), matVP);
	Out[0].vTexUV = float2(x * g_fSizeX_UV, y * g_fSizeY_UV);

	vPosition = In[0].vPosition - vRight + vUp;
	Out[1].vPosition = mul(vector(vPosition, 1.f), matVP);
	Out[1].vTexUV = float2((x + 1) * g_fSizeX_UV, y * g_fSizeY_UV);

	vPosition = In[0].vPosition - vRight - vUp;
	Out[2].vPosition = mul(vector(vPosition, 1.f), matVP);
	Out[2].vTexUV = float2((x + 1) * g_fSizeX_UV, (y + 1) * g_fSizeY_UV);

	vPosition = In[0].vPosition + vRight - vUp;
	Out[3].vPosition = mul(vector(vPosition, 1.f), matVP);
	Out[3].vTexUV = float2(x * g_fSizeX_UV, (y + 1) * g_fSizeY_UV);

	Vertices.Append(Out[0]);
	Vertices.Append(Out[1]);
	Vertices.Append(Out[2]);
	Vertices.RestartStrip();

	Vertices.Append(Out[0]);
	Vertices.Append(Out[2]);
	Vertices.Append(Out[3]);
	Vertices.RestartStrip();
}

/* w나누기. */
/* 뷰퍼ㅗ트 변호ㅓㅏㄴ */
/*( 래스터라이즈 */


struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	/*SV_TARGET0 : 모든 정보가 결정된 픽셀이다. AND 0번째 렌더타겟에 그리기위한 색상이다. */
	float4		vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = g_Texture.Sample(PointSampler, In.vTexUV);

	bool bDiscard = Out.vColor.r < 0.1f && Out.vColor.g < 0.1f && Out.vColor.b < 0.1f;
	if (bDiscard || Out.vColor.a < 0.1f) discard;

	return Out;
}

technique11 DefaultTechnique
{
	pass Rect
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Trail, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}
