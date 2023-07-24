#include "pch.h"
#include "..\Public\ImGui_Animation_Tool.h"


IMPLEMENT_SINGLETON(CImGui_Animation_Tool)

CImGui_Animation_Tool::CImGui_Animation_Tool()
{

}

void CImGui_Animation_Tool::Control_Input(CGameInstance* pGameInstance, CAnimation::ANIMATIONDESC AnimationDesc)
{
    CAnimation::CONTROLDESC ControlDesc = m_pAnimation->Get_ControlDesc();

#pragma region Play(SpaceBar) 
    // 재생기능 spacebar
    if (pGameInstance->Get_DIKeyDown(DIK_SPACE))
    {
        if (ControlDesc.m_isPlay)        
            ControlDesc.m_isPlay = false;
        else
            ControlDesc.m_isPlay = true;
    }
#pragma endregion



    m_pAnimation->Set_ControlDesc(ControlDesc);
}

void CImGui_Animation_Tool::Animation_ImGui_Main()
{
    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    ImGui::Begin("AnimationTool");

    CAnimation::ANIMATIONDESC AnimationDesc = m_pAnimation->Get_AnimationDesc();

    Control_Input(pGameInstance, AnimationDesc);



#pragma region Index_List  

    ImGui::ListBox("listbox", &m_iAnimIndex, m_vecName_ForListBox.data(), (_int)(m_vecName_ForListBox.size()), 5);
   
#pragma endregion



#pragma region Slider  
    // Sliders ui
    static ImGuiSliderFlags AnimSliderflags = ImGuiSliderFlags_None;
 
    _float fCur_Time = (_float)AnimationDesc.m_dTimeAcc;
    _float fEnd_Time = (_float)AnimationDesc.m_dDuration;

    ImGui::Text("Current Time : %f", fCur_Time);
    ImGui::Text("End Time : %f", fEnd_Time);
    ImGui::SliderFloat("SliderFloat (0 -> 1)", &fCur_Time, 0.0f, fEnd_Time, "%.2f", AnimSliderflags);

#pragma endregion



    //값을 애니메이션컴포넌트에 보내주기
    AnimationDesc.m_dTimeAcc = fCur_Time;
    m_pAnimation->Set_AnimationDesc(AnimationDesc);


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
