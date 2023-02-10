#include "stdafx.h"
#include "..\Public\Level_ChapOne.h"

#include "Level_Loading.h"

#include "ConversationUI.h"
#include "GameInstance.h"
#include "ImGui_ProtoEditor.h"
#include "ImGui_PropertyEditor.h"
#include "EffectManager.h"
#include "Camera.h"
#include "Particle.h"

CLevel_ChapOne::CLevel_ChapOne(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CLevel(pDevice, pContext)
{
}

HRESULT Client::CLevel_ChapOne::Init()
{
	if (FAILED(CLevel::Init()))
		return E_FAIL;

	if (FAILED(Ready_Lights()))
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
	if (FAILED(Ready_Layer_Ally(TEXT("Layer_Ally"))))
		return E_FAIL;
	if (FAILED(Ready_Layer_MapObject(TEXT("Layer_MapObject"))))
		return E_FAIL;
	if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
		return E_FAIL;
	if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
		return E_FAIL;
	
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	pGameInstance->Stop_Sound(SOUND_BGM);
	pGameInstance->Play_Sound(L"FogtreeForest.mp3", 0.7f, true, SOUND_BGM);
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void Client::CLevel_ChapOne::Tick(_double TimeDelta)
{
	CLevel::Tick(TimeDelta);
	ImguiRenderTab();
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Key_Down(DIK_P))
	{
		if (FAILED(pGameInstance->OpenLevel(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_CHAP2))))
			return;
	}

	RELEASE_INSTANCE(CGameInstance);
}

void Client::CLevel_ChapOne::Late_Tick(_double TimeDelta)
{
	CLevel::Late_Tick(TimeDelta);
}

HRESULT Client::CLevel_ChapOne::Render()
{
	if (FAILED(CLevel::Render()))
		return E_FAIL;

	SetWindowText(g_hWnd, TEXT("Level : CHAPTER 1"));

	return S_OK;
}

void CLevel_ChapOne::ImguiRenderTab()
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

			pGameInstance->SaveMapObjectData(LEVEL_CHAP1, TEXT("Layer_Monster"), m_pSaveMapObjectFilePath);
			
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

			pGameInstance->LoadMapObjectData(LEVEL_CHAP1, TEXT("Layer_Monster"), m_pLoadMapObjectFilePath);
		}

		// close
		ImGuiFileDialog::Instance()->Close();
	}
	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CLevel_ChapOne::Ready_Lights()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance)

	LIGHTDESC			LightDesc;
	ZeroMemory(&LightDesc, sizeof LightDesc);

	LightDesc.eType = LIGHTDESC::TYPE_DIRECTIONAL;
	LightDesc.isEnable = true;
	LightDesc.vPosition = _float4(-5.f, 50.f, -5.f, 1.f);
	_float4 vDir = _float4(60.f, 0.f, 60.f, 1.f) - _float4(-5.f, 50.f, -5.f, 1.f);
	vDir.Normalize();
	LightDesc.vDirection =	vDir;
	LightDesc.vDiffuse = _float4(0.6f, 0.4f, 0.4f, 1.f);
	LightDesc.vAmbient = _float4(0.4f, 0.4f, 0.4f, 1.f);
	LightDesc.vSpecular = LightDesc.vDiffuse;

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance)

	return S_OK;
}

HRESULT CLevel_ChapOne::Ready_Layer_BackGround(const wstring& pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_Sky"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_Terrain"))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_ChapOne::Ready_Layer_Camera(const wstring & pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance)

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_PlayerCamera"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_Camera_Dynamic"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_LightCamera"))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance)

	return S_OK;
}

HRESULT CLevel_ChapOne::Ready_Layer_Player(const wstring & pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance)

		if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_Player"))))
			return E_FAIL;

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_PrinceH"))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance)

	return S_OK;
}

HRESULT CLevel_ChapOne::Ready_Layer_Monster(const wstring & pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_float4 vPos = _float4(95.f, 0.f, 95.f, 1.f);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_SkeletonWarrior"), &vPos)))
		return E_FAIL;

	vPos = _float4(100.f, 0.f, 100.f, 1.f);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_SkeletonWarrior"), &vPos)))
		return E_FAIL;

	vPos = _float4(110.f, 0.f, 100.f, 1.f);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_SkeletonWarrior"), &vPos)))
		return E_FAIL;

	vPos = _float4(115.f, 0.f, 95.f, 1.f);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_SkeletonWarrior"), &vPos)))
		return E_FAIL;

	vPos = _float4(105.f, 0.f, 95.f, 1.f);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_SkeletonWarrior"), &vPos)))
		return E_FAIL;
	
	//if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, L"Layer_Monster", TEXT("Prototype_GameObject_Flogas"))))
	//	return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_ChapOne::Ready_Layer_Ally(const wstring & pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance)

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_BalianBollwerk"))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance)

	return S_OK;
}

