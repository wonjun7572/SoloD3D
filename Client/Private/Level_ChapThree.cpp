#include "stdafx.h"
#include "..\Public\Level_ChapThree.h"

#include "Level_Loading.h"

#include "GameInstance.h"
#include "ImGui_ProtoEditor.h"
#include "ImGui_PropertyEditor.h"

CLevel_ChapThree::CLevel_ChapThree(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CLevel(pDevice, pContext)
{
}

HRESULT Client::CLevel_ChapThree::Init()
{
	if (FAILED(CLevel::Init()))
		return E_FAIL;

	if (FAILED(Ready_Ligths()))
		return E_FAIL;

	CGameInstance::GetInstance()->Clear_ImguiObjects();
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_PropertyEditor::Create(m_pDevice, m_pContext));

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;
	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
		return E_FAIL;
	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;
	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;
	if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
		return E_FAIL;
	if (FAILED(Ready_Layer_MapObject(TEXT("Layer_MapObject"))))
		return E_FAIL;

	return S_OK;
}

void Client::CLevel_ChapThree::Tick(_double TimeDelta)
{
	CLevel::Tick(TimeDelta);
	ImguiRenderTab();
}

void Client::CLevel_ChapThree::Late_Tick(_double TimeDelta)
{
	CLevel::Late_Tick(TimeDelta);
}

HRESULT Client::CLevel_ChapThree::Render()
{
	if (FAILED(CLevel::Render()))
		return E_FAIL;

	SetWindowText(g_hWnd, TEXT("Level : CHAPTER 3"));

	return S_OK;
}

void CLevel_ChapThree::ImguiRenderTab()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

	if (ImGui::Button("SetSaveFilePath"))
		ImGuiFileDialog::Instance()->OpenDialog("ChooseFileForSave", "Choose File", ".dat", "../Bin/");

	if (ImGuiFileDialog::Instance()->Display("ChooseFileForSave"))
	{
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
			std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();

			//string to wstring
			m_pSaveMapObjectFilePath.assign(filePathName.begin(), filePathName.end());

			pGameInstance->SaveMapObjectData(LEVEL_CHAP3, TEXT("Layer_MapObject"), m_pSaveMapObjectFilePath);

		}
		// close
		ImGuiFileDialog::Instance()->Close();
	}

	if (ImGui::Button("SetLoadFilePath"))
		ImGuiFileDialog::Instance()->OpenDialog("ChooseFileForLoad", "Choose File", ".dat", "../Bin/");

	if (ImGuiFileDialog::Instance()->Display("ChooseFileForLoad"))
	{
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();

			m_pLoadMapObjectFilePath.assign(filePathName.begin(), filePathName.end());

			pGameInstance->LoadMapObjectData(LEVEL_CHAP3, TEXT("Layer_MapObject"), m_pLoadMapObjectFilePath);
		}

		// close
		ImGuiFileDialog::Instance()->Close();
	}
	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CLevel_ChapThree::Ready_Ligths()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	LIGHTDESC			LightDesc;
	ZeroMemory(&LightDesc, sizeof LightDesc);

	LightDesc.eType = LIGHTDESC::TYPE_DIRECTIONAL;
	LightDesc.isEnable = true;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_ChapThree::Ready_Layer_BackGround(const wstring & pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP3, pLayerTag, TEXT("Prototype_GameObject_Sky"))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_ChapThree::Ready_Layer_Camera(const wstring & pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP3, pLayerTag, TEXT("Prototype_GameObject_PlayerCamera"))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_ChapThree::Ready_Layer_Player(const wstring & pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP3, pLayerTag, TEXT("Prototype_GameObject_Player"))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_ChapThree::Ready_Layer_Monster(const wstring & pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP3, pLayerTag, TEXT("Prototype_GameObject_SkeletonWarrior"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP3, pLayerTag, TEXT("Prototype_GameObject_SkeletonWarrior"))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_ChapThree::Ready_Layer_Effect(const wstring & pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	/*if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP3, pLayerTag, TEXT("Prototype_GameObject_Effect_Rect"))))
		return E_FAIL;*/

	//if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_Effect_Point"))))
	//	return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_ChapThree::Ready_Layer_MapObject(const wstring & pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP3, pLayerTag, TEXT("Prototype_GameObject_Anor"))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CLevel_ChapThree * CLevel_ChapThree::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLevel_ChapThree*		pInstance = new CLevel_ChapThree(pDevice, pContext);

	if (FAILED(pInstance->Init()))
	{
		MSG_BOX("Failed to Created : CLevel_ChapThree");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void Client::CLevel_ChapThree::Free()
{
	CLevel::Free();
}
