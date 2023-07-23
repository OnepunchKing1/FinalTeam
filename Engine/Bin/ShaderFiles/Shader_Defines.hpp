
sampler LinearSampler = sampler_state
{
	Filter = MIN_MAG_MIP_LINEAR;

	AddressU = WRAP;
	AddressV = WRAP;
};

sampler PointSampler = sampler_state
{
	Filter = MIN_MAG_MIP_POINT;

	AddressU = WRAP;
	AddressV = WRAP;
};

/* For.RasterizerState */
RasterizerState		RS_Default
{
	FillMode = Solid;
	CullMode = BACK;
	FrontCounterClockwise = false;
};

RasterizerState		RS_CULL_CW
{
	FillMode = Solid;
	CullMode = FRONT;
	FrontCounterClockwise = false;
};

RasterizerState		RS_CULL_OFF
{
	FillMode = Solid;
	CullMode = NONE;
	FrontCounterClockwise = false;
};

BlendState	 BS_Default
{
	BlendEnable[0] = false;
};

BlendState	 BS_AlphaBlending
{
	BlendEnable[0] = true;
	SrcBlend = SRC_ALPHA;
	DestBlend = INV_SRC_ALPHA;
	BlendOp = Add;
};

BlendState	 BS_OneByOne
{
	BlendEnable[0] = true;
	SrcBlend = ONE;
	DestBlend = ONE;
	BlendOp = Add;
};

DepthStencilState DS_Default
{
	DepthEnable = true;
	DepthWriteMask = all;
	DepthFunc = LESS_EQUAL;
};

DepthStencilState DS_None_ZWrite
{
	DepthEnable = true;
	DepthWriteMask = zero;
};

DepthStencilState DS_None_ZEnable_None_ZWrite
{
	DepthEnable = false;
	DepthWriteMask = zero;
};
