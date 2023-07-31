#include "Shader_Defines.hpp"

matrix         g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix         g_ViewMatrixInv, g_ProjMatrixInv;
texture2D      g_Texture;
vector         g_vCamPosition;

vector         g_vLightDir;
vector         g_vLightPos;
float          g_fLightRange;
vector         g_vLightDiffuse;
vector         g_vLightAmbient;
vector         g_vLightSpecular;

vector         g_vMtrlAmbient = vector(0.6f, 0.6f, 0.6f, 1.f);
vector         g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);

texture2D      g_NormalTexture;
texture2D      g_DiffuseTexture;
texture2D      g_ShadeTexture;
texture2D      g_SpecularTexture;
texture2D      g_DepthTexture;
texture2D      g_ShadowDepthTexture;
texture2D      g_SSAOTexture;
texture2D      g_BlurTexture;
texture2D      g_BlurXTexture;
texture2D      g_BlurYTexture;
texture2D      g_CombineBlurTexture;

texture2D      g_FinalTexture; // ���۵� �ؽ�ó
texture2D	   g_BloomTextrue; // ��� �ؽ�ó
texture2D	   g_HDRTexture; // ��� + �� �ؽ�ó

matrix         g_matProj;
matrix         g_matViewInv;
matrix         g_matProjInv;
matrix         g_matLightView;

bool           g_bSSAO;
bool		   g_bSSAOSwitch;

bool		   g_bInvert;
bool		   g_bGrayScale;
bool		   g_bSepia;

//===================================================
float g_fRadius;
float g_fBias;
float g_fFar = 300.f;
float g_fFalloff = 0.000002f;
float g_fStrength = 0.0007f;
float g_fTotStrength = 1.38f;
float g_fInvSamples = 1.f / 16.f;

float3 g_vRandom[16] =
{
	float3(0.2024537f, 0.841204f, -0.9060141f),
	float3(-0.2200423f, 0.6282339f, -0.8275437f),
	float3(0.3677659f, 0.1086345f, -0.4466777f),
	float3(0.8775856f, 0.4617546f, -0.6427765f),
	float3(0.7867433f, -0.141479f, -0.1567597f),
	float3(0.4839356f, -0.8253108f, -0.1563844f),
	float3(0.4401554f, -0.4228428f, -0.3300118f),
	float3(0.0019193f, -0.8048455f, 0.0726584f),
	float3(-0.7578573f, -0.5583301f, 0.2347527f),
	float3(-0.4540417f, -0.252365f, 0.0694318f),
	float3(-0.0483353f, -0.2527294f, 0.5924745f),
	float3(-0.4192392f, 0.2084218f, -0.3672943f),
	float3(-0.8433938f, 0.1451271f, 0.2202872f),
	float3(-0.4037157f, -0.8263387f, 0.4698132f),
	float3(-0.6657394f, 0.6298575f, 0.6342437f),
	float3(-0.0001783f, 0.2834622f, 0.8343929f),
};

//�ܰ��� ���̵�
float3x3      Kx = { -1, 0, 1,
				  -2, 0, 2,
				  -1, 0, 1 };

float3x3      Ky = { 1, 2, 1,
				  0, 0, 0,
				  -1, -2, -1 };
float2         g_Pixeloffset;

sampler ShadowDepthSampler = sampler_state
{
	texture = g_ShadowDepthTexture;
	filter = min_mag_mip_linear;
	AddressU = clamp;
	AddressV = clamp;
};
sampler HDRSampler = sampler_state
{
	texture = g_FinalTexture;
	filter = min_mag_mip_linear;
	AddressU = clamp;
	AddressV = clamp;
};
sampler BloomOriSampler = sampler_state
{
	texture = g_BloomTextrue;
	filter = min_mag_mip_linear;
	AddressU = clamp;
	AddressV = clamp;
};
sampler BloomSampler = sampler_state
{
	texture = g_HDRTexture;
	filter = min_mag_mip_linear;
	AddressU = clamp;
	AddressV = clamp;
};

