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
		//�Է� ����
		_float4	 m_Input_Dir = { 0.f, 0.f, 0.f, 0.f };

		//���� ���� �ľ�  0:�븻, 1:����, 2:��������
		_int	m_iAwaken = { 0 };
		_double		m_dTime_Awaken_Duration = { 10.0 };

		// �ǰ�ó��
		_bool	m_Down_Dmg_Small = { false };
		_bool	m_Down_Dmg_Blow = { false };
		_bool	m_Down_Dmg_Spin = { false };
		_bool	m_Down_Dmg_Bound = { false };
		_bool	m_Down_Dmg_Great_Blow = { false };

		// ����
		_bool	m_isRestrict_KeyInput = { false };
		_bool	m_isRestrict_Move = { false };
		_bool	m_isRestrict_Jump = { false };
		_bool	m_isRestrict_JumpCombo = { false };
		_bool	m_isRestrict_Throw = { false };
		_bool	m_isRestrict_Charge = { false };
		_bool	m_isRestrict_Dash = { false };
		_bool	m_isRestrict_Step = { false };
		_bool	m_isRestrict_DoubleStep = { false };
		_bool	m_isRestrict_Special = { false };

		//��Ʈ��� ����
		_bool	m_isHitMotion = { false };


		//�޸��� Ű��ǲ
		_bool	m_State_Battle_Run = { false };
		_bool	m_Down_Battle_Run = { false };
		_bool	m_Up_Battle_Run = { false };

		//���� Ű��ǲ
		_bool	m_Down_Battle_Jump = { false };
		_bool	m_Down_Battle_JumpMove = { false };

		//���� ���� Ű��ǲ
		_bool	m_Down_Battle_Jump_Attack = { false };
		_bool	m_Down_Battle_Jump_TrackAttack = { false };

		//�޺����� Ű��ǲ
		_bool	m_Down_Battle_Combo = { false };
		_bool	m_Down_Battle_Combo_Up = { false };
		_bool	m_Down_Battle_Combo_Down = { false };
		_bool	m_isPressing_While_Restrict = { false };

		//���� ���� Ű��ǲ
		_bool	m_Down_Battle_Charge = { false };
		_bool	m_State_Battle_Charge = { false };

		//��ų���� Ű��ǲ
		_bool	m_Down_Skill_Normal = { false };
		_bool	m_Down_Skill_Move = { false };
		_bool	m_Down_Skill_Guard = { false };

		//���� Ű��ǲ
		_bool	m_Down_Battle_Guard = { false };
		_bool	m_State_Battle_Guard = { false };
		_bool	m_Up_Battle_Guard = { false };

		//������ Ű��ǲ
		_bool	m_Down_Battle_Throw = { false };
		//��ġ�� Ű��ǲ
		_bool	m_Down_Battle_Push = { false };


		// ��� Ű��ǲ
		_bool	m_Down_Battle_Dash = { false };

		// ���� Ű��ǲ
		_bool	m_Down_Battle_Step = { false };

		// ���� Ű��ǲ
		_bool	m_Down_Battle_Awaken = { false };

		//����� ���� Ű��ǲ 
		_bool	m_Down_Battle_Special = { false };
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


protected: //�ִϸ��̼� ����� �Լ���
	void	Dir_Setting(_bool Reverse);

	void	Trigger_Hit(_double dTimeDelta);
	void	Key_Input(_double dTimeDelta);
	void	Key_Input_Battle_Move(_double dTimeDelta);
	void	Key_Input_Battle_Jump(_double dTimeDelta);
	void	Key_Input_Battle_Attack(_double dTimeDelta);
	void	Key_Input_Battle_ChargeAttack(_double dTimeDelta);
	void	Key_Input_Battle_Skill(_double dTimeDelta);
	void	Key_Input_Battle_Guard(_double dTimeDelta);
	void	Key_Input_Battle_Dash(_double dTimeDelta);
	void	Key_Input_Battle_Awaken(_double dTimeDelta);
	void	Key_Input_Battle_Special(_double dTimeDelta);


protected: // �ִϸ��̼� ����� ������
	PLAYERMOVESET  m_Moveset;

	_bool	m_isSpecialHit = { false };

	//��Ÿ�� ����
	_bool		m_isCool_MoveKey = { false };
	_double		m_dTime_MoveKey = { 0.0 };


	//�޺� ����
	_bool	m_isComboing = { false };

	// ��� ��
	_bool		m_isThrowing = { false };
	_bool		m_isMaintain_Guard = { false };

	//charge ������
	_double		m_dDelay_Charge_J = { 0.0 };
	_double		m_dDelay_Charge_W = { 0.0 };
	_bool		m_isCan_Charge = { false };
	_bool		m_isCharging = { false };

	//Step ��
	_float4		m_vLook = { 0.0f, 0.0f, 0.0f, 0.0f };
	_bool	m_isForward = { false };
	_bool	m_isBack = { false };
	_bool	m_isLeft = { false };
	_bool	m_isRight = { false };

protected:
	/* �ӽ� �ڵ� */
	_uint	m_iNumAnim = { 0 };
	// ���� Ȯ�ο�
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