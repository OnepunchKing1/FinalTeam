#pragma once

#include "Client_Defines.h"
#include "Character.h"

BEGIN(Engine)

END

BEGIN(Client)

class CPlayer : public CCharacter
{
public:
	typedef struct tagPlayerMoveset
	{
		//입력 방향
		_float4	 m_Input_Dir = { 0.f, 0.f, 0.f, 0.f };

		//무빙 제한
		_bool	m_isRestrict_Move = { false };

		//달리기 키인풋
		_bool	m_State_Battle_Run = { false };
		_bool	m_Down_Battle_Run = { false };
		_bool	m_Up_Battle_Run = { false };

		//콤보공격 키인풋
		_bool	m_Down_Battle_Combo = { false };
		_bool	m_Down_Battle_Combo_Up = { false };
		_bool	m_Down_Battle_Combo_Down = { false };
		_bool	m_isPressing_While_Combo = { false };

		//스킬공격 키인풋
		_bool	m_Down_Skill_Normal = { false };
		_bool	m_Down_Skill_Move = { false };
		_bool	m_Down_Skill_Guard = { false };


	}PLAYERMOVESET;

protected:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double dTimeDelta) override;
	virtual void	LateTick(_double dTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_ShadowDepth();


protected: //애니메이션 제어용 함수들
	void	Key_Input(_double dTimeDelta);
	void	Key_Input_Battle_Move(_double dTimeDelta);
	void	Key_Input_Battle_Attack(_double dTimeDelta);
	void	Key_Input_Battle_Skill(_double dTimeDelta);

protected: // 애니메이션 제어용 변수들
	PLAYERMOVESET  m_Moveset;

	//쿨타임 적용
	_bool		m_isCool_MoveKey = { false };
	_double		m_dTime_MoveKey = { 0.0 };


protected:
	/* 임시 코드 */
	_uint	m_iNumAnim = { 0 };
	// 렌더 확인용
	_uint	m_iMeshNum = { 0 };



protected:
	// Outline Default
	_float	m_fOutlineThickness = 0.9f;
	// Outline Face
	_float	m_fOutlineFaceThickness = 0.3f;



protected:
	HRESULT Add_Components();
	HRESULT	SetUp_ShaderResources();

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END