sampler DepthSampler = sampler_state
{
	texture = g_DepthTexture;
	filter = min_mag_mip_linear;
	AddressU = wrap;
	AddressV = wrap;
};
sampler NormalSampler = sampler_state
{
	texture = g_NormalTexture;
	filter = min_mag_mip_linear;
	AddressU = wrap;
	AddressV = wrap;
};

sampler BlurSampler = sampler_state
{
	//texture = g_SSAOTexture;
   // texture = g_DiffuseTexture;

	texture = g_BlurTexture;
	filter = min_mag_mip_linear;
	AddressU = clamp;
	AddressV = clamp;

};
sampler BlurXSampler = sampler_state
{
	//texture = g_SSAOTexture;
   // texture = g_DiffuseTexture;

	texture = g_BlurXTexture;
	filter = min_mag_mip_linear;
	AddressU = clamp;
	AddressV = clamp;

};

struct VS_IN
{
	float3      vPosition : POSITION;
	float2      vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4      vPosition : SV_POSITION;
	float2      vTexUV : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT      Out = (VS_OUT)0;

	matrix      matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matrix      matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	return Out;
}

struct PS_IN
{
	float4      vPosition : SV_POSITION;
	float2      vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	vector      vColor : SV_TARGET0;
};


PS_OUT  PS_MAIN_DEBUG(PS_IN In)
{
	PS_OUT   Out = (PS_OUT)0;

	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexUV);

	return Out;
}

struct PS_OUT_LIGHT
{
	vector      vShade : SV_TARGET0;
	vector      vSpecular : SV_TARGET1;
};

PS_OUT_LIGHT PS_MAIN_DIRECTIONAL(PS_IN In)
{
	PS_OUT_LIGHT      Out = (PS_OUT_LIGHT)0;

	vector      vNormalDesc = g_NormalTexture.Sample(PointSampler, In.vTexUV);
	vector      vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);
	vector      vSSAO = g_SSAOTexture.Sample(LinearSampler, In.vTexUV);

	if (g_bSSAOSwitch == false)
		Out.vShade = g_vLightDiffuse * (max(dot(normalize(g_vLightDir) * -1.f, vNormal), 0.f) + (g_vLightAmbient * g_vMtrlAmbient));

	else if (g_bSSAOSwitch == true)
		Out.vShade = g_vLightDiffuse * (max(dot(normalize(g_vLightDir) * -1.f, vNormal), 0.f) + ((g_vLightAmbient * vSSAO)));

	//Out.vShade *= vSSAO;
	/*Out.vShade = g_vLightDiffuse * (max(dot(normalize(g_vLightDir) * -1.f, vNormal), 0.f) + (g_vLightAmbient * g_vMtrlAmbient));*/
	Out.vShade.a = 1.f;

	vector      vReflect = reflect(normalize(g_vLightDir), vNormal);

	vector      vDepth = g_DepthTexture.Sample(PointSampler, In.vTexUV);
	float      fViewZ = vDepth.x * 300.f;


	vector      vWorldPos;

	/* ���������� ��ġ .*/
	vWorldPos.x = In.vTexUV.x * 2.f - 1.f;
	vWorldPos.y = In.vTexUV.y * -2.f + 1.f;
	vWorldPos.z = vDepth.y;
	vWorldPos.w = 1.f;

	/* �佺���̽��� ��ġ .*/
	vWorldPos *= fViewZ;
	vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

	/* ���� �����̽��� ��ġ .*/
	vWorldPos = mul(vWorldPos, g_ViewMatrixInv);



	vector      vLook = vWorldPos - g_vCamPosition;

	Out.vSpecular.xyz = (g_vLightSpecular * g_vMtrlSpecular) * pow(max(dot(normalize(vReflect) * -1.f, normalize(vLook)), 0.f), 30.f);

	return Out;
}

