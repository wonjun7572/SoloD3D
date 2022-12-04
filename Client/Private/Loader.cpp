#include "stdafx.h"
#include "..\Public\Loader.h"

#include "GameInstance.h"

/*Logo*/
#include "BackGround.h"
#include "Enter_KeyUI.h"

#include "TestCylinder.h"
#include "TestSphere.h"
#include "TestCube.h"
#include "Terrain.h"
#include "Ocean.h"

#include "Sheila.h"

/*Statue*/
#include "DragonStatue_1.h"
#include "DragonStatue_4.h"

/*Forest*/
#include "FieldGrass_1.h"

/*GuFeng*/
#include "GF_Arch.h"

#include "EmptyGameObject.h"

CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:m_pDevice(pDevice),
	m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

_uint APIENTRY LoadingThread(void* pArg)
{
	CLoader*		pLoader = (CLoader*)pArg;

	EnterCriticalSection(&pLoader->Get_CriticalSection());

	pLoader->SwitchForThread(pLoader->Get_NextLevelID());

	LeaveCriticalSection(&pLoader->Get_CriticalSection());

	return 0;
}

void CLoader::SwitchForThread(LEVEL eNextLevelID)
{
	switch (eNextLevelID)
	{
	case LEVEL_LOGO:
		Loading_ForLogo();
		break;
	case LEVEL_CHAP1:
		Loading_ForChapter_1();
		break;
	case LEVEL_CHAP2:
		Loading_ForChapter_2();
		break;
	case LEVEL_TOOL:
		Loading_ForTool();
		break;
	}
	return;
}

HRESULT CLoader::Init(LEVEL eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;
	
	InitializeCriticalSection(&m_CriticalSection);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, LoadingThread, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading_ForLogo()
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	m_strLoadingText = TEXT("텍스쳐를 로딩중입니다. ");

	/*For.Prototype_Component_Texture_Logo*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Logo"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Logo/HUD_BMI_LOGO_US.dds")))))
		return E_FAIL;

	/*For.Prototype_Component_Texture_EnterKey*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_EnterKey"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Logo/HUD_EnterKey.dds")))))
		return E_FAIL;

	m_strLoadingText = TEXT("버퍼를 로딩중입니다. ");

	m_strLoadingText = TEXT("모델을 로딩중입니다. ");

	m_strLoadingText = TEXT("셰이더를 로딩중입니다. ");

	m_strLoadingText = TEXT("객체원형을 생성중입니다. ");

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround"), CBackGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EnterKeyUI"), CEnter_KeyUI::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Empty*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Empty"), CEmptyGameObject::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	m_isFinished = true;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLoader::Loading_ForChapter_1()
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	m_strLoadingText = TEXT("텍스쳐를 로딩중입니다. ");

	/* For.Prototype_Component_Texture_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/BMI_Terrain/Level1/Diffuse/TextureD_%d.dds"), 7))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Brush*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Texture_Brush"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Brush.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Filter*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Texture_Filter"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Filter.bmp"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Ocean_Height*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Texture_Ocean_Height"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Meshes/Ocean/T_LargeWaves_H.png"), 1))))
		return E_FAIL;

	m_strLoadingText = TEXT("버퍼를 로딩중입니다. ");
	
	/* For.Prototype_Component_VIBuffer_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height.bmp")))))
		return E_FAIL;


	m_strLoadingText = TEXT("모델을 로딩중입니다. ");

	/* For.Prototype_Component_Model_Fiona */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Model_Fiona"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Fiona/Fiona.fbx"))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Ocean */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Model_Ocean"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/Ocean/Ocean.fbx"))))
		return E_FAIL;

	///* For.Prototype_Component_Model_DragonStatue_1 */
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Model_DragonStatue_1"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/DragonStatue/1/DragonStatue_1.fbx"))))
	//	return E_FAIL;

	///* For.Prototype_Component_Model_DragonStatue_4 */
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Model_DragonStatue_4"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/DragonStatue/4/DragonStatue_4.fbx"))))
	//	return E_FAIL;

	///* For.Prototype_Component_Model_FieldGrass_1 */
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Model_FieldGrass_1"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/Forest/Grass/FieldGrass_1.fbx"))))
	//	return E_FAIL;

	///* For.Prototype_Component_Model_GF_Arch */
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Model_GF_Arch"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/Art/GuFeng/Arch/Arch.fbx"))))
	//	return E_FAIL;

	{
		/*For. Prototype_EMPTY!!!*/
	/*	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Model_ChaBei"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/Art/GuFeng/ChaBei/ChaBei.fbx"))))
			return E_FAIL;

		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Model_ChineseLamp"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/Art/GuFeng/ChineseLamp/ChineseLamp.fbx"))))
			return E_FAIL;

		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Model_ClothesRack"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/Art/GuFeng/ClothesRack/ClothesRack.fbx"))))
			return E_FAIL;*/
	}

	m_strLoadingText = TEXT("셰이더를 로딩중입니다. ");

	/* For.Prototype_Component_Shader_VtxModel*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Shader_VtxModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxModel.hlsl"), VTXMODEL_DECLARATION::Elements, VTXMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxAnimModel*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimModel.hlsl"), VTXANIMMODEL_DECLARATION::Elements, VTXANIMMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	m_strLoadingText = TEXT("객체원형을 생성중입니다. ");

	/* For.Prototype_GameObject_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"), CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Sheila */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sheila"),CSheila::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Ocean */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Ocean"), COcean::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	///* For.Prototype_GameObject_DragonStatue_1 */
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_DragonStatue_1"), CDragonStatue_1::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

	///* For.Prototype_GameObject_DragonStatue_4 */
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_DragonStatue_4"), CDragonStatue_4::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

	///* For.Prototype_GameObject_FieldGrass_1*/
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FieldGrass_1"), CFieldGrass_1::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

	///* For.Prototype_GameObject_GF_Arch*/
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_GF_Arch"), CGF_Arch::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

	m_strLoadingText = TEXT("로딩끝. ");

	m_isFinished = true;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLoader::Loading_ForChapter_2()
{
	m_strLoadingText = TEXT("텍스쳐를 로딩중입니다. ");

	m_strLoadingText = TEXT("버퍼를 로딩중입니다. ");

	m_strLoadingText = TEXT("모델을 로딩중입니다. ");

	m_strLoadingText = TEXT("셰이더를 로딩중입니다. ");

	m_strLoadingText = TEXT("로딩끝. ");

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_ForTool()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	m_strLoadingText = TEXT("텍스쳐를 로딩중입니다. ");

	m_strLoadingText = TEXT("버퍼를 로딩중입니다. ");

	m_strLoadingText = TEXT("모델을 로딩중입니다. ");

	m_strLoadingText = TEXT("셰이더를 로딩중입니다. ");

	m_strLoadingText = TEXT("객체원형을 생성중입니다. ");

	m_strLoadingText = TEXT("로딩끝. ");

	m_isFinished = true;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CLoader * CLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID)
{
	CLoader*		pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Init(eNextLevelID)))
	{
		MSG_BOX("Failed to Created : CLoader");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLoader::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);
	CloseHandle(m_hThread);
	DeleteObject(m_hThread);
	DeleteCriticalSection(&m_CriticalSection);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
