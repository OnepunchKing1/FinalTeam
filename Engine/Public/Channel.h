#pragma once
#include "Base.h"

BEGIN(Engine)

class CChannel final : public CBase
{
private:
	CChannel();
	virtual ~CChannel() = default;

public:
	HRESULT Initialize(const aiNodeAnim* pAIChannel, _uint iBoneIndex);
	void	Invalidate(class CModel* pModel, _uint& pCurrentKeyFrame, _double TrackPosition);

private:
	char			m_szName[MAX_PATH] = { "" };

	_uint			m_iBoneIndex = { 0 };

private:
	_uint				m_iNumKeyFrames = { 0 };
	vector<KEYFRAME>	m_KeyFrames;

public:
	static CChannel* Create(const aiNodeAnim* pAIChannel, _uint iBoneIndex);
	virtual void Free() override;
};

END