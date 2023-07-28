#pragma once
#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CUI abstract : public CGameObject
{
protected:
	CUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI(const CUI& rhs);
	virtual ~CUI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double dTimeDelta) override;
	virtual void	LateTick(_double dTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	//_float		m_fX, m_fY, m_fSizeX, m_fSizeY;
	//_float4x4	m_ProjMatrix;

protected:
	_bool	Pt_InUI();

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END