HRESULT CLevel_ChapOne::Ready_Layer_MapObject(const wstring & pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance)

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_TownA"))))
		return E_FAIL;
	
	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_BridgeCastle"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_Stair"))))
		return E_FAIL;

	_float4 vPos = _float4(100.9f, 14.f, 42.8f, 1.f);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_FlameE"), &vPos)))
		return E_FAIL;

	vPos = _float4(100.9f, 14.f, 55.1f, 1.f);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_FlameE"), &vPos)))
		return E_FAIL;

	vPos = _float4(100.9f, 14.f, 75.f, 1.f);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_FlameE"), &vPos)))
		return E_FAIL;

	vPos = _float4(109.5f, 14.f, 42.8f, 1.f);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_FlameE"), &vPos)))
		return E_FAIL;

	vPos = _float4(109.5f, 14.f, 55.1f, 1.f);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_FlameE"), &vPos)))
		return E_FAIL;

	vPos = _float4(109.5f, 14.f, 75.f, 1.f);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_FlameE"), &vPos)))
		return E_FAIL;

	vPos = _float4(121.5f, 14.f, 75.3f, 1.f);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_FlameE"), &vPos)))
		return E_FAIL;

	vPos = _float4(121.5f, 14.f, 94.7f, 1.f);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_FlameE"), &vPos)))
		return E_FAIL;

	vPos = _float4(121.5f, 14.f, 114.1f, 1.f);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_FlameE"), &vPos)))
		return E_FAIL;

	vPos = _float4(88.8f, 14.f, 75.3f, 1.f);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_FlameE"), &vPos)))
		return E_FAIL;

	vPos = _float4(88.8f, 14.f, 94.7f, 1.f);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_FlameE"), &vPos)))
		return E_FAIL;

	vPos = _float4(88.8f, 14.f, 114.1f, 1.f);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_FlameE"), &vPos)))
		return E_FAIL;

	vPos = _float4(5.6f, 0.f, -3.f, 1.f);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_Rock_1"), &vPos)))
		return E_FAIL;

	vPos = _float4(26.953f, 0.f, -3.025f, 1.f);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_Rock_2"), &vPos)))
		return E_FAIL;

	vPos = _float4(64.043f, 0.779f, -7.510f, 1.f);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_Rock_2"), &vPos)))
		return E_FAIL;

	vPos = _float4(93.444f, 0.f, -11.924f, 1.f);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_Rock_2"), &vPos)))
		return E_FAIL;

	vPos = _float4(128.f, 0.f, 23.f, 1.f);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_Rock_2"), &vPos)))
		return E_FAIL;

	vPos = _float4(128.f, 0.f, 54.f, 1.f);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_Rock_2"), &vPos)))
		return E_FAIL;

	vPos = _float4(138.f, 0.f, 88.f, 1.f);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_Rock_2"), &vPos)))
		return E_FAIL;

	vPos = _float4(48.041f, 0.f, 0.664f, 1.f);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_Rock_3"), &vPos)))
		return E_FAIL;

	vPos = _float4(85.f, 0.f, 2.f, 1.f);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_Rock_3"), &vPos)))
		return E_FAIL;

	vPos = _float4(0.320f, -2.744f, 18.228f, 1.f);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_Rock_4"), &vPos)))
		return E_FAIL;

	vPos = _float4(121.173f, 0.f, 5.f, 1.f);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_Rock_4"), &vPos)))
		return E_FAIL;

	vPos = _float4(112.018f, -0.573f, -1.45f, 1.f);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_Rock_4"), &vPos)))
		return E_FAIL;

	vPos = _float4(36.073f, 5.f, 31.573f, 1.f);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_Torchlight"), &vPos)))
		return E_FAIL;

	vPos = _float4(37.946f, 5.f, 46.914f, 1.f);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_Torchlight"), &vPos)))
		return E_FAIL;

	vPos = _float4(28.638f, 5.f, 47.234f, 1.f);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_Torchlight"), &vPos)))
		return E_FAIL;

	vPos = _float4(33.215f, 5.f, 13.235f, 1.f);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_Torchlight"), &vPos)))
		return E_FAIL;

	vPos = _float4(62.376f, 5, 7.831f, 1.f);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_Torchlight"), &vPos)))
		return E_FAIL;

	vPos = _float4(86.143f, 5.f, 9.061f, 1.f);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_Torchlight"), &vPos)))
		return E_FAIL;

	vPos = _float4(28.660f, 5.5f, 47.245f, 1.f);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_Torchlight_Flame"), &vPos)))
		return E_FAIL;

	vPos = _float4(37.834f, 5.5f, 47.245f, 1.f);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_Torchlight_Flame"), &vPos)))
		return E_FAIL;

	vPos = _float4(36.081f, 5.5f, 31.486f, 1.f);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_Torchlight_Flame"), &vPos)))
		return E_FAIL;

	vPos = _float4(33.199f, 5.5f, 13.255f, 1.f);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_Torchlight_Flame"), &vPos)))
		return E_FAIL;

	vPos = _float4(62.418f, 5.5f, 7.849f, 1.f);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_Torchlight_Flame"), &vPos)))
		return E_FAIL;

	vPos = _float4(86.153f, 5.5f, 9.075f, 1.f);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_Torchlight_Flame"), &vPos)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance)

	return S_OK;
}

