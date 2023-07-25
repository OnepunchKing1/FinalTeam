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
		_bool			m_isPlay = { true };
	}CONTROLDESC;

private:
	CAnimation();
	CAnimation(const CAnimation& rhs);
	virtual ~CAnimation() = default;

public:
	HRESULT Initialize(ifstream* pFin, class CModel* pModel);
	void	Invalidate_TransformationMatrices(class CModel* pModel, _double dTimeDelta);

public:
	//Get
	ANIMATIONDESC Get_AnimationDesc() { return m_AnimationDesc; }
	CONTROLDESC	  Get_ControlDesc() { return m_ControlDesc; }

	//Set
	void	Set_AnimationDesc(ANIMATIONDESC animdesc) { m_AnimationDesc = animdesc; }
	void	Set_ControlDesc(CONTROLDESC control) { m_ControlDesc = control; }

private:
	ANIMATIONDESC	m_AnimationDesc;
	CONTROLDESC		m_ControlDesc;
	
public:
	static CAnimation* Create(ifstream* pFin, class CModel* pModel);
	CAnimation* Clone();
	virtual void Free() override;
};

END