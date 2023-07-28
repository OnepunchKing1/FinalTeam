#include "..\Public\Model.h"

#include "Animation.h"
#include "Texture.h"
#include "Shader.h"
#include "Mesh.h"
#include "Bone.h"

CModel::CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CModel::CModel(const CModel& rhs)
	: CComponent(rhs)
	, m_iNumMeshes(rhs.m_iNumMeshes)
	, m_Meshes(rhs.m_Meshes)
	, m_iNumMaterials(rhs.m_iNumMaterials)
	, m_Materials(rhs.m_Materials)
	, m_PivotMatrix(rhs.m_PivotMatrix)
	, m_iCurrentAnimIndex(rhs.m_iCurrentAnimIndex)
	, m_iNumAnimations(rhs.m_iNumAnimations)
{
	for (auto& pPrototypeBone : rhs.m_Bones)
		m_Bones.emplace_back(pPrototypeBone->Clone());

	for (auto& pMesh : m_Meshes)
		Safe_AddRef(pMesh);

	for (auto& Material : m_Materials)
	{
		for (_uint i = 0; i < TEXTURE_TYPE_MAX; i++)
			Safe_AddRef(Material.pTexture[i]);
	}

	for (auto& pPrototypeAnim : rhs.m_Animations)
		m_Animations.emplace_back(pPrototypeAnim->Clone());
}

CBone* CModel::Get_Bone(const char* pBoneName)
{
	auto iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone* pBone)->_bool
		{
			if (false == strcmp(pBone->Get_Name(), pBoneName))
				return true;

			return false;
		}
	);

	if (iter == m_Bones.end())
		return nullptr;

	return *iter;
}

_int CModel::Get_BoneIndex(const char* pBoneName)
{
	_int iIndex = { -1 };

	_uint iNumBones = (_uint)m_Bones.size();

	for (_uint i = 0; i < iNumBones; i++)
	{
		if (false == strcmp(m_Bones[i]->Get_Name(), pBoneName))
		{
			iIndex = i;
			break;
		}
	}

	return iIndex;
}

HRESULT CModel::Initialize_Prototype(TYPE eModelType, const char* pModelFilePath, _fmatrix PivotMatrix)
{
	m_eModelType = eModelType;

	/*char szDrive[MAX_PATH] = { "" };
	char szDir[MAX_PATH] = { "" };
	char szFileName[MAX_PATH] = { "" };
	_splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDir, MAX_PATH, szFileName, MAX_PATH, nullptr, 0);
	
	char szFullPath[MAX_PATH] = { "" };

	strcpy_s(szFullPath, szDrive);
	strcat_s(szFullPath, szDir);
	strcat_s(szFullPath, szFileName);
	strcat_s(szFullPath, ".bin");*/
	
	ifstream fin;
	fin.open(pModelFilePath, ios::binary);

	if (false == fin.is_open())
	{
		MSG_BOX("Failed to ReadModelBinaryFile : CModel");
		return E_FAIL;
	}

	XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);

	/*if (FAILED(Ready_HierarchyBones(m_pAIScene->mRootNode, -1)))
	{
		MSG_BOX("Failed to Ready_HierarchyBones : CModel");
		return E_FAIL;
	}*/

	if (FAILED(Ready_HierarchyBones(&fin)))
	{
		MSG_BOX("Failed to Ready_HierarchyBones : CModel");
		return E_FAIL;
	}

	if (FAILED(Ready_Meshes(&fin)))
	{
		MSG_BOX("Failed to Ready_Meshes : CModel");
		return E_FAIL;
	}

	if (FAILED(Ready_Materials(pModelFilePath, &fin)))
	{
		MSG_BOX("Failed to Ready_Materials : CModel");
		return E_FAIL;
	}

	if (FAILED(Ready_Animations(&fin)))
	{
		MSG_BOX("Failed to Ready_Animations : CModel");
		return E_FAIL;
	}

	fin.close();

	return S_OK;
}