PS_OUT_LIGHT PS_MAIN_POINT(PS_IN In)
{
	PS_OUT_LIGHT      Out = (PS_OUT_LIGHT)0;

	vector      vNormalDesc = g_NormalTexture.Sample(PointSampler, In.vTexUV);
	vector      vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);
	vector      vDepth = g_DepthTexture.Sample(PointSampler, In.vTexUV);
	float      fViewZ = vDepth.x * 300.f;
	vector      vWorldPos;

	/* ���������� ��ġ .*/
	vWorldPos.x = In.vTexUV.x * 2.f - 1.f;
	vWorldPos.y = In.vTexUV.y * -2.f + 1.f;
	vWorldPos.z = vDepth.y;
	vWorldPos.w = 1.f;

	/* �佺���̽��� ��ġ .*/
	vWorldPos *= fViewZ;
	vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

	/* ���� �����̽��� ��ġ .*/
	vWorldPos = mul(vWorldPos, g_ViewMatrixInv);


	vector      vLightDir = vWorldPos - g_vLightPos;

	float      fDistance = length(vLightDir);


	/* 0 ~ 1 */
	float      fAtt = saturate((g_fLightRange - fDistance) / g_fLightRange);



	Out.vShade = g_vLightDiffuse * (max(dot(normalize(vLightDir) * -1.f, vNormal), 0.f) + (g_vLightAmbient * g_vMtrlAmbient)) * fAtt;
	Out.vShade.a = 1.f;

	vector      vReflect = reflect(normalize(vLightDir), vNormal);

	vector      vLook = vWorldPos - g_vCamPosition;

	Out.vSpecular.xyz = (g_vLightSpecular * g_vMtrlSpecular) * pow(max(dot(normalize(vReflect) * -1.f, normalize(vLook)), 0.f), 30.f) * fAtt;

	return Out;
}




