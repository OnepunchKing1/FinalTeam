#include "pch.h"
#include "..\Public\Loader_Tool.h"

#include "GameInstance.h"
#include "Camera_Tool.h"
#include "Player_Tool.h"
#include "Terrain_Tool.h"

//AnimTool¿ë
#include "AnimCharacter_Tool.h"

CLoader_Tool::CLoader_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

unsigned int APIENTRY Loading_Main(void* pArg)
{
	CLoader_Tool* pLoader = (CLoader_Tool*)pArg;

	CoInitializeEx(nullptr, 0);

	EnterCriticalSection(pLoader->Get_CS());

	HRESULT hr = 0;
	
	switch (pLoader->Get_LevelID())
	{
	case LEVEL_TOOL:
		hr = pLoader->LoadingForTool();
		break;
	}

	if (FAILED(hr))
		return 1;

	LeaveCriticalSection(pLoader->Get_CS());

	return 0;
}

HRESULT CLoader_Tool::Initialize(LEVELID eLevelID)
{
	m_eLevelID = eLevelID;

	InitializeCriticalSection(&m_CS);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, Loading_Main, this, 0, nullptr);
	if(0 == m_hThread)
	{
		MSG_BOX("Failed to BeginThread");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLoader_Tool::LoadingForTool()
{
	SetWindowText(g_hWnd, TEXT("LoadingForTool"));

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

#pragma region COMPONENTS

	SetWindowText(g_hWnd, TEXT("Loading Texture..."));
#pragma region Texture

#pragma region EnvironmentTexture
	/* Prototype_Component_Texture_Mask */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Mask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Mask%d.png")))))
	{
		MSG_BOX("Failed to Add_Prototype_Component_Texture_Mask");
		return E_FAIL;
	}

	/* Prototype_Component_Texture_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Ground%d_bc.dds"), 2))))
	{
		MSG_BOX("Failed to Add_Prototype_Component_Texture_Terrain");
		return E_FAIL;
	}
#pragma endregion

#pragma region RampTexture

#pragma endregion

#pragma region EffectTexture

#pragma endregion

#pragma region UITexture

#pragma endregion


#pragma endregion

	SetWindowText(g_hWnd, TEXT("Loading Model..."));
#pragma region Model
	
#pragma region Buffer
	/* Prototype_Component_VIBuffer_Terrain */	
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Heightmap.bmp")))))
	{
		MSG_BOX("Failed to Add_Prototype_Component_VIBuffer_Terrain");
		return E_FAIL;
	}
#pragma endregion
	_matrix		PivotMatrix = XMMatrixIdentity();

#pragma region Effect

#pragma endregion

#pragma region Character
	/* Prototype_Component_Model_Tanjiro */
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Tanjiro"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Tanjiro/Tanjiro.bin", PivotMatrix))))
	{
		MSG_BOX("Failed to Add_Prototype_Model_Tanjiro");
		return E_FAIL;
	}



#pragma endregion

#pragma region NonCharacter
	/* Prototype_Component_Model_TestBox */
	PivotMatrix = XMMatrixTranslation(-21.9f, 215.2f, -158.6f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_TestBox"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/TestBox/mdlChest2.bin", PivotMatrix))))
	{
		MSG_BOX("Failed to Add_Prototype_Model_TestBox");
		return E_FAIL;
	}
#pragma endregion

#pragma region Terrain
	/* Prototype_Component_Model_Terrain */
	PivotMatrix = XMMatrixIdentity();
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Terrain"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environments/Terrain/Terrain.bin", PivotMatrix))))
	{
		MSG_BOX("Failed to Add_Prototype_Model_Terrain");
		return E_FAIL;
	}

	/* Prototype_Component_Navigation */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/Data/Navigation.dat")))))
	{
		MSG_BOX("Failed to Add_Prototype_Component_Navigation");
		return E_FAIL;
	}

#pragma endregion

#pragma endregion

	SetWindowText(g_hWnd, TEXT("Loading Shader..."));
#pragma region Shader
	/* Prototype_Component_Shader_VtxNorTex */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX_DECL::Elements, VTXNORTEX_DECL::iNumElements))))
	{
		MSG_BOX("Failed to Add_Prototype_Component_Shader_VtxNorTex");
		return E_FAIL;
	}

	/* Prototype_Component_Shader_VtxModel */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Shader_VtxModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxModel.hlsl"), VTXMODEL_DECL::Elements, VTXMODEL_DECL::iNumElements))))
	{
		MSG_BOX("Failed to Add_Prototype_Component_Shader_VtxModel");
		return E_FAIL;
	}

	/* Prototype_Component_Shader_VtxAnimModel */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimModel.hlsl"), VTXANIMMODEL_DECL::Elements, VTXANIMMODEL_DECL::iNumElements))))
	{
		MSG_BOX("Failed to Add_Prototype_Component_Shader_VtxAnimModel");
		return E_FAIL;
	}
#pragma endregion

	SetWindowText(g_hWnd, TEXT("Loading ETC..."));
#pragma region Etc
	/* Prototype_Component_Collider_AABB */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Collider_AABB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB))))
	{
		MSG_BOX("Failed to Add_Prototype_Component_Collider_AABB");
		return E_FAIL;
	}

	/* Prototype_Component_Collider_OBB */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Collider_OBB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_OBB))))
	{
		MSG_BOX("Failed to Add_Prototype_Component_Collider_OBB");
		return E_FAIL;
	}

	/* Prototype_Component_Collider_Sphere */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Collider_Sphere"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_SPHERE))))
	{
		MSG_BOX("Failed to Add_Prototype_Component_Collider_Sphere");
		return E_FAIL;
	}
#pragma endregion


#pragma endregion

#pragma region GAMEOBJECTS

	SetWindowText(g_hWnd, TEXT("Loading GameObject..."));

	CImGui_Manager_Tool* pUI = CImGui_Manager_Tool::GetInstance();
	Safe_AddRef(pUI);

#pragma region Object
	/* Prototype_GameObject_Camera_Tool */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Tool"),
		CCamera_Tool::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed to Add_Prototype_GameObject_Camera_Tool");
		return E_FAIL;
	}

	/* Prototype_GameObject_Player_Tool */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player_Tool"),
		CPlayer_Tool::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed to Add_Prototype_GameObject_Player_Tool");
		return E_FAIL;
	}

	//AnimTool¿ë
	/* Prototype_GameObject_AnimCharacter_Tool */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_AnimCharacter_Tool"),
		CAnimCharacter_Tool::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed to Add_Prototype_GameObject_AnimCharacter_Tool");
		return E_FAIL;
	}
#pragma endregion

#pragma region Environment

	/* Prototype_GameObject_Terrain_Tool */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain_Tool"),
		CTerrain_Tool::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed to Add_Prototype_GameObject_Terrain_Tool");
		return E_FAIL;
	}
#pragma endregion

#pragma region UI

#pragma endregion

#pragma region Particale

#pragma endregion

#pragma region Effect

#pragma endregion


	Safe_Release(pUI);
#pragma endregion

	Safe_Release(pGameInstance);

	SetWindowText(g_hWnd, TEXT("Loading Finished!!"));
	m_isFinished = true;

	return S_OK;
}

CLoader_Tool* CLoader_Tool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVELID eLevelID)
{
	CLoader_Tool* pInstance = new CLoader_Tool(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eLevelID)))
	{
		MSG_BOX("Failed to Create : CLoader_Tool");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoader_Tool::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);

	DeleteCriticalSection(&m_CS);

	CloseHandle(m_hThread);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
