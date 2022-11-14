
/* 9 : 네모, 지형, 모델을 그리기 위한 셰이더다. */
/* 11 : 특정 구성을 가진 정점들로 이루어진 모델을 그리기위한 셰이더다. */

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

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
	//Out.vPosition = float4(In.vPosition, 1.f);
	Out.vTexUV = In.vTexUV;

	return Out;
}

/* TriangleList : 정점세개를 모두 vs를 통과시킨 후, 세 정점의 위치를 각 위치벡터의 w값으로 나눈다. (투영)  */
/* 뷰포트 변환. */
/* 래스터 라이즈를 수행한다.(정점정보를 기반으로 하여 픽셀의 정보를 생성한다. )*/


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

	Out.vColor = vector(1.f, 0.f, 0.f, 1.f);
	
	return Out;
}

technique11 DefaultTechnique
{
	pass Rect
	{
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}
