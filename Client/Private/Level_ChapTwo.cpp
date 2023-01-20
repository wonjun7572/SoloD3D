#include "stdafx.h"
#include "..\Public\Level_ChapTwo.h"

#include "Level_Loading.h"

#include "GameInstance.h"
#include "ImGui_ProtoEditor.h"
#include "ImGui_PropertyEditor.h"

#include "Ally.h"
#include "TrollQ.h"
#include "TrollA.h"

CLevel_ChapTwo::CLevel_ChapTwo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CLevel(pDevice, pContext)
{
}

HRESULT Client::CLevel_ChapTwo::Init()
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
	if (FAILED(Ready_Layer_Tree(TEXT("Layer_Tree"))))
		return E_FAIL;
	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;
	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;
	if (FAILED(Ready_Layer_Ally(TEXT("Layer_Ally"))))
		return E_FAIL;
	if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
		return E_FAIL;
	if (FAILED(Ready_Layer_MapObject(TEXT("Layer_MapObject"))))
		return E_FAIL;

	return S_OK;
}

void Client::CLevel_ChapTwo::Tick(_double TimeDelta)
{
	CLevel::Tick(TimeDelta);
	ImguiRenderTab();
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Key_Down(DIK_P))
	{
		if (FAILED(pGameInstance->OpenLevel(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_CHAP3))))
			return;
	}

	RELEASE_INSTANCE(CGameInstance);
}

void Client::CLevel_ChapTwo::Late_Tick(_double TimeDelta)
{
	CLevel::Late_Tick(TimeDelta);
}

HRESULT Client::CLevel_ChapTwo::Render()
{
	if (FAILED(CLevel::Render()))
		return E_FAIL;

	SetWindowText(g_hWnd, TEXT("Level : CHAPTER 2"));

	return S_OK;
}

