#include "stdafx.h"
#include "..\public\Terrain.h"
#include "GameInstance.h"
#include "MathUtils.h"

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

HRESULT CTerrain::Init(void * pArg)
{
	if (FAILED(__super::Init(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	Ready_FilterBuffer();

	m_iDiffuseTexCnt = m_pTextureCom[TYPE_DIFFUSE]->Get_CntTex();

	return S_OK;
}

void CTerrain::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	if (m_bPicking)
	{
		m_vBrushPos = PickingOnTerrain(m_pVIBufferCom, m_pTransformCom);
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		
		if (m_bHeight)
		{
			if (pGameInstance->Get_DIMouseState(DIM_LB) && pGameInstance->Get_DIKeyState(DIK_LALT))
				m_pVIBufferCom->DynamicBufferControlForSave(m_vBrushPos, m_fBrushRange, static_cast<unsigned char>(m_fHeight), TimeDelta, m_bDefaultHeight);
		}

		if (m_bFilter)
		{
			if (pGameInstance->Get_DIMouseState(DIM_LB) && pGameInstance->Get_DIKeyState(DIK_LALT))
				DynanicBuffer_ForBrush();
		}
		
		RELEASE_INSTANCE(CGameInstance);
	}
}

void CTerrain::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_PRIORITY, this);
}

HRESULT CTerrain::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(m_iPassNum);

	m_pVIBufferCom->Render();

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

		ImGui::Checkbox("IsPicking", &m_bPicking); 

		ImGui::Checkbox("Height", &m_bHeight);  ImGui::SameLine();
		ImGui::Checkbox("Filter", &m_bFilter);

		ImGui::Checkbox("IsDefaultHeight", &m_bDefaultHeight);
		ImGui::DragFloat("HeightY", &m_fHeight, 0.1f, 0.0f, 255.0f);
		if (ImGui::Button("SaveHeight"))
			m_pVIBufferCom->SaveHeightMap();

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

HRESULT	CTerrain::Ready_FilterBuffer()
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
	
	return S_OK;
}

HRESULT CTerrain::DynanicBuffer_ForBrush()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	HRESULT hr = 0;

	// 아 필터 이미지는 저기 위에서부터 칠해주는데
	// 버텍스는 반대로 그려주고있구나

	_uint iResult = (static_cast<_uint>(m_vBrushPos.z) * 128) + (static_cast<_uint>(m_vBrushPos.x));
	m_pPixel[iResult] = D3DCOLOR_ARGB(255, 0, 0, 0);

	ZeroMemory(&m_TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	m_TextureDesc.Width = 128;		// 2의 배수로 맞춰야됀다.
	m_TextureDesc.Height = 128;		// 2의 배수로 맞춰야됀다.
	m_TextureDesc.MipLevels = 1;
	m_TextureDesc.ArraySize = 1;
	m_TextureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	m_TextureDesc.SampleDesc.Quality = 0;
	m_TextureDesc.SampleDesc.Count = 1;

	m_TextureDesc.Usage = D3D11_USAGE_DYNAMIC;	// 동적으로 만들어야지 락 언락가능
	m_TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	m_TextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	// CPU는 동적할때 무조건
	m_TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&m_TextureDesc, nullptr, &m_pTexture2D)))
		return E_FAIL;

	D3D11_MAPPED_SUBRESOURCE		SubResource;
	ZeroMemory(&SubResource, sizeof SubResource);

	m_pContext->Map(m_pTexture2D, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);  //DX_9 Lock ==Map

	memcpy(SubResource.pData, m_pPixel, (sizeof(_ulong) *m_TextureDesc.Width * m_TextureDesc.Height));

	m_pContext->Unmap(m_pTexture2D, 0);

	Safe_Release(m_pTextureCom[TYPE_FILTER]);

	Remove_Component(TEXT("Com_Filter"));

	hr = DirectX::SaveDDSTextureToFile(m_pContext, m_pTexture2D, TEXT("../Bin/Resources/Textures/Terrain/Filter.dds"));

	Safe_Release(m_pTexture2D);

	if (FAILED(pGameInstance->Remove_Prototype(pGameInstance->GetCurLevelIdx(), TEXT("Prototype_Component_Texture_CustomFilter"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Texture_CustomFilter"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Filter.dds"), 1))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Texture_CustomFilter"), TEXT("Com_Filter"),
		(CComponent**)&m_pTextureCom[TYPE_FILTER])))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
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
	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Texture_CustomFilter"), TEXT("Com_Filter"),
		(CComponent**)&m_pTextureCom[TYPE_FILTER])))
		return E_FAIL;

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

	if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof _float4)))
		return E_FAIL;

	/* For.Lights */
	const LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(0);
	if (nullptr == pLightDesc)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	if (FAILED(m_pTextureCom[TYPE_DIFFUSE]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTextureA",m_iDiffuseATexNum)))
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

	Safe_Delete(m_pPixel);

	for (auto& pTexture : m_pTextureCom)
		Safe_Release(pTexture);
	
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
