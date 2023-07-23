#include "..\Public\Bone.h"
#include "Model.h"

CBone::CBone()
{
}

HRESULT CBone::Initialize(ifstream* pFin)
{
	_uint iSize = { 0 };
	pFin->read(reinterpret_cast<char*>(&iSize), sizeof(_uint));
	pFin->read(m_szName, iSize);
	strcat_s(m_szName, "\0");
	pFin->read(reinterpret_cast<char*>(&m_TransformationMatrix), sizeof(_float4x4));
	pFin->read(reinterpret_cast<char*>(&m_iParentIndex), sizeof(_int));
	XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_OffsetMatrix, XMMatrixIdentity());

	return S_OK;
}

void CBone::Invalidate_CombinedTransformationMatrix(CModel* pModel)
{
	if (-1 != m_iParentIndex)
	{
		CBone* pParent = pModel->Get_Bone(m_iParentIndex);
		if (nullptr == pParent)
			return;

		XMStoreFloat4x4(&m_CombinedTransformationMatrix,
			XMLoadFloat4x4(&m_TransformationMatrix) * XMLoadFloat4x4(&pParent->m_CombinedTransformationMatrix));
	}
	else
	{
		XMStoreFloat4x4(&m_CombinedTransformationMatrix,
			XMLoadFloat4x4(&m_TransformationMatrix));
	}
}

CBone* CBone::Create(ifstream* pFin)
{
	CBone* pInstance = new CBone();

	if (FAILED(pInstance->Initialize(pFin)))
	{
		MSG_BOX("Failed to Created : CBone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CBone* CBone::Clone()
{
	return new CBone(*this);
}

void CBone::Free()
{
}
