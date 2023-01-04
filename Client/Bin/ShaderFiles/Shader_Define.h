sampler	DefaultSampler = sampler_state
{

};

sampler DiffuseSampler = sampler_state
{
	filter = min_mag_mip_linear;
	AddressU = wrap;
	AddressV = wrap;
};

sampler SpecularSampler = sampler_state
{
	filter = min_mag_mip_linear;
	AddressU = wrap;
	AddressV = wrap;
};

sampler NormalSampler = sampler_state
{
	filter = min_mag_mip_linear;
	AddressU = wrap;
	AddressV = wrap;
};


RasterizerState RS_Default
{
	FillMode = Solid;
	CullMode = Back;
	FrontCounterClockwise = false;
};

RasterizerState RS_WireFrame
{ 
	FillMode = WireFrame; 
	CullMode = Back;
	FrontCounterClockwise = false;
};

RasterizerState RS_CW
{
	FillMode = Solid;
	CullMode = Front;
	FrontCounterClockwise = false;
};

RasterizerState RS_None
{
	FillMode = Solid;
	CullMode = None;
};

DepthStencilState DS_Default
{
	DepthEnable = true;
	DepthWriteMask = all;
	DepthFunc = less_equal;
};

DepthStencilState DS_ZEnable_ZWriteEnable_FALSE
{
	DepthEnable = false;
	DepthWriteMask = zero;
};

DepthStencilState DS_Test
{
	DepthEnable = true;
	DepthWriteMask = zero;
};

/* 텍스쳐로부터 픽셀의 색을 샘플링(가져온다) 해오는 방식. */
sampler LinearSampler = sampler_state
{
	filter = min_mag_mip_linear;
	AddressU = wrap;
	AddressV = wrap;
};

sampler PointSampler = sampler_state
{
	filter = min_mag_mip_Point;
	AddressU = wrap;
	AddressV = wrap;
};

BlendState BS_Default
{
	BlendEnable[0] = false;
};

BlendState BS_AlphaBlending
{
	BlendEnable[0] = true;

	SrcBlend = src_alpha;
	DestBlend = inv_Src_Alpha;
	BlendOp = add;
};

BlendState BS_One
{
	BlendEnable[0] = true;
	SrcBlend = one;
	DestBlend = one;
	BlendOp = add;
};

BlendState BS_Trail
{
	BlendEnable[0] = true;
	SrcBlend = src_alpha;
	DestBlend = one;
	BlendOp = add;
};

float3x3 Kx = { -1, 0, 1,
-2, 0, 2,
-1, 0, 1 };

float3x3 Ky = { 1, 2, 1,
0, 0, 0,
-1,-2,-1 };


/* 샘플링 해오는 함수 */
/* dx9 : tex2D(DefaultSampler, In.vTexUV);*/
/* dx11 : g_Texture.Sample(DefaultSampler, In.vTexUV); */