PS_OUT PS_MAIN_DEFERRED(PS_IN In)
{
	PS_OUT         Out = (PS_OUT)0;

	vector      vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector      vShade = saturate(g_ShadeTexture.Sample(LinearSampler, In.vTexUV));
	vector      vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexUV);
	vector      vDepth = g_DepthTexture.Sample(PointSampler, In.vTexUV);
	//vector      vSSAO = g_SSAOTexture.Sample(PointClampSampler, In.vTexUV);
	vShade = ceil(vShade * 3) / 3.0f; // ���� 3�� �̰� �ٵ� ���� 5�����

	/*if (vDiffuse.a == 0.f)
		discard;*/
		//vector adjustedDiffuse = vDiffuse * vSSAO;
		//vector adjustedShade = vShade * vSSAO;

		//// ���� ���� ����� �����Ͽ� ���� ����� ����
		//Out.vColor = adjustedDiffuse * adjustedShade;


		/*if (vShade.r < 0.21f)
			vShade.rgb = float3(0.2f, 0.2f, 0.2f);
		else if (vShade.r >= 0.21f && vShade.r < 0.41f)
			vShade.rgb = float3(0.4f, 0.4f, 0.4f);
		else if (vShade.r >= 0.41f && vShade.r <= 1.f)
			vShade.rgb = float3(0.7f, 0.7f, 0.7f);*/

	Out.vColor = (vDiffuse) * (vShade);

	if (true == g_bGrayScale)
		Out.vColor.rgb = dot(Out.vColor.rgb, float3(0.3f, 0.59f, 0.11f));

	if (true == g_bInvert)
		Out.vColor = float4(1.0f - Out.vColor.r, 1.0f - Out.vColor.g, 1.0f - Out.vColor.b, Out.vColor.a);

	if (true == g_bSepia)
	{
		float4 sepia;
		sepia.a = Out.vColor.a;
		sepia.r = dot(Out.vColor.rgb, float3(0.393f, 0.769f, 0.189f));
		sepia.g = dot(Out.vColor.rgb, float3(0.349f, 0.686f, 0.168f));
		sepia.b = dot(Out.vColor.rbb, float3(0.272f, 0.534f, 0.131f));

		Out.vColor = vector(sepia.r, sepia.g, sepia.b, sepia.a);
	}



	if (Out.vColor.a == 0.f)
		discard;

	//�׸��� ����

	vector      vDepthInfo = g_DepthTexture.Sample(DepthSampler, In.vTexUV);
	float      fViewZ = vDepthInfo.x * 300.0f;

	vector      vPosition;

	vPosition.x = (In.vTexUV.x * 2.f - 1.f) * fViewZ;
	vPosition.y = (In.vTexUV.y * -2.f + 1.f) * fViewZ;
	vPosition.z = vDepthInfo.y * fViewZ;
	vPosition.w = fViewZ;

	vPosition = mul(vPosition, g_matProjInv);

	vPosition = mul(vPosition, g_matViewInv);

	vPosition = mul(vPosition, g_matLightView);

	vector      vUVPos = mul(vPosition, g_matProj);

	float2      vNewUV;

	vNewUV.x = (vUVPos.x / vUVPos.w) * 0.5f + 0.5f;
	vNewUV.y = (vUVPos.y / vUVPos.w) * -0.5f + 0.5f;

	vector      vShadowDepthInfo = g_ShadowDepthTexture.Sample(ShadowDepthSampler, vNewUV);

	/*if (vPosition.z - 0.1f > vShadowDepthInfo.r * 300.0f)
	{
		vector vColor = vector(0.7f, 0.7f, 0.7f, 1.f);
		Out.vColor *= vColor;
	}*/

	if (vPosition.z > vShadowDepthInfo.r * 300.0f + 0.1f)
	{
		vector vColor = vector(0.9f, 0.9f, 0.9f, 1.f);
		Out.vColor *= vColor;
	}
	else if (vPosition.z > vShadowDepthInfo.r * 300.0f + 0.2f)
	{
		vector vColor = vector(0.9f, 0.9f, 0.9f, 0.9f);
		Out.vColor *= vColor;
	}
	else if (vPosition.z > vShadowDepthInfo.r * 300.0f + 0.3f)
	{
		vector vColor = vector(0.9f, 0.9f, 0.9f, 0.8f);
		Out.vColor *= vColor;
	}
	else if (vPosition.z > vShadowDepthInfo.r * 300.0f + 0.4f)
	{
		vector vColor = vector(0.9f, 0.9f, 0.9f, 0.7f);
		Out.vColor *= vColor;
	}
	else if (vPosition.z > vShadowDepthInfo.r * 300.0f + 0.5f)
	{
		vector vColor = vector(0.9f, 0.9f, 0.9f, 0.6f);
		Out.vColor *= vColor;
	}
	else if (vPosition.z > vShadowDepthInfo.r * 300.0f + 0.6f)
	{
		vector vColor = vector(0.9f, 0.9f, 0.9f, 0.5f);
		Out.vColor *= vColor;
	}
	else if (vPosition.z > vShadowDepthInfo.r * 300.0f + 0.7f)
	{
		vector vColor = vector(0.9f, 0.9f, 0.9f, 0.4f);
		Out.vColor *= vColor;
	}
	else if (vPosition.z > vShadowDepthInfo.r * 300.0f + 0.8f)
	{
		vector vColor = vector(0.9f, 0.9f, 0.9f, 0.3f);
		Out.vColor *= vColor;
	}
	else if (vPosition.z > vShadowDepthInfo.r * 300.0f + 0.9f)
	{
		vector vColor = vector(0.9f, 0.9f, 0.9f, 0.2f);
		Out.vColor *= vColor;
	}
	else if (vPosition.z > vShadowDepthInfo.r * 300.0f + 1.f)
	{
		vector vColor = vector(0.9f, 0.9f, 0.9f, 0.1f);
		Out.vColor *= vColor;
	}


	return Out;
}