HRESULT CModel::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CModel::Play_Animation(_double dTimeDelta)
{
	//애니메이션이 바뀔 시에 기존 애니메이션의 timeacc초기화
	if (m_iSaveAnimIndex != m_iCurrentAnimIndex)
	{
		m_Animations[m_iSaveAnimIndex]->Reset_TimeAcc();
		m_iSaveAnimIndex = m_iCurrentAnimIndex;
		m_isCombo_Trigger = false;
	}

	//콤보애니메이션이 아닐시 combo_doing강제 초기화
	if (false == m_Animations[m_iCurrentAnimIndex]->Get_ControlDesc().m_isCombo)
	{
		m_isCombo_Doing = false;
	}


	_int	NextAnim = -1;
	// 선형보간 invalidate
	if (m_isLinearOn)
	{
		m_isLinearOn = m_Animations[m_iCurrentAnimIndex]->Invalidate_Linear_TransformationMatrices(this, dTimeDelta, m_isPlay, m_LastKeys);
	}
	// 일반 invalidate
	else
	{
		NextAnim = m_Animations[m_iCurrentAnimIndex]->Invalidate_TransformationMatrices(this, dTimeDelta, m_isPlay, m_isCombo_Trigger);
	}
	
	/* 현재 재생해야할 애니메이션 번호 n == m_iCurrentAnimIndex
	*  n번의 애니메이션의 행렬 상태로 Trans행렬을 갱신한다.
	*/

	//위에서 갱신한 Trans를 이용하여 모든 뼈의 Combined를 갱신한다
	for (auto& pBone : m_Bones)
	{
		pBone->Invalidate_CombinedTransformationMatrix(this);
	}

	
	// 애니메이션이 끝나서 다음 애니메이션 리턴받을 시
	if( 0 <= NextAnim)
	{
		//Combo애니메이션일 경우 여기로 진행
		if (m_isCombo_Trigger) 
		{
			// 트리거가 켜지면 여기에 함수

			//콤보 분기 일경우,
			if (m_isCombo_Another)
			{
				m_isCombo_Another = false;
				NextAnim = m_iCombo_AnotherRoute;
			}
		}
		//그냥 애니메이션 진행
		else
		{
			//현재 애님과 바뀐 애님이 같지 않을 경우 선형보간
			if (m_iCurrentAnimIndex != NextAnim)
			{
				m_isLinearOn = true;

				m_LastKeys.clear();
				m_LastKeys = m_Animations[m_iCurrentAnimIndex]->Get_LastKeys();

				m_isCombo_Doing = false;
			}

			//콤보 진행이 아닐시 트리거가 안들어오면,
			if (m_isCombo_Doing)
			{
				m_isCombo_Doing = false;
			}
		}

		m_iCurrentAnimIndex = NextAnim;
		m_Animations[m_iSaveAnimIndex]->Reset_TimeAcc(); // 기존 애니메이션 초기화

		m_isCombo_Trigger = false;
	}

	m_iSaveAnimIndex = m_iCurrentAnimIndex;

	return S_OK;
}

HRESULT CModel::Render(_uint iMeshIndex)
{
	m_Meshes[iMeshIndex]->Render();

	return S_OK;
}

HRESULT CModel::Bind_ShaderResource(_uint iMeshIndex, CShader* pShader, const char* pConstantName, MESHMATERIALS::TEXTURETYPE eType)
{
	return m_Materials[m_Meshes[iMeshIndex]->Get_MaterialIndex()].pTexture[eType]->Bind_ShaderResourceView(pShader, pConstantName);
	/* A메쉬가 그려지기 위해 필요한 B 머티리얼이 가진 텍스처들 중 C를 셰이더의 전역변수에 던져준다
	* A메쉬가 그려지기 위해 필요한 B 머티리얼의 인덱스 = m_Meshes[iMeshIndex]->Get_MaterialIndex()
	* B머티리얼이 가진 텍스처들 중 C = m_Materials[].pTexture[eType]
	* C를 셰이더의 전역변수에 던져준다 = ->Bind_ShaderResourceView(pShader, pConstantName)
	*/
}

HRESULT CModel::Bind_ShaderBoneMatrices(_uint iMeshIndex, CShader* pShader, const char* pConstantName)
{
	_float4x4	BoneMatrices[256];

	m_Meshes[iMeshIndex]->Get_BoneMatrices(BoneMatrices, XMLoadFloat4x4(&m_PivotMatrix), this);

	return pShader->SetUp_Matrix_Array(pConstantName, BoneMatrices, 256);
}

HRESULT CModel::Ready_Meshes(ifstream* pFin)
{
	if (nullptr == pFin)
		return E_FAIL;

	pFin->read(reinterpret_cast<char*>(&m_iNumMeshes), sizeof(_uint));

	for (_uint i = 0; i < m_iNumMeshes; i++)
	{
		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, m_eModelType, pFin, XMLoadFloat4x4(&m_PivotMatrix), this);
		if (nullptr == pMesh)
		{
			MSG_BOX("Failed to CreateMesh : CModel");
			return E_FAIL;
		}

		m_Meshes.emplace_back(pMesh);
	}

	return S_OK;
}

