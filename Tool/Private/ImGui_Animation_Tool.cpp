#include "pch.h"
#include "..\Public\ImGui_Animation_Tool.h"


IMPLEMENT_SINGLETON(CImGui_Animation_Tool)

CImGui_Animation_Tool::CImGui_Animation_Tool()
{

}


void CImGui_Animation_Tool::Animation_ImGui_Main()
{
    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    ImGui::Begin("AnimationTool");

    CAnimation::ANIMATIONDESC AnimationDesc = m_pAnimation->Get_AnimationDesc();
    CAnimation::CONTROLDESC ControlDesc = m_pAnimation->Get_ControlDesc();

#pragma region Play(SpaceBar) 
    // 재생기능 spacebar
    if (pGameInstance->Get_DIKeyDown(DIK_SPACE))
    {
        if (AnimationDesc.m_dDuration == AnimationDesc.m_dTimeAcc)
        {
            AnimationDesc.m_dTimeAcc = 0.0;
            m_pAnimation->Set_AnimationDesc(AnimationDesc);
            ControlDesc.m_isPlay = true;
        }
        else
        {
            if (ControlDesc.m_isPlay)
                ControlDesc.m_isPlay = false;
            else
                ControlDesc.m_isPlay = true;
        }
        m_pAnimation->Set_ControlDesc(ControlDesc);
    }
   
#pragma endregion



#pragma region Index_List  

    ImGui::ListBox("Animation_List", &m_iAnimIndex, m_vecName_ForListBox.data(), (_int)(m_vecName_ForListBox.size()), 5);

    ImGui::Text("Animation_Index : %d", m_iAnimIndex);

    // 애니메이션 변경시
    if (m_iSave_AnimIndex != m_iAnimIndex)
    {
        AnimationDesc.m_dTimeAcc = 0.0;
        ControlDesc.m_isPlay = false;
    }
    m_iSave_AnimIndex = m_iAnimIndex;

#pragma endregion


    ImGui::Text("");  


#pragma region Play_Slider  
    // Sliders ui
    static ImGuiSliderFlags AnimSliderflags = ImGuiSliderFlags_None;
 
    _float fCur_Time = (_float)AnimationDesc.m_dTimeAcc;
    _float fEnd_Time = (_float)AnimationDesc.m_dDuration;

    ImGui::Text("Current Time : %f", fCur_Time);
    ImGui::Text("End Time : %f", fEnd_Time);
    ImGui::SliderFloat("Play_Slider", &fCur_Time, 0.0f, fEnd_Time, "%.2f", AnimSliderflags);

    //슬라이더 값을 애니메이션컴포넌트에 보내주기
    AnimationDesc.m_dTimeAcc = fCur_Time;
    m_pAnimation->Set_AnimationDesc(AnimationDesc);

#pragma endregion


    ImGui::Text("");


#pragma region AnimaSpeed_Slider  

   _float fSpeed = ControlDesc.m_fAnimationSpeed;

    ImGui::Text("Speed : %f", fSpeed);
    ImGui::SliderFloat("FrameSpeed ", &fSpeed, 0.1f, 2.0f, "%.2f");

    ControlDesc.m_fAnimationSpeed = fSpeed;
    m_pAnimation->Set_ControlDesc(ControlDesc);
    
#pragma endregion


    ImGui::Text("");


#pragma region Loop  
    // loop, nonLoop check
    m_isLoop_Check = ControlDesc.m_isLoop;
    ImGui::Checkbox("Loop_Animation", &m_isLoop_Check);
    ControlDesc.m_isLoop = m_isLoop_Check;
    m_pAnimation->Set_ControlDesc(ControlDesc);

#pragma endregion




#pragma region Connect_Animation
    // Connect
    

#pragma endregion



    ImGui::End();
    Safe_Release(pGameInstance);
}

void CImGui_Animation_Tool::Set_vecName(vector<char*> vecName)
{
    size_t iNameSize = vecName.size();
    m_vecName_ForListBox.reserve(iNameSize); // Reserve memory to avoid reallocation
    for (size_t i = 0; i < iNameSize; i++)
        m_vecName_ForListBox.push_back(vecName[i]);
}

void CImGui_Animation_Tool::Free()
{
}
