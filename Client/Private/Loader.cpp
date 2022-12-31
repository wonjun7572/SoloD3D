#include "stdafx.h"
#include "..\Public\Loader.h"

#include "GameInstance.h"

#include "BackGround.h"

#include "Sky.h"

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
#include "Upper.h"
#include "Lower.h"
#include "Glove.h"
#include "Shoulder.h"
#include "Boots.h"
#include "Belt.h"
#include "Helmet.h"

#include "Demon.h"
#include "SkeletonWarrior.h"

#include "PlayerCamera.h"

#include "Effect_Rect.h"
#include "Effect_Point.h"

#include "TownA.h"
#include "BridgeCastle.h"
#include "Stair.h"

unsigned int	g_LEVEL = 0;

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
	case LEVEL_CHAP3:
		Loading_ForChapter_3();
		break;
	}
	return;
}

HRESULT CLoader::Init(LEVEL eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;

	g_LEVEL = eNextLevelID;
	
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

	m_strLoadingText = TEXT("버퍼를 로딩중입니다. ");

	m_strLoadingText = TEXT("모델을 로딩중입니다. ");

	m_strLoadingText = TEXT("셰이더를 로딩중입니다. ");

	m_strLoadingText = TEXT("객체원형을 생성중입니다. ");

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
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/NewFilter.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Sky*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Texture_Sky"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/SkyBox/Sky.dds")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Weapon_Effect*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Texture_Weapon_Effect"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Bless/Effect/FX_blizzardDecal_01_TEX_KJS.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Weapon_Skill1_Effect*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Texture_Weapon_Skill1_Effect"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Bless/Effect/FX_AuraHole_001_TEX_HKB.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Ocean_Height*/
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Texture_Ocean_Height"),
	//	CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Meshes/Ocean/T_LargeWaves_H.png"), 1))))
	//	return E_FAIL;

	m_strLoadingText = TEXT("버퍼를 로딩중입니다. ");
	
	/* For.Prototype_Component_VIBuffer_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height.bmp")))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Sphere */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_VIBuffer_Sphere"),
		CVIBuffer_Sphere::Create(m_pDevice, m_pContext, 1.f))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Cube */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Rect_Instancing */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_VIBuffer_Rect_Instancing"),
		CVIBuffer_Rect_Instancing::Create(m_pDevice, m_pContext, 3))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Point_Instancing */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_VIBuffer_Point_Instancing"),
		CVIBuffer_Point_Instancing::Create(m_pDevice, m_pContext, 30))))
		return E_FAIL;

	m_strLoadingText = TEXT("콜라이더를 로딩중입니다. ");

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

	m_strLoadingText = TEXT("모델을 로딩중입니다. ");

	_matrix			PivotMatrix = XMMatrixIdentity();

	// For. MONSTER
	{
		/* For.Prototype_Component_Model_Demon */
		PivotMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Model_Demon"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Meshes/Bless/Demon/Demon.model"), PivotMatrix))))
			return E_FAIL;

		/* For.Prototype_Component_Model_SkeletonWarrior */
		PivotMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Model_SkeletonWarrior"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Meshes/Bless/SkeletonWarrior/SkeletonWarrior.model"), PivotMatrix))))
			return E_FAIL;
	}

	// For. PLAYER
	{
		/* For.Prototype_Component_Model_HumanF */
		PivotMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Model_HumanF"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Meshes/Bless/Glove/Glove.model"), PivotMatrix))))
			return E_FAIL;

		/* For.Prototype_Component_Model_Upper */
		PivotMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Model_Upper"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Meshes/Bless/Upper/Upper.model"), PivotMatrix))))
			return E_FAIL;

		/* For.Prototype_Component_Model_Lower */
		PivotMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Model_Lower"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Meshes/Bless/Lower/Lower.model"), PivotMatrix))))
			return E_FAIL;

		/* For.Prototype_Component_Model_Glove */
		PivotMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Model_Glove"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Meshes/Bless/Face/Face.model"), PivotMatrix))))
			return E_FAIL;

		/* For.Prototype_Component_Model_Shoulder */
		PivotMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Model_Shoulder"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Meshes/Bless/Shoulder/Shoulder.model"), PivotMatrix))))
			return E_FAIL;

		/* For.Prototype_Component_Model_Boots */
		PivotMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Model_Boots"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Meshes/Bless/Boots/Boots.model"), PivotMatrix))))
			return E_FAIL;

		/* For.Prototype_Component_Model_Belt */
		PivotMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Model_Belt"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Meshes/Bless/Belt/Belt.model"), PivotMatrix))))
			return E_FAIL;

		/* For.Prototype_Component_Model_Helmet */
		PivotMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Model_Helmet"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Meshes/Bless/Helmet/Helmet.model"), PivotMatrix))))
			return E_FAIL;

		/* For.Prototype_Component_Model_Sword*/
		PivotMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f)
			* XMMatrixRotationX(XMConvertToRadians(97.5f))
			* XMMatrixRotationY(XMConvertToRadians(78.7f))
			* XMMatrixRotationZ(XMConvertToRadians(1.f));
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Model_Sword"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Resources/Meshes/Bless/TAX/TAX.model"), PivotMatrix))))
			return E_FAIL;
	}

	// For. Maps
	{
		/* For.Tower_BridgeCastle */
		PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Tower_BridgeCastle"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Resources/Meshes/Bless/Maps/Tower_BridgeCastle/Tower_BridgeCastle.model"), PivotMatrix))))
			return E_FAIL;

		/* For.TownA */
		PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(90.0f));
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("TownA"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Resources/Meshes/Bless/Maps/TownA/TownA.model"), PivotMatrix))))
			return E_FAIL;

		/* For.Stair */
		PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Stair"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Resources/Meshes/Bless/Maps/Stair/Stair.model"), PivotMatrix))))
			return E_FAIL;
	}
	m_strLoadingText = TEXT("셰이더를 로딩중입니다. ");
		
	m_strLoadingText = TEXT("네비게이션 정보 로딩중입니다. ");
	/* For.Prototype_Component_Navigation */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/Data/Navigation_CHAP1.dat")))))
		return E_FAIL;

	m_strLoadingText = TEXT("객체원형을 생성중입니다. ");

	/* For.Prototype_GameObject_Sky*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sky"), CSky::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	{
		/* For.Prototype_GameObject_Player*/
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player"), CPlayer::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		/* For.Prototype_GameObject_Weapon*/
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon"), CWeapon::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		/* For.Prototype_GameObject_Upper*/
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Upper"), CUpper::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		/* For.Prototype_GameObject_Lower*/
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Lower"), CLower::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		/* For.Prototype_GameObject_Glove*/
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Glove"), CGlove::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		/* For.Prototype_GameObject_Shoulder*/
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Shoulder"), CShoulder::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		/* For.Prototype_GameObject_Boots*/
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Boots"), CBoots::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		/* For.Prototype_GameObject_Belt*/
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Belt"), CBelt::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		/* For.Prototype_GameObject_Helmet*/
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Helmet"), CHelmet::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}

	{
		/* For.Prototype_GameObject_Demon*/
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Demon"), CDemon::Create(m_pDevice, m_pContext))))
			return E_FAIL;
		
		/* For.Prototype_GameObject_SkeletonWarrior*/
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SkeletonWarrior"), CSkeletonWarrior::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	
	/* For. Prototype_GameObject_Terrain*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"), CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_PlayerCamera*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PlayerCamera"), CPlayerCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Effect_Rect*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Rect"), CEffect_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Effect_Point*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Point"), Effect_Point::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Cube*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Cube"), CTestCube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_TownA*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TownA"), CTownA::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_BridgeCastle*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BridgeCastle"), CBridgeCastle::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Stair*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Stair"), CStair::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	m_strLoadingText = TEXT("로딩끝. ");

	m_isFinished = true;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLoader::Loading_ForChapter_2()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	m_strLoadingText = TEXT("텍스쳐를 로딩중입니다. ");

	/* For.Prototype_Component_Texture_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP2, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/BMI_Terrain/Level1/Diffuse/TextureD_%d.dds"), 7))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Brush*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP2, TEXT("Prototype_Component_Texture_Brush"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Brush.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Filter*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP2, TEXT("Prototype_Component_Texture_Filter"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/NewFilter.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Sky*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP2, TEXT("Prototype_Component_Texture_Sky"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/SkyBox/Sky.dds")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Weapon_Effect*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP2, TEXT("Prototype_Component_Texture_Weapon_Effect"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Bless/Effect/FX_blizzardDecal_01_TEX_KJS.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Weapon_Skill1_Effect*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP2, TEXT("Prototype_Component_Texture_Weapon_Skill1_Effect"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Bless/Effect/FX_AuraHole_001_TEX_HKB.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Ocean_Height*/
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP2, TEXT("Prototype_Component_Texture_Ocean_Height"),
	//	CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Meshes/Ocean/T_LargeWaves_H.png"), 1))))
	//	return E_FAIL;

	m_strLoadingText = TEXT("버퍼를 로딩중입니다. ");

	/* For.Prototype_Component_VIBuffer_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP2, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height.bmp")))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Sphere */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP2, TEXT("Prototype_Component_VIBuffer_Sphere"),
		CVIBuffer_Sphere::Create(m_pDevice, m_pContext, 1.f))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Cube */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP2, TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Rect_Instancing */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP2, TEXT("Prototype_Component_VIBuffer_Rect_Instancing"),
		CVIBuffer_Rect_Instancing::Create(m_pDevice, m_pContext, 3))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Point_Instancing */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP2, TEXT("Prototype_Component_VIBuffer_Point_Instancing"),
		CVIBuffer_Point_Instancing::Create(m_pDevice, m_pContext, 30))))
		return E_FAIL;

	m_strLoadingText = TEXT("콜라이더를 로딩중입니다. ");

	/* For.Prototype_Component_Collider_AABB*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP2, TEXT("Prototype_Component_Collider_AABB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB))))
		return E_FAIL;
	/* For.Prototype_Component_Collider_OBB*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP2, TEXT("Prototype_Component_Collider_OBB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_OBB))))
		return E_FAIL;
	/* For.Prototype_Component_Collider_SPHERE*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP2, TEXT("Prototype_Component_Collider_SPHERE"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_SPHERE))))
		return E_FAIL;

	m_strLoadingText = TEXT("모델을 로딩중입니다. ");

	_matrix			PivotMatrix = XMMatrixIdentity();

	// For. PLAYER
	{
		/* For.Prototype_Component_Model_HumanF */
		PivotMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP2, TEXT("Prototype_Component_Model_HumanF"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Meshes/Bless/Glove/Glove.model"), PivotMatrix))))
			return E_FAIL;

		/* For.Prototype_Component_Model_Upper */
		PivotMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP2, TEXT("Prototype_Component_Model_Upper"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Meshes/Bless/Upper/Upper.model"), PivotMatrix))))
			return E_FAIL;

		/* For.Prototype_Component_Model_Lower */
		PivotMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP2, TEXT("Prototype_Component_Model_Lower"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Meshes/Bless/Lower/Lower.model"), PivotMatrix))))
			return E_FAIL;

		/* For.Prototype_Component_Model_Glove */
		PivotMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP2, TEXT("Prototype_Component_Model_Glove"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Meshes/Bless/Face/Face.model"), PivotMatrix))))
			return E_FAIL;

		/* For.Prototype_Component_Model_Shoulder */
		PivotMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP2, TEXT("Prototype_Component_Model_Shoulder"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Meshes/Bless/Shoulder/Shoulder.model"), PivotMatrix))))
			return E_FAIL;

		/* For.Prototype_Component_Model_Boots */
		PivotMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP2, TEXT("Prototype_Component_Model_Boots"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Meshes/Bless/Boots/Boots.model"), PivotMatrix))))
			return E_FAIL;

		/* For.Prototype_Component_Model_Belt */
		PivotMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP2, TEXT("Prototype_Component_Model_Belt"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Meshes/Bless/Belt/Belt.model"), PivotMatrix))))
			return E_FAIL;

		/* For.Prototype_Component_Model_Helmet */
		PivotMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP2, TEXT("Prototype_Component_Model_Helmet"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Meshes/Bless/Helmet/Helmet.model"), PivotMatrix))))
			return E_FAIL;

		/* For.Prototype_Component_Model_Sword*/
		PivotMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f)
			* XMMatrixRotationX(XMConvertToRadians(97.5f))
			* XMMatrixRotationY(XMConvertToRadians(78.7f))
			* XMMatrixRotationZ(XMConvertToRadians(1.f));
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP2, TEXT("Prototype_Component_Model_Sword"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Resources/Meshes/Bless/TAX/TAX.model"), PivotMatrix))))
			return E_FAIL;
	}

	/* For.Medieval_City */
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(90.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP2, TEXT("Medieval_City"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Resources/Meshes/Bless/Maps/Medieval_City/Medieval_City.model"), PivotMatrix))))
		return E_FAIL;

	m_strLoadingText = TEXT("셰이더를 로딩중입니다. ");

	m_strLoadingText = TEXT("네비게이션 정보 로딩중입니다. ");
	
	/* For.Prototype_Component_Navigation */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP2, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/Data/Navigation_CHAP1.dat")))))
		return E_FAIL;


	/* For.Prototype_GameObject_Sky*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sky_Chap2"), CSky::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	{
		/* For.Prototype_GameObject_Player*/
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player_Chap2"), CPlayer::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		/* For.Prototype_GameObject_Weapon*/
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Chap2"), CWeapon::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		/* For.Prototype_GameObject_Upper*/
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Upper_Chap2"), CUpper::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		/* For.Prototype_GameObject_Lower*/
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Lower_Chap2"), CLower::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		/* For.Prototype_GameObject_Glove*/
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Glove_Chap2"), CGlove::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		/* For.Prototype_GameObject_Shoulder*/
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Shoulder_Chap2"), CShoulder::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		/* For.Prototype_GameObject_Boots*/
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Boots_Chap2"), CBoots::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		/* For.Prototype_GameObject_Belt*/
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Belt_Chap2"), CBelt::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		/* For.Prototype_GameObject_Helmet*/
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Helmet_Chap2"), CHelmet::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}

	{
		/* For.Prototype_GameObject_Demon*/
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Demon_Chap2"), CDemon::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		/* For.Prototype_GameObject_SkeletonWarrior*/
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SkeletonWarrior_Chap2"), CSkeletonWarrior::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}

	/* For. Prototype_GameObject_Terrain*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain_Chap2"), CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_PlayerCamera*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PlayerCamera_Chap2"), CPlayerCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Effect_Rect*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Rect_Chap2"), CEffect_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Effect_Point*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Point_Chap2"), Effect_Point::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Cube*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Cube_Chap2"), CTestCube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	m_strLoadingText = TEXT("로딩끝. ");

	m_isFinished = true;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLoader::Loading_ForChapter_3()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	m_strLoadingText = TEXT("텍스쳐를 로딩중입니다. ");

	m_strLoadingText = TEXT("버퍼를 로딩중입니다. ");

	m_strLoadingText = TEXT("모델을 로딩중입니다. ");

	_matrix			PivotMatrix = XMMatrixIdentity();

	/* For.Anor */
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(90.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP3, TEXT("Anor"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Resources/Meshes/Bless/Maps/Anor_Londo/Anor.model"), PivotMatrix))))
		return E_FAIL;

	m_strLoadingText = TEXT("셰이더를 로딩중입니다. ");

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
