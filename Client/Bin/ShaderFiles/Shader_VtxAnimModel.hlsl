
#include "Shader_Defines.hpp"

matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

matrix		g_BoneMatrices[256];

float4		g_vCamPosition;

texture2D	g_DiffuseTexture;

float		g_fFar = 300.f;

//OutLineColor_JH
float4			g_lineColor = float4(0.f, 0.f, 0.f, 1.f);
float			g_OutlineThickness;
float			g_OutlineFaceThickness;

//�ܰ��� ���̵�
float3x3      Kx = { -1, 0, 1,
				  -2, 0, 2,
				  -1, 0, 1 };

float3x3      Ky = { 1, 2, 1,
				  0, 0, 0,
				  -1, -2, -1 };

struct VS_IN
{
	float3		vPosition	: POSITION;
	float3		vNormal		: NORMAL;
	float2		vTexUV	: TEXCOORD0;
	float3		vTangent	: TANGENT;
	uint4		vBlendIndices : BLENDINDEX;
	float4		vBlendWeights : BLENDWEIGHT;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
	float4		vProjPos : TEXCOORD2;
};

VS_OUT VS_Main(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matrix		matWVP = mul(matWV, g_ProjMatrix);

	/*���� Ư�� �޽���Ʈ�� Ȥ�� ���� ������ ���� �ʰ� �׷����ٶ��. BlendIndices, BlednWeight��δ� 0���� ä������ ������ ��찡 �߻��ϳ�. */
	float		fWeightW = 1.f - (In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z);

	/* */
	matrix		BoneMatrix = g_BoneMatrices[In.vBlendIndices.x] * In.vBlendWeights.x +
		g_BoneMatrices[In.vBlendIndices.y] * In.vBlendWeights.y +
		g_BoneMatrices[In.vBlendIndices.z] * In.vBlendWeights.z +
		g_BoneMatrices[In.vBlendIndices.w] * fWeightW;

	/* ���ý����̽� ������ �ִԿ����ӿ� ���µ��� ������ ��ȯ��Ű��. */
	vector		vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);
	vector		vNormal = mul(vector(In.vNormal, 0.f), BoneMatrix);

	Out.vPosition = mul(vPosition, matWVP);
	Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));
	Out.vTexUV = In.vTexUV;
	Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
	Out.vProjPos = Out.vPosition;

	return Out;
}

VS_OUT VS_Outline(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matrix		matWVP = mul(matWV, g_ProjMatrix);

	/*���� Ư�� �޽���Ʈ�� Ȥ�� ���� ������ ���� �ʰ� �׷����ٶ��. BlendIndices, BlednWeight��δ� 0���� ä������ ������ ��찡 �߻��ϳ�. */
	float		fWeightW = 1.f - (In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z);

	/* */
	matrix		BoneMatrix = g_BoneMatrices[In.vBlendIndices.x] * In.vBlendWeights.x +
		g_BoneMatrices[In.vBlendIndices.y] * In.vBlendWeights.y +
		g_BoneMatrices[In.vBlendIndices.z] * In.vBlendWeights.z +
		g_BoneMatrices[In.vBlendIndices.w] * fWeightW;

	/* ���ý����̽� ������ �ִԿ����ӿ� ���µ��� ������ ��ȯ��Ű��. */
	vector		vPosition = mul(vector(In.vPosition.xyz + In.vNormal.xyz * g_OutlineThickness, 1.f), BoneMatrix);
	vector		vNormal = mul(vector(In.vNormal, 0.f), BoneMatrix);
	

	Out.vPosition = mul(vPosition, matWVP);
	Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));
	Out.vTexUV = In.vTexUV;
	Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
	Out.vProjPos = Out.vPosition;

	return Out;
}
VS_OUT VS_OutlineFace(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matrix		matWVP = mul(matWV, g_ProjMatrix);

	/*���� Ư�� �޽���Ʈ�� Ȥ�� ���� ������ ���� �ʰ� �׷����ٶ��. BlendIndices, BlednWeight��δ� 0���� ä������ ������ ��찡 �߻��ϳ�. */
	float		fWeightW = 1.f - (In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z);

	/* */
	matrix		BoneMatrix = g_BoneMatrices[In.vBlendIndices.x] * In.vBlendWeights.x +
		g_BoneMatrices[In.vBlendIndices.y] * In.vBlendWeights.y +
		g_BoneMatrices[In.vBlendIndices.z] * In.vBlendWeights.z +
		g_BoneMatrices[In.vBlendIndices.w] * fWeightW;

	/* ���ý����̽� ������ �ִԿ����ӿ� ���µ��� ������ ��ȯ��Ű��. */
	vector		vPosition = mul(vector(In.vPosition.xyz + In.vNormal.xyz * g_OutlineFaceThickness, 1.f), BoneMatrix);
	vector		vNormal = mul(vector(In.vNormal, 0.f), BoneMatrix);


	Out.vPosition = mul(vPosition, matWVP);
	Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));
	Out.vTexUV = In.vTexUV;
	Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
	Out.vProjPos = Out.vPosition;

	return Out;
}

