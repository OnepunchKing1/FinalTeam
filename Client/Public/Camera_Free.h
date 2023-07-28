#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Engine)
END

BEGIN(Client)

class CCamera_Free final : public CCamera
{
private:
	CCamera_Free(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Free(const CCamera_Free& rhs);
	virtual ~CCamera_Free() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double dTimeDelta) override;
	virtual void	LateTick(_double dTimeDelta) override;
	virtual			HRESULT Render() override;

	/////////////////////////////////////////////// 임시 카메라 달아드렸습니다. /////////////////////////////////////////////////////////////////
private: 
	void TargetCamera(_double dTimeDelta);
	void TurnMouse(_double TimeDelta);
	void LockMouse();
private:
	_float			m_fDistance = { 2.f };
	_bool			m_bCamChange = { false };
	_bool			m_bLockMouse = { false };

public:
	static CCamera_Free* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END