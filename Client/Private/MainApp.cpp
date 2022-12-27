#include "stdafx.h"
#include "..\Public\MainApp.h"

#include "GameInstance.h"
#include "Level_Loading.h"

#include "Camera_Dynamic.h"
#include "LoadingBackGround.h"

CMainApp::CMainApp()
	:m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CMainApp::Init()
{
	if (m_pGameInstance == nullptr)
		return E_FAIL;

	GRAPHIC_DESC			GraphicDesc;
	ZeroMemory(&GraphicDesc, sizeof(GRAPHIC_DESC));
	
	GraphicDesc.hWnd = g_hWnd;
	GraphicDesc.hInstance = g_hInst;
	GraphicDesc.iViewportSizeX = g_iWinSizeX;
	GraphicDesc.iViewportSizeY = g_iWinSizeY;
	GraphicDesc.eWinMode = GRAPHIC_DESC::MODE_WIN;

	if (FAILED(m_pGameInstance->Init_Engine(LEVEL_END, GraphicDesc, &m_pDevice, &m_pContext)))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Component()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_GameObject()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Fonts(m_pDevice, m_pContext, TEXT("Font_Comic"), TEXT("../Bin/Resources/Fonts/131.spritefont"))))
		return E_FAIL;

	if (FAILED(Start_Level(LEVEL_LOGO)))
		return E_FAIL;

	return S_OK;
}

void CMainApp::Tick(_double deltaTime)
{
	if (m_pGameInstance == nullptr)
		return;
	
#ifdef _DEBUG
	m_TimeAcc += deltaTime;
#endif 

	m_pGameInstance->Tick_Engine(deltaTime);
}

HRESULT CMainApp::Render()
{
	if (m_pGameInstance == nullptr)
		return E_FAIL;

	m_pGameInstance->Render_ImGui();

	m_pGameInstance->Clear_Graphic_Device(&_float4(0.5f, 0.5f, 0.5f, 1.f));

	m_pRenderer->Draw_RenderGroup();

	m_pGameInstance->Render_Update_ImGui();

	m_pGameInstance->RenderLevel();

#ifdef _DEBUG

	++m_iNumCallDraw;

	if (m_TimeAcc >= 1.f)
	{
		wsprintf(m_szFPS, TEXT("배고파 : %d"), m_iNumCallDraw);


		m_iNumCallDraw = 0;
		m_TimeAcc = 0.f;
	}

	m_pGameInstance->Render_Font(TEXT("Font_Comic"), m_szFPS, _float2(100.f, 0.f), 0.f, _float2(1.f, 1.f), XMVectorSet(1.f, 0.f, 0.f, 1.f));

#endif

	m_pGameInstance->Present();

	return S_OK;
}

HRESULT CMainApp::Resize_BackBuffer()
{
	GRAPHIC_DESC	tGraphicDesc;
	ZeroMemory(&tGraphicDesc, sizeof(GRAPHIC_DESC));

	tGraphicDesc.hWnd = g_hWnd;
	if (!g_bFullScreen)
	{
		tGraphicDesc.iViewportSizeX = g_iWinSizeX;
		tGraphicDesc.iViewportSizeY = g_iWinSizeY;
	}
	else
	{
		tGraphicDesc.iViewportSizeX = GetSystemMetrics(SM_CXSCREEN);
		tGraphicDesc.iViewportSizeY = GetSystemMetrics(SM_CYSCREEN);
	}

	if (FAILED(m_pGameInstance->Update_SwapChain(tGraphicDesc.hWnd, tGraphicDesc.iViewportSizeX, tGraphicDesc.iViewportSizeY, g_bFullScreen, g_bNeedResizeSwapChain)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Start_Level(LEVEL eLevelID)
{
	if (LEVEL_LOADING == eLevelID || m_pGameInstance == nullptr)
		return E_FAIL;

	if (FAILED(m_pGameInstance->OpenLevel(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, eLevelID))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_Component()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	/* For.Prototype_Component_Renderer */
	if (FAILED(m_pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"),
		m_pRenderer = CRenderer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	Safe_AddRef(m_pRenderer);

	/* For.Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxTex */
	if (FAILED(m_pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxTex.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxNorTex */
	if (FAILED(m_pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX_DECLARATION::Elements, VTXNORTEX_DECLARATION::iNumElements))))
		return E_FAIL;

	/*For.Prototype_Component_Texture_LoadingBackGround*/
	if (FAILED(m_pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_LoadingBackGround"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/BlueArchive/Loading/Loading_%d.png"), 6))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_GameObject()
{
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Dynamic"), CCamera_Dynamic::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_LoadingBackGround"), CLoadingBackGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

///* 내가 만든 네비게이션 대로 움직일수 있도록 하는 툴 제작 들어가야함*/
//_ulong		dwByte = 0;
//HANDLE		hFile = CreateFile(TEXT("../Bin/Data/Navigation.dat"), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
//if (0 == hFile)
//return E_FAIL;
//
//
//// 여기 밑으로 만들어주면 될듯
//_float3		vPoints[3];
//
//vPoints[0] = _float3(0.0f, 0.f, 5.0f);
//vPoints[1] = _float3(5.0f, 0.f, 0.0f);
//vPoints[2] = _float3(0.0f, 0.f, 0.0f);
//WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);
//
//vPoints[0] = _float3(0.0f, 0.f, 5.0f);
//vPoints[1] = _float3(5.0f, 0.f, 5.0f);
//vPoints[2] = _float3(5.0f, 0.f, 0.0f);
//WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);
//
//vPoints[0] = _float3(0.0f, 0.f, 10.0f);
//vPoints[1] = _float3(5.0f, 0.f, 5.0f);
//vPoints[2] = _float3(0.0f, 0.f, 5.0f);
//WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);
//
//vPoints[0] = _float3(5.0f, 0.f, 5.0f);
//vPoints[1] = _float3(10.0f, 0.f, 0.0f);
//vPoints[2] = _float3(5.0f, 0.f, 0.0f);
//WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);
//
//CloseHandle(hFile);

CMainApp * CMainApp::Create()
{
	CMainApp* pInstance = new CMainApp;

	if (FAILED(pInstance->Init()))
	{
		MSG_BOX("CMainApp::Create FAILED!!");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CMainApp::Free()
{
	m_pGameInstance->Clear_ImguiObjects();

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);

	CGameInstance::Release_Engine();
}