HRESULT CModel::Ready_Materials(const char* pModelFilePath, ifstream* pFin)
{
	pFin->read(reinterpret_cast<char*>(&m_iNumMaterials), sizeof(_uint));

	//읽은 파일(fbx)의 머티리얼 수 만큼 반복
	for (_uint i = 0; i < m_iNumMaterials; i++)
	{
		MESHMATERIALS MeshMaterial;
		ZeroMemory(&MeshMaterial, sizeof MeshMaterial);

		//머티리얼이 가질 수 있는 최대 텍스처의 갯수(18)만큼 반복
		for (_uint j = 0; j < TEXTURE_TYPE_MAX; j++)
		{
			_uint iSize = { 0 };
			pFin->read(reinterpret_cast<char*>(&iSize), sizeof(_uint));
			char szFilePath[MAX_PATH] = { "" };

			if (1 >= iSize)
				continue;
			else
			{
				pFin->read(szFilePath, iSize);
				strcat_s(szFilePath, "\0");
			}

			//가져온 텍스처의 경로에 잘못된 값이 있을 수 있으므로 파일 이름과 경로를 다시 잡아주는 작업

			char szDrive[MAX_PATH] = { "" };
			char szDir[MAX_PATH] = { "" };
			//경로는 모델 파일을 기준으로 가져온다 = 모델과 텍스처가 같은 경로에 있어야 한다
			_splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDir, MAX_PATH, nullptr, 0, nullptr, 0);

			char szFileName[MAX_PATH] = { "" };
			char szExt[MAX_PATH] = { "" };

			_splitpath_s(szFilePath, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

			char szFullPath[MAX_PATH] = { "" };

			strcpy_s(szFullPath, szDrive);
			strcat_s(szFullPath, szDir);
			strcat_s(szFullPath, szFileName);
			strcat_s(szFullPath, szExt);

			_tchar	szConvertFullPath[MAX_PATH] = TEXT("");

			MultiByteToWideChar(CP_ACP, 0, szFullPath, (int)strlen(szFullPath), szConvertFullPath, MAX_PATH);

			//만든 경로의 파일을 찾아서 텍스처를 만든다

			MeshMaterial.pTexture[j] = CTexture::Create(m_pDevice, m_pContext, szConvertFullPath);
			if (nullptr == MeshMaterial.pTexture[j])
				return E_FAIL;
		}

		m_Materials.emplace_back(MeshMaterial);
	}

	return S_OK;
}

HRESULT CModel::Ready_HierarchyBones(ifstream* pFin)
{
	_uint iNumBones = { 0 };
	pFin->read(reinterpret_cast<char*>(&iNumBones), sizeof(_uint));

	m_Bones.reserve(iNumBones);

	for (_uint i = 0; i < iNumBones; i++)
	{
		CBone* pBone = CBone::Create(pFin);
		if (nullptr == pBone)
			return E_FAIL;

		m_Bones.emplace_back(pBone);
	}

	return S_OK;
}

HRESULT CModel::Ready_Animations(ifstream* pFin)
{
	pFin->read(reinterpret_cast<char*>(&m_iNumAnimations), sizeof(_uint));

	for (_uint i = 0; i < m_iNumAnimations; i++)
	{
		CAnimation* pAnimation = CAnimation::Create(pFin, this);
		if (nullptr == pAnimation)
			return E_FAIL;

		m_Animations.emplace_back(pAnimation);
	}

	return S_OK;
}

CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eModelType, const char* pModelFilePath, _matrix PivotMatrix)
{
	CModel* pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eModelType, pModelFilePath, PivotMatrix)))
	{
		MSG_BOX("Failed to Created : CModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CModel::Clone(void* pArg)
{
	CModel* pInstance = new CModel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CModel::Free()
{
	__super::Free();

	for (auto& pAnimation : m_Animations)
		Safe_Release(pAnimation);

	m_Animations.clear();

	for (auto& pBone : m_Bones)
		Safe_Release(pBone);

	m_Bones.clear();


	for (auto& pMesh : m_Meshes)
		Safe_Release(pMesh);

	m_Meshes.clear();

	for (auto& Material : m_Materials)
	{
		for (_uint i = 0; i < TEXTURE_TYPE_MAX; i++)
			Safe_Release(Material.pTexture[i]);
	}

	m_Materials.clear();
}
