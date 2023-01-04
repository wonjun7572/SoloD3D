#include "..\public\VIBuffer_Terrain.h"
#include "GameUtils.h"
#include "Frustum.h"
#include "QuadTree.h"

CVIBuffer_Terrain::CVIBuffer_Terrain(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer(pDevice, pContext)
	,m_pPos(nullptr)
{
	ZeroMemory(&m_fH, sizeof(BITMAPFILEHEADER));
	ZeroMemory(&m_iH, sizeof(BITMAPINFOHEADER));
}

CVIBuffer_Terrain::CVIBuffer_Terrain(const CVIBuffer_Terrain & rhs)
	: CVIBuffer(rhs)
	,m_pPos(rhs.m_pPos)
	,m_iNumVerticesX(rhs.m_iNumVerticesX)
	,m_iNumVerticesZ(rhs.m_iNumVerticesZ)
	,m_pVertices(rhs.m_pVertices)
	,m_pIndices(rhs.m_pIndices)
	,m_pHeightPixel(rhs.m_pHeightPixel)
	,m_strFilePath(rhs.m_strFilePath)
	,m_pQuadTree(rhs.m_pQuadTree)
{
	Safe_AddRef(m_pQuadTree);
	memcpy(&m_fH, &rhs.m_fH, sizeof(BITMAPFILEHEADER));
	memcpy(&m_iH, &rhs.m_iH, sizeof(BITMAPINFOHEADER));
}

HRESULT CVIBuffer_Terrain::Init_Prototype(const _tchar* pHeightMapFilePath)
{
	if (FAILED(__super::Init_Prototype()))
		return E_FAIL;
	
	m_strFilePath = pHeightMapFilePath;

	_ulong		dwByte = 0;
	HANDLE		hFile = CreateFile(pHeightMapFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	m_pHeightPixel = nullptr;

	ReadFile(hFile, &m_fH, sizeof BITMAPFILEHEADER, &dwByte, nullptr);
	ReadFile(hFile, &m_iH, sizeof BITMAPINFOHEADER, &dwByte, nullptr);
	m_iNumVerticesX = m_iH.biWidth;
	m_iNumVerticesZ = m_iH.biHeight;
	m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;
	m_pPos = new _float3[m_iNumVertices];

	m_pHeightPixel = new _ulong[m_iNumVertices];
	ZeroMemory(m_pHeightPixel, sizeof(_ulong) * m_iNumVertices);

	ReadFile(hFile, m_pHeightPixel, sizeof(_ulong) * m_iNumVertices, &dwByte, nullptr);

	CloseHandle(hFile);

	m_iNumVertexBuffers = 1;
	m_iStride = sizeof(VTXNORTEX);

	m_iNumPrimitive = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_iIndicesSizePerPrimitive = sizeof(FACEINDICES32);
	m_iNumIndicesPerPrimitive = 3;
	m_iNumIndices = m_iNumIndicesPerPrimitive * m_iNumPrimitive;
	
#pragma region VERTEX_BUFFER
	m_pVertices = new VTXNORTEX[m_iNumVertices];
	ZeroMemory(m_pVertices, sizeof(VTXNORTEX) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVerticesZ; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX; ++j)
		{
			_uint			iIndex = i * m_iNumVerticesX + j;
			m_pVertices[iIndex].vPosition = _float3(static_cast<float>(j), static_cast<float>(m_pHeightPixel[iIndex] & 0x000000ff), static_cast<float>(i));
			m_pPos[iIndex] = _float3(m_pVertices[iIndex].vPosition.x, m_pVertices[iIndex].vPosition.y, m_pVertices[iIndex].vPosition.z);
			m_pVertices[iIndex].vNormal = _float3(0.f, 0.f, 0.f);
			m_pVertices[iIndex].vTexUV = _float2(j / (m_iNumVerticesX - 1.0f), i / (m_iNumVerticesZ - 1.0f));
		}
	}

#pragma endregion
	
#pragma region INDEX_BUFFER
	m_pIndices = new FACEINDICES32[m_iNumPrimitive];
	ZeroMemory(m_pIndices, sizeof(FACEINDICES32) * m_iNumPrimitive);

	_uint		iNumFaces = 0;

	for (_uint i = 0; i < m_iNumVerticesZ - 1; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX - 1; ++j)
		{
			_uint			iIndex = i * m_iNumVerticesX + j;

			_uint			iIndices[4] = {
				iIndex + m_iNumVerticesX,
				iIndex + m_iNumVerticesX + 1,
				iIndex + 1,
				iIndex
			};

			_vector			vSour, vDest, vNormal;

			m_pIndices[iNumFaces]._0 = iIndices[0];
			m_pIndices[iNumFaces]._1 = iIndices[1];
			m_pIndices[iNumFaces]._2 = iIndices[2];

			vSour = XMLoadFloat3(&m_pVertices[m_pIndices[iNumFaces]._1].vPosition) - XMLoadFloat3(&m_pVertices[m_pIndices[iNumFaces]._0].vPosition);
			vDest = XMLoadFloat3(&m_pVertices[m_pIndices[iNumFaces]._2].vPosition) - XMLoadFloat3(&m_pVertices[m_pIndices[iNumFaces]._1].vPosition);
			vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));

			XMStoreFloat3(&m_pVertices[m_pIndices[iNumFaces]._0].vNormal, XMVector3Normalize(XMLoadFloat3(&m_pVertices[m_pIndices[iNumFaces]._0].vNormal) + vNormal));
			XMStoreFloat3(&m_pVertices[m_pIndices[iNumFaces]._1].vNormal, XMVector3Normalize(XMLoadFloat3(&m_pVertices[m_pIndices[iNumFaces]._1].vNormal) + vNormal));
			XMStoreFloat3(&m_pVertices[m_pIndices[iNumFaces]._2].vNormal, XMVector3Normalize(XMLoadFloat3(&m_pVertices[m_pIndices[iNumFaces]._2].vNormal) + vNormal));

			++iNumFaces;

			m_pIndices[iNumFaces]._0 = iIndices[0];
			m_pIndices[iNumFaces]._1 = iIndices[2];
			m_pIndices[iNumFaces]._2 = iIndices[3];

			vSour = XMLoadFloat3(&m_pVertices[m_pIndices[iNumFaces]._1].vPosition) - XMLoadFloat3(&m_pVertices[m_pIndices[iNumFaces]._0].vPosition);
			vDest = XMLoadFloat3(&m_pVertices[m_pIndices[iNumFaces]._2].vPosition) - XMLoadFloat3(&m_pVertices[m_pIndices[iNumFaces]._1].vPosition);
			vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));

			XMStoreFloat3(&m_pVertices[m_pIndices[iNumFaces]._0].vNormal, XMVector3Normalize(XMLoadFloat3(&m_pVertices[m_pIndices[iNumFaces]._0].vNormal) + vNormal));
			XMStoreFloat3(&m_pVertices[m_pIndices[iNumFaces]._1].vNormal, XMVector3Normalize(XMLoadFloat3(&m_pVertices[m_pIndices[iNumFaces]._1].vNormal) + vNormal));
			XMStoreFloat3(&m_pVertices[m_pIndices[iNumFaces]._2].vNormal, XMVector3Normalize(XMLoadFloat3(&m_pVertices[m_pIndices[iNumFaces]._2].vNormal) + vNormal));

			++iNumFaces;
		}
	}