PS_OUT PS_MAIN_DEFERRED_Test(PS_IN In)
{
	PS_OUT         Out = (PS_OUT)0;

	vector      vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector      vShade = g_ShadeTexture.Sample(LinearSampler, In.vTexUV);
	vector      vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexUV);

	Out.vColor = vDiffuse * vShade + vSpecular;

	if (0.f == Out.vColor.a)
		discard;

	return Out;
}

//==============================Bloom======================================
PS_OUT PS_Bloom(PS_IN In)
{
	PS_OUT         Out = (PS_OUT)0;

	float fBrightColor = 0.f;
	vector vFragColor = g_FinalTexture.Sample(LinearSampler, In.vTexUV);

	float fBrightness = dot(vFragColor.rgb, float3(0.2126f, 0.7152f, 0.0722f));
	//float fBrightness = dot(vFragColor.rgb, float3(0.1126f, 0.9152f, 0.1222f));
	if (fBrightness > 0.8f)
		fBrightColor = vector(vFragColor.rgb, 1.f);



	Out.vColor = fBrightColor;

	return Out;
}

PS_OUT PS_Apply_Bloom(PS_IN In)
{
	PS_OUT         Out = (PS_OUT)0;

	vector vHDRColor = g_FinalTexture.Sample(HDRSampler, In.vTexUV); // ���� �ؽ�ó
	vector vBloomOriTex = g_BloomTextrue.Sample(BloomOriSampler, In.vTexUV); // ��� ������ �ؽ�ó
	vector vBloomColor = g_HDRTexture.Sample(BloomSampler, In.vTexUV); // ��� + ������ �ؽ�ó

	if (vHDRColor.a == 0.f)
		discard;

	vector vBloom = pow(pow(abs(vBloomColor), 2.2f) + pow(abs(vBloomOriTex), 2.2f), 1.f / 2.2f);

	vector vOut = (vHDRColor);

	vOut = pow(abs(vOut), 2.2f);
	vBloom = pow(abs(vBloom), 2.2f);

	vOut += vBloom;
	Out.vColor = pow(abs(vOut), 1 / 2.2f);

	if (Out.vColor.a == 0.f)
		discard;



	return Out;
}

//==============================Blur======================================
float m_TexW = 1280.f;
float m_TexH = 720.f;

static const float Weight[13] =
{
	/*0.0561, 0.1353, 0.278, 0.4868, 0.7261, 0.9231,
	1, 0.9231, 0.7261, 0.4868, 0.278, 0.1353, 0.0561*/
	 0.002216,
	0.008764,
	0.026995,
	0.064759,
	0.120985,
	0.176033,
	0.199471,
	0.176033,
	0.120985,
	0.064759,
	0.026995,
	0.008764,
	0.002216,
	//0.1122, 0.2706, 0.556, 0.9736, 1.4522, 1.8462, 2, 1.8462, 1.4522, 0.9736, 0.556, 0.2706, 0.1122


};
static const float Total = /*6.2108*/1.00000012 /*12.4216*/;
//static const float Total = 2.636;
//static const float FinalWeight[7] =
//{
//
//	0.2434, 0.36305, 0.46155,
//	0.5, 0.46155, 0.36305, 0.2434
//};
////static const float FinalTotal = 5.272;
//static const float FinalTotal = 2.636;
static const float FinalWeight[7] =
{

	0.1434, 0.26305, 0.46155,
	0.5, 0.46155, 0.26305, 0.1434
};
//static const float FinalTotal = 5.272;
static const float FinalTotal = 2.236;



