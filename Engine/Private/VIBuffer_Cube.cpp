#include "..\Public\VIBuffer_Cube.h"

CVIBuffer_Cube::CVIBuffer_Cube(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Cube::CVIBuffer_Cube(const CVIBuffer_Cube & rhs)
	:CVIBuffer(rhs)
{
}

HRESULT CVIBuffer_Cube::Init_Prototype()
{
	if (FAILED(__super::Init_Prototype()))
		return E_FAIL;

	m_iNumVertexBuffers = 1;
	m_iStride = sizeof(VTXMODEL);
	m_iNumVertices = 24;
	m_iNumPrimitive = 12;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_iIndicesSizePerPrimitive = sizeof(FACEINDICES32);
	m_iNumIndicesPerPrimitive = 3;
	m_iNumIndices = m_iNumIndicesPerPrimitive * m_iNumPrimitive;

#pragma region VERTEX_BUFFER

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	float w2 = 0.5f;
	float h2 = 0.5f;
	float d2 = 0.5f;

	VTXMODEL*			pVertices = new VTXMODEL[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXMODEL));

	pVertices[0].vPosition	= _float3(-w2, -h2, -d2);
	pVertices[0].vTexUV		= _float2(0.f, 1.f);
	pVertices[0].vNormal	= _float3(0.f, 0.f, -1.f);
	pVertices[0].vTangent	= _float3(1.f, 0.f, 0.f);
	
	pVertices[1].vPosition	= _float3(-w2, +h2, -d2);
	pVertices[1].vTexUV		= _float2(0.f, 0.f);
	pVertices[1].vNormal	= _float3(0.f, 0.f, -1.f);
	pVertices[1].vTangent	= _float3(1.f, 0.f, 0.f);
	
	pVertices[2].vPosition	= _float3(+w2, +h2, -d2);
	pVertices[2].vTexUV		= _float2(1.0f, 0.f);
	pVertices[2].vNormal	= _float3(0.f, 0.f, -1.f);
	pVertices[2].vTangent	= _float3(1.f, 0.f, 0.f);
	
	pVertices[3].vPosition	= _float3(+w2, -h2, -d2);
	pVertices[3].vTexUV		= _float2(1.f, 1.f);
	pVertices[3].vNormal	= _float3(0.f, 0.f, -1.f);
	pVertices[3].vTangent	= _float3(1.f, 0.f, 0.f);

	pVertices[4].vPosition	= _float3(-w2, -h2, +d2);
	pVertices[4].vTexUV		= _float2(1.f, 1.f);
	pVertices[4].vNormal	= _float3(0.f, 0.f, 1.f);
	pVertices[4].vTangent	= _float3(-1.f, 0.f, 0.f);
	
	pVertices[5].vPosition	= _float3(+w2, -h2, +d2);
	pVertices[5].vTexUV		= _float2(0.f, 1.f);
	pVertices[5].vNormal	= _float3(0.f, 0.f, 1.f);
	pVertices[5].vTangent	= _float3(-1.f, 0.f, 0.f);
	
	pVertices[6].vPosition	= _float3(+w2, +h2, +d2);
	pVertices[6].vTexUV		= _float2(0.f, 0.f);
	pVertices[6].vNormal	= _float3(0.f, 0.f, 1.f);
	pVertices[6].vTangent	= _float3(-1.f, 0.f, 0.f);
	
	pVertices[7].vPosition	= _float3(-w2, +h2, +d2);
	pVertices[7].vTexUV		= _float2(1.f, 0.f);
	pVertices[7].vNormal	= _float3(0.f, 0.f, 1.f);
	pVertices[7].vTangent	= _float3(-1.f, 0.f, 0.f);

	pVertices[8].vPosition	= _float3(-w2, +h2, -d2);
	pVertices[8].vTexUV		= _float2(0.0f, 1.f);
	pVertices[8].vNormal	= _float3(0.f, 1.f, 0.f);
	pVertices[8].vTangent	= _float3(1.f, 0.f, 0.f);
	
	pVertices[9].vPosition	= _float3(-w2, +h2, +d2);
	pVertices[9].vTexUV		= _float2(0.0f, 0.f);
	pVertices[9].vNormal	= _float3(0.f, 1.f, 0.f);
	pVertices[9].vTangent	= _float3(1.f, 0.f, 0.f);
	
	pVertices[10].vPosition = _float3(+w2, +h2, +d2);
	pVertices[10].vTexUV	= _float2(1.0f, 0.f);
	pVertices[10].vNormal	= _float3(0.f, 1.f, 0.f);
	pVertices[10].vTangent	= _float3(1.f, 0.f, 0.f);
	
	pVertices[11].vPosition = _float3(+w2, +h2, -d2);
	pVertices[11].vTexUV	= _float2(1.0f, 1.f);
	pVertices[11].vNormal	= _float3(0.f, 1.f, 0.f);
	pVertices[11].vTangent	= _float3(1.f, 0.f, 0.f);

	pVertices[12].vPosition = _float3(-w2, -h2, -d2);
	pVertices[12].vTexUV = _float2(1.f, 1.f);
	pVertices[12].vNormal = _float3(0.f, -1.f, 0.f);
	pVertices[12].vTangent = _float3(-1.f, 0.f, 0.f);
	
	pVertices[13].vPosition = _float3(+w2, -h2, -d2);
	pVertices[13].vTexUV = _float2(0.f, 1.f);
	pVertices[13].vNormal = _float3(0.f, -1.f, 0.f);
	pVertices[13].vTangent = _float3(-1.f, 0.f, 0.f);
	
	pVertices[14].vPosition = _float3(+w2, -h2, +d2);
	pVertices[14].vTexUV = _float2(0.f, 0.f);
	pVertices[14].vNormal = _float3(0.f, -1.f, 0.f);
	pVertices[14].vTangent = _float3(-1.f, 0.f, 0.f);
	
	pVertices[15].vPosition = _float3(-w2, -h2, +d2);
	pVertices[15].vTexUV = _float2(1.f, 0.f);
	pVertices[15].vNormal = _float3(0.f, -1.f, 0.f);
	pVertices[15].vTangent = _float3(-1.f, 0.f, 0.f);

	pVertices[16].vPosition = _float3(-w2, -h2, +d2);
	pVertices[16].vTexUV = _float2(0.f, 1.f);
	pVertices[16].vNormal = _float3(-1.f, 0.f, 0.f);
	pVertices[16].vTangent = _float3(0.f, 0.f, -1.f);

	pVertices[17].vPosition = _float3(-w2, +h2, +d2);
	pVertices[17].vTexUV = _float2(0.f, 0.f);
	pVertices[17].vNormal = _float3(-1.f, 0.f, 0.f);
	pVertices[17].vTangent = _float3(0.f, 0.f, -1.f);

	pVertices[18].vPosition = _float3(-w2, +h2, -d2);
	pVertices[18].vTexUV = _float2(1.f, 0.f);
	pVertices[18].vNormal = _float3(-1.f, 0.f, 0.f);
	pVertices[18].vTangent = _float3(0.f, 0.f, -1.f);

	pVertices[19].vPosition = _float3(-w2, -h2, -d2);
	pVertices[19].vTexUV = _float2(1.f, 1.f);
	pVertices[19].vNormal = _float3(-1.f, 0.f, 0.f);
	pVertices[19].vTangent = _float3(0.f, 0.f, -1.f);

	pVertices[20].vPosition = _float3(+w2, -h2, -d2);
	pVertices[20].vTexUV = _float2(0.f, 1.f);
	pVertices[20].vNormal = _float3(1.f, 0.f, 0.f);
	pVertices[20].vTangent = _float3(0.f, 0.f, 1.f);

	pVertices[21].vPosition = _float3(+w2, +h2, -d2);
	pVertices[21].vTexUV = _float2(0.f, 0.f);
	pVertices[21].vNormal = _float3(1.f, 0.f, 0.f);
	pVertices[21].vTangent = _float3(0.f, 0.f, 1.f);

	pVertices[22].vPosition = _float3(+w2, +h2, +d2);
	pVertices[22].vTexUV = _float2(1.f, 0.f);
	pVertices[22].vNormal = _float3(1.f, 0.f, 0.f);
	pVertices[22].vTangent = _float3(0.f, 0.f, 1.f);

	pVertices[23].vPosition = _float3(+w2, -h2, +d2);
	pVertices[23].vTexUV = _float2(1.f, 1.f);
	pVertices[23].vNormal = _float3(1.f, 0.f, 0.f);
	pVertices[23].vTangent = _float3(0.f, 0.f, 1.f);

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

#pragma endregion

#pragma region INDEX_BUFFER

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iIndicesSizePerPrimitive * m_iNumPrimitive;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.StructureByteStride = 0;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	FACEINDICES32*		pIndices = new FACEINDICES32[m_iNumPrimitive];
	ZeroMemory(pIndices, sizeof(FACEINDICES32) * m_iNumPrimitive);

	pIndices[0]._0 = 0;
	pIndices[0]._1 = 1;
	pIndices[0]._2 = 2;

	pIndices[1]._0 = 0;
	pIndices[1]._1 = 2;
	pIndices[1]._2 = 3;

	pIndices[2]._0 = 4;
	pIndices[2]._1 = 5;
	pIndices[2]._2 = 6;

	pIndices[3]._0 = 4;
	pIndices[3]._1 = 6;
	pIndices[3]._2 = 7;

	pIndices[4]._0 = 8;
	pIndices[4]._1 = 9;
	pIndices[4]._2 = 10;

	pIndices[5]._0 = 8;
	pIndices[5]._1 = 10;
	pIndices[5]._2 = 11;

	pIndices[6]._0 = 12;
	pIndices[6]._1 = 13;
	pIndices[6]._2 = 14;

	pIndices[7]._0 = 12;
	pIndices[7]._1 = 14;
	pIndices[7]._2 = 15;

	pIndices[8]._0 = 16;
	pIndices[8]._1 = 17;
	pIndices[8]._2 = 18;

	pIndices[9]._0 = 16;
	pIndices[9]._1 = 18;
	pIndices[9]._2 = 19;

	pIndices[10]._0 = 20;
	pIndices[10]._1 = 21;
	pIndices[10]._2 = 22;

	pIndices[11]._0 = 20;
	pIndices[11]._1 = 22;
	pIndices[11]._2 = 23;
	
	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Cube::Init(void * pArg)
{
	if (FAILED(__super::Init(pArg)))
		return E_FAIL;

	return S_OK;
}

CVIBuffer_Cube * CVIBuffer_Cube::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CVIBuffer_Cube*		pInstance = new CVIBuffer_Cube(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Cube");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CVIBuffer_Cube::Clone(void * pArg)
{
	CVIBuffer_Cube*		pInstance = new CVIBuffer_Cube(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Cube");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVIBuffer_Cube::Free()
{
	__super::Free();
}
