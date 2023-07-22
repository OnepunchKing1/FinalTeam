#pragma once

#include "GameObject.h"
#include "Transform.h"

BEGIN(Engine)

class ENGINE_DLL CCamera abstract : public CGameObject
{
public:
	typedef struct tagCameraDesc{
		
		_float4		vEye;
		_float4		vAt;
		_float4		vAxisY;
		
		_float		fFovY, fAspect, fNearZ, fFarZ;

		_double		dSensitivity = { 0.5 };		//°¨µµ

		CTransform::TRANSFORMDESC TransformDesc;
	}CAMERADESC;
protected:
	CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera(const CCamera& rhs);
	virtual ~CCamera() = default;

public:
	_double Get_Sensitivity() { return m_CameraDesc.dSensitivity; }

	void	Set_Sensitivity(_double dSensitivity) { m_CameraDesc.dSensitivity += dSensitivity; }

	void	Set_Fov(_float fFovY) { m_CameraDesc.fFovY += fFovY; }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void	Tick(_double dTimeDelta);
	virtual void	LateTick(_double dTimeDelta);
	virtual			HRESULT Render();

protected:
	CTransform*			m_pTransform = { nullptr };
	CAMERADESC			m_CameraDesc;
	class CPipeLine*	m_pPipeLine = { nullptr };

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END	