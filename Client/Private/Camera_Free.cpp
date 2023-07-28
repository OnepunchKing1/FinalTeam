#include "pch.h"
#include "..\Public\Camera_Free.h"

#include "GameInstance.h"

#include "Player.h"

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
	if (FAILED(__super::Initialize(pArg)))
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
	if (pGameInstance->Get_DIKeyDown(DIK_0))
	{
		if (false == m_bCamChange)
			m_bCamChange = true;
		else if (true == m_bCamChange)
			m_bCamChange = false;
	}
	if (pGameInstance->Get_DIKeyDown(DIK_9))
	{
		if (false == m_bLockMouse)
			m_bLockMouse = true;
		else if (true == m_bLockMouse)
			m_bLockMouse = false;
	}
	if (pGameInstance->Get_DIKeyDown(DIK_LCONTROL))
		Shake(1.f, 100);

	if(true == m_bLockMouse)
	LockMouse();

	if (false == m_bCamChange)
	{
		if (pGameInstance->Get_DIKeyState(DIK_W) & 0x80)
			m_pTransformCom->Go_Straight(dTimeDelta);

		if (pGameInstance->Get_DIKeyState(DIK_S) & 0x80)
			m_pTransformCom->Go_Backward(dTimeDelta);

		if (pGameInstance->Get_DIKeyState(DIK_A) & 0x80)
			m_pTransformCom->Go_Left(dTimeDelta);

		if (pGameInstance->Get_DIKeyState(DIK_D) & 0x80)
			m_pTransformCom->Go_Right(dTimeDelta);

		/*if (pGameInstance->Get_DIKeyState(DIK_F7) & 0x80)
			Set_Fov(0.1f);

		if (pGameInstance->Get_DIKeyState(DIK_F8) & 0x80)
			Set_Fov(-0.1f);*/

		// Camera_Shake
		

		_long MouseMove = { 0 };

		if (MouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::DIMS_X))
		{
			//m_pTransform->Turn(m_pTransform->Get_State(CTransform::STATE_UP), (dTimeDelta * MouseMove * Get_Sensitivity()));
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), (dTimeDelta * MouseMove * Get_Sensitivity()));
		}

		if (MouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::DIMS_Y))
		{
			m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), (dTimeDelta * MouseMove * Get_Sensitivity()));
		}
	}
	if (m_bCamChange == true)
	{
		TargetCamera(dTimeDelta);
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

void CCamera_Free::TargetCamera(_double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	CTransform* m_pTargetTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Transform")));

	_vector vTargetPos = m_pTargetTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vCamPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vCamLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	_vector vTargetLook = m_pTargetTransformCom->Get_State(CTransform::STATE_LOOK);

	_float fChaseDistance = XMVectorGetX(XMVector3Length(vTargetPos - vCamPosition));
	_double ChaseSpeed;

	vCamLook *= m_fDistance;
	_vector vTest = { 0.f, 1.f, 0.f, 0.f };
	vTargetPos += vTest;
	vTargetPos -= vCamLook;

	if (fChaseDistance > m_fDistance)
	{
		ChaseSpeed = fChaseDistance / m_fDistance;
	}
	else
		ChaseSpeed = 1.0;


	m_pTransformCom->Chase_Target(vTargetPos, dTimeDelta, ChaseSpeed);

	TurnMouse(dTimeDelta);
}

void CCamera_Free::TurnMouse(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_long		MouseMove = 0;

	if (MouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::DIMS_X))
	{

		_vector vCamPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		//플레이어의 위치로 이동

		vCamPos += XMVector3Normalize(vLook) * m_fDistance;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCamPos);


		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), TimeDelta * MouseMove * 0.2f);

		//갔던 만큼 다시 돌아옴
		vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

		vCamPos -= XMVector3Normalize(vLook) * m_fDistance;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCamPos);


	}

	if (MouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::DIMS_Y))
	{

		_vector vCamPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

		vCamPos += XMVector3Normalize(vLook) * m_fDistance;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCamPos);

		//플레이어의 위치에서 회전
		_double Angle = TimeDelta * MouseMove * 0.2f;
		m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), Angle);


		//갔던 만큼 다시 돌아옴
		vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		vCamPos -= XMVector3Normalize(vLook) * m_fDistance;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCamPos);
	}
	Safe_Release(pGameInstance);
}

void CCamera_Free::LockMouse()
{
	POINT		ptMouse{ g_iWinSizeX >> 1, g_iWinSizeY >> 1 };

	ClientToScreen(g_hWnd, &ptMouse);
	SetCursorPos(ptMouse.x, ptMouse.y);
	ShowCursor(false);
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
