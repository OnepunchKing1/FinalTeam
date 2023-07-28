#pragma once

#include "Client_Defines.h"
#include "Player.h"

BEGIN(Engine)

END

BEGIN(Client)

class CPlayer_Tanjiro final : public CPlayer
{
public:
	enum ANIM {
		ANIM_ATK_COMBO = 21,
		ANIM_ATK_SKILL_GUARD = 34, ANIM_ATK_SKILL_MOVE = 38, ANIM_ATK_SKILL_NORMAL = 42,
		ANIM_BATTLE_IDLE = 82,
		ANIM_BATTLE_RUN = 87, ANIM_BATTLE_RUN_END = 89,
		ANIM_END = 116
	};

private:
	CPlayer_Tanjiro(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer_Tanjiro(const CPlayer_Tanjiro& rhs);
	virtual ~CPlayer_Tanjiro() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double dTimeDelta) override;
	virtual void	LateTick(_double dTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_ShadowDepth();


private: //애니메이션 제어용 함수들
	void	EventCall_Control(_double dTimeDelta);

	void	Animation_Control(_double dTimeDelta);
	void	Animation_Control_Battle_Move(_double dTimeDelta);
	void	Animation_Control_Battle_Attack(_double dTimeDelta);
	void	Animation_Control_Battle_Skill(_double dTimeDelta);



private: //애니메이션 제어용 변수들
	//Move
	_float	m_fMove_Speed = { 2.f };

private:
	/* 임시 코드 */
	_uint	m_iNumAnim = { 0 };
	// 렌더 확인용
	_uint	m_iMeshNum = { 0 };



private:
	// Outline Default
	_float	m_fOutlineThickness = 0.9f;
	// Outline Face
	_float	m_fOutlineFaceThickness = 0.3f;



private:
	HRESULT Add_Components();
	HRESULT	SetUp_ShaderResources();

public:
	static CPlayer_Tanjiro* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END