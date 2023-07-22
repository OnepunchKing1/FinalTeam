#pragma once

#include "Tool_Defines.h"
#include "Base.h"

BEGIN(Tool)

class CUI_Tool final : public CBase
{
	DECLARE_SINGLETON(CUI_Tool)
private:
	CUI_Tool();
	virtual ~CUI_Tool() = default;

public:
	void ImGui_Set();
	void ImGUI_ShowDemo();

#pragma region Place Objects
public:
	const _tchar* Get_SelectedLayer_ImGui();
	const _tchar* Get_SelectedObject_ImGui();

public:
	void	Add_LayerKeys(const char* pKeys) {
		m_Layers.emplace_back(pKeys);
	}
	void	Add_ObjectKeys(const char* pKeys) {
		m_Objects.emplace_back(pKeys);
	}

private:
	vector<const char*> m_Layers;
	_uint	m_iLayerIndex = { 0 };
	_tchar	m_szSelectedLayer[MAX_PATH] = { TEXT("") };

private:
	vector<const char*> m_Objects;
	_uint	m_iObjectIndex = { 0 };
	_tchar	m_szSelectedObject[MAX_PATH] = { TEXT("") };
#pragma endregion

#pragma region Camera Set
public:
	_double Get_CameraSpeed() { return (_double)m_fCameraSpeed; }
	void Set_CameraSpeed(_double dSpeed) { m_fCameraSpeed = (_float)dSpeed; }
private:
	_float m_fCameraSpeed;
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