#include "..\Public\ColliderManager.h"

CColliderManager::CColliderManager()
{
}

HRESULT CColliderManager::Inititalize()
{
	return S_OK;
}

HRESULT CColliderManager::Check_Collider(_uint iLevelIndex, _double dTimeDelta)
{
	if (FAILED(Check_PlayerToMonster(iLevelIndex, dTimeDelta)))
	{
		MSG_BOX("Failed to Check_PlayerToMonster");
		return E_FAIL;
	}

	if (FAILED(Check_MonsterToMonster(iLevelIndex, dTimeDelta)))
	{
		MSG_BOX("Failed to Check_PlayerToMonster");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CColliderManager::Check_PlayerToMonster(_uint iLevelIndex, _double dTimeDelta)
{
	return S_OK;
}

HRESULT CColliderManager::Check_MonsterToMonster(_uint iLevelIndex, _double dTimeDelta)
{
	return S_OK;
}

void CColliderManager::Free()
{
}
