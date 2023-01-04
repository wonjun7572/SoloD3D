#include "..\Public\Mesh.h"
#include "MathUtils.h"
#include "Bone.h"

CMesh::CMesh(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CVIBuffer(pDevice,pContext)
{
}

CMesh::CMesh(const CMesh & rhs)
	:CVIBuffer(rhs)
	,m_eType(rhs.m_eType)
	,m_iMaterialIndex(rhs.m_iMaterialIndex)
	,m_iNumBones(rhs.m_iNumBones)
{
}

HRESULT CMesh::Init_Prototype(CModel::TYPE eType, MeshLoadTag& tMeshLoad, class CModel* pModel)
{
	m_eType = eType;

	if (FAILED(__super::Init_Prototype()))
		return E_FAIL;

	m_iMaterialIndex = tMeshLoad.iMaterialIndex;
	m_iNumVertexBuffers = 1;
	m_iNumVertices = tMeshLoad.iNumVertices;
	m_iNumPrimitive = tMeshLoad.iNumPrimitive;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_iIndicesSizePerPrimitive = sizeof(FACEINDICES32);
	m_iNumIndicesPerPrimitive = 3;
	m_iNumIndices = m_iNumIndicesPerPrimitive * m_iNumPrimitive;

#pragma region VERTEX_BUFFER

	HRESULT hr = 0;

	if (CModel::TYPE_NONANIM == m_eType)
		hr = Ready_VertexBuffer_NonAnimModel(tMeshLoad, pModel);
	else
		hr = Ready_VertexBuffer_AnimModel(tMeshLoad, pModel);

	if (FAILED(hr))
		return E_FAIL;

#pragma endregion

#pragma region INDEX_BUFFER

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iIndicesSizePerPrimitive * m_iNumPrimitive;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.StructureByteStride = 0;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	
	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = tMeshLoad.pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

#pragma endregion

	return S_OK;
}

HRESULT CMesh::Init(void * pArg)
{
	return S_OK;
}

void CMesh::Load_MeshBones(CModel* pModel, HANDLE hFile)
{
	DWORD dwByte = 0;

	ReadFile(hFile, &m_iNumBones, sizeof(_uint), &dwByte, nullptr);

	for (size_t i = 0; i < m_iNumBones; ++i)
	{
		//load name
		char	szName[MAX_PATH];
		ReadFile(hFile, szName, sizeof(char) * MAX_PATH, &dwByte, nullptr);

		//load offsetmatrix
		_float4x4 OffsetMatrix;
		ReadFile(hFile, &OffsetMatrix, sizeof(_float4x4), &dwByte, nullptr);

		CBone* pBone = pModel->Get_BonePtr(szName);

		if (pBone == nullptr)
			return;
		// transpose를 하고 저장했기에 이대로 가도 괜찮을듯?
		pBone->Set_OffsetMatrix(OffsetMatrix);
		m_Bones.push_back(pBone);
		Safe_AddRef(pBone);
	}
}

void  CMesh::SetUp_BoneMatrices(_float4x4* pBoneMatrices, _float4x4 PivotMatrix)
{
	//항등
	if (m_iNumBones == 0)
		XMStoreFloat4x4(&pBoneMatrices[0], XMMatrixIdentity());

	_uint		iNumBones = 0;

	for (auto& pBone : m_Bones)
	{
		// BoneMatrix = 오프셋매트릭스 * 컴바인드매트릭스;
		pBoneMatrices[iNumBones++] = CMathUtils::Mul_Matrix(CMathUtils::Mul_Matrix(pBone->Get_OffsetMatrix(), pBone->Get_CombindMatrix()), PivotMatrix);
	}
}

HRESULT CMesh::Ready_VertexBuffer_NonAnimModel(MeshLoadTag& tMeshLoad, CModel* pModel)
{
	m_iStride = sizeof(VTXMODEL);
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = tMeshLoad.pModelVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMesh::Ready_VertexBuffer_AnimModel(MeshLoadTag& tMeshLoad, CModel* pModel)
{
	m_iStride = sizeof(VTXANIMMODEL);
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	
	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = tMeshLoad.pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	return S_OK;
}

CMesh * CMesh::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, CModel::TYPE eType, MeshLoadTag tMeshLoad, CModel* pModel)
{
	CMesh*		pInstance = new CMesh(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype(eType, tMeshLoad, pModel)))
	{
		MSG_BOX("Failed to Created : CMesh");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CMesh::Clone(void * pArg)
{
	CMesh*		pInstance = new CMesh(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMesh::Free()
{
	__super::Free();

	for (auto& pBone : m_Bones)
		Safe_Release(pBone);

	m_Bones.clear();
}
