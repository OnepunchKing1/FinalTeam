#include "pch.h"
#include "..\Public\Camera_Free.h"

#include "GameInstance.h"

CCamera_Free::CCamera_Free(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera(pDevice, pContext)
{
}

CCamera_Free::CCamera_Free(const CCamera_Free& rhs)
	: CCamera(rhs)
{
}

HRESULT CCamera_Free::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
	{
		MSG_BOX("Failed to Initialize_Prototype : Camera_Free");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CCamera_Free::Initialize(void* pArg)
{
	if(FAILED(__super::Initialize(pArg)))
	{
		MSG_BOX("Failed to Initialize : Camera_Free");
		return E_FAIL;
	}

	return S_OK;
}

void CCamera_Free::Tick(_double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Get_DIKeyState(DIK_W) & 0x80)
		m_pTransform->Go_Straight(dTimeDelta);

	if (pGameInstance->Get_DIKeyState(DIK_S) & 0x80)
		m_pTransform->Go_Backward(dTimeDelta);

	if (pGameInstance->Get_DIKeyState(DIK_A) & 0x80)
		m_pTransform->Go_Left(dTimeDelta);

	if (pGameInstance->Get_DIKeyState(DIK_D) & 0x80)
		m_pTransform->Go_Right(dTimeDelta);
	
	if (pGameInstance->Get_DIKeyState(DIK_F7) & 0x80)
		Set_Fov(0.1f);

	if (pGameInstance->Get_DIKeyState(DIK_F8) & 0x80)
		Set_Fov(-0.1f);

	_long MouseMove = { 0 };

	if (MouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::DIMS_X))
	{
		//m_pTransform->Turn(m_pTransform->Get_State(CTransform::STATE_UP), (dTimeDelta * MouseMove * Get_Sensitivity()));
		m_pTransform->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), (dTimeDelta * MouseMove * Get_Sensitivity()));
	}

	if (MouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::DIMS_Y))
	{
		m_pTransform->Turn(m_pTransform->Get_State(CTransform::STATE_RIGHT), (dTimeDelta * MouseMove * Get_Sensitivity()));
	}


	Safe_Release(pGameInstance);

	__super::Tick(dTimeDelta);
}

void CCamera_Free::LateTick(_double dTimeDelta)
{
	__super::LateTick(dTimeDelta);
}

HRESULT CCamera_Free::Render()
{
	return S_OK;
}

CCamera_Free* CCamera_Free::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamera_Free* pInstance = new CCamera_Free(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCamera_Free");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCamera_Free::Clone(void* pArg)
{
	CCamera_Free* pInstance = new CCamera_Free(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CCamera_Free");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Free::Free()
{
	__super::Free();
}
