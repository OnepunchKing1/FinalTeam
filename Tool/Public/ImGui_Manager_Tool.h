#pragma once

#include "Tool_Defines.h"
#include "Base.h"

#include "Camera.h"

BEGIN(Tool)

class CImGui_Manager_Tool final : public CBase
{
	DECLARE_SINGLETON(CImGui_Manager_Tool)
private:
	CImGui_Manager_Tool();
	virtual ~CImGui_Manager_Tool() = default;

public:
	void ImGui_Set();
	void ImGUI_ShowDemo();


#pragma region Animation Set

public:
	void Animation_ImGui_Set();
private:
	

#pragma endregion




#pragma region Camera Set
public:
	CCamera::CAMERADESC Get_CameraDesc() { return m_CameraDesc; }
	void Set_CameraDesc(void* pArg) { memcpy(&m_CameraDesc, pArg, sizeof m_CameraDesc); }
private:
	CCamera::CAMERADESC		m_CameraDesc;
#pragma endregion

#pragma region Light Set

public:
	void Set_DefaultLight_ImGui(LIGHTDESC LightDesc);

private:
	_float4 m_vLightDesc[4];

#pragma endregion
	
#pragma region toCopy

#pragma endregion

public:
	HRESULT Initialize_ImGui(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Tick_ImGui();
	void Render_ImGui();
	void Release_ImGui();

public:
	virtual void Free() override;
};

END