#include "..\Public\Mesh.h"

#include "Model.h"
#include "Bone.h"

CMesh::CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CMesh::CMesh(const CMesh& rhs)
	: CVIBuffer(rhs)
	, m_iMaterialIndex(rhs.m_iMaterialIndex)
	, m_iNumBones(rhs.m_iNumBones)
	, m_Bones(rhs.m_Bones)
{
	strcpy_s(m_szName, rhs.m_szName);
}

void CMesh::Get_BoneMatrices(_float4x4* pBoneMatrices, _fmatrix PivotMatrix, CModel* pModel)
{
	for (_uint i = 0; i < m_iNumBones; i++)
	{
		CBone* pBone = pModel->Get_Bone(m_Bones[i]);

		XMStoreFloat4x4(&pBoneMatrices[i],
			pBone->Get_OffsetMatrix() *
			pBone->Get_CombinedTransformationMatrix() * PivotMatrix);
	}
}

HRESULT CMesh::Initialize_Prototype(CModel::TYPE eModelType, const aiMesh* pAIMesh, _fmatrix PivotMatrix, CModel* pModel)
{
	strcpy_s(m_szName, pAIMesh->mName.data);
	m_iMaterialIndex = pAIMesh->mMaterialIndex;

	m_iNumVertexBuffers = 1;
	m_iNumVertices = pAIMesh->mNumVertices;
	m_iNumIndices = pAIMesh->mNumFaces * 3;
	m_iIndexStride = sizeof(_ulong);
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_iStride = CModel::TYPE_NONANIM == eModelType ? sizeof(VTXMODEL) : sizeof(VTXANIMMODEL);

	m_VertexPos.reserve(m_iNumVertices);
	m_Faces.reserve(pAIMesh->mNumFaces);

#pragma region VERTEXBUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStride;

	HRESULT hr = { 0 };

	if (CModel::TYPE_NONANIM == eModelType)
	{
		hr = Ready_VertexBuffer_NonAnim(pAIMesh, PivotMatrix);
	}
	else
	{
		hr = Ready_VertexBuffer_Anim(pAIMesh, pModel);
	}

	if (FAILED(hr))
		return E_FAIL;
	
#pragma endregion


#pragma region INDEXBUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	_ulong* pIndices = new _ulong[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ulong) * m_iNumIndices);

	_uint iNumIndices = { 0 };

	_uint iNumFaces = pAIMesh->mNumFaces;

	for (_uint i = 0; i < iNumFaces; i++)
	{
		aiFace AIFace = pAIMesh->mFaces[i];

		pIndices[iNumIndices] = AIFace.mIndices[0];
		pIndices[iNumIndices + 1] = AIFace.mIndices[1];
		pIndices[iNumIndices + 2] = AIFace.mIndices[2];

		iNumIndices += 3;

		m_Faces.emplace_back(_uint3(AIFace.mIndices[0], AIFace.mIndices[1] ,AIFace.mIndices[2]));
	}

	ZeroMemory(&m_SubresourceData, sizeof m_SubresourceData);
	m_SubresourceData.pSysMem = pIndices;

	if (FAILED(m_pDevice->CreateBuffer(&m_BufferDesc, &m_SubresourceData, &m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);
#pragma endregion

	return S_OK;
}

HRESULT CMesh::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CMesh::Ready_VertexBuffer_NonAnim(const aiMesh* pAIMesh, _fmatrix PivotMatrix)
{
	VTXMODEL* pVertices = new VTXMODEL[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXMODEL) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), PivotMatrix));
		
		m_VertexPos.emplace_back(_float4(pVertices[i].vPosition.x, pVertices[i].vPosition.y, pVertices[i].vPosition.z, 1.f));

		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vNormal, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vNormal), PivotMatrix));
		
		if (nullptr == pAIMesh->mTextureCoords[0])
		{
			if (nullptr != pAIMesh->mTangents)
				pVertices[i].vTangent = { pAIMesh->mTangents[i].x,pAIMesh->mTangents[i].y, pAIMesh->mTangents[i].z };
		}
		else
		{
			memcpy(&pVertices[i].vUV, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));

			memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
		}
	}

	ZeroMemory(&m_SubresourceData, sizeof m_SubresourceData);
	m_SubresourceData.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&m_BufferDesc, &m_SubresourceData, &m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);
	
	return S_OK;
}

