#include "stdafx.h"
#include "..\public\Terrain.h"
#include "GameInstance.h"
#include "MathUtils.h"
#include "TestCube.h"
#include "Cell.h"

CTerrain::CTerrain(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CTerrain::CTerrain(const CTerrain & rhs)
	: CGameObject(rhs)
{

}

HRESULT CTerrain::Init_Prototype()
{
	if (FAILED(__super::Init_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT	CTerrain::Ready_FilterBuffer()
{
	if (g_LEVEL == LEVEL_CHAP1)
	{
		m_pPixel = new _ulong[128 * 128];

		for (_uint i = 0; i < 128; ++i)
		{
			for (_uint j = 0; j < 128; ++j)
			{
				_uint iIndex = i * 128 + j;
				m_pPixel[iIndex] = D3DCOLOR_ARGB(255, 255, 255, 255);
			}
		}
	}
	else if (g_LEVEL == LEVEL_CHAP2)
	{
		m_pPixel = new _ulong[512 * 512];

		for (_uint i = 0; i < 512; ++i)
		{
			for (_uint j = 0; j < 512; ++j)
			{
				_uint iIndex = i * 512 + j;
				m_pPixel[iIndex] = D3DCOLOR_ARGB(255, 255, 255, 255);
			}
		}
	}

	return S_OK;
}

HRESULT CTerrain::Dynamic_FilterBuffer()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (g_LEVEL == LEVEL_CHAP1)
	{
		ID3D11Texture2D*	pTexture2D = nullptr;

		D3D11_TEXTURE2D_DESC	TextureDesc;
		ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

		HRESULT hr = 0;

		TextureDesc.Width = 128;		// 2의 배수로 맞춰야됀다.
		TextureDesc.Height = 128;		// 2의 배수로 맞춰야됀다.
		TextureDesc.MipLevels = 1;
		TextureDesc.ArraySize = 1;
		TextureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		TextureDesc.SampleDesc.Quality = 0;
		TextureDesc.SampleDesc.Count = 1;

		TextureDesc.Usage = D3D11_USAGE_DYNAMIC;	// 동적으로 만들어야지 락 언락가능
		TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		TextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	// CPU는 동적할때 무조건
		TextureDesc.MiscFlags = 0;

		if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pTexture2D)))
			return E_FAIL;

		D3D11_MAPPED_SUBRESOURCE		SubResource;
		ZeroMemory(&SubResource, sizeof SubResource);

		m_pContext->Map(pTexture2D, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);  //DX_9 Lock ==Map

		memcpy(SubResource.pData, m_pPixel, (sizeof(_ulong) * TextureDesc.Width * TextureDesc.Height));

		m_pContext->Unmap(pTexture2D, 0);

		Safe_Release(m_pTextureCom[TYPE_FILTER]);
		Remove_Component(TEXT("Com_Filter"));

		hr = DirectX::SaveDDSTextureToFile(m_pContext, pTexture2D, TEXT("../Bin/Resources/Textures/Terrain/NewFilter.dds"));

		Safe_Release(pTexture2D);

		pGameInstance->Remove_ProtoComponent(pGameInstance->GetCurLevelIdx(), TEXT("Prototype_Component_Texture_Filter"));

		hr = (pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Texture_Filter"),
			CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/NewFilter.dds"), TYPE_FILTER)));

		hr = __super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Texture_Filter"), TEXT("Com_Filter"),
			(CComponent**)&m_pTextureCom[TYPE_FILTER]);

		if (FAILED(hr))
			return E_FAIL;
	}
	else if (g_LEVEL == LEVEL_CHAP2)
	{
		ID3D11Texture2D*	pTexture2D = nullptr;

		D3D11_TEXTURE2D_DESC	TextureDesc;
		ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

		HRESULT hr = 0;

		TextureDesc.Width = 512;		// 2의 배수로 맞춰야됀다.
		TextureDesc.Height = 512;		// 2의 배수로 맞춰야됀다.
		TextureDesc.MipLevels = 1;
		TextureDesc.ArraySize = 1;
		TextureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		TextureDesc.SampleDesc.Quality = 0;
		TextureDesc.SampleDesc.Count = 1;

		TextureDesc.Usage = D3D11_USAGE_DYNAMIC;	// 동적으로 만들어야지 락 언락가능
		TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		TextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	// CPU는 동적할때 무조건
		TextureDesc.MiscFlags = 0;

		if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pTexture2D)))
			return E_FAIL;

		D3D11_MAPPED_SUBRESOURCE		SubResource;
		ZeroMemory(&SubResource, sizeof SubResource);

		m_pContext->Map(pTexture2D, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);  //DX_9 Lock ==Map

		memcpy(SubResource.pData, m_pPixel, (sizeof(_ulong) * TextureDesc.Width * TextureDesc.Height));

		m_pContext->Unmap(pTexture2D, 0);

		Safe_Release(m_pTextureCom[TYPE_FILTER]);
		Remove_Component(TEXT("Com_Filter"));

		hr = DirectX::SaveDDSTextureToFile(m_pContext, pTexture2D, TEXT("../Bin/Resources/Textures/Terrain/Newfilter_Chap2_Change.dds"));

		Safe_Release(pTexture2D);

		pGameInstance->Remove_ProtoComponent(pGameInstance->GetCurLevelIdx(), TEXT("Prototype_Component_Texture_Filter"));


		hr = (pGameInstance->Add_Prototype(LEVEL_CHAP2, TEXT("Prototype_Component_Texture_Filter"),
			CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Newfilter_Chap2_Change.dds"), TYPE_FILTER)));

		hr = __super::Add_Component(LEVEL_CHAP2, TEXT("Prototype_Component_Texture_Filter"), TEXT("Com_Filter"),
			(CComponent**)&m_pTextureCom[TYPE_FILTER]);

		if (FAILED(hr))
			return E_FAIL;
	}
	else if (g_LEVEL == LEVEL_CHAP3)
	{
		ID3D11Texture2D*	pTexture2D = nullptr;

		D3D11_TEXTURE2D_DESC	TextureDesc;
		ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

		HRESULT hr = 0;

		TextureDesc.Width = 256;		// 2의 배수로 맞춰야됀다.
		TextureDesc.Height = 256;		// 2의 배수로 맞춰야됀다.
		TextureDesc.MipLevels = 1;
		TextureDesc.ArraySize = 1;
		TextureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		TextureDesc.SampleDesc.Quality = 0;
		TextureDesc.SampleDesc.Count = 1;

		TextureDesc.Usage = D3D11_USAGE_DYNAMIC;	// 동적으로 만들어야지 락 언락가능
		TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		TextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	// CPU는 동적할때 무조건
		TextureDesc.MiscFlags = 0;

		if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pTexture2D)))
			return E_FAIL;

		D3D11_MAPPED_SUBRESOURCE		SubResource;
		ZeroMemory(&SubResource, sizeof SubResource);

		m_pContext->Map(pTexture2D, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);  //DX_9 Lock ==Map

		memcpy(SubResource.pData, m_pPixel, (sizeof(_ulong) * TextureDesc.Width * TextureDesc.Height));

		m_pContext->Unmap(pTexture2D, 0);

		Safe_Release(m_pTextureCom[TYPE_FILTER]);
		Remove_Component(TEXT("Com_Filter"));

		hr = DirectX::SaveDDSTextureToFile(m_pContext, pTexture2D, TEXT("../Bin/Resources/Textures/Terrain/Newfilter_Chap3_Change.dds"));

		Safe_Release(pTexture2D);

		pGameInstance->Remove_ProtoComponent(pGameInstance->GetCurLevelIdx(), TEXT("Prototype_Component_Texture_Filter"));


		hr = (pGameInstance->Add_Prototype(LEVEL_CHAP3, TEXT("Prototype_Component_Texture_Filter"),
			CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Newfilter_Chap3_Change.dds"), TYPE_FILTER)));

		hr = __super::Add_Component(LEVEL_CHAP3, TEXT("Prototype_Component_Texture_Filter"), TEXT("Com_Filter"),
			(CComponent**)&m_pTextureCom[TYPE_FILTER]);

		if (FAILED(hr))
			return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CTerrain::Dynamic_Navi()
{
	/*
	DWORD	dwByte = 0;

	HANDLE		hFile = CreateFile(TEXT("../Bin/Data/Navigation.dat")
		, GENERIC_WRITE
		, 0
		, nullptr
		, CREATE_ALWAYS
		, FILE_ATTRIBUTE_NORMAL
		, 0);

	if (0 == hFile)
		return;

	for (auto& iter : m_NaviPosList)
		WriteFile(hFile, &iter, sizeof(_float3), &dwByte, nullptr);

	CloseHandle(hFile);
	*/
	if (FAILED(m_pNavigationCom->CreateCell(m_vPoints)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTerrain::Save_Navi(_int iIndex)
{
	if (FAILED(m_pNavigationCom->Save_Navigation(iIndex)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTerrain::Load_CubeList(_int iIndex)
{
	_tchar* pDataName = new _tchar[MAX_PATH];
	
	switch (iIndex)
	{
	case LEVEL_CHAP1:
		lstrcpy(pDataName, TEXT("../Bin/Data/Navigation_1_CUBE.dat"));
		break;
	case LEVEL_CHAP2:
		lstrcpy(pDataName, TEXT("../Bin/Data/Navigation_2_CUBE.dat"));
		break;
	case LEVEL_CHAP3:
		lstrcpy(pDataName, TEXT("../Bin/Data/Navigation_3_CUBE.dat"));
		break;
	}

	DWORD	dwByte = 0;

	HANDLE      hFile = CreateFile(pDataName
		, GENERIC_READ
		, FILE_SHARE_READ
		, 0
		, OPEN_EXISTING
		, FILE_ATTRIBUTE_NORMAL
		, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	_uint iSize = 0;
	ReadFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	for (_uint i = 0; i < iSize; ++i)
	{
		CTestCube* pTestCube = static_cast<CTestCube*>(pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Cube")));
		_vector vPos;
		ReadFile(hFile, &vPos, sizeof(_vector), &dwByte, nullptr);
		pTestCube->Get_TransformCom()->Set_State(CTransform::STATE_TRANSLATION, vPos);
		m_pCubeList.push_back(pTestCube);
	}

	RELEASE_INSTANCE(CGameInstance);

	MSG_BOX("Load_Complete!!");

	CloseHandle(hFile);

	Safe_Delete_Array(pDataName);

	return S_OK;
}

HRESULT CTerrain::Save_CubeList(_int iIndex)
{
	_tchar* pDataName = new _tchar[MAX_PATH];

	switch (iIndex)
	{
	case LEVEL_CHAP1:
		lstrcpy(pDataName, TEXT("../Bin/Data/Navigation_1_CUBE.dat"));
		break;
	case LEVEL_CHAP2:
		lstrcpy(pDataName, TEXT("../Bin/Data/Navigation_2_CUBE.dat"));
		break;
	case LEVEL_CHAP3:
		lstrcpy(pDataName, TEXT("../Bin/Data/Navigation_3_CUBE.dat"));
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

	_uint i = 0;
	i = static_cast<_uint>(m_pCubeList.size());
	WriteFile(hFile, &i, sizeof(_uint), &dwByte, nullptr);

	for (auto& pCube : m_pCubeList)
	{
		_vector cubepos = pCube->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION);
		WriteFile(hFile, &cubepos, sizeof(_vector), &dwByte, nullptr);
	}

	MSG_BOX("Save_Complete!!");

	CloseHandle(hFile);

	Safe_Delete_Array(pDataName);

	return S_OK;
}

void CTerrain::AdjustCellPoint()
{
	//함수가 실행됐다는 건 배열이 다 찼다는 의미.
	const _vector vPointA = XMLoadFloat3(&m_vPoints[0]);
	const _vector vPointB = XMLoadFloat3(&m_vPoints[1]);
	const _vector vPointC = XMLoadFloat3(&m_vPoints[2]);

	const _vector vAB = vPointB - vPointA;
	const _vector vBC = vPointC - vPointB;

	_float3 vCross;
	XMStoreFloat3(&vCross, XMVector3Cross(vAB, vBC));

	//// 수직 안되
	//if (CMathUtils::FloatCmp(vCross.y, 0.f))
	//	return false;

	// 반시계면 변경
	if (vCross.y < 0)
	{
		_float3 tmp = m_vPoints[1];
		m_vPoints[1] = m_vPoints[2];
		m_vPoints[2] = tmp;
	}
}

void CTerrain::Set_Brush(const _float4 & vPos, const _float & fRange)
{
	m_vBrushPos = vPos;
	m_fBrushRange = fRange;
}

void CTerrain::Add_NaviCell(HWND hWnd, _uint iWinsizeX, _uint iWinsizey, const class CVIBuffer_Terrain* pTerrainBufferCom, const class CTransform* pTerrainTransformCom)
{
	POINT		ptMouse{};

	GetCursorPos(&ptMouse);
	ScreenToClient(hWnd, &ptMouse);

	_float3		fPoint;

	fPoint.x = _float(ptMouse.x / (iWinsizeX * 0.5f) - 1.f);
	fPoint.y = _float(ptMouse.y / -(iWinsizey * 0.5f) + 1.f);
	fPoint.z = 1.f;

	_vector		vecPoint = XMLoadFloat3(&fPoint);

	vecPoint = XMVectorSetW(vecPoint, 1.f);

	CGameInstance*  pGameInstance = GET_INSTANCE(CGameInstance);

	_matrix		matProj;

	matProj = pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ);

	matProj = XMMatrixInverse(nullptr, matProj);

	vecPoint = XMVector3TransformCoord(vecPoint, matProj);

	_float3 fRayPos = { 0.f, 0.f, 0.f };

	_vector		vecRayPos = XMLoadFloat3(&fRayPos);

	vecRayPos = XMVectorSetW(vecRayPos, 1.f);
	
	_vector		vecDir = vecPoint - vecRayPos;

	_matrix		matView;

	matView = pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW);

	matView = XMMatrixInverse(nullptr, matView);
	vecRayPos = XMVector3TransformCoord(vecRayPos, matView); // XMVector3TransformCoord입력 벡터의 w 구성요소를 무시하고 값 1.0을 대신 사용합니다. 반환된 벡터의 w 구성 요소는 항상 1.0입니다.
	vecDir = XMVector3TransformNormal(vecDir, matView); // XMVector3TransformNormal 는 회전 및 크기 조정을 위해 입력 행렬 행 0, 1 및 2를 사용하여 변환을 수행하고 행 3을 무시합니다.

	_matrix		matWorld = pTerrainTransformCom->Get_WorldMatrix();

	matWorld = XMMatrixInverse(nullptr, matWorld);
	vecRayPos = XMVector3TransformCoord(vecRayPos, matWorld);
	vecDir = XMVector3Normalize(XMVector3TransformNormal(vecDir, matWorld));

	_vector	fTest = vecRayPos;
	_vector	fNorDir = vecDir;

	const _float3*	pTerrainVtx = pTerrainBufferCom->Get_VtxPos();

	_ulong		dwVtxCntX = pTerrainBufferCom->Get_VtxCntX();
	_ulong		dwVtxCntZ = pTerrainBufferCom->Get_VtxCntZ();

	_ulong	dwVtxIdx[3]{};
	_float	fU = 0.f;
	_float	fV = 0.f;
	_float  fDist = 0.f;

	_uint iCount = 0;

	for (auto& iter : m_pCubeList)
	{
		if (static_cast<CTestCube*>(iter)->Peeking())
		{
			_vector vImshi = iter->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION);

			m_vPoints[m_iNaviPointCount] = _float3(XMVectorGetX(vImshi), XMVectorGetY(vImshi), XMVectorGetZ(vImshi));

			++m_iNaviPointCount;

			if (m_iNaviPointCount != 3)
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}

		}
		else
			++iCount;
	}

	if (iCount == m_pCubeList.size())
	{
		for (_ulong i = 0; i < dwVtxCntZ - 1; ++i)
		{
			for (_ulong j = 0; j < dwVtxCntX - 1; ++j)
			{
				_ulong dwIndex = i * dwVtxCntX + j;


				dwVtxIdx[0] = dwIndex + dwVtxCntX;
				dwVtxIdx[1] = dwIndex + dwVtxCntX + 1;
				dwVtxIdx[2] = dwIndex + 1;

				//  FXMVECTOR Origin, FXMVECTOR Direction, FXMVECTOR V0, GXMVECTOR V1, HXMVECTOR V2, float& Dist
				// Origin, Direction, Triangle initial vector 3 pack, cross distance
				if (TriangleTests::Intersects(vecRayPos, vecDir,
					XMLoadFloat3(&pTerrainVtx[dwVtxIdx[1]]),
					XMLoadFloat3(&pTerrainVtx[dwVtxIdx[0]]),
					XMLoadFloat3(&pTerrainVtx[dwVtxIdx[2]]),
					fDist))
				{
					fTest += fNorDir * fDist;
					fTest = XMVectorSetW(fTest, 1.f);
					CTestCube* pTestCube = static_cast<CTestCube*>(pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Cube")));
					CTransform* pTestCubeTrans = dynamic_cast<CTransform*>(pTestCube->Get_TransformCom());
					pTestCubeTrans->Set_State(CTransform::STATE_TRANSLATION, fTest);
					m_vPoints[m_iNaviPointCount] = _float3(XMVectorGetX(fTest), XMVectorGetY(fTest), XMVectorGetZ(fTest));
					m_pCubeList.push_back(pTestCube);

					++m_iNaviPointCount;

					break;
				}

				dwVtxIdx[0] = dwIndex + dwVtxCntX;
				dwVtxIdx[1] = dwIndex + 1;
				dwVtxIdx[2] = dwIndex;

				if (TriangleTests::Intersects(vecRayPos, vecDir,
					XMLoadFloat3(&pTerrainVtx[dwVtxIdx[2]]),
					XMLoadFloat3(&pTerrainVtx[dwVtxIdx[1]]),
					XMLoadFloat3(&pTerrainVtx[dwVtxIdx[0]]),
					fDist))
				{
					fTest += fNorDir * fDist;
					fTest = XMVectorSetW(fTest, 1.f);
					CTestCube* pTestCube = static_cast<CTestCube*>(pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Cube")));
					CTransform* pTestCubeTrans = dynamic_cast<CTransform*>(pTestCube->Get_TransformCom());
					
					pTestCubeTrans->Set_State(CTransform::STATE_TRANSLATION, fTest);
					m_vPoints[m_iNaviPointCount] = _float3(XMVectorGetX(fTest), XMVectorGetY(fTest), XMVectorGetZ(fTest));
					m_pCubeList.push_back(pTestCube);

					++m_iNaviPointCount;

					break;
				}
			}
		}
	}
	
	RELEASE_INSTANCE(CGameInstance);

	return;
}

HRESULT CTerrain::Init(void * pArg)
{
	if (FAILED(__super::Init(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	Ready_FilterBuffer();

	m_iDiffuseTexCnt = m_pTextureCom[TYPE_DIFFUSE]->Get_CntTex();
	
	m_strObjName = L"Terrain";

	return S_OK;
}

void CTerrain::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	for (auto iter : m_pCubeList)
		iter->Tick(TimeDelta);

	if (m_bPicking && m_bHeight)
	{
		m_vBrushPos = PickingOnTerrain(m_pVIBufferCom, m_pTransformCom);
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		if(pGameInstance->Get_DIMouseState(DIM_LB) && pGameInstance->Get_DIKeyState(DIK_LALT))
			m_pVIBufferCom->DynamicBufferControlForSave(m_vBrushPos, m_fBrushRange, static_cast<unsigned char>(m_fHeight), TimeDelta , m_bDefaultHeight);
		RELEASE_INSTANCE(CGameInstance);
	}
	else if (m_bPicking && m_bFilter)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		if (pGameInstance->Get_DIMouseState(DIM_LB))
		{
			PickingForFilter(m_pVIBufferCom, m_pTransformCom);
			Dynamic_FilterBuffer();
		}
		RELEASE_INSTANCE(CGameInstance);
	}
	else if (m_bNavi)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		if (pGameInstance->Mouse_Down(DIM_LB) && pGameInstance->Key_Pressing(DIK_R))
		{
			Add_NaviCell(g_hWnd, g_iWinSizeX, g_iWinSizeY, m_pVIBufferCom, m_pTransformCom);
		}
		RELEASE_INSTANCE(CGameInstance);
	}

	if(g_LEVEL == LEVEL_CHAP3)
		m_fTimeDelta += static_cast<float>(TimeDelta * m_fWaveTime);
}

void CTerrain::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	for (auto iter : m_pCubeList)
		iter->Late_Tick(TimeDelta);

	m_pVIBufferCom->Culling(m_pTransformCom->Get_WorldMatrix());

	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

#ifdef _DEBUG
		m_pRendererCom->Add_DebugRenderGroup(m_pNavigationCom);
#endif

	}
}

HRESULT CTerrain::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);
	
	m_pVIBufferCom->Render();

#ifdef _DEBUG
	for (auto& iter : m_pCubeList)
		iter->Render();
	
	if (m_pNavigationCom)
	{
		if (m_iDeleteCellNum != -1)
			m_pNavigationCom->Render_pickingCell(m_iDeleteCellNum);
	}
#endif

	return S_OK;
}

void CTerrain::Imgui_RenderProperty()
{
	if (ImGui::CollapsingHeader("For. Texture"))
	{
		ImGui::Text("TYPE_SHADERPASS : ");	ImGui::SameLine();
		ImGui::Text(to_string(m_iPassNum).c_str());
		ImGui::SameLine();
		ImGui::Text(" / ");
		ImGui::SameLine();
		ImGui::Text(to_string(m_pShaderCom->Get_NumPasses()).c_str());
		ImGui::RadioButton("SOLID",  &m_iPassNum, 0); ImGui::SameLine();
		ImGui::RadioButton("WIRE" ,  &m_iPassNum, 1);

		ImGui::Text("TYPE_DIFFUSE : ");	ImGui::SameLine();
		ImGui::RadioButton("DIFFUSEA", &m_iTexRadioBtn, 0); ImGui::SameLine();
		ImGui::RadioButton("DIFFUSEB", &m_iTexRadioBtn, 1); ImGui::SameLine();

		if(m_iTexRadioBtn == 0)
			ImGui::Text(to_string(m_iDiffuseATexNum).c_str());
		else if (m_iTexRadioBtn == 1)
			ImGui::Text(to_string(m_iDiffuseBTexNum).c_str());
		ImGui::SameLine();
		ImGui::Text(" / ");
		ImGui::SameLine();
		ImGui::Text(to_string(m_pTextureCom[TYPE_DIFFUSE]->Get_CntTex()).c_str());
			
		for (_uint i = 0; i < m_pTextureCom[TYPE_DIFFUSE]->Get_CntTex(); ++i)
		{
			if (ImGui::ImageButton((void*)m_pTextureCom[TYPE_DIFFUSE]->Get_Texture(i), ImVec2(60.f, 60.f)))
			{
				if (m_iTexRadioBtn == 0)
					m_iDiffuseATexNum = i;
				else if (m_iTexRadioBtn == 1)
					m_iDiffuseBTexNum = i;
			}

			if (i == 0 || (i + 1) % 6)
				ImGui::SameLine();
		}

		ImGui::NewLine();

		ImGui::Text("TYPE_BRUSH : ");
		ImGui::SameLine();
		ImGui::Text(to_string(m_iBrushTexNum).c_str());
		ImGui::SameLine();
		ImGui::Text(" / ");
		ImGui::SameLine();
		ImGui::Text(to_string(m_pTextureCom[TYPE_BRUSH]->Get_CntTex()).c_str());

		for (_uint i = 0; i < m_pTextureCom[TYPE_BRUSH]->Get_CntTex(); ++i)
		{
			if (ImGui::ImageButton((void*)m_pTextureCom[TYPE_BRUSH]->Get_Texture(i), ImVec2(60.f, 60.f)))
				m_iBrushTexNum = i;

			if (i == 0 || (i + 1) % 6)
				ImGui::SameLine();
		}

		ImGui::NewLine();

		ImGui::Text("TYPE_FILTER : ");
		ImGui::SameLine();
		ImGui::Text(to_string(m_iFilterTexNum).c_str());
		ImGui::SameLine();
		ImGui::Text(" / ");
		ImGui::SameLine();
		ImGui::Text(to_string(m_pTextureCom[TYPE_FILTER]->Get_CntTex()).c_str());

		for (_uint i = 0; i < m_pTextureCom[TYPE_FILTER]->Get_CntTex(); ++i)
		{
			if (ImGui::ImageButton((void*)m_pTextureCom[TYPE_FILTER]->Get_Texture(i), ImVec2(60.f, 60.f)))
				m_iFilterTexNum = i;

			if (i == 0 || (i + 1) % 6)
				ImGui::SameLine();
		}

		ImGui::NewLine();
	}
	if (ImGui::CollapsingHeader("For. TexPos"))
	{
		ImGui::Text("BrushPosition");
		float fBrushPos[3] = { m_vBrushPos.x ,m_vBrushPos.y ,m_vBrushPos.z };
		ImGui::DragFloat3("Brush_Pos", fBrushPos, 0.1f, -1000.0f, 1000.0f);
		m_vBrushPos.x = fBrushPos[0];
		m_vBrushPos.y = fBrushPos[1];
		m_vBrushPos.z = fBrushPos[2];
		ImGui::DragFloat("Brush_Range", &m_fBrushRange, 0.1f, 0.0f, 50.0f);

		ImGui::Checkbox("IsPicking", &m_bPicking); ImGui::SameLine();
		ImGui::Checkbox("Height", &m_bHeight);
		ImGui::Checkbox("IsDefaultHeight", &m_bDefaultHeight);
		ImGui::DragFloat("HeightY", &m_fHeight, 0.1f, 0.0f, 255.0f);
		if (ImGui::Button("SaveHeight"))
			m_pVIBufferCom->SaveHeightMap();
		ImGui::Checkbox("Filter", &m_bFilter);
	}
	if (m_pNavigationCom)
	{
		if (ImGui::CollapsingHeader("For. Navi"))
		{
			ImGui::Checkbox("Navi", &m_bNavi);

			if (!m_bDeleteCell)
			{
				_uint i = m_pNavigationCom->isPicking_NaviCell(g_hWnd, g_iWinSizeX, g_iWinSizeY);
				m_iDeleteCellNum = i;
			}

			if (ImGui::IsMouseClicked(1))
			{
				m_bDeleteCell = true;
			}

			ImGui::Text("DeleteCell? -> "); ImGui::SameLine();
			ImGui::Text("%d", m_iDeleteCellNum);

			if (ImGui::Button("DeleteCell"))
				m_pNavigationCom->DeleteCell(m_iDeleteCellNum);

			if (ImGui::Button("ResetPicking"))
				m_bDeleteCell = false;

			if (m_iNaviPointCount >= 3)
			{
				for (auto& pCube : m_pCubeList)
				{
					if (XMVector3Equal(pCube->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION), XMLoadFloat3(&m_vPoints[0])))
					{
						ImGui::Begin("Point_A");
						static_cast<CTestCube*>(pCube)->Imgui_Transform(&m_vPoints[0]);
						ImGui::End();
					}
					if (XMVector3Equal(pCube->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION), XMLoadFloat3(&m_vPoints[1])))
					{
						ImGui::Begin("Point_B");
						static_cast<CTestCube*>(pCube)->Imgui_Transform(&m_vPoints[1]);
						ImGui::End();
					}
					if (XMVector3Equal(pCube->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION), XMLoadFloat3(&m_vPoints[2])))
					{
						ImGui::Begin("Point_C");
						static_cast<CTestCube*>(pCube)->Imgui_Transform(&m_vPoints[2]);
						ImGui::End();
					}
				}

				if (ImGui::Button("RealSetUp"))
				{
					AdjustCellPoint();
					Dynamic_Navi();
					m_iNaviPointCount = 0;
				}
			}

			ImGui::RadioButton("CHAP_ONE", &m_iChapNum, 2); ImGui::SameLine();
			ImGui::RadioButton("CHAP_TWO", &m_iChapNum, 3); ImGui::SameLine();
			ImGui::RadioButton("CHAP_THREE", &m_iChapNum, 4);

			if (ImGui::Button("SaveNavi"))
			{
				// 저장할떄 큐브도 저장하자
				Save_Navi(m_iChapNum);
				Save_CubeList(m_iChapNum);
			}

			if (ImGui::Button("LoadCube"))
			{
				Load_CubeList(m_iChapNum);
			}

			ImGui::NewLine();

			if (ImGui::Button("DeleteRecentCell"))
			{
				m_pNavigationCom->DeleteRecentCell();
				m_iNaviPointCount = 0;
			}

			ImGui::NewLine();
			ImGui::NewLine();
			ImGui::NewLine();

			if (m_pNavigationCom->Get_Cell(m_iDeleteCellNum) != nullptr)
			{
				CCell* pCell = m_pNavigationCom->Get_Cell(m_iDeleteCellNum);
				if (pCell != nullptr)
				{
					_float vA[3];
					vA[0] = pCell->Get_Point(CCell::POINT_A).x;
					vA[1] = pCell->Get_Point(CCell::POINT_A).y;
					vA[2] = pCell->Get_Point(CCell::POINT_A).z;
					_float vB[3];
					vB[0] = pCell->Get_Point(CCell::POINT_B).x;
					vB[1] = pCell->Get_Point(CCell::POINT_B).y;
					vB[2] = pCell->Get_Point(CCell::POINT_B).z;
					_float vC[3];
					vC[0] = pCell->Get_Point(CCell::POINT_C).x;
					vC[1] = pCell->Get_Point(CCell::POINT_C).y;
					vC[2] = pCell->Get_Point(CCell::POINT_C).z;

					ImGui::InputFloat3("vA", vA);
					ImGui::InputFloat3("vB", vB);
					ImGui::InputFloat3("vC", vC);
				}
			}

			ImGui::NewLine();

			if (ImGui::Button("Reset"))
			{
				m_pNavigationCom->ResetCell();
			}
		}
	}
}

_float4 CTerrain::PickingOnTerrain(const CVIBuffer_Terrain * pTerrainBufferCom, const CTransform * pTerrainTransformCom)
{
	POINT		ptMouse{};

	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	_float3         vMousePos;

	vMousePos.x = _float(ptMouse.x / (g_iWinSizeX * 0.5f) - 1);
	vMousePos.y = _float(ptMouse.y / (g_iWinSizeY * -0.5f) + 1);
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
	
	vRayPos = XMVector3TransformCoord(vRayPos, pTerrainTransformCom->Get_WorldMatrixInverse());
	vRayDir = XMVector3TransformNormal(vRayDir, pTerrainTransformCom->Get_WorldMatrixInverse());
	
	RELEASE_INSTANCE(CGameInstance);

	const _float3* pTerrainVtx = pTerrainBufferCom->Get_VtxPos();

	_uint iVtxCntX = pTerrainBufferCom->Get_VtxCntX();
	_uint iVtxCntZ = pTerrainBufferCom->Get_VtxCntZ();

	_uint iVtxIdx[3]{};
	_float fDist;

	for (_uint i = 0; i < iVtxCntZ - 1; ++i)
	{
		for (_uint j = 0; j < iVtxCntX - 1; ++j)
		{
			_ulong dwIndex = i * iVtxCntX + j;

			// 오른쪽 위
			iVtxIdx[0] = dwIndex + iVtxCntX;
			iVtxIdx[1] = dwIndex + iVtxCntX + 1;
			iVtxIdx[2] = dwIndex + 1;

			if (TriangleTests::Intersects(vRayPos, vRayDir, XMLoadFloat3(&pTerrainVtx[iVtxIdx[1]]),
				XMLoadFloat3(&pTerrainVtx[iVtxIdx[0]]),
				XMLoadFloat3(&pTerrainVtx[iVtxIdx[2]]), fDist))
			{
				return _float4(pTerrainVtx[iVtxIdx[1]].x + (pTerrainVtx[iVtxIdx[0]].x - pTerrainVtx[iVtxIdx[1]].x),
					0.f,
					pTerrainVtx[iVtxIdx[1]].z + (pTerrainVtx[iVtxIdx[2]].z - pTerrainVtx[iVtxIdx[1]].z),
					1.f);
			}

			// 왼쪽 아래
			iVtxIdx[0] = dwIndex + iVtxCntX;
			iVtxIdx[1] = dwIndex + 1;
			iVtxIdx[2] = dwIndex;

			if (TriangleTests::Intersects(vRayPos, vRayDir, XMLoadFloat3(&pTerrainVtx[iVtxIdx[2]]),
				XMLoadFloat3(&pTerrainVtx[iVtxIdx[1]]),
				XMLoadFloat3(&pTerrainVtx[iVtxIdx[0]]), fDist))
			{
				return _float4(pTerrainVtx[iVtxIdx[2]].x + (pTerrainVtx[iVtxIdx[1]].x - pTerrainVtx[iVtxIdx[2]].x),
					0.f,
					pTerrainVtx[iVtxIdx[2]].z + (pTerrainVtx[iVtxIdx[0]].z - pTerrainVtx[iVtxIdx[2]].z),
					1.f);
			}
		}
	}

	return _float4();
}

_bool CTerrain::PickingForFilter(const CVIBuffer_Terrain * pTerrainBufferCom, const CTransform * pTerrainTransformCom)
{
	POINT		ptMouse{};

	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	_float3         vMousePos;

	vMousePos.x = _float(ptMouse.x / (g_iWinSizeX * 0.5f) - 1);
	vMousePos.y = _float(ptMouse.y / (g_iWinSizeY * -0.5f) + 1);
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

	vRayPos = XMVector3TransformCoord(vRayPos, pTerrainTransformCom->Get_WorldMatrixInverse());
	vRayDir = XMVector3TransformNormal(vRayDir, pTerrainTransformCom->Get_WorldMatrixInverse());

	RELEASE_INSTANCE(CGameInstance);

	const _float3* pTerrainVtx = pTerrainBufferCom->Get_VtxPos();

	_uint iVtxCntX = pTerrainBufferCom->Get_VtxCntX();
	_uint iVtxCntZ = pTerrainBufferCom->Get_VtxCntZ();

	_uint iVtxIdx[3]{};
	_float fDist;

	for (_uint i = 0; i < iVtxCntZ - 1; ++i)
	{
		for (_uint j = 0; j < iVtxCntX - 1; ++j)
		{
			_ulong dwIndex = i * iVtxCntX + j;

			// 오른쪽 위
			iVtxIdx[0] = dwIndex + iVtxCntX;
			iVtxIdx[1] = dwIndex + iVtxCntX + 1;
			iVtxIdx[2] = dwIndex + 1;

			if (TriangleTests::Intersects(vRayPos, vRayDir, XMLoadFloat3(&pTerrainVtx[iVtxIdx[1]]),
				XMLoadFloat3(&pTerrainVtx[iVtxIdx[0]]),
				XMLoadFloat3(&pTerrainVtx[iVtxIdx[2]]), fDist))
			{
				 _float4 vPos = _float4(pTerrainVtx[iVtxIdx[1]].x + (pTerrainVtx[iVtxIdx[0]].x - pTerrainVtx[iVtxIdx[1]].x),
					0.f,
					pTerrainVtx[iVtxIdx[1]].z + (pTerrainVtx[iVtxIdx[2]].z - pTerrainVtx[iVtxIdx[1]].z),
					1.f);
				
				 _uint iResult = 0;
				
				 if(g_LEVEL == LEVEL_CHAP1)
					iResult = (static_cast<_uint>(vPos.z) * 128) + (static_cast<_uint>(vPos.x));
				 else if (g_LEVEL == LEVEL_CHAP2)
					 iResult = (static_cast<_uint>(vPos.z) * 512) + (static_cast<_uint>(vPos.x));
				 else if (g_LEVEL == LEVEL_CHAP3)
					 iResult = (static_cast<_uint>(vPos.z) * 256) + (static_cast<_uint>(vPos.x));
	                 
				 m_pPixel[iResult] = 0;
				 return true;
			}

			// 왼쪽 아래
			iVtxIdx[0] = dwIndex + iVtxCntX;
			iVtxIdx[1] = dwIndex + 1;
			iVtxIdx[2] = dwIndex;

			if (TriangleTests::Intersects(vRayPos, vRayDir, XMLoadFloat3(&pTerrainVtx[iVtxIdx[2]]),
				XMLoadFloat3(&pTerrainVtx[iVtxIdx[1]]),
				XMLoadFloat3(&pTerrainVtx[iVtxIdx[0]]), fDist))
			{
				_float4 vPos = _float4(pTerrainVtx[iVtxIdx[2]].x + (pTerrainVtx[iVtxIdx[1]].x - pTerrainVtx[iVtxIdx[2]].x),
					0.f,
					pTerrainVtx[iVtxIdx[2]].z + (pTerrainVtx[iVtxIdx[0]].z - pTerrainVtx[iVtxIdx[2]].z),
					1.f);
				
				_uint iResult = 0;

				if (g_LEVEL == LEVEL_CHAP1)
					iResult = (static_cast<_uint>(vPos.z) * 128) + (static_cast<_uint>(vPos.x));
				else if (g_LEVEL == LEVEL_CHAP2)
					iResult = (static_cast<_uint>(vPos.z) * 512) + (static_cast<_uint>(vPos.x));
				else if (g_LEVEL == LEVEL_CHAP3)
					iResult = (static_cast<_uint>(vPos.z) * 256) + (static_cast<_uint>(vPos.x));

				m_pPixel[iResult] = 0;
				return true;
			}
		}
	}

	return false;
}

HRESULT CTerrain::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxNorTex"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (g_LEVEL == LEVEL_CHAP1)
	{
		/* For.Com_VIBuffer */
		if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_VIBuffer_Terrain"), TEXT("Com_VIBuffer"),
			(CComponent**)&m_pVIBufferCom)))
			return E_FAIL;
		
		/* For.Com_Texture */
		if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Texture_Terrain"), TEXT("Com_Texture"),
			(CComponent**)&m_pTextureCom[TYPE_DIFFUSE])))
			return E_FAIL;

		/* For.Com_Brush*/
		if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Texture_Brush"), TEXT("Com_Brush"),
			(CComponent**)&m_pTextureCom[TYPE_BRUSH])))
			return E_FAIL;

		/* For.Com_Filter*/
		if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Texture_Filter"), TEXT("Com_Filter"),
			(CComponent**)&m_pTextureCom[TYPE_FILTER])))
			return E_FAIL;

		/* For.Com_Navigation */
		if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"),
			(CComponent**)&m_pNavigationCom)))
			return E_FAIL;

		m_iDiffuseATexNum = 3;
		m_iDiffuseBTexNum = 4;
	}
	else if (g_LEVEL == LEVEL_CHAP2)
	{
		/* For.Com_VIBuffer */
		if (FAILED(__super::Add_Component(LEVEL_CHAP2, TEXT("Prototype_Component_VIBuffer_Terrain"), TEXT("Com_VIBuffer"),
			(CComponent**)&m_pVIBufferCom)))
			return E_FAIL;

		/* For.Com_Texture */
		if (FAILED(__super::Add_Component(LEVEL_CHAP2, TEXT("Prototype_Component_Texture_Terrain"), TEXT("Com_Texture"),
			(CComponent**)&m_pTextureCom[TYPE_DIFFUSE])))
			return E_FAIL;

		/* For.Com_Brush*/
		if (FAILED(__super::Add_Component(LEVEL_CHAP2, TEXT("Prototype_Component_Texture_Brush"), TEXT("Com_Brush"),
			(CComponent**)&m_pTextureCom[TYPE_BRUSH])))
			return E_FAIL;

		/* For.Com_Filter*/
		if (FAILED(__super::Add_Component(LEVEL_CHAP2, TEXT("Prototype_Component_Texture_Filter"), TEXT("Com_Filter"),
			(CComponent**)&m_pTextureCom[TYPE_FILTER])))
			return E_FAIL;

		/* For.Com_Navigation */
		if (FAILED(__super::Add_Component(LEVEL_CHAP2, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"),
			(CComponent**)&m_pNavigationCom)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CTerrain::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Matrix("g_WorldMatrix", &m_pTransformCom->Get_World4x4())))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	if (FAILED(m_pTextureCom[TYPE_DIFFUSE]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTextureA", m_iDiffuseATexNum)))
		return E_FAIL;

	if (FAILED(m_pTextureCom[TYPE_DIFFUSE]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTextureB", m_iDiffuseBTexNum)))
		return E_FAIL;

	if (FAILED(m_pTextureCom[TYPE_BRUSH]->Bind_ShaderResource(m_pShaderCom, "g_BrushTexture", m_iBrushTexNum)))
		return E_FAIL;

	if (FAILED(m_pTextureCom[TYPE_FILTER]->Bind_ShaderResource(m_pShaderCom, "g_FilterTexture", m_iFilterTexNum)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_fBrushRange", &m_fBrushRange, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_vBrushPos", &m_vBrushPos, sizeof(_float4))))
		return E_FAIL;
	

	return S_OK;
}

CTerrain * CTerrain::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTerrain*		pInstance = new CTerrain(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Created : CTerrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CTerrain::Clone(void * pArg)
{
	CTerrain*		pInstance = new CTerrain(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTerrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTerrain::Free()
{
	__super::Free();

	for (auto& pTexture : m_pTextureCom)
		Safe_Release(pTexture);
	
	if (m_bClone)
		Safe_Delete_Array(m_pPixel);

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

	for (auto& pTestCube : m_pCubeList)
		Safe_Release(pTestCube);

	m_pCubeList.clear();
}
