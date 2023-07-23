#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
public:
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END };
private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	_uint Get_NumMeshes() const {
		return m_iNumMeshes;
	}
	
	_uint Get_NumAnims() const {
		return m_iNumAnimations;
	}

	class CMesh* Get_Mesh(_uint iIndex) {
		return m_Meshes[iIndex];
	}

	class CBone* Get_Bone(const char* pBoneName);
	class CBone* Get_Bone(_uint iIndex) {
		return m_Bones[iIndex];
	}

	_int Get_BoneIndex(const char* pBoneName);

public:
	void Set_Animation(_uint iAnimIndex) {
		m_iCurrentAnimIndex = iAnimIndex;
	}

public:
	HRESULT Initialize_Prototype(TYPE eModelType, const char* pModelFilePath, _fmatrix PivotMatrix);
	HRESULT Initialize(void* pArg) override;

public:
	HRESULT Play_Animation(_double dTimeDelta);
	HRESULT Render(_uint iMeshIndex);
	HRESULT Bind_ShaderResource(_uint iMeshIndex, class CShader* pShader, const char* pConstantName, MESHMATERIALS::TEXTURETYPE eType);
	HRESULT Bind_ShaderBoneMatrices(_uint iMeshIndex, class CShader* pShader, const char* pConstantName);

private:
	_float4x4					m_PivotMatrix;
	TYPE						m_eModelType = { TYPE_END };

private:
	_uint						m_iNumMeshes = { 0 };
	vector<class CMesh*>		m_Meshes;

private:
	_uint						m_iNumMaterials = { 0 };
	vector<MESHMATERIALS>		m_Materials;

private:
	vector<class CBone*>		m_Bones;

private:
	_uint						m_iCurrentAnimIndex = { 0 };
	_uint						m_iNumAnimations = { 0 };
	vector<class CAnimation*>	m_Animations;

private:
	HRESULT Ready_Meshes(ifstream* pFin);
	HRESULT Ready_Materials(const char* pModelFilePath, ifstream* pFin);
	HRESULT Ready_HierarchyBones(ifstream* pFin);
	HRESULT Ready_Animations(ifstream* pFin);
	
public:
	static CModel* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, TYPE eModelType, const char* pModelFilePath, _matrix PivotMatrix);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END