HRESULT CMesh::Ready_VertexBuffer_Anim(const aiMesh* pAIMesh, CModel* pModel)
{
	VTXANIMMODEL* pVertices = new VTXANIMMODEL[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXANIMMODEL) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));

		m_VertexPos.emplace_back(_float4(pVertices[i].vPosition.x, pVertices[i].vPosition.y, pVertices[i].vPosition.z, 1.f));

		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));

		if (nullptr == pAIMesh->mTextureCoords[0])
		{
			if (nullptr != pAIMesh->mTangents)
				pVertices[i].vTangent = { pAIMesh->mTangents[i].x,pAIMesh->mTangents[i].y, pAIMesh->mTangents[i].z };
		}
		else
		{
			memcpy(&pVertices[i].vUV, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));

			memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
		}

		/*memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));*/
	}

	//메쉬에 영향을 주는 뼈의 갯수
	m_iNumBones = pAIMesh->mNumBones;

	for (_uint i = 0; i < m_iNumBones; i++)
	{
		//메쉬에 영향을 주는 i번째 뼈
		aiBone* pAIBone = pAIMesh->mBones[i];
		
		//뼈와 정점의 로컬 스페이스가 다를 수 있는데, 이를 보정해주는 행렬
		_float4x4 OffsetMatrix;
		memcpy(&OffsetMatrix, &pAIBone->mOffsetMatrix, sizeof(_float4x4));

		//모델에서 로드한 뼈 중 메쉬에 영향을 주는 i번째 뼈와 이름이 같은 뼈를 찾아 주소를 저장한다.
		CBone* pBone = pModel->Get_Bone(pAIBone->mName.data);
		if (nullptr == pBone)
			return E_FAIL;

		//R,U,L이 Row 정렬 상태 - Transpose로 Column 정렬로 바꿔주고 Set해준다.
		pBone->Set_OffsetMatrix(XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix)));

		m_Bones.emplace_back(pModel->Get_BoneIndex(pAIBone->mName.data));

		//i번째 뼈가 영향을 주는 정점의 갯수
		for (_uint j = 0; j < pAIBone->mNumWeights; j++)
		{
			//pAIBone->mWeights[j].mVertexId = i번째 뼈가 영향을 주는 j정점의 인덱스"Idx"
			if (0.f == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.x)
			{
				//"Idx" 정점에 영향을 주는 뼈 x = i
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.x = i;

				//"Idx" 정점에 뼈 x가 몇 %의 영향을 주는가
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.x = pAIBone->mWeights[j].mWeight;
			}
			else if (0.f == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.y)
			{
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.y = i;
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.y = pAIBone->mWeights[j].mWeight;
			}
			else if (0.f == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.z)
			{
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.z = i;
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.z = pAIBone->mWeights[j].mWeight;
			}
			else
			{
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.w = i;
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.w = pAIBone->mWeights[j].mWeight;
			}

		}
	}

	//뼈(애니메이션)가 없는 메쉬에 뼈를 강제로 1로 넣어줘서 모델의 애니메이션이 재생될 때
	//메쉬가 제 자리에서 재생될 수 있도록 예외처리를 해준다.
	if (0 == m_iNumBones)
	{
		m_iNumBones = 1;

		m_Bones.emplace_back(pModel->Get_BoneIndex(m_szName));
	}

	ZeroMemory(&m_SubresourceData, sizeof m_SubresourceData);
	m_SubresourceData.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&m_BufferDesc, &m_SubresourceData, &m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

CMesh* CMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eModelType, const aiMesh* pAIMesh, _fmatrix PivotMatrix, CModel* pModel)
{
	CMesh* pInstance = new CMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eModelType, pAIMesh, PivotMatrix, pModel)))
	{
		MSG_BOX("Failed to Created : CMesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CMesh::Clone(void* pArg)
{
	CMesh* pInstance = new CMesh(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMesh::Free()
{
	__super::Free();

	m_Bones.clear();
}