PS_OUT PS_BlurX(PS_IN _In)
{
	PS_OUT         Out = (PS_OUT)0;

	float2	t = _In.vTexUV;
	float2	uv = 0;

	float	tu = 1.f / m_TexW;



	if (g_bSSAO == true)
	{
		for (int i = -6; i < 6; ++i)
		{
			uv = t + float2(tu * i, 0);
			Out.vColor += Weight[6 + i] * g_BlurTexture.Sample(BlurSampler, uv);
		}
	}
	else
	{
		for (int i = -3; i < 3; ++i)
		{
			uv = t + float2(tu * i, 0);
			Out.vColor += FinalWeight[3 + i] * g_BlurTexture.Sample(BlurSampler, uv);
		}
	}
	if (g_bSSAO == true)
		Out.vColor /= Total;
	else
		Out.vColor /= FinalTotal;

	/*if (Out.vColor.a == 0.f)
		discard;
	if (Out.vColor.a == 1.f)
		discard;
	if (Out.vColor.r == float(1.f) && Out.vColor.g == float(1.f) && Out.vColor.b == float(1.f))
		discard;
	if (Out.vColor.r == float(0.f) && Out.vColor.g == float(0.f) && Out.vColor.b == float(0.f))
		discard;*/

	return Out;
}

PS_OUT PS_BlurY(PS_IN _In)
{
	PS_OUT         Out = (PS_OUT)0;

	float2 t = _In.vTexUV;
	float2 uv = 0;

	float tv = 1.f / (m_TexH / 2.f);

	if (g_bSSAO == true)
	{
		for (int i = -6; i < 6; ++i)
		{
			uv = t + float2(0, tv * i);
			Out.vColor += Weight[6 + i] * g_BlurTexture.Sample(BlurSampler, uv);
		}
	}
	else
	{
		for (int i = -3; i < 3; ++i)
		{
			uv = t + float2(0, tv * i);
			Out.vColor += FinalWeight[3 + i] * g_BlurTexture.Sample(BlurSampler, uv);
		}
	}

	if (g_bSSAO == true)
		Out.vColor /= Total;
	else
		Out.vColor /= FinalTotal;

	/*if (Out.vColor.a == 0.f)
		discard;
	if (Out.vColor.a == 1.f)
		discard;
	if (Out.vColor.r == float(1.f) && Out.vColor.g == float(1.f) && Out.vColor.b == float(1.f))
		discard;
	if (Out.vColor.r == float(0.f) && Out.vColor.g == float(0.f) && Out.vColor.b == float(0.f))
		discard;*/

	return Out;
}
PS_OUT PS_BlurX_3(PS_IN _In)
{
	PS_OUT         Out = (PS_OUT)0;

	float2	t = _In.vTexUV;
	float2	uv = 0;

	float	tu = 1.f / m_TexW;


	for (int i = -3; i < 3; ++i)
	{
		uv = t + float2(tu * i, 0);
		Out.vColor += FinalWeight[3 + i] * g_BlurTexture.Sample(BlurSampler, uv);
	}


	Out.vColor /= FinalTotal;

	/*if (Out.vColor.a == 0.f)
		discard;
	if (Out.vColor.a == 1.f)
		discard;
	if (Out.vColor.r == float(1.f) && Out.vColor.g == float(1.f) && Out.vColor.b == float(1.f))
		discard;
	if (Out.vColor.r == float(0.f) && Out.vColor.g == float(0.f) && Out.vColor.b == float(0.f))
		discard;*/

	return Out;
}
PS_OUT PS_BlurY_3(PS_IN _In)
{
	PS_OUT         Out = (PS_OUT)0;

	float2 t = _In.vTexUV;
	float2 uv = 0;

	float tv = 1.f / (m_TexH / 2.f);


	for (int i = -3; i < 3; ++i)
	{
		uv = t + float2(0, tv * i);
		Out.vColor += FinalWeight[3 + i] * g_BlurTexture.Sample(BlurSampler, uv);
	}

	Out.vColor /= FinalTotal;

	/*if (Out.vColor.a == 0.f)
		discard;
	if (Out.vColor.a == 1.f)
		discard;
	if (Out.vColor.r == float(1.f) && Out.vColor.g == float(1.f) && Out.vColor.b == float(1.f))
		discard;
	if (Out.vColor.r == float(0.f) && Out.vColor.g == float(0.f) && Out.vColor.b == float(0.f))
		discard;*/

	return Out;
}
PS_OUT PS_Combine_Blur(PS_IN In)
{
	PS_OUT      Out = (PS_OUT)0;

	vector      vFinal = g_Texture.Sample(LinearSampler, In.vTexUV);
	vector      vBlurX = g_BlurXTexture.Sample(LinearSampler, In.vTexUV);
	vector      vBlurY = g_BlurYTexture.Sample(LinearSampler, In.vTexUV);

	vector      vSSAO = g_SSAOTexture.Sample(LinearSampler, In.vTexUV);
	if (vFinal.a == 0.f)
		discard;

	//vFinal *= vSSAO.r;
	Out.vColor = ((vFinal + vBlurX + vBlurY) / 3.f);

	/*if (Out.vColor.a == 0.f)
		discard;*/
		/*if (Out.vColor.a == 1.f)
			discard;
		if (Out.vColor.r == float(1.f) && Out.vColor.g == float(1.f) && Out.vColor.b == float(1.f))
			discard;
		if (Out.vColor.r == float(0.f) && Out.vColor.g == float(0.f) && Out.vColor.b == float(0.f))
			discard;*/

	return Out;
}
PS_OUT PS_Combine_SSAOBlur(PS_IN In)
{
	PS_OUT      Out = (PS_OUT)0;

	vector      vFinal = g_Texture.Sample(LinearSampler, In.vTexUV);
	vector      vBlurX = g_BlurXTexture.Sample(LinearSampler, In.vTexUV);
	vector      vBlurY = g_BlurYTexture.Sample(LinearSampler, In.vTexUV);


	if (vFinal.a == 0.f)
		discard;


	Out.vColor = ((vFinal + vBlurX + vBlurY) / 3.f);

	if (Out.vColor.a == 0.f)
		discard;
	/*if (Out.vColor.a == 1.f)
		discard;
	if (Out.vColor.r == float(1.f) && Out.vColor.g == float(1.f) && Out.vColor.b == float(1.f))
		discard;
	if (Out.vColor.r == float(0.f) && Out.vColor.g == float(0.f) && Out.vColor.b == float(0.f))
		discard;*/

	return Out;
}