#pragma endregion
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC; // DEFAULT
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iStride;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // 0 툴을 위해 지금은 이렇게
	m_BufferDesc.MiscFlags = 0;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = m_pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iIndicesSizePerPrimitive * m_iNumPrimitive;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.StructureByteStride = 0;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // vertex와 마찬가지
	m_BufferDesc.MiscFlags = 0;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = m_pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	m_pQuadTree = CQuadTree::Create(m_iNumVerticesX * m_iNumVerticesZ - m_iNumVerticesX,
		m_iNumVerticesX * m_iNumVerticesZ - 1, m_iNumVerticesX - 1, 0);

	return S_OK;
}

HRESULT CVIBuffer_Terrain::Init(void * pArg)
{
	if (FAILED(__super::Init(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_Terrain::DynamicBufferControlForSave(_float4 vBrushPos, _float fBrushRange, _double _Height, _double TimeDelta, bool defaultHeight)
{

	for (_uint i = 0; i < m_iNumVerticesZ; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX; ++j)
		{
			_uint			iIndex = i * m_iNumVerticesX + j;
			
			if (vBrushPos.x - fBrushRange <= m_pVertices[iIndex].vPosition.x && m_pVertices[iIndex].vPosition.x < vBrushPos.x + fBrushRange &&
				vBrushPos.z - fBrushRange <= m_pVertices[iIndex].vPosition.z && m_pVertices[iIndex].vPosition.z < vBrushPos.z + fBrushRange)
			{
				m_pVertices[iIndex].vPosition.x = static_cast<float>(j);

				if (m_pVertices[iIndex].vPosition.y >= 0 || m_pVertices[iIndex].vPosition.y <= 255)
				{
					if (!defaultHeight)
						m_pVertices[iIndex].vPosition.y += static_cast<float>(_Height * TimeDelta);
					else
						m_pVertices[iIndex].vPosition.y = static_cast<float>(_Height);
				}
				
				if (m_pVertices[iIndex].vPosition.y < 0)
					m_pVertices[iIndex].vPosition.y = 0;

				if (m_pVertices[iIndex].vPosition.y > 255)
					m_pVertices[iIndex].vPosition.y = 255;
				m_pVertices[iIndex].vPosition.z = static_cast<float>(i);

				_ulong fY = static_cast<_ulong>(m_pVertices[iIndex].vPosition.y);
				m_pHeightPixel[iIndex] = D3DCOLOR_ARGB(fY, fY, fY, fY);
			}
			m_pPos[iIndex] = _float3(m_pVertices[iIndex].vPosition.x, m_pVertices[iIndex].vPosition.y, m_pVertices[iIndex].vPosition.z);
		}
	}

	_uint		iNumFaces = 0;

	for (_uint i = 0; i < m_iNumVerticesZ - 1; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX - 1; ++j)
		{
			_uint			iIndex = i * m_iNumVerticesX + j;

			_uint			iIndices[4] = {
				iIndex + m_iNumVerticesX,
				iIndex + m_iNumVerticesX + 1,
				iIndex + 1,
				iIndex
			};

			_vector			vSour, vDest, vNormal;

			vSour = XMLoadFloat3(&m_pVertices[m_pIndices[iNumFaces]._1].vPosition) - XMLoadFloat3(&m_pVertices[m_pIndices[iNumFaces]._0].vPosition);
			vDest = XMLoadFloat3(&m_pVertices[m_pIndices[iNumFaces]._2].vPosition) - XMLoadFloat3(&m_pVertices[m_pIndices[iNumFaces]._1].vPosition);
			vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));

			XMStoreFloat3(&m_pVertices[m_pIndices[iNumFaces]._0].vNormal, XMVector3Normalize(XMLoadFloat3(&m_pVertices[m_pIndices[iNumFaces]._0].vNormal) + vNormal));
			XMStoreFloat3(&m_pVertices[m_pIndices[iNumFaces]._1].vNormal, XMVector3Normalize(XMLoadFloat3(&m_pVertices[m_pIndices[iNumFaces]._1].vNormal) + vNormal));
			XMStoreFloat3(&m_pVertices[m_pIndices[iNumFaces]._2].vNormal, XMVector3Normalize(XMLoadFloat3(&m_pVertices[m_pIndices[iNumFaces]._2].vNormal) + vNormal));

			++iNumFaces;

			vSour = XMLoadFloat3(&m_pVertices[m_pIndices[iNumFaces]._1].vPosition) - XMLoadFloat3(&m_pVertices[m_pIndices[iNumFaces]._0].vPosition);
			vDest = XMLoadFloat3(&m_pVertices[m_pIndices[iNumFaces]._2].vPosition) - XMLoadFloat3(&m_pVertices[m_pIndices[iNumFaces]._1].vPosition);
			vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));

			XMStoreFloat3(&m_pVertices[m_pIndices[iNumFaces]._0].vNormal, XMVector3Normalize(XMLoadFloat3(&m_pVertices[m_pIndices[iNumFaces]._0].vNormal) + vNormal));
			XMStoreFloat3(&m_pVertices[m_pIndices[iNumFaces]._1].vNormal, XMVector3Normalize(XMLoadFloat3(&m_pVertices[m_pIndices[iNumFaces]._1].vNormal) + vNormal));
			XMStoreFloat3(&m_pVertices[m_pIndices[iNumFaces]._2].vNormal, XMVector3Normalize(XMLoadFloat3(&m_pVertices[m_pIndices[iNumFaces]._2].vNormal) + vNormal));

			++iNumFaces;
		}
	}

	D3D11_MAPPED_SUBRESOURCE			SubResourceVtx;
	ZeroMemory(&SubResourceVtx, sizeof SubResourceVtx);

	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResourceVtx);

	memcpy(SubResourceVtx.pData, m_pVertices, sizeof(VTXNORTEX) * m_iNumVertices);

	m_pContext->Unmap(m_pVB, 0);

	D3D11_MAPPED_SUBRESOURCE			SubResourceIdx;
	ZeroMemory(&SubResourceIdx, sizeof SubResourceIdx);

	m_pContext->Map(m_pIB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResourceIdx);

	memcpy(SubResourceIdx.pData, m_pIndices, sizeof(FACEINDICES32) * m_iNumPrimitive);

	m_pContext->Unmap(m_pIB, 0);

	return S_OK;
}

