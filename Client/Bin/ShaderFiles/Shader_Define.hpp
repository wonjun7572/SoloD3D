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

/* ���ø� �ؿ��� �Լ� */
/* dx9 : tex2D(DefaultSampler, In.vTexUV);*/
/* dx11 : g_Texture.Sample(DefaultSampler, In.vTexUV); */