VS_OUT VS_MAIN_SHADOW(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	/* �ϵ���� ��Ű��. */
	float		fWeightW = 1.f - (In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z);

	matrix		BoneMatrix = g_BoneMatrices[In.vBlendIndices.x] * In.vBlendWeights.x +
		g_BoneMatrices[In.vBlendIndices.y] * In.vBlendWeights.y +
		g_BoneMatrices[In.vBlendIndices.z] * In.vBlendWeights.z +
		g_BoneMatrices[In.vBlendIndices.w] * fWeightW;

	vector		vPosition = mul(float4(In.vPosition, 1.f), BoneMatrix);
	vector		vNormal = normalize(mul(float4(In.vNormal, 0.f), BoneMatrix));

	/*vector		vPosition = mul(vector(In.vPosition.xyz + In.vNormal.xyz * 0.005f, 1.f), BoneMatrix);
	vector		vNormal = mul(vector(In.vNormal, 0.f), BoneMatrix);*/

	Out.vPosition = mul(vPosition, matWVP);
	Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));
	Out.vTexUV = In.vTexUV;
	Out.vProjPos = Out.vPosition;
	Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);

	return Out;
}


struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
	float4		vProjPos : TEXCOORD2;
};

struct PS_OUT
{
	vector		vDiffuse : SV_TARGET0;
	vector		vNormal : SV_TARGET1;
	vector		vDepth : SV_TARGET2;
};

struct PS_OUT_DEFERRED
{
	vector		vDiffuse : SV_TARGET0;
	vector		vNormal : SV_TARGET1;
	vector		vDepth : SV_TARGET2;
};

PS_OUT  PS_Main(PS_IN _In)
{
	PS_OUT	Out = (PS_OUT)0;

	vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearClampSampler, _In.vTexUV);

	//if (vMtrlDiffuse.a < 0.1f)
	//	discard;

	Out.vDiffuse = vMtrlDiffuse;
	Out.vDiffuse.a = 1.f;
	Out.vNormal = vector(_In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(_In.vProjPos.w / 1.f, _In.vProjPos.z / _In.vProjPos.w, 0.f, 0.f);
	//(�� �����̽��� z, ���� �����̽��� z, 0.f, 0.f)

	return Out;
};
//OutLine_PS
PS_OUT  PS_Outline(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	

	
	float4 outlineColor = g_lineColor;

	float4 diffuseColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	if (diffuseColor.a < 0.1f)
		discard;

	// �ܰ��� ������ ������ ���� Alpha ���� 1�� ����
	outlineColor.a = 1.0f;

	// �ܰ��� �β��� �����Ͽ� �ܰ��� ����� �Ϲ� ������ ����� �ռ�
	float outlineFactor = saturate(length(In.vNormal.xyz) * g_OutlineThickness);
	float blendFactor = smoothstep(0.f, 1.f, outlineFactor); // smoothstep(float edge0, float edge1, float x); edge 0 : �������� edge 1 : ���� ������ �� x: �������

	vector Color = lerp(diffuseColor, outlineColor, blendFactor);

	Out.vDiffuse = Color;

	// In.vNormal xyz������ -1 ~ 1
	// Out.vNormal ������� �� �ִ� xyz���� 0 ~ 1
	//Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	//Out.vDepth = vector(In.vProjPos.w / 300, In.vProjPos.z / In.vProjPos.w, 0.f, 0.f);

	return Out;
}
PS_OUT_DEFERRED PS_MAIN_SHADOW(PS_IN In)
{
	PS_OUT_DEFERRED		Out = (PS_OUT_DEFERRED)0;

	Out.vDiffuse.r = In.vProjPos.w / 300.f;

	Out.vDiffuse.a = 1.f;

	return Out;
}
	

