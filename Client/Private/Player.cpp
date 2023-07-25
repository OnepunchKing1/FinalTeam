#include "pch.h"
#include "..\Public\Player.h"

#include "GameInstance.h"

CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCharacter(pDevice, pContext)
{
}

CPlayer::CPlayer(const CPlayer& rhs)
	: CCharacter(rhs)
{
}

HRESULT CPlayer::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pModelCom->Set_Animation(7);

	return S_OK;
}

void CPlayer::Tick(_double dTimeDelta)
{
	if (true == m_isDead)
		return;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Get_DIKeyState(DIK_HOME) & 0x80)
	{
		++m_iNumAnim;
		if (m_pModelCom->Get_NumAnims() <= m_iNumAnim)
			m_iNumAnim = m_pModelCom->Get_NumAnims() - 1;
		m_pModelCom->Set_Animation(m_iNumAnim);
	}

	if (pGameInstance->Get_DIKeyState(DIK_END) & 0x80)
	{
		if (0 < m_iNumAnim)
			--m_iNumAnim;
		if (0 > m_iNumAnim)
			m_iNumAnim = 0;
		m_pModelCom->Set_Animation(m_iNumAnim);
	}
	if (pGameInstance->Get_DIKeyState(DIK_UP))
	{
		m_pTransformCom->Go_Straight(dTimeDelta);
	}
	if (pGameInstance->Get_DIKeyState(DIK_DOWN))
	{
		m_pTransformCom->Go_Backward(dTimeDelta);
	}
	if (pGameInstance->Get_DIKeyState(DIK_LEFT))
	{
		m_pTransformCom->Turn(XMVectorSet(0.f,1.f,0.f,0.f),-dTimeDelta);
	}
	if (pGameInstance->Get_DIKeyState(DIK_RIGHT))
	{
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), dTimeDelta);
	}


	Safe_Release(pGameInstance);

	m_pModelCom->Play_Animation(dTimeDelta);

	__super::Tick(dTimeDelta);

	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
		return;
	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOWDEPTH, this)))
		return;

}

void CPlayer::LateTick(_double dTimeDelta)
{
	__super::LateTick(dTimeDelta);

	

#ifdef _DEBUG
	/*if (FAILED(m_pRendererCom->Add_DebugGroup(m_pNavigationCom)))
		return;*/
#endif // _DEBUG
}

HRESULT CPlayer::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	//Outline Render
	for (m_iMeshNum = 0; m_iMeshNum < iNumMeshes; m_iMeshNum++)
	{


		if (FAILED(m_pModelCom->Bind_ShaderResource(m_iMeshNum, m_pShaderCom, "g_DiffuseTexture", MESHMATERIALS::TextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_ShaderBoneMatrices(m_iMeshNum, m_pShaderCom, "g_BoneMatrices")))
			return E_FAIL;

		if (m_iMeshNum == 2)
			m_pShaderCom->Begin(2);
		else
			m_pShaderCom->Begin(1);

		m_pModelCom->Render(m_iMeshNum);
	}

	// Default Render
	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_ShaderResource(i, m_pShaderCom, "g_DiffuseTexture", MESHMATERIALS::TextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_ShaderBoneMatrices(i, m_pShaderCom, "g_BoneMatrices")))
			return E_FAIL;

		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CPlayer::Render_ShadowDepth()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();


	_vector vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_vector	vLightEye = XMVectorSet(-5.f, 10.f, -5.f, 1.f);
	_vector	vLightAt = XMVectorSet(60.f, 0.f, 60.f, 1.f);
	_vector	vLightUp = XMVectorSet(0.f, 1.f, 0.f, 1.f);


	_matrix      LightViewMatrix = XMMatrixLookAtLH(vLightEye, vLightAt, vLightUp);
	_float4x4   FloatLightViewMatrix;
	XMStoreFloat4x4(&FloatLightViewMatrix, LightViewMatrix);

	if (FAILED(m_pShaderCom->SetUp_Matrix("g_ViewMatrix",
		&FloatLightViewMatrix)))
		return E_FAIL;

	_matrix      LightProjMatrix;
	_float4x4   FloatLightProjMatrix;

	LightProjMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(120.f), _float(1280) / _float(720), 0.2f, 300.f);
	XMStoreFloat4x4(&FloatLightProjMatrix, LightProjMatrix);

	if (FAILED(m_pShaderCom->SetUp_Matrix("g_ProjMatrix",
		&FloatLightProjMatrix)))
		return E_FAIL;


	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_ShaderResource(i, m_pShaderCom, "g_DiffuseTexture", MESHMATERIALS::TextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_ShaderBoneMatrices(i, m_pShaderCom, "g_BoneMatrices")))
			return E_FAIL;


 
 		m_pShaderCom->Begin(3);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CPlayer::Add_Components()
{
	/* for.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Tanjiro"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		MSG_BOX("Failed to Add_Com_Model : CPlayer");
		return E_FAIL;
	}

	/* for.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSG_BOX("Failed to Add_Com_Shader : CPlayer");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CPlayer::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_float4x4 ViewMatrix = pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW);
	if (FAILED(m_pShaderCom->SetUp_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;

	_float4x4 ProjMatrix = pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ);
	if (FAILED(m_pShaderCom->SetUp_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;


	// OutlineThickness
	if (FAILED(m_pShaderCom->SetUp_RawValue("g_OutlineThickness", &m_fOutlineThickness, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetUp_RawValue("g_OutlineFaceThickness", &m_fOutlineFaceThickness, sizeof(_float))))
		return E_FAIL;





	Safe_Release(pGameInstance);

	return S_OK;
}

CPlayer* CPlayer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayer* pInstance = new CPlayer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayer::Clone(void* pArg)
{
	CPlayer* pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();
}
