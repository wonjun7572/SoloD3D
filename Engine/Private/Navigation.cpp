#include "..\public\Navigation.h"
#include "Cell.h"
#include "Shader.h"
#include "PipeLine.h"
#include "DebugDraw.h"
#include "MathUtils.h"
#include "GameInstance.h"

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
	HANDLE		hFile = CreateFile(pNavigationDataFilePath,
		GENERIC_READ
		, 0
		, nullptr
		, OPEN_EXISTING
		, FILE_ATTRIBUTE_NORMAL
		, 0);

	if (INVALID_HANDLE_VALUE == hFile)
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

_bool CNavigation::isMove_OnNavigation(_fvector TargetPos, OUT _float4* vOldPos)
{
	if (-1 == m_NaviDesc.iCurrentIndex)
		return false;

	_int		iNeighborIndex = -1;

	/* 움직이고 난 결과위치가 쎌 안에 있다면.  */
	if (true == m_Cells[m_NaviDesc.iCurrentIndex]->isIn(TargetPos, vOldPos, &iNeighborIndex))
	{
		return true; // 움직여. 
					 /* 움직이고 난 결과위치가 이쎌을 벗어난다면. */
	}
	else
	{
		/* 나간방향으로 이웃이 있었다면. */
		if (-1 != iNeighborIndex)
		{
			while (true)
			{
				if (-1 == iNeighborIndex)
				{
					return false;
				}
				if (true == m_Cells[iNeighborIndex]->isIn(TargetPos, vOldPos, &iNeighborIndex))
				{
					// m_NaviDesc.iCurrentIndex = 이웃의 인덱스;
					m_NaviDesc.iCurrentIndex = iNeighborIndex;
					return true;
				}
			}
		}
		/* 나간방향으로 이웃이 없었다면 */
		else
		{
			return false;
		}
	}
}

_bool CNavigation::isHeighit_OnNavigation(_fvector TargetPos, _float * yPos)
{
	if (-1 == m_NaviDesc.iCurrentIndex || m_Cells.size() <= 0)
		return false;

	_float3 vPos; 
	XMStoreFloat3(&vPos, TargetPos);
	
	*yPos = CMathUtils::GetHeightFromPoints(vPos,
		m_Cells[m_NaviDesc.iCurrentIndex]->Get_Point(CCell::POINT_A),
		m_Cells[m_NaviDesc.iCurrentIndex]->Get_Point(CCell::POINT_B),
		m_Cells[m_NaviDesc.iCurrentIndex]->Get_Point(CCell::POINT_C));

	return true;
}

_int CNavigation::isPicking_NaviCell(HWND hWnd, _uint iWinsizeX, _uint iWinsizey)
{
	POINT		ptMouse{};

	GetCursorPos(&ptMouse);
	ScreenToClient(hWnd, &ptMouse);

	_float3         vMousePos;

	vMousePos.x = _float(ptMouse.x / (iWinsizeX * 0.5f) - 1);
	vMousePos.y = _float(ptMouse.y / (iWinsizey * -0.5f) + 1);
	vMousePos.z = 0.f;

	_vector	vecMousePos = XMLoadFloat3(&vMousePos);
	vecMousePos = XMVectorSetW(vecMousePos, 1.f);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_matrix      ProjMatrixInv;
	ProjMatrixInv = pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ);
	ProjMatrixInv = XMMatrixInverse(nullptr, ProjMatrixInv);
	vecMousePos = XMVector3TransformCoord(vecMousePos, ProjMatrixInv);

	_vector	vRayDir, vRayPos;

	vRayPos = { 0.f, 0.f, 0.f , 1.f };
	vRayDir = vecMousePos - vRayPos;

	_matrix      ViewMatrixInv;
	ViewMatrixInv = pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW);
	ViewMatrixInv = XMMatrixInverse(nullptr, ViewMatrixInv);
	vRayPos = XMVector3TransformCoord(vRayPos, ViewMatrixInv);
	vRayDir = XMVector3TransformNormal(vRayDir, ViewMatrixInv);
	vRayDir = XMVector3Normalize(vRayDir);
	
	RELEASE_INSTANCE(CGameInstance);

	_float  fDist = 0.f;

	_uint iSize = m_Cells.size();

	for (_uint i = 0; i < iSize; ++i)
	{
		if (TriangleTests::Intersects(vRayPos, vRayDir,
			XMLoadFloat4(&_float4(m_Cells[i]->Get_Point(CCell::POINT_A).x, m_Cells[i]->Get_Point(CCell::POINT_A).y, m_Cells[i]->Get_Point(CCell::POINT_A).z, 1.f)),
			XMLoadFloat4(&_float4(m_Cells[i]->Get_Point(CCell::POINT_B).x, m_Cells[i]->Get_Point(CCell::POINT_B).y, m_Cells[i]->Get_Point(CCell::POINT_B).z, 1.f)),
			XMLoadFloat4(&_float4(m_Cells[i]->Get_Point(CCell::POINT_C).x, m_Cells[i]->Get_Point(CCell::POINT_C).y, m_Cells[i]->Get_Point(CCell::POINT_C).z, 1.f)),
			fDist))
		{
			return i;
		}
	}

	return -1;
}

