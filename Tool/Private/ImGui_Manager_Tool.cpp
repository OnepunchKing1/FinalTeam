#include "pch.h"
#include "..\Public\ImGui_Manager_Tool.h"

#include "GameInstance.h"

IMPLEMENT_SINGLETON(CImGui_Manager_Tool)

CImGui_Manager_Tool::CImGui_Manager_Tool()
{

}

void CImGui_Manager_Tool::ImGui_Set()
{
    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    ImGui::Begin("Tool");
    
#pragma region Camera Set
    if (ImGui::TreeNode("Camera Set"))
    {
        _float fSpeed = (_float)m_CameraDesc.TransformDesc.dSpeedPerSec;
        ImGui::SliderFloat("Speed", &fSpeed, 0.f, 1000.0f, "%.1f");
        m_CameraDesc.TransformDesc.dSpeedPerSec = (_double)fSpeed;

        ImGui::SliderFloat("Far", &m_CameraDesc.fFarZ, 0.1f, 10000.0f, "%.1f");

        ImGui::SliderFloat("Fov", &m_CameraDesc.fFovY, 0.1f, 2.f, "%.1f");

        ImGui::TreePop();

    }
#pragma endregion

#pragma region Light Set
    if (ImGui::TreeNode("Light Set"))
    {
        ImGui::SeparatorText("Direction");


        if (ImGui::BeginTable("", 2))
        {
            ImGui::TableNextColumn();
            ImGui::SliderFloat3("Dir", (_float*)&m_vLightDesc[0], -1.f, 1.f);

            ImGui::TableNextColumn();
            if (ImGui::Button("SetDir"))
                pGameInstance->Set_Light(0, 0, m_vLightDesc[0]);

            ImGui::TableNextColumn();
            ImGui::SliderFloat3("Diffuse", (_float*)&m_vLightDesc[1], 0.f, 1.f);

            ImGui::TableNextColumn();
            if (ImGui::Button("SetDiffuse"))
                pGameInstance->Set_Light(0, 1, m_vLightDesc[1]);

            ImGui::TableNextColumn();
            ImGui::SliderFloat3("Ambient", (_float*)&m_vLightDesc[2], 0.f, 1.f);

            ImGui::TableNextColumn();
            if (ImGui::Button("SetAmbient"))
                pGameInstance->Set_Light(0, 2, m_vLightDesc[2]);

            ImGui::TableNextColumn();
            ImGui::SliderFloat3("Specular", (_float*)&m_vLightDesc[3], 0.f, 1.f);

            ImGui::TableNextColumn();
            if (ImGui::Button("SetSpecular"))
                pGameInstance->Set_Light(0, 3, m_vLightDesc[3]);

            ImGui::TableNextColumn();
            if (ImGui::Button("SetAll"))
                for (_uint i = 0; i < 4; i++)
                    pGameInstance->Set_Light(0, i, m_vLightDesc[i]);
            ImGui::EndTable();
        }

        ImGui::TreePop();
    }
#pragma endregion

    ImGui::End();

    Safe_Release(pGameInstance);
}

void CImGui_Manager_Tool::ImGUI_ShowDemo()
{
	_bool show_deme_window = true;

	if (show_deme_window)
		ImGui::ShowDemoWindow(&show_deme_window);
}

void CImGui_Manager_Tool::Animation_ImGui_Set()
{
    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);
    ImGui::Begin("AnimationTool");




    ImGui::End();
    Safe_Release(pGameInstance);
}

#pragma region Light Set
void CImGui_Manager_Tool::Set_DefaultLight_ImGui(LIGHTDESC LightDesc)
{
    m_vLightDesc[0] = LightDesc.vLightDir;
    m_vLightDesc[1] = LightDesc.vLightDiffuse;
    m_vLightDesc[2] = LightDesc.vLightAmbient;
    m_vLightDesc[3] = LightDesc.vLightSpecular;
}
#pragma endregion

HRESULT CImGui_Manager_Tool::Initialize_ImGui(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    ImGui_ImplWin32_Init(g_hWnd);
    ImGui_ImplDX11_Init(pDevice, pContext);

    return S_OK;
}

void CImGui_Manager_Tool::Tick_ImGui()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void CImGui_Manager_Tool::Render_ImGui()
{
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void CImGui_Manager_Tool::Release_ImGui()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}


void CImGui_Manager_Tool::Free()
{
}
