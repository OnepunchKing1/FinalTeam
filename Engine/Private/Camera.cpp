#include "..\Public\Camera.h"
#include "PipeLine.h"

CCamera::CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
	, m_pPipeLine(CPipeLine::GetInstance())
{
	Safe_AddRef(m_pPipeLine);
}

CCamera::CCamera(const CCamera& rhs)
	: CGameObject(rhs)
	, m_pPipeLine(rhs.m_pPipeLine)
{
	Safe_AddRef(m_pPipeLine);
}

HRESULT CCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera::Initialize(void* pArg)
{
	m_pTransformCom = CTransform::Create(m_pDevice, m_pContext);

	if (nullptr == m_pTransformCom)
		return E_FAIL;
		
	memcpy(&m_CameraDesc, pArg, sizeof m_CameraDesc);

	if(FAILED(m_pTransformCom->Initialize(&m_CameraDesc.TransformDesc)))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_CameraDesc.vEye));

	m_pTransformCom->LookAt(XMLoadFloat4(&m_CameraDesc.vAt));

	return S_OK;
}

void CCamera::Tick(_double dTimeDelta)
{
	if (nullptr == m_pPipeLine)
		return;

	m_pPipeLine->Set_Transform(CPipeLine::D3DTS_VIEW, m_pTransformCom->Get_WorldFloat4x4_Inverse());

	_float4x4 ProjMatrix;

	XMStoreFloat4x4(&ProjMatrix, XMMatrixPerspectiveFovLH(m_CameraDesc.fFovY, m_CameraDesc.fAspect, m_CameraDesc.fNearZ, m_CameraDesc.fFarZ));

	m_pPipeLine->Set_Transform(CPipeLine::D3DTS_PROJ, ProjMatrix);
}

void CCamera::LateTick(_double dTimeDelta)
{
}

HRESULT CCamera::Render()
{
	return S_OK;
}

void CCamera::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pPipeLine);
}
