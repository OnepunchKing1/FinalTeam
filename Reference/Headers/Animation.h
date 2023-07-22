#pragma once
#include "Base.h"

BEGIN(Engine)

class CAnimation final : public CBase
{
private:
	CAnimation();
	CAnimation(const CAnimation& rhs);
	virtual ~CAnimation() = default;

public:
	HRESULT Initialize(const aiAnimation* pAIAnimation, class CModel* pModel);
	void	Invalidate_TransformationMatrices(class CModel* pModel, _double dTimeDelta);

private:
	char						m_szName[MAX_PATH] = { "" };
	_double						m_dDuration = { 0.0 };
	_double						m_dTickPerSecond = { 0.0 };
	_double						m_dTimeAcc = { 0.0 };
	_bool						m_isFinish = { false };

	_uint						m_iNumChannels = { 0 };

	vector<class CChannel*>		m_Channels;

	vector<_uint>				m_iCurrentKeyFrames;

public:
	static CAnimation* Create(const aiAnimation* pAIAnimation, class CModel* pModel);
	CAnimation* Clone();
	virtual void Free() override;
};

END