void CLevel_ChapTwo::ImguiRenderTab()
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

			pGameInstance->SaveMapObjectData(LEVEL_CHAP2, TEXT("Layer_MapObject"), m_pSaveMapObjectFilePath);

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

			pGameInstance->LoadMapObjectData(LEVEL_CHAP2, TEXT("Layer_MapObject"), m_pLoadMapObjectFilePath);
		}

		// close
		ImGuiFileDialog::Instance()->Close();
	}
	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CLevel_ChapTwo::Ready_Ligths()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	LIGHTDESC			LightDesc;
	ZeroMemory(&LightDesc, sizeof LightDesc);

	LightDesc.eType = LIGHTDESC::TYPE_DIRECTIONAL;
	LightDesc.isEnable = true;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.4f, 0.4f, 0.4f, 1.f);
	LightDesc.vSpecular = LightDesc.vDiffuse;

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_ChapTwo::Ready_Layer_BackGround(const wstring & pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP2, pLayerTag, TEXT("Prototype_GameObject_Sky"))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_ChapTwo::Ready_Layer_Camera(const wstring & pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP2, pLayerTag, TEXT("Prototype_GameObject_PlayerCamera"))))
		return E_FAIL;
   
	//if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP2, pLayerTag, TEXT("Prototype_GameObject_Camera_Dynamic"))))
	//	return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_ChapTwo::Ready_Layer_Player(const wstring & pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP2, pLayerTag, TEXT("Prototype_GameObject_Player"))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_ChapTwo::Ready_Layer_Ally(const wstring & pLayerTag)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CAlly::ALLYDESC AllyDesc;
	ZeroMemory(&AllyDesc, sizeof AllyDesc);

	AllyDesc.fRadian = 180.f;
	AllyDesc.vPos = _float4(315.f , 0.f ,75.f ,1.f);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP2, pLayerTag, TEXT("Prototype_GameObject_Abelardo"), &AllyDesc)))
		return E_FAIL;

	AllyDesc.fRadian = 180.f;
	AllyDesc.vPos = _float4(312.5f, 0.f, 75.f, 1.f);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP2, pLayerTag, TEXT("Prototype_GameObject_Chinuwa"), &AllyDesc)))
		return E_FAIL;

	AllyDesc.fRadian = 180.f;
	AllyDesc.vPos = _float4(310.f, 0.f, 75.f, 1.f);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP2, pLayerTag, TEXT("Prototype_GameObject_Chitata"), &AllyDesc)))
		return E_FAIL;

	AllyDesc.fRadian = 180.f;
	AllyDesc.vPos = _float4(307.5f, 0.f, 75.f, 1.f);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP2, pLayerTag, TEXT("Prototype_GameObject_Delilah"), &AllyDesc)))
		return E_FAIL;

	AllyDesc.fRadian = 180.f;
	AllyDesc.vPos = _float4(314.f, 0.f, 50.f, 1.f);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP2, pLayerTag, TEXT("Prototype_GameObject_BalianBollwerk"), &AllyDesc)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_ChapTwo::Ready_Layer_Monster(const wstring & pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CTrollQ::TROLLQDESC TrollQDesc;
	ZeroMemory(&TrollQDesc, sizeof TrollQDesc);

	TrollQDesc.iGroup = 1;
	TrollQDesc.fRadian = 45.f;
	TrollQDesc.vPos = _float4(45.5f, 0.f, 183.375f, 1.f);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP2, pLayerTag, TEXT("Prototype_GameObject_TrollQ"), &TrollQDesc)))
		return E_FAIL;

	TrollQDesc.iGroup = 2;
	TrollQDesc.fRadian = 150.f;
	TrollQDesc.vPos = _float4(103.4f, 0.f, 379.f, 1.f);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP2, pLayerTag, TEXT("Prototype_GameObject_TrollQ"), &TrollQDesc)))
		return E_FAIL;

	TrollQDesc.iGroup = 3;
	TrollQDesc.fRadian = 200.f;
	TrollQDesc.vPos = _float4(317.f, 0.f, 410.f, 1.f);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP2, pLayerTag, TEXT("Prototype_GameObject_TrollQ"), &TrollQDesc)))
		return E_FAIL;
	
	CTrollA::TROLLADESC TrollADesc;
	ZeroMemory(&TrollADesc, sizeof TrollADesc);

	TrollADesc.iGroup = 1;
	TrollADesc.fRadian = 45.f;
	TrollADesc.vPos = _float4(52.f, 0.f, 185.f, 1.f);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP2, pLayerTag, TEXT("Prototype_GameObject_TrollA"), &TrollADesc)))
		return E_FAIL;

	TrollADesc.iGroup = 1;
	TrollADesc.fRadian = 45.f;
	TrollADesc.vPos = _float4(47.f, 0.f, 190.f, 1.f);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP2, pLayerTag, TEXT("Prototype_GameObject_TrollA"), &TrollADesc)))
		return E_FAIL;

	TrollADesc.iGroup = 1;
	TrollADesc.fRadian = 45.f;
	TrollADesc.vPos = _float4(38.f, 0.f, 183.f, 1.f);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP2, pLayerTag, TEXT("Prototype_GameObject_TrollA"), &TrollADesc)))
		return E_FAIL;

	TrollADesc.iGroup = 1;
	TrollADesc.fRadian = 45.f;
	TrollADesc.vPos = _float4(45.f, 0.f, 176.f, 1.f);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP2, pLayerTag, TEXT("Prototype_GameObject_TrollA"), &TrollADesc)))
		return E_FAIL;

	TrollADesc.iGroup = 2;
	TrollADesc.fRadian = 150.f;
	TrollADesc.vPos = _float4(102.75f, 0.f, 373.f, 1.f);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP2, pLayerTag, TEXT("Prototype_GameObject_TrollA"), &TrollADesc)))
		return E_FAIL;

	TrollADesc.iGroup = 2;
	TrollADesc.fRadian = 150.f;
	TrollADesc.vPos = _float4(110.f, 0.f, 376.f, 1.f);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP2, pLayerTag, TEXT("Prototype_GameObject_TrollA"), &TrollADesc)))
		return E_FAIL;

	TrollADesc.iGroup = 2;
	TrollADesc.fRadian = 150.f;
	TrollADesc.vPos = _float4(105.f, 0.f, 386.f, 1.f);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP2, pLayerTag, TEXT("Prototype_GameObject_TrollA"), &TrollADesc)))
		return E_FAIL;

	TrollADesc.iGroup = 2;
	TrollADesc.fRadian = 150.f;
	TrollADesc.vPos = _float4(98.f, 0.f, 382.f, 1.f);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP2, pLayerTag, TEXT("Prototype_GameObject_TrollA"), &TrollADesc)))
		return E_FAIL;

	TrollADesc.iGroup = 3;
	TrollADesc.fRadian = 200.f;
	TrollADesc.vPos = _float4(312.f, 0.f, 407.f, 1.f);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP2, pLayerTag, TEXT("Prototype_GameObject_TrollA"), &TrollADesc)))
		return E_FAIL;

	TrollADesc.iGroup = 3;
	TrollADesc.fRadian = 200.f;
	TrollADesc.vPos = _float4(319.f, 0.f, 405.f, 1.f);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP2, pLayerTag, TEXT("Prototype_GameObject_TrollA"), &TrollADesc)))
		return E_FAIL;

	TrollADesc.iGroup = 3;
	TrollADesc.fRadian = 200.f;
	TrollADesc.vPos = _float4(312.f, 0.f, 414.f, 1.f);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP2, pLayerTag, TEXT("Prototype_GameObject_TrollA"), &TrollADesc)))
		return E_FAIL;

	TrollADesc.iGroup = 3;
	TrollADesc.fRadian = 200.f;
	TrollADesc.vPos = _float4(322.f, 0.f, 411.f, 1.f);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP2, pLayerTag, TEXT("Prototype_GameObject_TrollA"), &TrollADesc)))
		return E_FAIL;

	_float4 vPos = _float4(300.f , 0.f, 135.f , 1.f);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP2, pLayerTag, TEXT("Prototype_GameObject_ZombieA"), &vPos)))
		return E_FAIL;

	vPos = _float4(290.f, 0.f, 133.f, 1.f);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP2, pLayerTag, TEXT("Prototype_GameObject_ZombieA"), &vPos)))
		return E_FAIL;

	vPos = _float4(303.f, 0.f, 164.f, 1.f);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP2, pLayerTag, TEXT("Prototype_GameObject_ZombieA"), &vPos)))
		return E_FAIL;

	vPos = _float4(299.6f, 0.f, 195.f, 1.f);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP2, pLayerTag, TEXT("Prototype_GameObject_ZombieA"), &vPos)))
		return E_FAIL;

	vPos = _float4(289.f, 0.f, 220.f, 1.f);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP2, pLayerTag, TEXT("Prototype_GameObject_ZombieA"), &vPos)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_ChapTwo::Ready_Layer_Effect(const wstring & pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_ChapTwo::Ready_Layer_MapObject(const wstring & pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP2, pLayerTag, TEXT("Prototype_GameObject_Medieval_City"))))
		return E_FAIL;
	
	RELEASE_INSTANCE(CGameInstance);
	
	return S_OK;
}

HRESULT CLevel_ChapTwo::Ready_Layer_Tree(const wstring & pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP2, pLayerTag, TEXT("Prototype_GameObject_Tree"))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CLevel_ChapTwo * CLevel_ChapTwo::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLevel_ChapTwo*		pInstance = new CLevel_ChapTwo(pDevice, pContext);

	if (FAILED(pInstance->Init()))
	{
		MSG_BOX("Failed to Created : CLevel_ChapTwo");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void Client::CLevel_ChapTwo::Free()
{
	CLevel::Free();
}
