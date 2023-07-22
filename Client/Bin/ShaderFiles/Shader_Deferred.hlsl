
#include "Shader_Defines.hpp"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix			g_ViewMatrixInv, g_ProjMatrixInv;
texture2D		g_Texture;

vector			g_vCamPosition;

vector			g_vLightDir;
vector			g_vLightDiffuse;
vector			g_vLightAmbient;
vector			g_vLightSpecular;

vector			g_vMtrlAmbient = vector(0.6f, 0.6f, 0.6f, 1.f);
vector			g_vMtrlSpecular = vector(0.7f, 0.7f, 0.7f, 0.7f);

texture2D		g_NormalTexture;
texture2D		g_DiffuseTexture;
texture2D		g_ShadeTexture;
texture2D		g_SpecularTexture;
texture2D		g_DepthTexture;

float			g_fFar = 3000.f;

struct VS_IN
{
	float3		vPosition : POSITION;	
	float2		vUV : TEXCOORD0;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;	
	float2		vUV : TEXCOORD0;
};

VS_OUT VS_Main(VS_IN _In)
{	
	VS_OUT		Out = (VS_OUT)0;	

	matrix		matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matrix		matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(_In.vPosition, 1.f), matWVP);
	Out.vUV = _In.vUV;

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vUV : TEXCOORD0;
};

struct PS_OUT
{
	vector		vColor : SV_TARGET0;	
};

PS_OUT  PS_Main_Debug(PS_IN _In)
{
	PS_OUT	Out = (PS_OUT)0;
	
	Out.vColor = g_Texture.Sample(LinearSampler, _In.vUV);

	return Out;		
}

struct PS_OUT_LIGHT
{
	vector		vShade : SV_TARGET0;
	vector		vSpecular : SV_TARGET1;
};

PS_OUT_LIGHT  PS_Main_Directional(PS_IN _In)
{
	PS_OUT_LIGHT	Out = (PS_OUT_LIGHT)0;
	 
	/* for Shade */
	vector	vNormalDesc = g_NormalTexture.Sample(PointSampler, _In.vUV);
	vector	vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);
	
	Out.vShade = (max(dot(normalize(g_vLightDir) * -1.f, vNormal), 0.f) + (g_vLightAmbient * g_vMtrlAmbient));
	Out.vShade.a = 1.f;

	/* for Specular */
	vector	vReflect = reflect(normalize(g_vLightDir), vNormal);

	vector	vDepth = g_DepthTexture.Sample(PointSampler, _In.vUV);
	float	fViewZ = vDepth.x * 300.f;

	vector vWorldPos;

	vWorldPos.x = _In.vUV.x * 2.f - 1.f;
	vWorldPos.y = _In.vUV.y * -2.f + 1.f;
	vWorldPos.z = vDepth.y;
	vWorldPos.w = 1.f;

	vWorldPos *= fViewZ;
	vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

	vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

	vector		vLook = vWorldPos - g_vCamPosition;

	Out.vSpecular.xyz = 0.6f * ((g_vLightSpecular * g_vMtrlSpecular) * pow(max(dot(normalize(vReflect) * -1.f, normalize(vLook)), 0.f), 30.f));

	return Out;
}

PS_OUT_LIGHT  PS_Main_Point(PS_IN _In)
{
	PS_OUT_LIGHT	Out = (PS_OUT_LIGHT)0;

	vector	vNormalDesc = g_NormalTexture.Sample(LinearSampler, _In.vUV);
	vector	vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);

	Out.vShade = max(dot(normalize(g_vLightDir) * -1.f, vNormal), 0.f);
	Out.vShade.a = 1.f;

	return Out;
}

PS_OUT  PS_Main_Deferred(PS_IN _In)
{
	PS_OUT	Out = (PS_OUT)0;

	vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, _In.vUV);
	vector vShade = g_ShadeTexture.Sample(LinearSampler, _In.vUV);
	vector vSpecular = g_SpecularTexture.Sample(LinearSampler, _In.vUV);

	/*if (0.2f >= vShade.r)
	{
		vShade.rgb = float3(0.2f, 0.2f, 0.2f);
	}
	else if (0.2f < vShade.r && 0.6f <= vShade.r)
	{
		vShade.rgb = float3(0.6f, 0.6f, 0.6f);
	}
	else
	{
		vShade.rgb = float3(0.9f, 0.9f, 0.9f);
	}*/

	Out.vColor = vDiffuse * vShade + vSpecular;

	if (0.f == Out.vColor.a)
		discard;

	return Out;
}

technique11 DefaultTechnique
{		
	pass Debug
	{		
		SetRasterizerState(RS_Default);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);

		VertexShader = compile vs_5_0 VS_Main();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_Main_Debug();
	}

	pass Light_Directional
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_None_ZEnable_None_ZWrite, 0);

		VertexShader = compile vs_5_0 VS_Main();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_Main_Directional();
	}

	pass Light_Point
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_None_ZEnable_None_ZWrite, 0);

		VertexShader = compile vs_5_0 VS_Main();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_Main_Point();
	}

	pass Deferred_Blend
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_None_ZEnable_None_ZWrite, 0);

		VertexShader = compile vs_5_0 VS_Main();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_Main_Deferred();
	}
}