HRESULT CVIBuffer_Terrain::SaveHeightMap()
{
	FILE* fp = nullptr;
	char* szName = new char[MAX_PATH];
	CGameUtils::wc2c(m_strFilePath.c_str(), szName);

	fopen_s(&fp, szName, "wb");

	fwrite(&m_fH, sizeof BITMAPFILEHEADER, 1, fp);
	fwrite(&m_iH, sizeof(BITMAPINFOHEADER),1, fp);
	fwrite(m_pHeightPixel,sizeof(_ulong) * m_iNumVertices, 1, fp);
	
	fclose(fp);

	Safe_Delete_Array(szName);

	return S_OK;
}

void CVIBuffer_Terrain::Culling(_fmatrix WorldMatrix)
{
	CFrustum*			pFrustum = GET_INSTANCE(CFrustum);

	pFrustum->Transform_ToLocalSpace(WorldMatrix);

	_uint			iNumFaces = 0;

//#ifdef USE_QUADTREE
	m_pQuadTree->Culling(pFrustum, m_pPos, m_pIndices, &iNumFaces);

//#else
//	for (_uint i = 0; i < m_iNumVerticesZ - 1; ++i)
//	{
//		for (_uint j = 0; j < m_iNumVerticesX - 1; ++j)
//		{
//			_uint iIndex = i * m_iNumVerticesX + j;
//
//			_uint			iIndices[4] = {
//				iIndex + m_iNumVerticesX,
//				iIndex + m_iNumVerticesX + 1,
//				iIndex + 1,
//				iIndex
//			};
//
//			_bool			isIn[4] = {
//				pFrustum->isInFrustum_LocalSpace(XMLoadFloat4(&m_pPos[iIndices[0]])),
//				pFrustum->isInFrustum_LocalSpace(XMLoadFloat4(&m_pPos[iIndices[1]])),
//				pFrustum->isInFrustum_LocalSpace(XMLoadFloat4(&m_pPos[iIndices[2]])),
//				pFrustum->isInFrustum_LocalSpace(XMLoadFloat4(&m_pPos[iIndices[3]]))
//			};
//
//			/* 우상단 삼각형이 그려져야하니? */
//			if (true == isIn[0] &&
//				true == isIn[1] &&
//				true == isIn[2])
//			{
//				m_pIndices[iNumFaces]._0 = iIndices[0];
//				m_pIndices[iNumFaces]._1 = iIndices[1];
//				m_pIndices[iNumFaces]._2 = iIndices[2];
//				++iNumFaces;
//			}
//
//			/* 좌하단 삼각형이 그려져야하니? */
//			if (true == isIn[0] &&
//				true == isIn[2] &&
//				true == isIn[3])
//			{
//				m_pIndices[iNumFaces]._0 = iIndices[0];
//				m_pIndices[iNumFaces]._1 = iIndices[2];
//				m_pIndices[iNumFaces]._2 = iIndices[3];
//				++iNumFaces;
//			}
//		}
//	}
//#endif

	D3D11_MAPPED_SUBRESOURCE		SubResources;
	ZeroMemory(&SubResources, sizeof(D3D11_MAPPED_SUBRESOURCE));

	m_pContext->Map(m_pIB, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResources);

	memcpy(SubResources.pData, m_pIndices, sizeof(FACEINDICES32) * iNumFaces);

	m_pContext->Unmap(m_pIB, 0);

	m_iNumIndices = iNumFaces * 3;

	RELEASE_INSTANCE(CFrustum);
}

CVIBuffer_Terrain * CVIBuffer_Terrain::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar* pHeightMapFilePath)
{
	CVIBuffer_Terrain*		pInstance = new CVIBuffer_Terrain(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype(pHeightMapFilePath)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CVIBuffer_Terrain::Clone(void * pArg)
{
	CVIBuffer_Terrain*		pInstance = new CVIBuffer_Terrain(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}


void CVIBuffer_Terrain::Free()
{
	__super::Free();

	if (!m_bClone)
	{
		Safe_Delete_Array(m_pHeightPixel);
		Safe_Delete_Array(m_pVertices);
		Safe_Delete_Array(m_pIndices);
		Safe_Delete_Array(m_pPos);
	}

	Safe_Release(m_pQuadTree);
}
