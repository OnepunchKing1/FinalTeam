#pragma once

#include "Tool_Defines.h"
#include "Character_Tool.h"

BEGIN(Engine)

END

BEGIN(Tool)

class CAnimCharacter_Tool final : public CCharacter_Tool
{
private:
	CAnimCharacter_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAnimCharacter_Tool(const CAnimCharacter_Tool& rhs);
	virtual ~CAnimCharacter_Tool() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double dTimeDelta) override;
	virtual void	LateTick(_double dTimeDelta) override;
	virtual HRESULT Render() override;

private:
	// 임시 코드 
	_uint	m_iNumAnim = { 0 };


private: //ImGui 변수
	CImGui_Manager_Tool* m_pImGui = { nullptr };


private:
	HRESULT Add_Components();
	HRESULT	SetUp_ShaderResources();

public:
	static CAnimCharacter_Tool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END