HRESULT CLevel_ChapOne::Ready_Layer_Effect(const wstring & pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance)
	/*	CParticle::DESC desc;
	ZeroMemory(&desc, sizeof(CParticle::DESC));

	desc.fShowTime = 0.f;
	desc.fSizeUV_X = 1.f / 4.f;
	desc.fSizeUV_Y = 1.f / 8.f;
	desc.iSpriteCount_X = 4;
	desc.iSpriteCount_Y = 8;
	desc.pTextureTag = L"Texture_FX_fireball_008";
	desc.ViBufferDesc.iNumSprite = 32;
	desc.ViBufferDesc.vMaxSize = _float2(1.024f * 1.2f, 0.512f * 1.2f);
	desc.ViBufferDesc.vMinSize = _float2(1.024f * 0.7f, 0.512f * 0.7f);
	desc.ViBufferDesc.fMinTime = 0.1f;
	desc.ViBufferDesc.fMaxTime = 0.3f;
	desc.ViBufferDesc.fRange = 1.2f;
	desc.ViBufferDesc.fRangeOffset = 1.f;
	desc.ViBufferDesc.fMinSpreadSizeOffset = 0.1f;
	desc.ViBufferDesc.fMaxSpreadSizeOffset = 0.3f;
	desc.ViBufferDesc.iNumInstances = 150;
	desc.ViBufferDesc.iNumSpreadInstances = 100;

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_Particle"), &desc)))
		return E_FAIL;

	ZeroMemory(&desc, sizeof(CParticle::DESC));
	desc.fShowTime = 0.f;
	desc.fSizeUV_X = 1.f / 4.f;
	desc.fSizeUV_Y = 1.f / 4.f;
	desc.iSpriteCount_X = 4;
	desc.iSpriteCount_Y = 4;
	desc.pTextureTag = L"FX_lightning_001_TEX_KJS";
	desc.ViBufferDesc.iNumSprite = 16;
	desc.ViBufferDesc.vMaxSize = _float2(0.256f * 1.2f, 1.024f * 1.2f);
	desc.ViBufferDesc.vMinSize = _float2(0.256f * 0.7f, 1.024f * 0.7f);
	desc.ViBufferDesc.fMinTime = 0.1f;
	desc.ViBufferDesc.fMaxTime = 0.3f;
	desc.ViBufferDesc.fRange = 1.2f;
	desc.ViBufferDesc.fRangeOffset = 1.f;
	desc.ViBufferDesc.fMinSpreadSizeOffset = 0.1f;
	desc.ViBufferDesc.fMaxSpreadSizeOffset = 0.3f;
	desc.ViBufferDesc.iNumInstances = 150;
	desc.ViBufferDesc.iNumSpreadInstances = 100;

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_Particle"), &desc)))
		return E_FAIL;
	
	ZeroMemory(&desc, sizeof(CParticle::DESC));
	desc.fShowTime = 0.f;
	desc.fSizeUV_X = 1.f / 4.f;
	desc.fSizeUV_Y = 1.f / 4.f;
	desc.iSpriteCount_X = 4;
	desc.iSpriteCount_Y = 4;
	desc.pTextureTag = L"FX_lightning_003_TEX_KJS";
	desc.ViBufferDesc.iNumSprite = 16;
	desc.ViBufferDesc.vMaxSize = _float2(0.512f * 1.2f, 0.512f * 1.2f);
	desc.ViBufferDesc.vMinSize = _float2(0.512f * 0.7f, 0.512f * 0.7f);
	desc.ViBufferDesc.fMinTime = 0.1f;
	desc.ViBufferDesc.fMaxTime = 0.3f;
	desc.ViBufferDesc.fRange = 1.2f;
	desc.ViBufferDesc.fRangeOffset = 1.f;
	desc.ViBufferDesc.fMinSpreadSizeOffset = 0.1f;
	desc.ViBufferDesc.fMaxSpreadSizeOffset = 0.3f;
	desc.ViBufferDesc.iNumInstances = 150;
	desc.ViBufferDesc.iNumSpreadInstances = 100;

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_Particle"), &desc)))
		return E_FAIL;

	ZeroMemory(&desc, sizeof(CParticle::DESC));
	desc.fShowTime = 0.f;
	desc.fSizeUV_X = 1.f / 4.f;
	desc.fSizeUV_Y = 1.f / 4.f;
	desc.iSpriteCount_X = 4;
	desc.iSpriteCount_Y = 4;
	desc.pTextureTag = L"FX_lightning_004_TEX_KJS";
	desc.ViBufferDesc.iNumSprite = 16;
	desc.ViBufferDesc.vMaxSize = _float2(0.512f * 1.2f, 0.512f * 1.2f);
	desc.ViBufferDesc.vMinSize = _float2(0.512f * 0.7f, 0.512f * 0.7f);
	desc.ViBufferDesc.fMinTime = 0.1f;
	desc.ViBufferDesc.fMaxTime = 0.3f;
	desc.ViBufferDesc.fRange = 1.2f;
	desc.ViBufferDesc.fRangeOffset = 1.f;
	desc.ViBufferDesc.fMinSpreadSizeOffset = 0.1f;
	desc.ViBufferDesc.fMaxSpreadSizeOffset = 0.3f;
	desc.ViBufferDesc.iNumInstances = 150;
	desc.ViBufferDesc.iNumSpreadInstances = 100;

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_Particle"), &desc)))
		return E_FAIL;

	ZeroMemory(&desc, sizeof(CParticle::DESC));
	desc.fShowTime = 0.f;
	desc.fSizeUV_X = 1.f / 4.f;
	desc.fSizeUV_Y = 1.f / 4.f;
	desc.iSpriteCount_X = 4;
	desc.iSpriteCount_Y = 4;
	desc.pTextureTag = L"FX_lightning_005_TEX_KJS";
	desc.ViBufferDesc.iNumSprite = 16;
	desc.ViBufferDesc.vMaxSize = _float2(0.512f * 1.2f, 0.512f * 1.2f);
	desc.ViBufferDesc.vMinSize = _float2(0.512f * 0.7f, 0.512f * 0.7f);
	desc.ViBufferDesc.fMinTime = 0.1f;
	desc.ViBufferDesc.fMaxTime = 0.3f;
	desc.ViBufferDesc.fRange = 1.2f;
	desc.ViBufferDesc.fRangeOffset = 1.f;
	desc.ViBufferDesc.fMinSpreadSizeOffset = 0.1f;
	desc.ViBufferDesc.fMaxSpreadSizeOffset = 0.3f;
	desc.ViBufferDesc.iNumInstances = 150;
	desc.ViBufferDesc.iNumSpreadInstances = 100;

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_Particle"), &desc)))
		return E_FAIL;

	ZeroMemory(&desc, sizeof(CParticle::DESC));
	desc.fShowTime = 0.f;
	desc.fSizeUV_X = 1.f / 4.f;
	desc.fSizeUV_Y = 1.f / 4.f;
	desc.iSpriteCount_X = 4;
	desc.iSpriteCount_Y = 4;
	desc.pTextureTag = L"FX_lightning_006_TEX_KJS";
	desc.ViBufferDesc.iNumSprite = 16;
	desc.ViBufferDesc.vMaxSize = _float2(0.512f * 1.2f, 0.512f * 1.2f);
	desc.ViBufferDesc.vMinSize = _float2(0.512f * 0.7f, 0.512f * 0.7f);
	desc.ViBufferDesc.fMinTime = 0.1f;
	desc.ViBufferDesc.fMaxTime = 0.3f;
	desc.ViBufferDesc.fRange = 1.2f;
	desc.ViBufferDesc.fRangeOffset = 1.f;
	desc.ViBufferDesc.fMinSpreadSizeOffset = 0.1f;
	desc.ViBufferDesc.fMaxSpreadSizeOffset = 0.3f;
	desc.ViBufferDesc.iNumInstances = 150;
	desc.ViBufferDesc.iNumSpreadInstances = 100;

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_Particle"), &desc)))
		return E_FAIL;

	ZeroMemory(&desc, sizeof(CParticle::DESC));
	desc.fShowTime = 0.f;
	desc.fSizeUV_X = 1.f / 4.f;
	desc.fSizeUV_Y = 1.f / 4.f;
	desc.iSpriteCount_X = 4;
	desc.iSpriteCount_Y = 4;
	desc.pTextureTag = L"FX_lightning_007_TEX_KJS";
	desc.ViBufferDesc.iNumSprite = 16;
	desc.ViBufferDesc.vMaxSize = _float2(0.256f * 1.2f, 1.024f * 1.2f);
	desc.ViBufferDesc.vMinSize = _float2(0.256f * 0.7f, 1.024f * 0.7f);
	desc.ViBufferDesc.fMinTime = 0.1f;
	desc.ViBufferDesc.fMaxTime = 0.3f;
	desc.ViBufferDesc.fRange = 1.2f;
	desc.ViBufferDesc.fRangeOffset = 1.f;
	desc.ViBufferDesc.fMinSpreadSizeOffset = 0.1f;
	desc.ViBufferDesc.fMaxSpreadSizeOffset = 0.3f;
	desc.ViBufferDesc.iNumInstances = 150;
	desc.ViBufferDesc.iNumSpreadInstances = 100;

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_Particle"), &desc)))
		return E_FAIL;

	ZeroMemory(&desc, sizeof(CParticle::DESC));
	desc.fShowTime = 0.f;
	desc.fSizeUV_X = 1.f / 4.f;
	desc.fSizeUV_Y = 1.f / 4.f;
	desc.iSpriteCount_X = 4;
	desc.iSpriteCount_Y = 4;
	desc.pTextureTag = L"FX_lightning_008_TEX_KJS";
	desc.ViBufferDesc.iNumSprite = 16;
	desc.ViBufferDesc.vMaxSize = _float2(0.256f * 1.2f, 1.024f * 1.2f);
	desc.ViBufferDesc.vMinSize = _float2(0.256f * 0.7f, 1.024f * 0.7f);
	desc.ViBufferDesc.fMinTime = 0.1f;
	desc.ViBufferDesc.fMaxTime = 0.3f;
	desc.ViBufferDesc.fRange = 1.2f;
	desc.ViBufferDesc.fRangeOffset = 1.f;
	desc.ViBufferDesc.fMinSpreadSizeOffset = 0.1f;
	desc.ViBufferDesc.fMaxSpreadSizeOffset = 0.3f;
	desc.ViBufferDesc.iNumInstances = 150;
	desc.ViBufferDesc.iNumSpreadInstances = 100;

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_Particle"), &desc)))
		return E_FAIL;

	ZeroMemory(&desc, sizeof(CParticle::DESC));
	desc.fShowTime = 0.f;
	desc.fSizeUV_X = 1.f / 4.f;
	desc.fSizeUV_Y = 1.f / 4.f;
	desc.iSpriteCount_X = 4;
	desc.iSpriteCount_Y = 4;
	desc.pTextureTag = L"FX_lightning_009_TEX_KJS";
	desc.ViBufferDesc.iNumSprite = 16;
	desc.ViBufferDesc.vMaxSize = _float2(0.512f * 1.2f, 1.024f * 1.2f);
	desc.ViBufferDesc.vMinSize = _float2(0.512f * 0.7f, 1.024f * 0.7f);
	desc.ViBufferDesc.fMinTime = 0.1f;
	desc.ViBufferDesc.fMaxTime = 0.3f;
	desc.ViBufferDesc.fRange = 1.2f;
	desc.ViBufferDesc.fRangeOffset = 1.f;
	desc.ViBufferDesc.fMinSpreadSizeOffset = 0.1f;
	desc.ViBufferDesc.fMaxSpreadSizeOffset = 0.3f;
	desc.ViBufferDesc.iNumInstances = 150;
	desc.ViBufferDesc.iNumSpreadInstances = 100;

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_Particle"), &desc)))
		return E_FAIL;

	ZeroMemory(&desc, sizeof(CParticle::DESC));
	desc.fShowTime = 0.f;
	desc.fSizeUV_X = 1.f / 5.f;
	desc.fSizeUV_Y = 1.f / 5.f;
	desc.iSpriteCount_X = 5;
	desc.iSpriteCount_Y = 5;
	desc.pTextureTag = L"FX_lightning_010_TEX_KJS";
	desc.ViBufferDesc.iNumSprite = 25;
	desc.ViBufferDesc.vMaxSize = _float2(0.512f * 1.2f, 0.512f * 1.2f);
	desc.ViBufferDesc.vMinSize = _float2(0.512f * 0.7f, 0.512f * 0.7f);
	desc.ViBufferDesc.fMinTime = 0.1f;
	desc.ViBufferDesc.fMaxTime = 0.3f;
	desc.ViBufferDesc.fRange = 1.2f;
	desc.ViBufferDesc.fRangeOffset = 1.f;
	desc.ViBufferDesc.fMinSpreadSizeOffset = 0.1f;
	desc.ViBufferDesc.fMaxSpreadSizeOffset = 0.3f;
	desc.ViBufferDesc.iNumInstances = 150;
	desc.ViBufferDesc.iNumSpreadInstances = 100;

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_Particle"), &desc)))
		return E_FAIL;

	ZeroMemory(&desc, sizeof(CParticle::DESC));
	desc.fShowTime = 0.f;
	desc.fSizeUV_X = 1.f / 5.f;
	desc.fSizeUV_Y = 1.f / 5.f;
	desc.iSpriteCount_X = 5;
	desc.iSpriteCount_Y = 5;
	desc.pTextureTag = L"FX_lightning_011_TEX_KJS";
	desc.ViBufferDesc.iNumSprite = 25;
	desc.ViBufferDesc.vMaxSize = _float2(0.512f * 1.2f, 0.512f * 1.2f);
	desc.ViBufferDesc.vMinSize = _float2(0.512f * 0.7f, 0.512f * 0.7f);
	desc.ViBufferDesc.fMinTime = 0.1f;
	desc.ViBufferDesc.fMaxTime = 0.3f;
	desc.ViBufferDesc.fRange = 1.2f;
	desc.ViBufferDesc.fRangeOffset = 1.f;
	desc.ViBufferDesc.fMinSpreadSizeOffset = 0.1f;
	desc.ViBufferDesc.fMaxSpreadSizeOffset = 0.3f;
	desc.ViBufferDesc.iNumInstances = 150;
	desc.ViBufferDesc.iNumSpreadInstances = 100;

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_Particle"), &desc)))
		return E_FAIL;

	ZeroMemory(&desc, sizeof(CParticle::DESC));
	desc.fShowTime = 0.f;
	desc.fSizeUV_X = 1.f / 4.f;
	desc.fSizeUV_Y = 1.f / 4.f;
	desc.iSpriteCount_X = 4;
	desc.iSpriteCount_Y = 4;
	desc.pTextureTag = L"FX_lightning_012_TEX_KJS";
	desc.ViBufferDesc.iNumSprite = 16;
	desc.ViBufferDesc.vMaxSize = _float2(0.512f * 1.2f, 1.024f * 1.2f);
	desc.ViBufferDesc.vMinSize = _float2(0.512f * 0.7f, 1.024f * 0.7f);
	desc.ViBufferDesc.fMinTime = 0.1f;
	desc.ViBufferDesc.fMaxTime = 0.3f;
	desc.ViBufferDesc.fRange = 1.2f;
	desc.ViBufferDesc.fRangeOffset = 1.f;
	desc.ViBufferDesc.fMinSpreadSizeOffset = 0.1f;
	desc.ViBufferDesc.fMaxSpreadSizeOffset = 0.3f;
	desc.ViBufferDesc.iNumInstances = 150;
	desc.ViBufferDesc.iNumSpreadInstances = 100;

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_Particle"), &desc)))
		return E_FAIL;*/

	/*ZeroMemory(&desc, sizeof(CParticle::DESC));
	desc.fShowTime = 0.f;
	desc.fSizeUV_X = 1.f / 4.f;
	desc.fSizeUV_Y = 1.f / 4.f;
	desc.iSpriteCount_X = 4;
	desc.iSpriteCount_Y = 4;
	desc.pTextureTag = L"FX_fire_sq_Edit5";
	desc.ViBufferDesc.iNumSprite = 16;
	desc.ViBufferDesc.vMaxSize = _float2(1.024f * 1.2f, 1.024f * 1.2f);
	desc.ViBufferDesc.vMinSize = _float2(1.024f * 0.7f, 1.024f * 0.7f);
	desc.ViBufferDesc.fMinTime = 0.1f;
	desc.ViBufferDesc.fMaxTime = 0.3f;
	desc.ViBufferDesc.fRange = 1.2f;
	desc.ViBufferDesc.fRangeOffset = 1.f;
	desc.ViBufferDesc.fMinSpreadSizeOffset = 0.1f;
	desc.ViBufferDesc.fMaxSpreadSizeOffset = 0.3f;
	desc.ViBufferDesc.iNumInstances = 150;
	desc.ViBufferDesc.iNumSpreadInstances = 100;

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_Particle"), &desc)))
		return E_FAIL;

	ZeroMemory(&desc, sizeof(CParticle::DESC));
	desc.fShowTime = 0.f;
	desc.fSizeUV_X = 1.f / 4.f;
	desc.fSizeUV_Y = 1.f / 4.f;
	desc.iSpriteCount_X = 4;
	desc.iSpriteCount_Y = 4;
	desc.pTextureTag = L"FX_fire_sq_Edit6";
	desc.ViBufferDesc.iNumSprite = 16;
	desc.ViBufferDesc.vMaxSize = _float2(1.024f * 1.2f, 1.024f * 1.2f);
	desc.ViBufferDesc.vMinSize = _float2(1.024f * 0.7f, 1.024f * 0.7f);
	desc.ViBufferDesc.fMinTime = 0.1f;
	desc.ViBufferDesc.fMaxTime = 0.3f;
	desc.ViBufferDesc.fRange = 1.2f;
	desc.ViBufferDesc.fRangeOffset = 1.f;
	desc.ViBufferDesc.fMinSpreadSizeOffset = 0.1f;
	desc.ViBufferDesc.fMaxSpreadSizeOffset = 0.3f;
	desc.ViBufferDesc.iNumInstances = 150;
	desc.ViBufferDesc.iNumSpreadInstances = 100;

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_Particle"), &desc)))
		return E_FAIL;

	ZeroMemory(&desc, sizeof(CParticle::DESC));
	desc.fShowTime = 0.f;
	desc.fSizeUV_X = 1.f / 2.f;
	desc.fSizeUV_Y = 1.f / 2.f;
	desc.iSpriteCount_X = 2;
	desc.iSpriteCount_Y = 2;
	desc.pTextureTag = L"FX_SparkFlip_010_TEX_HKB";
	desc.ViBufferDesc.iNumSprite = 4;
	desc.ViBufferDesc.vMaxSize = _float2(0.512f * 1.2f, 0.512f * 1.2f);
	desc.ViBufferDesc.vMinSize = _float2(0.512f * 0.7f, 0.512f * 0.7f);
	desc.ViBufferDesc.fMinTime = 0.1f;
	desc.ViBufferDesc.fMaxTime = 0.3f;
	desc.ViBufferDesc.fRange = 1.2f;
	desc.ViBufferDesc.fRangeOffset = 1.f;
	desc.ViBufferDesc.fMinSpreadSizeOffset = 0.1f;
	desc.ViBufferDesc.fMaxSpreadSizeOffset = 0.3f;
	desc.ViBufferDesc.iNumInstances = 150;
	desc.ViBufferDesc.iNumSpreadInstances = 100;

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_Particle"), &desc)))
		return E_FAIL;*/

	//ZeroMemory(&desc, sizeof(CParticle::DESC));
	//desc.fShowTime = 0.f;
	//desc.fSizeUV_X = 1.f / 8.f;
	//desc.fSizeUV_Y = 1.f / 4.f;
	//desc.iSpriteCount_X = 8;
	//desc.iSpriteCount_Y = 4;
	//desc.pTextureTag = L"FX_SmokeDarkFlipBlend";
	//desc.ViBufferDesc.iNumSprite = 32;
	//desc.ViBufferDesc.vMaxSize = _float2(1.024f * 1.2f, 0.512f * 1.2f);
	//desc.ViBufferDesc.vMinSize = _float2(1.024f * 0.7f, 0.512f * 0.7f);
	//desc.ViBufferDesc.fMinTime = 0.1f;
	//desc.ViBufferDesc.fMaxTime = 0.3f;
	//desc.ViBufferDesc.fRange = 1.2f;
	//desc.ViBufferDesc.fRangeOffset = 1.f;
	//desc.ViBufferDesc.fMinSpreadSizeOffset = 0.1f;
	//desc.ViBufferDesc.fMaxSpreadSizeOffset = 0.3f;
	//desc.ViBufferDesc.iNumInstances = 150;
	//desc.ViBufferDesc.iNumSpreadInstances = 100;

	//if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_Particle"), &desc)))
	//	return E_FAIL;

	//ZeroMemory(&desc, sizeof(CParticle::DESC));
	//desc.fShowTime = 0.f;
	//desc.fSizeUV_X = 1.f / 6.f;
	//desc.fSizeUV_Y = 1.f / 6.f;
	//desc.iSpriteCount_X = 6;
	//desc.iSpriteCount_Y = 6;
	//desc.pTextureTag = L"FX_explosion_002_TEX_KJS";
	//desc.ViBufferDesc.iNumSprite = 36;
	//desc.ViBufferDesc.vMaxSize = _float2(1.024f * 1.2f, 1.024f * 1.2f);
	//desc.ViBufferDesc.vMinSize = _float2(1.024f * 0.7f, 1.024f * 0.7f);
	//desc.ViBufferDesc.fMinTime = 0.1f;
	//desc.ViBufferDesc.fMaxTime = 0.3f;
	//desc.ViBufferDesc.fRange = 1.2f;
	//desc.ViBufferDesc.fRangeOffset = 1.f;
	//desc.ViBufferDesc.fMinSpreadSizeOffset = 0.1f;
	//desc.ViBufferDesc.fMaxSpreadSizeOffset = 0.3f;
	//desc.ViBufferDesc.iNumInstances = 150;
	//desc.ViBufferDesc.iNumSpreadInstances = 100;

	//if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_Particle"), &desc)))
	//	return E_FAIL;

	//ZeroMemory(&desc, sizeof(CParticle::DESC));
	//desc.fShowTime = 0.f;
	//desc.fSizeUV_X = 1.f / 6.f;
	//desc.fSizeUV_Y = 1.f / 6.f;
	//desc.iSpriteCount_X = 6;
	//desc.iSpriteCount_Y = 6;
	//desc.pTextureTag = L"FX_fireaura_001_TEX_KJS";
	//desc.ViBufferDesc.iNumSprite = 36;
	//desc.ViBufferDesc.vMaxSize = _float2(1.024f * 1.2f, 1.024f * 1.2f);
	//desc.ViBufferDesc.vMinSize = _float2(1.024f * 0.7f, 1.024f * 0.7f);
	//desc.ViBufferDesc.fMinTime = 0.1f;
	//desc.ViBufferDesc.fMaxTime = 0.3f;
	//desc.ViBufferDesc.fRange = 1.2f;
	//desc.ViBufferDesc.fRangeOffset = 1.f;
	//desc.ViBufferDesc.fMinSpreadSizeOffset = 0.1f;
	//desc.ViBufferDesc.fMaxSpreadSizeOffset = 0.3f;
	//desc.ViBufferDesc.iNumInstances = 150;
	//desc.ViBufferDesc.iNumSpreadInstances = 100;

	//if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_Particle"), &desc)))
	//	return E_FAIL;

	//ZeroMemory(&desc, sizeof(CParticle::DESC));
	//desc.fShowTime = 0.f;
	//desc.fSizeUV_X = 1.f / 4.f;
	//desc.fSizeUV_Y = 1.f / 8.f;
	//desc.iSpriteCount_X = 4;
	//desc.iSpriteCount_Y = 8;
	//desc.pTextureTag = L"FX_fireball_022_edit_TEX_KJS";
	//desc.ViBufferDesc.iNumSprite = 32;
	//desc.ViBufferDesc.vMaxSize = _float2(0.512f * 1.2f, 1.024f * 1.2f);
	//desc.ViBufferDesc.vMinSize = _float2(0.512f * 0.7f, 1.024f * 0.7f);
	//desc.ViBufferDesc.fMinTime = 0.1f;
	//desc.ViBufferDesc.fMaxTime = 0.3f;
	//desc.ViBufferDesc.fRange = 1.2f;
	//desc.ViBufferDesc.fRangeOffset = 1.f;
	//desc.ViBufferDesc.fMinSpreadSizeOffset = 0.1f;
	//desc.ViBufferDesc.fMaxSpreadSizeOffset = 0.3f;
	//desc.ViBufferDesc.iNumInstances = 150;
	//desc.ViBufferDesc.iNumSpreadInstances = 100;

	//if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_Particle"), &desc)))
	//	return E_FAIL;

	//ZeroMemory(&desc, sizeof(CParticle::DESC));
	//desc.fShowTime = 0.f;
	//desc.fSizeUV_X = 1.f / 6.f;
	//desc.fSizeUV_Y = 1.f / 6.f;
	//desc.iSpriteCount_X = 6;
	//desc.iSpriteCount_Y = 6;
	//desc.pTextureTag = L"FX_firesample_001_TEX_kjs";
	//desc.ViBufferDesc.iNumSprite = 36;
	//desc.ViBufferDesc.vMaxSize = _float2(1.024f * 1.2f, 1.024f * 1.2f);
	//desc.ViBufferDesc.vMinSize = _float2(1.024f * 0.7f, 1.024f * 0.7f);
	//desc.ViBufferDesc.fMinTime = 0.1f;
	//desc.ViBufferDesc.fMaxTime = 0.3f;
	//desc.ViBufferDesc.fRange = 1.2f;
	//desc.ViBufferDesc.fRangeOffset = 1.f;
	//desc.ViBufferDesc.fMinSpreadSizeOffset = 0.1f;
	//desc.ViBufferDesc.fMaxSpreadSizeOffset = 0.3f;
	//desc.ViBufferDesc.iNumInstances = 150;
	//desc.ViBufferDesc.iNumSpreadInstances = 100;

	//if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_Particle"), &desc)))
	//	return E_FAIL;

	//ZeroMemory(&desc, sizeof(CParticle::DESC));
	//desc.fShowTime = 0.f;
	//desc.fSizeUV_X = 1.f / 6.f;
	//desc.fSizeUV_Y = 1.f / 6.f;
	//desc.iSpriteCount_X = 6;
	//desc.iSpriteCount_Y = 6;
	//desc.pTextureTag = L"FX_firesample_002_tex_kjs";
	//desc.ViBufferDesc.iNumSprite = 36;
	//desc.ViBufferDesc.vMaxSize = _float2(1.024f * 1.2f, 1.024f * 1.2f);
	//desc.ViBufferDesc.vMinSize = _float2(1.024f * 0.7f, 1.024f * 0.7f);
	//desc.ViBufferDesc.fMinTime = 0.1f;
	//desc.ViBufferDesc.fMaxTime = 0.3f;
	//desc.ViBufferDesc.fRange = 1.2f;
	//desc.ViBufferDesc.fRangeOffset = 1.f;
	//desc.ViBufferDesc.fMinSpreadSizeOffset = 0.1f;
	//desc.ViBufferDesc.fMaxSpreadSizeOffset = 0.3f;
	//desc.ViBufferDesc.iNumInstances = 150;
	//desc.ViBufferDesc.iNumSpreadInstances = 100;

	//if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_Particle"), &desc)))
	//	return E_FAIL;

	//ZeroMemory(&desc, sizeof(CParticle::DESC));
	//desc.fShowTime = 0.f;
	//desc.fSizeUV_X = 1.f / 8.f;
	//desc.fSizeUV_Y = 1.f / 4.f;
	//desc.iSpriteCount_X = 8;
	//desc.iSpriteCount_Y = 4;
	//desc.pTextureTag = L"FX_FlameFlip_007_TEX_HKB";
	//desc.ViBufferDesc.iNumSprite = 32;
	//desc.ViBufferDesc.vMaxSize = _float2(1.024f * 1.2f, 0.512f * 1.2f);
	//desc.ViBufferDesc.vMinSize = _float2(1.024f * 0.7f, 0.512f * 0.7f);
	//desc.ViBufferDesc.fMinTime = 0.1f;
	//desc.ViBufferDesc.fMaxTime = 0.3f;
	//desc.ViBufferDesc.fRange = 1.2f;
	//desc.ViBufferDesc.fRangeOffset = 1.f;
	//desc.ViBufferDesc.fMinSpreadSizeOffset = 0.1f;
	//desc.ViBufferDesc.fMaxSpreadSizeOffset = 0.3f;
	//desc.ViBufferDesc.iNumInstances = 150;
	//desc.ViBufferDesc.iNumSpreadInstances = 100;

	//if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_Particle"), &desc)))
	//	return E_FAIL;

	//ZeroMemory(&desc, sizeof(CParticle::DESC));
	//desc.fShowTime = 0.f;
	//desc.fSizeUV_X = 1.f / 2.f;
	//desc.fSizeUV_Y = 1.f / 2.f;
	//desc.iSpriteCount_X = 2;
	//desc.iSpriteCount_Y = 2;
	//desc.pTextureTag = L"FX_FlameFlip_009_TEX_HKB";
	//desc.ViBufferDesc.iNumSprite = 4;
	//desc.ViBufferDesc.vMaxSize = _float2(0.512f * 1.2f, 0.512f * 1.2f);
	//desc.ViBufferDesc.vMinSize = _float2(0.512f * 0.7f, 0.512f * 0.7f);
	//desc.ViBufferDesc.fMinTime = 0.1f;
	//desc.ViBufferDesc.fMaxTime = 0.3f;
	//desc.ViBufferDesc.fRange = 1.2f;
	//desc.ViBufferDesc.fRangeOffset = 1.f;
	//desc.ViBufferDesc.fMinSpreadSizeOffset = 0.1f;
	//desc.ViBufferDesc.fMaxSpreadSizeOffset = 0.3f;
	//desc.ViBufferDesc.iNumInstances = 150;
	//desc.ViBufferDesc.iNumSpreadInstances = 100;

	//if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_Particle"), &desc)))
	//	return E_FAIL;

	//ZeroMemory(&desc, sizeof(CParticle::DESC));
	//desc.fShowTime = 0.f;
	//desc.fSizeUV_X = 1.f / 4.f;
	//desc.fSizeUV_Y = 1.f / 4.f;
	//desc.iSpriteCount_X = 4;
	//desc.iSpriteCount_Y = 4;
	//desc.pTextureTag = L"FX_FlameFlip_010_TEX_HKB";
	//desc.ViBufferDesc.iNumSprite = 16;
	//desc.ViBufferDesc.vMaxSize = _float2(0.512f * 1.2f, 0.512f * 1.2f);
	//desc.ViBufferDesc.vMinSize = _float2(0.512f * 0.7f, 0.512f * 0.7f);
	//desc.ViBufferDesc.fMinTime = 0.1f;
	//desc.ViBufferDesc.fMaxTime = 0.3f;
	//desc.ViBufferDesc.fRange = 1.2f;
	//desc.ViBufferDesc.fRangeOffset = 1.f;
	//desc.ViBufferDesc.fMinSpreadSizeOffset = 0.1f;
	//desc.ViBufferDesc.fMaxSpreadSizeOffset = 0.3f;
	//desc.ViBufferDesc.iNumInstances = 150;
	//desc.ViBufferDesc.iNumSpreadInstances = 100;

	//if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_Particle"), &desc)))
	//	return E_FAIL;

	//ZeroMemory(&desc, sizeof(CParticle::DESC));
	//desc.fShowTime = 0.f;
	//desc.fSizeUV_X = 1.f / 4.f;
	//desc.fSizeUV_Y = 1.f / 4.f;
	//desc.iSpriteCount_X = 4;
	//desc.iSpriteCount_Y = 4;
	//desc.pTextureTag = L"FX_FlameFlip_011_TEX_HKB";
	//desc.ViBufferDesc.iNumSprite = 16;
	//desc.ViBufferDesc.vMaxSize = _float2(0.512f * 1.2f, 0.512f * 1.2f);
	//desc.ViBufferDesc.vMinSize = _float2(0.512f * 0.7f, 0.512f * 0.7f);
	//desc.ViBufferDesc.fMinTime = 0.1f;
	//desc.ViBufferDesc.fMaxTime = 0.3f;
	//desc.ViBufferDesc.fRange = 1.2f;
	//desc.ViBufferDesc.fRangeOffset = 1.f;
	//desc.ViBufferDesc.fMinSpreadSizeOffset = 0.1f;
	//desc.ViBufferDesc.fMaxSpreadSizeOffset = 0.3f;
	//desc.ViBufferDesc.iNumInstances = 150;
	//desc.ViBufferDesc.iNumSpreadInstances = 100;

	//if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_Particle"), &desc)))
	//	return E_FAIL;

	//ZeroMemory(&desc, sizeof(CParticle::DESC));
	//desc.fShowTime = 0.f;
	//desc.fSizeUV_X = 1.f / 6.f;
	//desc.fSizeUV_Y = 1.f / 6.f;
	//desc.iSpriteCount_X = 6;
	//desc.iSpriteCount_Y = 6;
	//desc.pTextureTag = L"FX_fumefie_005_TEX_KJS";
	//desc.ViBufferDesc.iNumSprite = 36;
	//desc.ViBufferDesc.vMaxSize = _float2(1.024f * 1.2f, 1.024f * 1.2f);
	//desc.ViBufferDesc.vMinSize = _float2(1.024f * 0.7f, 1.024f * 0.7f);
	//desc.ViBufferDesc.fMinTime = 0.1f;
	//desc.ViBufferDesc.fMaxTime = 0.3f;
	//desc.ViBufferDesc.fRange = 1.2f;
	//desc.ViBufferDesc.fRangeOffset = 1.f;
	//desc.ViBufferDesc.fMinSpreadSizeOffset = 0.1f;
	//desc.ViBufferDesc.fMaxSpreadSizeOffset = 0.3f;
	//desc.ViBufferDesc.iNumInstances = 150;
	//desc.ViBufferDesc.iNumSpreadInstances = 100;

	//if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_Particle"), &desc)))
	//	return E_FAIL;

	//ZeroMemory(&desc, sizeof(CParticle::DESC));
	//desc.fShowTime = 0.f;
	//desc.fSizeUV_X = 1.f / 6.f;
	//desc.fSizeUV_Y = 1.f / 6.f;
	//desc.iSpriteCount_X = 6;
	//desc.iSpriteCount_Y = 6;
	//desc.pTextureTag = L"FX_fumefire_001_TEX_KJS";
	//desc.ViBufferDesc.iNumSprite = 36;
	//desc.ViBufferDesc.vMaxSize = _float2(1.024f * 1.2f, 1.024f * 1.2f);
	//desc.ViBufferDesc.vMinSize = _float2(1.024f * 0.7f, 1.024f * 0.7f);
	//desc.ViBufferDesc.fMinTime = 0.1f;
	//desc.ViBufferDesc.fMaxTime = 0.3f;
	//desc.ViBufferDesc.fRange = 1.2f;
	//desc.ViBufferDesc.fRangeOffset = 1.f;
	//desc.ViBufferDesc.fMinSpreadSizeOffset = 0.1f;
	//desc.ViBufferDesc.fMaxSpreadSizeOffset = 0.3f;
	//desc.ViBufferDesc.iNumInstances = 150;
	//desc.ViBufferDesc.iNumSpreadInstances = 100;

	//if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_Particle"), &desc)))
	//	return E_FAIL;

	//ZeroMemory(&desc, sizeof(CParticle::DESC));
	//desc.fShowTime = 0.f;
	//desc.fSizeUV_X = 1.f / 6.f;
	//desc.fSizeUV_Y = 1.f / 6.f;
	//desc.iSpriteCount_X = 6;
	//desc.iSpriteCount_Y = 6;
	//desc.pTextureTag = L"FX_fumefire_002_TEX_KJS";
	//desc.ViBufferDesc.iNumSprite = 36;
	//desc.ViBufferDesc.vMaxSize = _float2(1.024f * 1.2f, 1.024f * 1.2f);
	//desc.ViBufferDesc.vMinSize = _float2(1.024f * 0.7f, 1.024f * 0.7f);
	//desc.ViBufferDesc.fMinTime = 0.1f;
	//desc.ViBufferDesc.fMaxTime = 0.3f;
	//desc.ViBufferDesc.fRange = 1.2f;
	//desc.ViBufferDesc.fRangeOffset = 1.f;
	//desc.ViBufferDesc.fMinSpreadSizeOffset = 0.1f;
	//desc.ViBufferDesc.fMaxSpreadSizeOffset = 0.3f;
	//desc.ViBufferDesc.iNumInstances = 150;
	//desc.ViBufferDesc.iNumSpreadInstances = 100;

	//if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_Particle"), &desc)))
	//	return E_FAIL;

	//ZeroMemory(&desc, sizeof(CParticle::DESC));
	//desc.fShowTime = 0.f;
	//desc.fSizeUV_X = 1.f / 6.f;
	//desc.fSizeUV_Y = 1.f / 6.f;
	//desc.iSpriteCount_X = 6;
	//desc.iSpriteCount_Y = 6;
	//desc.pTextureTag = L"FX_fumefire_003_TEX_KJS";
	//desc.ViBufferDesc.iNumSprite = 36;
	//desc.ViBufferDesc.vMaxSize = _float2(1.024f * 1.2f, 1.024f * 1.2f);
	//desc.ViBufferDesc.vMinSize = _float2(1.024f * 0.7f, 1.024f * 0.7f);
	//desc.ViBufferDesc.fMinTime = 0.1f;
	//desc.ViBufferDesc.fMaxTime = 0.3f;
	//desc.ViBufferDesc.fRange = 1.2f;
	//desc.ViBufferDesc.fRangeOffset = 1.f;
	//desc.ViBufferDesc.fMinSpreadSizeOffset = 0.1f;
	//desc.ViBufferDesc.fMaxSpreadSizeOffset = 0.3f;
	//desc.ViBufferDesc.iNumInstances = 150;
	//desc.ViBufferDesc.iNumSpreadInstances = 100;

	//if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_Particle"), &desc)))
	//	return E_FAIL;

	//ZeroMemory(&desc, sizeof(CParticle::DESC));
	//desc.fShowTime = 0.f;
	//desc.fSizeUV_X = 1.f / 6.f;
	//desc.fSizeUV_Y = 1.f / 6.f;
	//desc.iSpriteCount_X = 6;
	//desc.iSpriteCount_Y = 6;
	//desc.pTextureTag = L"FX_fumefire_004_TEX_KJS";
	//desc.ViBufferDesc.iNumSprite = 36;
	//desc.ViBufferDesc.vMaxSize = _float2(1.024f * 1.2f, 1.024f * 1.2f);
	//desc.ViBufferDesc.vMinSize = _float2(1.024f * 0.7f, 1.024f * 0.7f);
	//desc.ViBufferDesc.fMinTime = 0.1f;
	//desc.ViBufferDesc.fMaxTime = 0.3f;
	//desc.ViBufferDesc.fRange = 1.2f;
	//desc.ViBufferDesc.fRangeOffset = 1.f;
	//desc.ViBufferDesc.fMinSpreadSizeOffset = 0.1f;
	//desc.ViBufferDesc.fMaxSpreadSizeOffset = 0.3f;
	//desc.ViBufferDesc.iNumInstances = 150;
	//desc.ViBufferDesc.iNumSpreadInstances = 100;

	//if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_Particle"), &desc)))
	//	return E_FAIL;

	//ZeroMemory(&desc, sizeof(CParticle::DESC));
	//desc.fShowTime = 0.f;
	//desc.fSizeUV_X = 1.f / 4.f;
	//desc.fSizeUV_Y = 1.f / 4.f;
	//desc.iSpriteCount_X = 4;
	//desc.iSpriteCount_Y = 4;
	//desc.pTextureTag = L"FX_Word_BG_01_CJS";
	//desc.ViBufferDesc.iNumSprite = 16;
	//desc.ViBufferDesc.vMaxSize = _float2(0.512f * 1.2f, 0.512f * 1.2f);
	//desc.ViBufferDesc.vMinSize = _float2(0.512f * 0.7f, 0.512f * 0.7f);
	//desc.ViBufferDesc.fMinTime = 0.1f;
	//desc.ViBufferDesc.fMaxTime = 0.3f;
	//desc.ViBufferDesc.fRange = 1.2f;
	//desc.ViBufferDesc.fRangeOffset = 1.f;
	//desc.ViBufferDesc.fMinSpreadSizeOffset = 0.1f;
	//desc.ViBufferDesc.fMaxSpreadSizeOffset = 0.3f;
	//desc.ViBufferDesc.iNumInstances = 150;
	//desc.ViBufferDesc.iNumSpreadInstances = 100;

	//if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_Particle"), &desc)))
	//	return E_FAIL;

	RELEASE_INSTANCE(CGameInstance)
	return S_OK;
}

HRESULT CLevel_ChapOne::Ready_Layer_UI(const wstring & pLayerTag)
{
	return S_OK;
}

CLevel_ChapOne * CLevel_ChapOne::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLevel_ChapOne*		pInstance = new CLevel_ChapOne(pDevice, pContext);

	if (FAILED(pInstance->Init()))
	{
		MSG_BOX("Failed to Created : CLevel_ChapOne");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void Client::CLevel_ChapOne::Free()
{
	CLevel::Free();
}