//==============================SSAO======================================

float3 randomNormal(float2 tex)
{
	float noiseX = (frac(sin(dot(tex, float2(15.8989f, 76.132f) * 1.0f)) * 46336.23745f));
	float noiseY = (frac(sin(dot(tex, float2(11.9899f, 62.223f) * 2.0f)) * 34748.34744f));
	float noiseZ = (frac(sin(dot(tex, float2(13.3238f, 63.122f) * 3.0f)) * 59998.47362f));
	return normalize(float3(noiseX, noiseY, noiseZ));
}

PS_OUT PS_SSAO_Test(PS_IN _In)
{
	PS_OUT         Out = (PS_OUT)0;

	//float4 vDepth = g_DepthTexture.Sample(LinearClampSampler, _In.vTexUV);
	//float4 vNormal = g_NormalTexture.Sample(PointClampSampler, _In.vTexUV);

	half4      vNormal = g_NormalTexture.Sample(NormalSampler, _In.vTexUV);
	float4 vDepth = g_DepthTexture.Sample(DepthSampler, _In.vTexUV);


	//vNormal = float4(vNormal.xyz * 2.f - 1.f, 0.f);
	if (vNormal.a != 0.f)
	{
		Out.vColor = vector(1.f, 1.f, 1.f, 1.f);
		return Out;
	}
	vNormal = normalize(vNormal * 2.f - 1.f);
	vNormal.a = 0.f;
	float fViewZ = vDepth.r * g_fFar;
	half3 vHNormal = vNormal.rgb;

	vector      vPosition;

	vPosition.x = (_In.vTexUV.x * 2.f - 1.f) * fViewZ;
	vPosition.y = (_In.vTexUV.y * -2.f + 1.f) * fViewZ;
	vPosition.z = vDepth.y * fViewZ;
	vPosition.w = fViewZ;

	vPosition = mul(vPosition, g_matProjInv);
	vPosition = mul(vPosition, g_matViewInv);

	//vector      vUVPos = mul(vPosition, g_matProj);

	float2      vNewUV;

	vNewUV.x = (vPosition.x / vPosition.w) * 0.5f + 0.5f;
	vNewUV.y = (vPosition.y / vPosition.w) * -0.5f + 0.5f;

	float fDepth = vDepth.g * g_fFar * fViewZ;

	half3 vRay;
	half3 vReflect;
	half2 vRandomUV;
	float fOccNorm;
	float4 vRandomDepth;

	int iColor = 0;

	for (int i = 0; i < 16; ++i)
	{

		vRay = reflect(randomNormal(vNewUV), g_vRandom[i]);
		vReflect = normalize(reflect(vRay, vHNormal)) * g_fRadius;
		vReflect.x *= -1.f;
		vRandomUV = _In.vTexUV + vReflect.xy;
		vRandomDepth = g_DepthTexture.Sample(DepthSampler, vRandomUV);
		fOccNorm = vRandomDepth.g * g_fFar * fViewZ;

		if (fOccNorm <= fDepth + 0.3f + g_fBias)
			++iColor;
	}

	vector vAmbient = abs((iColor / 16.f) - 1.f);

	Out.vColor = (1.f - vAmbient);

	if (Out.vColor.a == 0.f)
		discard;
	/* if (Out.vColor.r == 0.f && Out.vColor.g == 0.f && Out.vColor.b == 0.f)
		 discard;*/


	return Out;
}


