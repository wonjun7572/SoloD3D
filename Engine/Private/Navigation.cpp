#include "..\public\Navigation.h"
#include "Cell.h"
#include "Shader.h"

CNavigation::CNavigation(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{

}

CNavigation::CNavigation(const CNavigation & rhs)
	: CComponent(rhs)
	, m_Cells(rhs.m_Cells)
#ifdef  _DEBUG
	,m_pShader(rhs.m_pShader)
#endif
{
	for (auto& pCell : m_Cells)
		Safe_AddRef(pCell);

#ifdef _DEBUG
	Safe_AddRef(m_pShader);
#endif

}

HRESULT CNavigation::Init_Prototype(const _tchar * pNavigationDataFilePath)
{
	_ulong		dwByte = 0;
	HANDLE		hFile = CreateFile(pNavigationDataFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	_float3		vPoints[CCell::POINT_END];

	while (true)
	{
		ReadFile(hFile, vPoints, sizeof(_float3) * CCell::POINT_END, &dwByte, nullptr);
		if (0 == dwByte)
			break;

		CCell*		pCell = CCell::Create(m_pDevice, m_pContext, vPoints, (_int)m_Cells.size());
		if (nullptr == pCell)
			return E_FAIL;

		m_Cells.push_back(pCell);
	}

	CloseHandle(hFile);

	if (FAILED(Ready_Neighbor()))
		return E_FAIL;

#ifdef _DEBUG

	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Navigation.hlsl"), VTXPOS_DECLARATION::Elements, VTXPOS_DECLARATION::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;

#endif

	return S_OK;
}

HRESULT CNavigation::Init(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_NaviDesc, pArg, sizeof(NAVIDESC));

	return S_OK;
}

_bool CNavigation::isMove_OnNavigation(_fvector TargetPos)
{
	if (-1 == m_NaviDesc.iCurrentIndex)
		return false;

	_int		iNeighborIndex = -1;

	/* �����̰� �� �����ġ�� �� �ȿ� �ִٸ�.  */
	if (true == m_Cells[m_NaviDesc.iCurrentIndex]->isIn(TargetPos, &iNeighborIndex))
		return true; // ������. 
					 /* �����̰� �� �����ġ�� �̽��� ����ٸ�. */
	else
	{
		/* ������������ �̿��� �־��ٸ餤. */
		if (-1 != iNeighborIndex)
		{
			while (true)
			{
				if (-1 == iNeighborIndex)
				{
					return false;
				}
				if (true == m_Cells[iNeighborIndex]->isIn(TargetPos, &iNeighborIndex))
				{
					// m_NaviDesc.iCurrentIndex = �̿��� �ε���;
					m_NaviDesc.iCurrentIndex = iNeighborIndex;
					return true;
				}
			}
		}
		/* ������������ �̿��� �����ٸ� */
		else
		{
			return false;
		}
	}
}

#ifdef _DEBUG
HRESULT CNavigation::Render()
{
	_float		fHeight = 0.0f;

	if (-1 == m_NaviDesc.iCurrentIndex)
	{
		fHeight = 0.0f;
		HRESULT hr = m_pShader->Set_RawValue("g_fHeight", &fHeight, sizeof(_float));
		m_pShader->Set_RawValue("g_vColor", &_float4(0.f, 1.f, 0.f, 1.f), sizeof(_float4));
	}
	else
	{
		fHeight = 0.1f;
		HRESULT hr = m_pShader->Set_RawValue("g_fHeight", &fHeight, sizeof(_float));
		m_pShader->Set_RawValue("g_vColor", &_float4(1.f, 0.f, 0.f, 1.f), sizeof(_float4));

		m_Cells[m_NaviDesc.iCurrentIndex]->Render(m_pShader);

		return S_OK;
	}
	
	for (auto& pCell : m_Cells)
	{
		if (nullptr != pCell)
			pCell->Render(m_pShader);
	}

	return S_OK;
}
#endif

HRESULT CNavigation::Ready_Neighbor()
{
	for (auto& pSourCell : m_Cells)
	{
		for (auto& pDestCell : m_Cells)
		{
			if (pSourCell == pDestCell)
				continue;

			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_A), pSourCell->Get_Point(CCell::POINT_B)))
			{
				pSourCell->Set_Neighbor(CCell::NEIGHBOR_AB, pDestCell);
			}

			else if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_B), pSourCell->Get_Point(CCell::POINT_C)))
			{
				pSourCell->Set_Neighbor(CCell::NEIGHBOR_BC, pDestCell);
			}

			else if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_C), pSourCell->Get_Point(CCell::POINT_A)))
			{
				pSourCell->Set_Neighbor(CCell::NEIGHBOR_CA, pDestCell);
			}
		}
	}

	return S_OK;
}

CNavigation * CNavigation::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pNavigationDataFilePath)
{
	CNavigation*		pInstance = new CNavigation(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype(pNavigationDataFilePath)))
	{
		MSG_BOX("Failed to Created : CNavigation");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CNavigation::Clone(void * pArg)
{
	CNavigation*		pInstance = new CNavigation(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Created : CNavigation");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CNavigation::Free()
{
	__super::Free();

	for (auto& pCell : m_Cells)
		Safe_Release(pCell);

	m_Cells.clear();

#ifdef _DEBUG
	Safe_Release(m_pShader);
#endif // _DEBUG
}
