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

HRESULT CMesh::Initialize_Prototype(CModel::TYPE eModelType, ifstream* pFin, _fmatrix PivotMatrix, CModel* pModel)
{
	_uint iSize = { 0 };
	pFin->read(reinterpret_cast<char*>(&iSize), sizeof(_uint));
	pFin->read(m_szName, iSize);
	strcat_s(m_szName, "\0");
	pFin->read(reinterpret_cast<char*>(&m_iMaterialIndex), sizeof(_uint));
	pFin->read(reinterpret_cast<char*>(&m_iNumVertices), sizeof(_uint));
	pFin->read(reinterpret_cast<char*>(&m_iNumIndices), sizeof(_uint));

	m_iNumVertexBuffers = 1;
	m_iIndexStride = sizeof(_ulong);
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_iStride = CModel::TYPE_NONANIM == eModelType ? sizeof(VTXMODEL) : sizeof(VTXANIMMODEL);

	m_VertexPos.reserve(m_iNumVertices);
	m_Faces.reserve(m_iNumIndices);
	m_iNumIndices *= 3;

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
		hr = Ready_VertexBuffer_NonAnim(pFin, PivotMatrix);
	}
	else
	{
		hr = Ready_VertexBuffer_Anim(pFin, pModel);
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

	_uint iNumFaces = m_iNumIndices / 3;

	for (_uint i = 0; i < iNumFaces; i++)
	{
		pFin->read(reinterpret_cast<char*>(&pIndices[iNumIndices]), sizeof(_uint));
		pFin->read(reinterpret_cast<char*>(&pIndices[iNumIndices + 1]), sizeof(_uint));
		pFin->read(reinterpret_cast<char*>(&pIndices[iNumIndices + 2]), sizeof(_uint));

		m_Faces.emplace_back(_uint3(pIndices[iNumIndices], pIndices[iNumIndices + 1], pIndices[iNumIndices + 2]));

		iNumIndices += 3;
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

HRESULT CMesh::Ready_VertexBuffer_NonAnim(ifstream* pFin, _fmatrix PivotMatrix)
{
	VTXMODEL* pVertices = new VTXMODEL[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXMODEL) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVertices; i++)
	{
		pFin->read(reinterpret_cast<char*>(&pVertices[i].vPosition), sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), PivotMatrix));
		
		m_VertexPos.emplace_back(_float4(pVertices[i].vPosition.x, pVertices[i].vPosition.y, pVertices[i].vPosition.z, 1.f));

		pFin->read(reinterpret_cast<char*>(&pVertices[i].vNormal), sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vNormal, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vNormal), PivotMatrix));
		
		pFin->read(reinterpret_cast<char*>(&pVertices[i].vUV), sizeof(_float2));
		pFin->read(reinterpret_cast<char*>(&pVertices[i].vTangent), sizeof(_float3));
	}

	ZeroMemory(&m_SubresourceData, sizeof m_SubresourceData);
	m_SubresourceData.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&m_BufferDesc, &m_SubresourceData, &m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);
	
	return S_OK;
}

HRESULT CMesh::Ready_VertexBuffer_Anim(ifstream* pFin, CModel* pModel)
{
	VTXANIMMODEL* pVertices = new VTXANIMMODEL[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXANIMMODEL) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVertices; i++)
	{
		pFin->read(reinterpret_cast<char*>(&pVertices[i].vPosition), sizeof(_float3));

		m_VertexPos.emplace_back(_float4(pVertices[i].vPosition.x, pVertices[i].vPosition.y, pVertices[i].vPosition.z, 1.f));

		pFin->read(reinterpret_cast<char*>(&pVertices[i].vNormal), sizeof(_float3));

		pFin->read(reinterpret_cast<char*>(&pVertices[i].vUV), sizeof(_float2));
		pFin->read(reinterpret_cast<char*>(&pVertices[i].vTangent), sizeof(_float3));
	}


	//메쉬에 영향을 주는 뼈의 갯수
	pFin->read(reinterpret_cast<char*>(&m_iNumBones), sizeof(_uint));

	for (_uint i = 0; i < m_iNumBones; i++)
	{
		//뼈와 정점의 로컬 스페이스가 다를 수 있는데, 이를 보정해주는 행렬
		_float4x4 OffsetMatrix;
		pFin->read(reinterpret_cast<char*>(&OffsetMatrix), sizeof(_float4x4));

		_uint iSize = { 0 };
		pFin->read(reinterpret_cast<char*>(&iSize), sizeof(_uint));
		char szName[MAX_PATH] = { "" };
		pFin->read(szName, iSize);
		strcat_s(szName, "\0");

		//모델에서 로드한 뼈 중 메쉬에 영향을 주는 i번째 뼈와 이름이 같은 뼈를 찾아 주소를 저장한다.
		CBone* pBone = pModel->Get_Bone(szName);
		if (nullptr == pBone)
			return E_FAIL;

		//R,U,L이 Row 정렬 상태 - Transpose로 Column 정렬로 바꿔주고 Set해준다.
		pBone->Set_OffsetMatrix(XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix)));

		m_Bones.emplace_back(pModel->Get_BoneIndex(szName));

		_uint iNumWeights = { 0 };
		pFin->read(reinterpret_cast<char*>(&iNumWeights), sizeof(_uint));

		//i번째 뼈가 영향을 주는 정점의 갯수
		for (_uint j = 0; j < iNumWeights; j++)
		{
			_uint iVertexID = { 0 };
			_float fWeights = { 0.f };

			pFin->read(reinterpret_cast<char*>(&iVertexID), sizeof(_uint));
			pFin->read(reinterpret_cast<char*>(&fWeights), sizeof(_float));


			//pAIBone->mWeights[j].mVertexId = i번째 뼈가 영향을 주는 j정점의 인덱스"Idx"
			if (0.f == pVertices[iVertexID].vBlendWeights.x)
			{
				//"Idx" 정점에 영향을 주는 뼈 x = i
				pVertices[iVertexID].vBlendIndices.x = i;

				//"Idx" 정점에 뼈 x가 몇 %의 영향을 주는가
				pVertices[iVertexID].vBlendWeights.x = fWeights;
			}
			else if (0.f == pVertices[iVertexID].vBlendWeights.y)
			{
				pVertices[iVertexID].vBlendIndices.y = i;
				pVertices[iVertexID].vBlendWeights.y = fWeights;
			}
			else if (0.f == pVertices[iVertexID].vBlendWeights.z)
			{
				pVertices[iVertexID].vBlendIndices.z = i;
				pVertices[iVertexID].vBlendWeights.z = fWeights;
			}
			else
			{
				pVertices[iVertexID].vBlendIndices.w = i;
				pVertices[iVertexID].vBlendWeights.w = fWeights;
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

CMesh* CMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eModelType, ifstream* pFin, _fmatrix PivotMatrix, CModel* pModel)
{
	CMesh* pInstance = new CMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eModelType, pFin, PivotMatrix, pModel)))
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
