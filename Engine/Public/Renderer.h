#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CRenderer final : public CComponent
{
public:
	enum RENDERGROUP { RENDER_PRIORITY, RENDER_NONBLEND, RENDER_NONLIGHT, RENDER_BLEND, RENDER_UI, RENDER_END };
private:
	CRenderer(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual ~CRenderer() = default;

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* pArg) override;

public:
	HRESULT Add_RenderGroup(RENDERGROUP eRenderGroup, class CGameObject* pGameObject);
	HRESULT Draw_RenderObjects();
	HRESULT	Add_RenderTarget(const _tchar* pMRT_Tag, const _tchar* pRenderTargetTag, enum DXGI_FORMAT eFormat, _float4 vColor);
	HRESULT Begin_MRT(const _tchar* pMRT_Tag);
	HRESULT End_MRT();
#ifdef _DEBUG
public:
	void OnOff_RenderTarget() { m_isRenderTarget = !m_isRenderTarget; }

public:
	HRESULT Add_DebugGroup(CComponent* pComponent);
#endif // _DEBUG


private:
	list<class CGameObject*>	m_RenderObjects[RENDER_END];
	class CTarget_Manager*		m_pTarget_Manager = { nullptr };
	class CLight_Manager*		m_pLight_Manager = { nullptr };

private:
	class CShader*			m_pShader = { nullptr };
	class CVIBuffer_Rect*	m_pVIBuffer = { nullptr };
	_float4x4				m_WorldMatrix;
	_float4x4				m_ViewMatrix;
	_float4x4				m_ProjMatrix;

#ifdef _DEBUG
private:
	list<CComponent*>		m_DebugRender;

	_bool					m_isRenderTarget = { false };
#endif // _DEBUG

private:
	HRESULT Render_Priority();
	HRESULT Render_NonBlend();
	HRESULT Render_NonLight();
	HRESULT Render_Blend();
	HRESULT Render_UI();

#ifdef _DEBUG
private:
	HRESULT Render_Debug();
#endif // _DEBUG

private:
	HRESULT Render_Lights();
	HRESULT Render_Deferred();

public:
	static CRenderer* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END