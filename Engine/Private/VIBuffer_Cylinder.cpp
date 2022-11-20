#include "..\Public\VIBuffer_Cylinder.h"

CVIBuffer_Cylinder::CVIBuffer_Cylinder(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Cylinder::CVIBuffer_Cylinder(const CVIBuffer_Cylinder & rhs)
	:CVIBuffer(rhs)
{

}

HRESULT CVIBuffer_Cylinder::Init_Prototype(_uint _segment)
{
	if (FAILED(__super::Init_Prototype()))
		return E_FAIL;

	m_iNumVertexBuffers = 1;
	m_iStride = sizeof(VTXNORTEX);
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_iIndicesSizePerPrimitive = sizeof(FACEINDICES32);
	m_iNumIndicesPerPrimitive = 3;

	m_iNumVertices = 0;
	m_iNumPrimitive = 0;

	vector<VTXNORTEX> vecVtxCylinder(m_iNumVertices);

	VTXNORTEX v;

	// Top center vecVtxCylinder (multiple vecVtxCylinders for texture coordinates).
	for (unsigned __int32 a = 0; a <= _segment; a++)
	{
		v.vPosition = XMFLOAT3(0.0f, 0.0f, 1.0f);
		v.vNormal = XMFLOAT3(0.0f, 0.0f, 1.0f);
		v.vTexUV = XMFLOAT2(static_cast<float>(a) / static_cast<float>(_segment), 0.0f);
		vecVtxCylinder.push_back(v);
		m_iNumVertices++;
	}
	// Top edge of cylinder: Normals vecVtxCylinder up for lighting of top surface.
	for (unsigned __int32 a = 0; a <= _segment; a++)
	{
		float angle = static_cast<float>(a) / static_cast<float>(_segment) * XM_2PI;
		v.vPosition = XMFLOAT3(cos(angle), sin(angle), 1.0f);
		v.vNormal = XMFLOAT3(0.0f, 0.0f, 1.0f);
		v.vTexUV = XMFLOAT2(static_cast<float>(a) / static_cast<float>(_segment), 0.0f);
		vecVtxCylinder.push_back(v);
		m_iNumVertices++;
	}
	// Top edge of cylinder: Normals vecVtxCylinder out for lighting of the side surface.
	for (unsigned __int32 a = 0; a <= _segment; a++)
	{
		float angle = static_cast<float>(a) / static_cast<float>(_segment) * XM_2PI;
		v.vPosition = XMFLOAT3(cos(angle), sin(angle), 1.0f);
		v.vNormal = XMFLOAT3(cos(angle), sin(angle), 0.0f);
		v.vTexUV = XMFLOAT2(static_cast<float>(a) / static_cast<float>(_segment), 0.0f);
		vecVtxCylinder.push_back(v);
		m_iNumVertices++;
	}
	// Bottom edge of cylinder: Normals vecVtxCylinder out for lighting of the side surface.
	for (unsigned __int32 a = 0; a <= _segment; a++)
	{
		float angle = static_cast<float>(a) / static_cast<float>(_segment) * XM_2PI;
		v.vPosition = XMFLOAT3(cos(angle), sin(angle), 0.0f);
		v.vNormal = XMFLOAT3(cos(angle), sin(angle), 0.0f);
		v.vTexUV = XMFLOAT2(static_cast<float>(a) / static_cast<float>(_segment), 1.0f);
		vecVtxCylinder.push_back(v);
		m_iNumVertices++;
	}
	// Bottom edge of cylinder: Normals vecVtxCylinder down for lighting of the bottom surface.
	for (unsigned __int32 a = 0; a <= _segment; a++)
	{
		float angle = static_cast<float>(a) / static_cast<float>(_segment) * XM_2PI;
		v.vPosition = XMFLOAT3(cos(angle), sin(angle), 0.0f);
		v.vNormal = XMFLOAT3(0.0f, 0.0f, -1.0f);
		v.vTexUV = XMFLOAT2(static_cast<float>(a) / static_cast<float>(_segment), 1.0f);
		vecVtxCylinder.push_back(v);
		m_iNumVertices++;
	}
	// Bottom center of cylinder: Normals vecVtxCylinder down for lighting on the bottom surface.
	for (unsigned __int32 a = 0; a <= _segment; a++)
	{
		v.vPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);
		v.vNormal = XMFLOAT3(0.0f, 0.0f, -1.0f);
		v.vTexUV = XMFLOAT2(static_cast<float>(a) / static_cast<float>(_segment), 1.0f);
		vecVtxCylinder.push_back(v);
		m_iNumVertices++;
	}

	vector<FACEINDICES32> vecIdx;

	for (unsigned __int32 a = 0; a < 6; a += 2)
	{
		unsigned __int32 p1 = a * (_segment + 1);
		unsigned __int32 p2 = (a + 1) * (_segment + 1);
		for (unsigned __int32 b = 0; b < _segment; b++)
		{
			if (a < 4)
			{
				vecIdx.push_back({ b + p1, b + p2 ,b + p2 + 1 });
				m_iNumPrimitive++;
			}
			if (a > 0)
			{
				vecIdx.push_back({ b + p1, b + p2 + 1, b + p1 + 1 });
				m_iNumPrimitive++;
			}
		}
	}

	m_iNumIndices = m_iNumIndicesPerPrimitive * m_iNumPrimitive;

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	VTXNORTEX*			pVertices = new VTXNORTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXNORTEX));

	memcpy(pVertices, vecVtxCylinder.data(), sizeof(VTXNORTEX) * vecVtxCylinder.size());

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iIndicesSizePerPrimitive * m_iNumPrimitive;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.StructureByteStride = 0;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	FACEINDICES32*		pIndices = new FACEINDICES32[m_iNumPrimitive];
	ZeroMemory(pIndices, sizeof(FACEINDICES32) * m_iNumPrimitive);

	memcpy(pIndices, vecIdx.data(), sizeof(FACEINDICES32) * vecIdx.size());

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

	return S_OK;
}

HRESULT CVIBuffer_Cylinder::Init(void * pArg)
{
	if (FAILED(__super::Init(pArg)))
		return E_FAIL;

	return S_OK;
}

CVIBuffer_Cylinder * CVIBuffer_Cylinder::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, _uint _segment)
{
	CVIBuffer_Cylinder*		pInstance = new CVIBuffer_Cylinder(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype(_segment)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Cylinder");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CVIBuffer_Cylinder::Clone(void * pArg)
{
	CVIBuffer_Cylinder*		pInstance = new CVIBuffer_Cylinder(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Cylinder");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVIBuffer_Cylinder::Free()
{
	__super::Free();
}