technique11 DefaultTechnique
{
	pass General // 0
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);

		VertexShader = compile vs_5_0 VS_Main();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_Main();
	}
	
	pass Outline // 1
	{
		SetRasterizerState(RS_CULL_CW);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);

		VertexShader = compile vs_5_0 VS_Outline();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_Outline();
	}

	pass OutlineFace // 2
	{
		SetRasterizerState(RS_CULL_CW);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);

		VertexShader = compile vs_5_0 VS_OutlineFace();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_Outline();
	}

	pass Shadow  // 3
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DS_None_ZEnable_None_ZWrite, 0);

		VertexShader = compile vs_5_0 VS_MAIN_SHADOW();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_SHADOW();
	}
};



////======================================���̴� �����=============================================
//// ���̴� = ����� ���� ������ ������ ����
//// DX9 = ������� ������ ������ ����
//
//// ��ġ�� �˾Ƽ� ���ִ� ������ ������������ ������� �ʰ�
//// ���� ������ ������������ ����� �����ϸ鼭
//// ��� �߰� �� ����ȭ�� �ϴ°� = ���̴��� ����Ѵ�
//
//// ���̴��� �����Ǵ� ����
//// ���̴� �ڵ带 ��üȭ���Ѿ� �Ѵ� - ���̴� Ŭ���� ������Ʈ�� ���� ó���� ��
//// ID3DX11Effect* pEffect ��ü - ���̴� Ŭ������ ����������
//// pEffect���� Apply() - ���̴��� �׸����̴�.
//
//// m_pContext->DrawIndexed();
//// �ε��� ������ ù��° �ε����� ������ ��������
//// �� ������ ���ڷ� ���ؽ� ���̴� �Լ��� ȣ���Ѵ� (VS_Main())
//// �Լ� ȣ���� ������, ����Ʈ ��ȯ�� ����, ����Ʈ ��ȯ���� ������
//// ���� �ε����� ������ �����ͼ� �ٽ� ������ �����Ѵ�.
//// 3��° �������� ������ ������ (�ﰢ���� �׸� �� �ְ� �Ǹ�)
//// �����Ͷ���� �����Ѵ�. - ������ ������ ������� �ȼ��� �����س����� ����
//
////���̴� ������ /fx (ȿ��) ���̴��̱� ������ technique11�� �����Ѵ�
////� �Լ��� ȣ��� ������ ���̴� ���� �ȿ��� �˷��ִ°�
////�ٸ� ������ ��쿡�� ���̴� ���Ͽ��� �Լ��� �����, Ŭ���̾�Ʈ���� � �Լ��� ȣ������ �������ش�.
////������ �ϵ��� �´� ���̴� �������� ���̴��� ������ �� �� �ټ��� technique11�� �����Ѵ�
//
//struct VS_IN
//{
//	// Ŭ�󿡼� �����ִ� ������ ���� ����ü - ������ ���� ���� �ٸ� ���̴� ������ ���� ��
//	// ex)VTXTEX
//	// : POSITION - �ø�ƽ - D3DDECLUSAGE ����ü�� _POSITION, _NORMAL ������
//	// ��������� �ø�ƽ�� �������� ���� ����Ұ�
//	float3	vPosition : POSITION;
//	float2	vTexCoord : TEXCOORD0;
//};
//
//struct VS_OUT
//{
//	// ���� �ڵ忡���� VS_Main()���� ����, ��, ���� ��ȯ�� ��� �����Ѵ�
//	// ���� ����� ���� �ڿ� w��������� �ؾ� ������ȯ�� �Ϸ�ȴ�.
//	// VS_IN ������ Position�� float3�̶� w�� ����
//	// ������ȯ�� �Ϸ��� w���� �־�� w�����⸦ �� �� �ֱ⶧���� float4�� �������� �Ѵ�
//	// IN�� OUT�� �ڷ����� �޶����� ������ IN, OUT�� ���� �����ߴ�.
//
//
//	float4 vPosition : SV_POSITION;
//	float2 vTexCoord : TEXCOORD0;
//};
//
//VS_OUT VS_Main(VS_IN _In)
//{
//	//���� �ϳ��� �޾Ƽ� ������ ����� ������ ���� ���̴�
//
//	//_In.vPosition * ���� * �� * ����
//	// mul() - ����� ���� �� ���
//	// DX9���� ����ߴ� TransformCoord�� �ٸ��� �ڵ����� z�����⸦ �������� �ʴ´�.
//	// �ܼ��� ���ϱ��� ���
//}
//
////������ ����, ��, ���� ����� ��� ���ϰ� �Լ��� ����Ǹ�
//// �ڵ����� z �����⸦ �����ؼ� ���� ��ȯ�� �Ϸ��Ѵ�.
//// ���� ����Ʈ ��ȯ�� �Ѵ�.
//
////�����Ͷ�����
//struct PS_IN
//{
//	// �ȼ� ���̴����� ����ϴ� ����ü
//	// �ȼ��� ������ ������� ��������� ������ VS_OUT���� ����ص� ����������
//	// �������� ���� PS_IN���� �����
//
//	//�ȼ� ���̵� �ܰ迡���� �̹� �ȼ��� ��ġ�� �������ֱ� ������ Position�� ��� �ʿ����� �ʴ� - �����ص� ����
//	float4 vPosition : SV_POSITION;
//	float2 vTexCoord : TEXCOORD0;
//};
//
//struct PS_OUT
//{
//	//�ȼ� ���̴��� OUT����ü
//	//�ȼ� ���̴��� �ȼ��� ���� �����ϱ� ������ ���� ���� float4(vector)�� ������ �ȴ�.
//	//TARGET�� 0~7���� ����, �� ��ȣ�� ����Ÿ�ٿ� ��������� ������ �׸���
//	vector vColor : SV_TARGET0;
//	vector vShade : SV_TARGET1;
//};
//
//
//// vector PS_Main(PS_IN _In) : SV_TARGET0 ���� ����ص� ������ OUT ����ü�� ���� ����� ����Ұ�(������ ���)
//PS_OUT PS_Main(PS_IN _In)
//{
//	// �ȼ��� �������� ���� �����Ѵ�.
//	// �ȼ��� �� ��ŭ ����Ǳ� ������ �ִ��� ������ ����°� ����
//
//	PS_OUT vOut = (PS_OUT)0;	//vOut�� 0���� �ʱ�ȭ
//
//
//	return vOut;
//}
//
//technique11 DefaultTechnique
//{
//	//������� ���� ���̴��� ������� ������ �ٸ� ���̴� ���� ��
//	//������� ���� ���̴��� �����ȴ�.
//	//���� ������� �ʵ��� ����� �� ���̴�. (������ ���)
//
//	//VertexShader = compile vs_5_0 VS_Main();
//	// ���ؽ� ���̴� = vs_5_0�������� �����ϰ�, VS_Main() �Լ��� ȣ���Ѵ�
//	// VS_Main()�Լ��� �������� ������ ������ �߻��Ѵ�
//	//GeometryShader = NULL;
//	//HullShader = NULL;
//	//DomainShader = NULL;
//	//PixelShader = compile vs_5_0 PS_Main();
//	// �ȼ� ���̴� = vs_5_0�������� �����ϰ�, PS_Main() �Լ��� ȣ���Ѵ�
//
//
//
//	//pass - ����� ĸ��ȭ
//
//	//0�� ���� - �Ϲ� - �׸���, �븻��, �𵨸� ���� ����
//	//1�� ���� - ���� - ����������� ��ȿ������ ����
//	//���� ���·� ���� ������ ���� ���¸� �ٸ��� �� ��쿡 ����ϱ� ����
//	//[���� ����] -- �߿�
//	pass General
//	{
//		VertexShader = compile vs_5_0 VS_Main();
//		GeometryShader = NULL;
//		HullShader = NULL;
//		DomainShader = NULL;
//		PixelShader = compile vs_5_0 PS_Main();
//	};
//
//	pass Cloaking
//	{
//		VertexShader = compile vs_5_0 VS_Main();
//		GeometryShader = NULL;
//		HullShader = NULL;
//		DomainShader = NULL;
//		PixelShader = compile vs_5_0 PS_Main();
//	};
//};
