#pragma once
#include "Base.h"

BEGIN(Engine)

class CObject_Manager final : public CBase
{
	DECLARE_SINGLETON(CObject_Manager)
private:
	CObject_Manager();
	virtual ~CObject_Manager() = default;

public:
	class CComponent*			Get_Component(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pComponentTag);
	class CGameObject*			Get_GameObject(_uint iLevelIndex, const _tchar* pLayerTag, _uint iIndex);
	list<class CGameObject*>*	Get_GameObjects(_uint iLevelIndex, const _tchar* pLayerTag);

public:
	HRESULT		Reserve_Containers(_uint iNumLevels);
	HRESULT		Add_Prototype(const _tchar* pPrototypeTag, class CGameObject* pPrototype);
	HRESULT		Add_GameObject(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pPrototypeTag, void* pArg, _bool isKeep);
	HRESULT		Add_GameObject(_uint iLevelIndex, const _tchar* pLayerTag, class CGameObject* pGameObject, _bool isKeep);
	class CGameObject* Clone_GameObject(const _tchar* pPrototypeTag, void* pArg);
	void		Clear_Layer(_uint iLevelIndex, const _tchar* pLayerTag);

	void		Tick(_double dTimeDelta);
	void		LateTick(_double dTimeDelta);
	void		Clear(_uint iLevelIndex);

private: /*���� ��ü*/
	unordered_map<const _tchar*, class CGameObject*>	m_uPrototypes;

private: /*�纻 ��ü*/
	_uint		m_iNumLevels = { 0 }; /*���� �迭�� Size�� ����� _uint*/

	unordered_map<const _tchar*, class CLayer*>*		m_pLayers = { nullptr };
	typedef unordered_map<const _tchar*, class CLayer*>	LAYERS;

private:
	class CGameObject*	Find_Prototype(const _tchar* pPrototypeTag);
	class CLayer*		Find_Layer(_uint iLevelIndex, const _tchar* pLayerTag);

public:
	virtual void Free() override;
};

END