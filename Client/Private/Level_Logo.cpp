#include "stdafx.h"
#include "..\Public\Level_Logo.h"

#include "Level_Loading.h"

#include "GameInstance.h"
#include "Camera_Dynamic.h"
#include "ImGui_ProtoEditor.h"
#include "ImGui_PropertyEditor.h"

CLevel_Logo::CLevel_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Logo::Init()
{
	if (FAILED(CLevel::Init()))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	CGameInstance::GetInstance()->Clear_ImguiObjects();
	CGameInstance::GetInstance()->Add_ImguiObject(CImGui_ProtoEditor::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_PropertyEditor::Create(m_pDevice, m_pContext));

	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	pGameInstance->LoadData(LEVEL_LOGO, TEXT("../Bin/MapData/LOGO_TRANSFORM.dat"));
	Safe_Release(pGameInstance);

	return S_OK;
}

void CLevel_Logo::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta); 
	ImguiRenderTab();
}

void CLevel_Logo::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Key_Down(DIK_RETURN))
	{
		if (FAILED(pGameInstance->OpenLevel(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_CHAP1))))
			return;
	}

	//if (pGameInstance->Key_Down(DIK_F1))
	//{
	//	if (FAILED(pGameInstance->OpenLevel(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_TOOL))))
	//		return;
	//}

	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CLevel_Logo::Render()
{
	if (FAILED(CLevel::Render()))
		return E_FAIL;

	SetWindowText(g_hWnd, TEXT("Level : LOGO"));

	return S_OK;
}

void CLevel_Logo::ImguiRenderTab()
{
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

	if (ImGui::Button("Transform_Save"))
	{
		CGameInstance*		pGameInstance = CGameInstance::GetInstance();
		pGameInstance->SaveData(LEVEL_LOGO, TEXT("../Bin/MapData/LOGO_TRANSFORM.dat"));
		Safe_Release(pGameInstance);
	}
	if (ImGui::Button("go"))
	{
		CGameInstance*		pGameInstance = CGameInstance::GetInstance();
		pGameInstance->Clone_GameObject(LEVEL_LOGO, TEXT("Layer_UI"), TEXT("Prototype_GameObject_EnterKeyUI"));
		Safe_Release(pGameInstance);
	}
}

HRESULT CLevel_Logo::Ready_Layer_UI(const wstring& pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	//if (FAILED(pGameInstance->Clone_GameObject(LEVEL_LOGO, pLayerTag, TEXT("Prototype_GameObject_EnterKeyUI"))))
	//{
	//	Safe_Release(pGameInstance);
	//	return E_FAIL;
	//}
	//
	//if (FAILED(pGameInstance->Clone_GameObject(LEVEL_LOGO, pLayerTag, TEXT("Prototype_GameObject_BackGround"))))
	//{
	//	Safe_Release(pGameInstance);
	//	return E_FAIL;
	//}

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_Camera(const wstring & pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	//if (FAILED(pGameInstance->Clone_GameObject(LEVEL_LOGO, pLayerTag, TEXT("Prototype_GameObject_Camera_Dynamic"))))
	//	return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CLevel_Logo * CLevel_Logo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Logo*		pInstance = new CLevel_Logo(pDevice, pContext);

	if (FAILED(pInstance->Init()))
	{
		MSG_BOX("Failed to Created : CLevel_Logo");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_Logo::Free()
{
	CLevel::Free();
}
