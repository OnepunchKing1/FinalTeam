#pragma once

#include "Tool_Defines.h"
#include "Base.h"

#include "Camera.h"
#include "Animation.h"


#include "GameInstance.h"

BEGIN(Tool)

class CImGui_Animation_Tool final : public CBase
{
	DECLARE_SINGLETON(CImGui_Animation_Tool)
private:
	CImGui_Animation_Tool();
	virtual ~CImGui_Animation_Tool() = default;


public: // 함수
	void Animation_ImGui_Main();


public: // GetSet
	void Set_Animation(CAnimation* pAnim) { m_pAnimation = pAnim; }

	void Set_vecName(vector<char*> vecName);

	_int Get_AnimIndex() { return m_iAnimIndex; }
	void Set_AnimIndex(_int index) { m_iAnimIndex = index; }

private: 
	// 변수
	CAnimation*		m_pAnimation = { nullptr };

	vector<const char*> m_vecName_ForListBox;
	_int	m_iAnimIndex = { 0 };
	_int	m_iSave_AnimIndex = { 0 };

	//Slider 변수
	_double		m_dFixed_Time = { 0.0 };
	_double		m_dCur_Time = { 0.0 };
	_double		m_dEnd_Time = { 0.0 };

	// loop checkbox
	_bool	m_isLoop_Check = { false };
	

public:
	virtual void Free() override;
};

END