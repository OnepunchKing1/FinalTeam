#include "pch.h"
#include "..\Public\TerrainMapObject.h"

#include "GameInstance.h"

CTerrainMapObject::CTerrainMapObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMapObject(pDevice, pContext)
{
}

CTerrainMapObject::CTerrainMapObject(const CTerrainMapObject& rhs)
	: CMapObject(rhs)
{
}

HRESULT CTerrainMapObject::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTerrainMapObject::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_MapObject_Info.iMapObjectType = MAPOBJECT_TERRAIN;

	return S_OK;
}

void CTerrainMapObject::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
}

void CTerrainMapObject::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);
}

HRESULT CTerrainMapObject::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

CTerrainMapObject* CTerrainMapObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTerrainMapObject* pInstance = new CTerrainMapObject(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTerrainMapObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTerrainMapObject::Clone(void* pArg)
{
	CTerrainMapObject* pInstance = new CTerrainMapObject(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTerrainMapObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTerrainMapObject::Free()
{
	__super::Free();
}
