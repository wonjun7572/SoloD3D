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


sampler	DefaultSampler = sampler_state
{

};

RasterizerState rsSolidframe{ FillMode = Solid; };
RasterizerState rsWireframe{ FillMode = WireFrame; };


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

BlendState BS_AddBlending
{
	BlendEnable[0] = true;

SrcBlend = one;
DestBlend = one;
BlendOp = add;
};


/* 샘플링 해오는 함수 */
/* dx9 : tex2D(DefaultSampler, In.vTexUV);*/
/* dx11 : g_Texture.Sample(DefaultSampler, In.vTexUV); */