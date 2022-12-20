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

/* �ؽ��ķκ��� �ȼ��� ���� ���ø�(�����´�) �ؿ��� ���. */
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


/* ���ø� �ؿ��� �Լ� */
/* dx9 : tex2D(DefaultSampler, In.vTexUV);*/
/* dx11 : g_Texture.Sample(DefaultSampler, In.vTexUV); */