#include "Shader_Defines.hpp"

matrix         g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix         g_ViewMatrixInv, g_ProjMatrixInv;
texture2D      g_Texture;
vector         g_vCamPosition;

vector         g_vLightDir;
vector         g_vLightPos;
float         g_fLightRange;
vector         g_vLightDiffuse;
vector         g_vLightAmbient;
vector         g_vLightSpecular;

vector         g_vMtrlAmbient = vector(0.4f, 0.4f, 0.4f, 1.f);
vector         g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);

texture2D      g_NormalTexture;
texture2D      g_DiffuseTexture;
texture2D      g_ShadeTexture;
texture2D      g_SpecularTexture;
texture2D      g_DepthTexture;
texture2D      g_ShadowDepthTexture;
texture2D      g_BlurTex;

matrix         g_matProj;
matrix         g_matViewInv;
matrix         g_matProjInv;
matrix         g_matLightView;

float m_TexW = 1280.f;
float m_TexH = 720.f;

float2         g_Pixeloffset;

sampler ShadowDepthSampler = sampler_state
{
    texture = g_ShadowDepthTexture;
    filter = min_mag_mip_linear;
    AddressU = clamp;
    AddressV = clamp;
};

sampler DepthSampler = sampler_state
{
    texture = g_DepthTexture;
    filter = min_mag_mip_linear;
    AddressU = clamp;
    AddressV = clamp;
};

sampler BlurSampler = sampler_state
{
    texture = g_BlurTex;
    filter = min_mag_mip_linear;
    AddressU = clamp;
    AddressV = clamp;
};
static const float Weight[] =
{
    0.0561, 0.1353, 0.278, 0.4868, 0.7261, 0.9231, 1,
    0.9231,0.7261,0.4868,0.278,0.1353,0.0561
};
static const float Total = 6.2108;

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

    Out.vShade = g_vLightDiffuse * (max(dot(normalize(g_vLightDir) * -1.f, vNormal), 0.f) + (g_vLightAmbient * g_vMtrlAmbient));
    Out.vShade.a = 1.f;

    vector      vReflect = reflect(normalize(g_vLightDir), vNormal);

    vector      vDepth = g_DepthTexture.Sample(PointSampler, In.vTexUV);
    float      fViewZ = vDepth.x * 300.f;


    vector      vWorldPos;

    /* 투영공간상에 위치 .*/
    vWorldPos.x = In.vTexUV.x * 2.f - 1.f;
    vWorldPos.y = In.vTexUV.y * -2.f + 1.f;
    vWorldPos.z = vDepth.y;
    vWorldPos.w = 1.f;

    /* 뷰스페이스상에 위치 .*/
    vWorldPos *= fViewZ;
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

    /* 월드 스페이스상에 위치 .*/
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

    /* 투영공간상에 위치 .*/
    vWorldPos.x = In.vTexUV.x * 2.f - 1.f;
    vWorldPos.y = In.vTexUV.y * -2.f + 1.f;
    vWorldPos.z = vDepth.y;
    vWorldPos.w = 1.f;

    /* 뷰스페이스상에 위치 .*/
    vWorldPos *= fViewZ;
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

    /* 월드 스페이스상에 위치 .*/
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
    vector      vShade = g_ShadeTexture.Sample(LinearSampler, In.vTexUV);
    vector      vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexUV);
    vector      vDepth = g_DepthTexture.Sample(PointSampler, In.vTexUV);

    //vShade = ceil(vShade * 3) / 3.0f; // 보통 3톤 이건 근데 자유 5톤까지

    if (vShade.r < 0.21f)
        vShade.rgb = float3(0.2f, 0.2f, 0.2f);
    else if (vShade.r >= 0.21f && vShade.r < 0.41f)
        vShade.rgb = float3(0.4f, 0.4f, 0.4f);
    else if (vShade.r >= 0.41f && vShade.r <= 1.f)
        vShade.rgb = float3(0.7f, 0.7f, 0.7f);

    Out.vColor = (vDiffuse) * (vShade) * 1.1f;
    //Out.vColor = saturate(Out.vColor);

    //그림자 적용

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
        vector vColor = vector(0.7f, 0.7f, 0.7f, 0.1f);
        Out.vColor *= vColor;
    }
    else if (vPosition.z > vShadowDepthInfo.r * 300.0f + 0.2f)
    {
        vector vColor = vector(0.7f, 0.7f, 0.7f, 0.2f);
        Out.vColor *= vColor;
    }
    else if (vPosition.z > vShadowDepthInfo.r * 300.0f + 0.3f)
    {
        vector vColor = vector(0.7f, 0.7f, 0.7f, 0.3f);
        Out.vColor *= vColor;
    }
    else if (vPosition.z > vShadowDepthInfo.r * 300.0f + 0.4f)
    {
        vector vColor = vector(0.7f, 0.7f, 0.7f, 0.4f);
        Out.vColor *= vColor;
    }
    else if (vPosition.z > vShadowDepthInfo.r * 300.0f + 0.5f)
    {
        vector vColor = vector(0.7f, 0.7f, 0.7f, 0.5f);
        Out.vColor *= vColor;
    }
    else if (vPosition.z > vShadowDepthInfo.r * 300.0f + 0.6f)
    {
        vector vColor = vector(0.7f, 0.7f, 0.7f, 0.6f);
        Out.vColor *= vColor;
    }
    else if (vPosition.z > vShadowDepthInfo.r * 300.0f + 0.7f)
    {
        vector vColor = vector(0.7f, 0.7f, 0.7f, 0.7f);
        Out.vColor *= vColor;
    }
    else if (vPosition.z > vShadowDepthInfo.r * 300.0f + 0.8f)
    {
        vector vColor = vector(0.7f, 0.7f, 0.7f, 0.8f);
        Out.vColor *= vColor;
    }
    else if (vPosition.z > vShadowDepthInfo.r * 300.0f + 0.9f)
    {
        vector vColor = vector(0.7f, 0.7f, 0.7f, 0.9f);
        Out.vColor *= vColor;
    }
    else if (vPosition.z > vShadowDepthInfo.r * 300.0f + 1.f)
    {
        vector vColor = vector(0.7f, 0.7f, 0.7f, 1.f);
        Out.vColor *= vColor;
    }


    if (Out.vColor.a == 0.f)
        discard;

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


technique11 DefaultTechnique
{
    pass Debug
    {
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
    {
        SetRasterizerState(RS_Default);
        SetBlendState(BS_OneByOne, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetDepthStencilState(DS_None_ZEnable_None_ZWrite, 0);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DIRECTIONAL();
    }

    pass Light_Point
    {
        SetRasterizerState(RS_Default);
        SetBlendState(BS_OneByOne, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetDepthStencilState(DS_None_ZEnable_None_ZWrite, 0);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_POINT();
    }

    pass Deferred_Blend
    {
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
    {
        SetRasterizerState(RS_Default);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetDepthStencilState(DS_None_ZEnable_None_ZWrite, 0);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DEFERRED_Test();
    }
}