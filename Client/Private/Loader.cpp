#include "stdafx.h"
#include "..\Public\Loader.h"

#include "GameInstance.h"

/*Logo*/
#include "BackGround.h"

#include "TestCylinder.h"
#include "TestSphere.h"
#include "TestCube.h"
#include "Terrain.h"
#include "Ocean.h"

#include "EmptyGameObject.h"

#include "Navigation.h"

#include "Player.h"
#include "ForkLift.h"

#include "Weapon.h"
#include "Shield.h"

#include "Monster.h"

#include "PlayerCamera.h"

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

	m_strLoadingText = TEXT("�ؽ��ĸ� �ε����Դϴ�. ");

	/*For.Prototype_Component_Texture_Logo*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Logo"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Logo/HUD_BMI_LOGO_US.dds")))))
		return E_FAIL;

	m_strLoadingText = TEXT("���۸� �ε����Դϴ�. ");

	m_strLoadingText = TEXT("���� �ε����Դϴ�. ");

	m_strLoadingText = TEXT("���̴��� �ε����Դϴ�. ");

	m_strLoadingText = TEXT("��ü������ �������Դϴ�. ");

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround"), CBackGround::Create(m_pDevice, m_pContext))))
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

	m_strLoadingText = TEXT("�ؽ��ĸ� �ε����Դϴ�. ");

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
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Texture_Ocean_Height"),
	//	CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Meshes/Ocean/T_LargeWaves_H.png"), 1))))
	//	return E_FAIL;

	m_strLoadingText = TEXT("���۸� �ε����Դϴ�. ");
	
	/* For.Prototype_Component_VIBuffer_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height.bmp")))))
		return E_FAIL;

	m_strLoadingText = TEXT("�ݶ��̴��� �ε����Դϴ�. ");

	/* For.Prototype_Component_Collider_AABB*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Collider_AABB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB))))
		return E_FAIL;
	/* For.Prototype_Component_Collider_OBB*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Collider_OBB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_OBB))))
		return E_FAIL;
	/* For.Prototype_Component_Collider_SPHERE*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Collider_SPHERE"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_SPHERE))))
		return E_FAIL;

	m_strLoadingText = TEXT("���� �ε����Դϴ�. ");

	/* For.Prototype_Component_Model_HumanF */
	_matrix			PivotMatrix = XMMatrixIdentity();
	PivotMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Model_HumanF"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Meshes/Bless/HumanF/HumanF.model"), PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Model_ForkLift"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Resources/Meshes/ForkLift/ForkLift.model"), PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Sword*/
	PivotMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) 
		* XMMatrixRotationX(XMConvertToRadians(180.0f)) 
		* XMMatrixRotationY(XMConvertToRadians(270.0f)) 
		* XMMatrixRotationZ(XMConvertToRadians(90.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Model_Sword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Resources/Meshes/Bless/OSW/OSW.model"), PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);
	/* For. Prototype_Component_Model_Shield */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Model_Shield"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Resources/Meshes/Bless/SLD/SLD.model"), PivotMatrix))))
		return E_FAIL;


	m_strLoadingText = TEXT("���̴��� �ε����Դϴ�. ");

	/* For.Prototype_Component_Shader_VtxModel*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Shader_VtxModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxModel.hlsl"), VTXMODEL_DECLARATION::Elements, VTXMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxAnimModel*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimModel.hlsl"), VTXANIMMODEL_DECLARATION::Elements, VTXANIMMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	m_strLoadingText = TEXT("�׺���̼� ���� �ε����Դϴ�. ");
	/* For.Prototype_Component_Navigation */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/Data/Navigation.dat")))))
		return E_FAIL;

	m_strLoadingText = TEXT("��ü������ �������Դϴ�. ");

	/* For.Prototype_GameObject_Player*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player"), CPlayer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_ForkLift*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ForkLift"), CForkLift::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Weapon*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon"), CWeapon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Shield*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Shield"), CShield::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Monster*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster"), CMonster::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* For. Prototype_GameObject_Terrain*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"), CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_PlayerCamera*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PlayerCamera"), CPlayerCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	m_strLoadingText = TEXT("�ε���. ");

	m_isFinished = true;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLoader::Loading_ForChapter_2()
{
	m_strLoadingText = TEXT("�ؽ��ĸ� �ε����Դϴ�. ");

	m_strLoadingText = TEXT("���۸� �ε����Դϴ�. ");

	m_strLoadingText = TEXT("���� �ε����Դϴ�. ");

	m_strLoadingText = TEXT("���̴��� �ε����Դϴ�. ");

	m_strLoadingText = TEXT("�ε���. ");

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_ForTool()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	m_strLoadingText = TEXT("�ؽ��ĸ� �ε����Դϴ�. ");

	m_strLoadingText = TEXT("���۸� �ε����Դϴ�. ");

	m_strLoadingText = TEXT("���� �ε����Դϴ�. ");

	m_strLoadingText = TEXT("���̴��� �ε����Դϴ�. ");

	m_strLoadingText = TEXT("��ü������ �������Դϴ�. ");

	m_strLoadingText = TEXT("�ε���. ");

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
