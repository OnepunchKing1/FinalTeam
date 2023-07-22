#include "..\Public\Level_Manager.h"

#include "GameInstance.h"
#include "Level.h"

IMPLEMENT_SINGLETON(CLevel_Manager)

CLevel_Manager::CLevel_Manager()
{
}

HRESULT CLevel_Manager::Open_Level(_uint iLevelIndex, CLevel* pNextLevel)
{
	if (nullptr == pNextLevel)
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	pGameInstance->Clear(m_iLevelIndex);

	Safe_Release(pGameInstance);

	Safe_Release(m_pCurrentLevel);
		
	m_pCurrentLevel = pNextLevel;

	m_iLevelIndex = iLevelIndex;

	return S_OK;
}

void CLevel_Manager::Tick_Level(_double dTimeDelta)
{
	if (nullptr == m_pCurrentLevel)
		return;
	
	m_pCurrentLevel->Tick(dTimeDelta);
}

void CLevel_Manager::Free()
{
	Safe_Release(m_pCurrentLevel);
}