HRESULT CNavigation::CreateCell(_float3 * pPoints)
{
	_float3		vPoints[CCell::POINT_END];

	vPoints[CCell::POINT_A] = pPoints[0];
	vPoints[CCell::POINT_B] = pPoints[1];
	vPoints[CCell::POINT_C] = pPoints[2];

	CCell*		pCell = CCell::Create(m_pDevice, m_pContext, vPoints, (_int)m_Cells.size());
	if (nullptr == pCell)
		return E_FAIL;

	m_Cells.push_back(pCell);

	if (FAILED(Ready_Neighbor()))
		return E_FAIL;

	return S_OK;
}

HRESULT CNavigation::DeleteCell(_uint iCellnum)
{
	if (iCellnum == m_Cells.size())
		m_Cells.pop_back();
	else
		m_Cells.erase(m_Cells.begin() + iCellnum);
	
	if (FAILED(Ready_Neighbor()))
		return E_FAIL;

	return S_OK;
}

HRESULT CNavigation::Save_Navigation(_uint iIndex)
{
	_tchar* pDataName = new _tchar[MAX_PATH];

	switch (iIndex)
	{
	case 2:
		lstrcpy(pDataName, TEXT("../Bin/Data/Navigation_CHAP1.dat"));
		break;
	case 3:
		lstrcpy(pDataName, TEXT("../Bin/Data/Navigation_CHAP2.dat"));
		break;
	case 4:
		lstrcpy(pDataName, TEXT("../Bin/Data/Navigation_CHAP3.dat"));
		break;
	}

	DWORD	dwByte = 0;

	HANDLE      hFile = CreateFile(pDataName
		, GENERIC_WRITE
		, 0
		, nullptr
		, CREATE_ALWAYS
		, FILE_ATTRIBUTE_NORMAL
		, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	for (auto& pCell : m_Cells)
	{
		WriteFile(hFile, &pCell->Get_Point(CCell::POINT_A), sizeof(_float3), &dwByte, nullptr);
		WriteFile(hFile, &pCell->Get_Point(CCell::POINT_B), sizeof(_float3), &dwByte, nullptr);
		WriteFile(hFile, &pCell->Get_Point(CCell::POINT_C), sizeof(_float3), &dwByte, nullptr);
	}

	MSG_BOX("Save_Complete!!");

	CloseHandle(hFile);
	Safe_Delete_Array(pDataName);
	return S_OK;
}

CCell * CNavigation::Get_Cell(_uint iIndex)
{
	if(iIndex < 0 || m_Cells.size() < iIndex)
		return nullptr;

	return m_Cells[iIndex];
}

#ifdef _DEBUG
HRESULT CNavigation::Render()
{
	_float		fHeight = 0.0f;

	if (-1 == m_NaviDesc.iCurrentIndex)
	{
		fHeight = 0.1f;
		HRESULT hr = m_pShader->Set_RawValue("g_fHeight", &fHeight, sizeof(_float));
		m_pShader->Set_RawValue("g_vColor", &_float4(0.f, 1.f, 0.f, 1.f), sizeof(_float4));
	}
	else
	{
		fHeight = 0.2f;
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

HRESULT CNavigation::Render_pickingCell(_int ipickingIndex)
{
	if (m_Cells.size() < ipickingIndex)
		return E_FAIL;

	if (m_Cells[ipickingIndex] != nullptr)
	{
		_float	fHeight = 0.2f;
		HRESULT hr = m_pShader->Set_RawValue("g_fHeight", &fHeight, sizeof(_float));
		m_pShader->Set_RawValue("g_vColor", &_float4(0.f, 0.f, 1.f, 1.f), sizeof(_float4));

		m_Cells[ipickingIndex]->Render(m_pShader);
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

HRESULT CNavigation::Update(const wstring pNavigationDataFilePath)
{
	DWORD	dwByte = 0;

	HANDLE	hFile = CreateFile(pNavigationDataFilePath.c_str(),
		GENERIC_READ,
		0,
		nullptr,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		0);

	if (hFile == INVALID_HANDLE_VALUE)
		return E_FAIL;

	for (auto& iter : m_Cells)
		Safe_Release(iter);

	m_Cells.clear();

	_float3		vPoints[CCell::POINT_END];

	while (true)
	{
		for (_uint i = 0; i < 3; ++i)
			ReadFile(hFile, &vPoints[i], sizeof(_float3), &dwByte, nullptr);

		if (dwByte == 0)
			break;

		CCell*		pCell = CCell::Create(m_pDevice, m_pContext, vPoints, (_int)m_Cells.size());

		if (pCell == nullptr)
			return E_FAIL;

		m_Cells.push_back(pCell);
	}

	CloseHandle(hFile);

	if (FAILED(Ready_Neighbor()))
		return E_FAIL;

	return S_OK;
}

_bool CNavigation::Set_CurreuntIndex(_fvector TargetPos)
{
	_uint iSize = m_Cells.size();

	for (_uint i = 0; i < iSize; ++i)
	{
		if (m_Cells[i]->isIn(TargetPos))
		{
			m_NaviDesc.iCurrentIndex = i;
			return true;
		}
	}
	return false;
}

void CNavigation::ResetCell()
{
	for (auto& pCell : m_Cells)
		Safe_Release(pCell);

	m_Cells.clear();
}

void CNavigation::DeleteRecentCell()
{
	Safe_Release(m_Cells.back());
	m_Cells.pop_back();

	Ready_Neighbor();
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
