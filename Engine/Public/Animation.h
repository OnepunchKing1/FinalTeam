#pragma once
#include "Base.h"

BEGIN(Engine)

class CAnimation final : public CBase
{
public:
	typedef struct tagAnimation
	{
		char						m_szName[MAX_PATH] = { "" };
		_double						m_dDuration = { 0.0 };
		_double						m_dTickPerSecond = { 0.0 };
		_double						m_dTimeAcc = { 0.0 };
		_bool						m_isFinish = { false };

		_uint						m_iNumChannels = { 0 };

		vector<class CChannel*>		m_Channels;

		vector<_uint>				m_iCurrentKeyFrames;

	}ANIMATIONDESC;

	typedef struct tagAnimation_Control
	{
		_float			m_fAnimationSpeed = { 1.0f };

		_int			m_iConnect_Anim = { 0 };

		_bool			m_isCombo = { false };
		_int			m_iConnect_ComboAnim = { 0 };
	}CONTROLDESC;

private:
	CAnimation();
	CAnimation(const CAnimation& rhs);
	virtual ~CAnimation() = default;

public:
	HRESULT Initialize(ifstream* pFin, class CModel* pModel);
	_int	Invalidate_TransformationMatrices(class CModel* pModel, _double dTimeDelta, _bool Play );
	_bool	Invalidate_Linear_TransformationMatrices(class CModel* pModel, _double dTimeDelta, _bool Play);

public:
	//Get
	ANIMATIONDESC Get_AnimationDesc() { return m_AnimationDesc; }
	CONTROLDESC	  Get_ControlDesc() { return m_ControlDesc; }

	class CChannel* Get_Channel(const char* pChannelName);

	_float3		Get_RootPosition() { return m_RootPosition; }
	

	//Set
	void	Set_AnimationDesc(ANIMATIONDESC animdesc) { m_AnimationDesc = animdesc; }
	void	Reset_TimeAcc() { m_AnimationDesc.m_dTimeAcc = 0.0; }

	void	Set_ControlDesc(CONTROLDESC control) { m_ControlDesc = control; }



private:
	ANIMATIONDESC	m_AnimationDesc;
	CONTROLDESC		m_ControlDesc;
	

private:
	_float3		m_RootPosition;
	

public:
	static CAnimation* Create(ifstream* pFin, class CModel* pModel);
	CAnimation* Clone();
	virtual void Free() override;
};

END