technique11 DefaultTechnique
{
	pass Debug
	{//0
		SetRasterizerState(RS_Default);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DEBUG();
	}

	pass Light_Diretional
	{//1
		SetRasterizerState(RS_Default);
		SetBlendState(BS_OneByOne_Engine, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_None_ZEnable_None_ZWrite, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DIRECTIONAL();
	}

	pass Light_Point
	{//2
		SetRasterizerState(RS_Default);
		SetBlendState(BS_OneByOne_Engine, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_None_ZEnable_None_ZWrite, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_POINT();
	}

	pass Deferred_Blend // ���۵�
	{//3
		SetRasterizerState(RS_Default);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_None_ZEnable_None_ZWrite, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DEFERRED();
	}

	pass Deferred_Test
	{//4
		SetRasterizerState(RS_Default);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_None_ZEnable_None_ZWrite, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DEFERRED_Test();
	}

	pass SSAO_Test
	{//5
		SetRasterizerState(RS_Default);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_None_ZEnable_None_ZWrite, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_SSAO_Test();
	}

	pass BlurX
	{//6
		SetRasterizerState(RS_Default);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_None_ZEnable_None_ZWrite, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_BlurX();
	}

	pass BlurY
	{//7
		SetRasterizerState(RS_Default);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_None_ZEnable_None_ZWrite, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_BlurY();
	}

	pass CombineBlur
	{//8
		SetRasterizerState(RS_Default);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_None_ZEnable_None_ZWrite, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_Combine_Blur();
	}

	pass CombineSSAOBlur
	{//9
		SetRasterizerState(RS_Default);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_None_ZEnable_None_ZWrite, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_Combine_SSAOBlur();
	}
	pass BlurX_3
	{//10
		SetRasterizerState(RS_Default);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_None_ZEnable_None_ZWrite, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_BlurX_3();
	}
	pass BlurY_11
	{//11
		SetRasterizerState(RS_Default);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_None_ZEnable_None_ZWrite, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_BlurY_3();
	}
	pass ExportBloom
	{//12
		SetRasterizerState(RS_Default);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_None_ZEnable_None_ZWrite, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_Bloom();
	}
	pass Bloom
	{//13
		SetRasterizerState(RS_Default);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_None_ZEnable_None_ZWrite, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_Apply_Bloom();
	}
}