#pragma once
#include "Base.h"

BEGIN(Engine)

class CChannel final : public CBase
{
private:
	CChannel();
	virtual ~CChannel() = default;

public:
	HRESULT Initialize(ifstream* pFin, const char* pName, _uint iBoneIndex);
	void	Invalidate(class CModel* pModel, _uint& pCurrentKeyFrame, _double TrackPosition);

private:
	char			m_szName[MAX_PATH] = { "" };

	_uint			m_iBoneIndex = { 0 };

private:
	_uint				m_iNumKeyFrames = { 0 };
	vector<KEYFRAME>	m_KeyFrames;

	//犁积苞 开犁积 备喊
	_double  m_dSave_TrackPosition = { 0.0 };
	_bool	 m_isReverse = { false };

public:
	static CChannel* Create(ifstream* pFin, const char* pName, _uint iBoneIndex);
	virtual void Free() override;
};

END