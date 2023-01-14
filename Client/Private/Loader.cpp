#include "stdafx.h"
#include "..\Public\Loader.h"

#include "GameInstance.h"

#include "BackGround.h"
#include "LogoUI.h"

#include "Sky.h"

#include "TestCylinder.h"
#include "TestSphere.h"
#include "TestCube.h"
#include "Terrain.h"

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
#include "ZombieA.h"
#include "TrollA.h"
#include "TrollQ.h"

#include "PlayerCamera.h"

#include "Effect_Rect.h"
#include "Effect_Point.h"

#include "TownA.h"
#include "BridgeCastle.h"
#include "Stair.h"

#include "Medieval_City.h"
#include "Anor.h"

#include "NorAtk_Trail1.h"
#include "NorAtk_Trail2.h"
#include "NorAtk_Trail3.h"

#include "Wing.h"
#include "Line_Aura.h"
#include "ThunderWave.h"
#include "FireBallLine.h"

#include "SkillChargingUI.h"
#include "ProgressBarUI.h"
#include "DamageFontUI.h"
#include "MonsterNameUI.h"

#include "Abelardo.h"
#include "BalianBollwerk.h"
#include "Chinuwa.h"
#include "Chitata.h"
#include "Delilah.h"

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
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Logo/Bless/Bless_%d.png"),89))))
		return E_FAIL;

	/*For.Prototype_Component_Texture_Logo*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Back"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Logo/Back.png")))))
		return E_FAIL;

	m_strLoadingText = TEXT("버퍼를 로딩중입니다. ");

	m_strLoadingText = TEXT("모델을 로딩중입니다. ");

	m_strLoadingText = TEXT("셰이더를 로딩중입니다. ");

	m_strLoadingText = TEXT("객체원형을 생성중입니다. ");

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround"), CBackGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_LogoUI"), CLogoUI::Create(m_pDevice, m_pContext))))
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
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Newfilter_Chap1.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Cube*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Texture_Cube"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/SkyBox/Sky.dds")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Sky*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Texture_Sky"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Bless/Sky/Chap1_Sky.png")))))
		return E_FAIL;

	// For. EffectTexture~
	{
		/* For.Texture_Trail*/
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Texture_Trail"),
			CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Bless/Effect/%d.png"), 35))))
			return E_FAIL;

		/* For. Texture_DamageFont */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Texture_DamageFont"),
			CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Bless/UI/Damage/Damage_%d.png"), 12))))
			return E_FAIL;
	}

	// For. UI
	{
		/* For.Texture_SkillChargeGuage*/
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Texture_SkillIcon"),
			CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Bless/UI/SkillIcon/Lups_%d.png"), 12))))
			return E_FAIL;

		/* For.Texture_ProgressBar*/
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Texture_ProgressBar"),
			CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Bless/UI/HPGauge/HPGauge_%d.png"), 6))))
			return E_FAIL;

		/* For. Texture_MonsterAimUI*/
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Texture_MonsterAimUI"),
			CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Bless/UI/Aim/Aim_%d.png"),2))))
			return E_FAIL;
	}

	m_strLoadingText = TEXT("버퍼를 로딩중입니다. ");
	
	/* For.Prototype_Component_VIBuffer_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height_Chap1.bmp")))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Sphere */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_VIBuffer_Sphere"),
		CVIBuffer_Sphere::Create(m_pDevice, m_pContext, 0.1f))))
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
		PivotMatrix = XMMatrixScaling(0.075f, 0.075f, 0.075f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Model_Demon"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Meshes/Bless/Demon/Demon.model"), PivotMatrix))))
			return E_FAIL;

		/* For.Prototype_Component_Model_SkeletonWarrior */
		PivotMatrix = XMMatrixScaling(0.075f, 0.075f, 0.075f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Model_SkeletonWarrior"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Meshes/Bless/SkeletonWarrior/SkeletonWarrior.model"), PivotMatrix))))
			return E_FAIL;

		/* For.Prototype_Component_Model_ZombieA */
		PivotMatrix = XMMatrixScaling(0.075f, 0.075f, 0.075f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Model_ZombieA"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Meshes/Bless/Zombie/ZombieA.model"), PivotMatrix))))
			return E_FAIL;

		/* For.Prototype_Component_Model_TrollA */
		PivotMatrix = XMMatrixScaling(0.075f, 0.075f, 0.075f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Model_TrollA"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Meshes/Bless/Troll/TrollA.model"), PivotMatrix))))
			return E_FAIL;

		/* For.Prototype_Component_Model_TrollQ */
		PivotMatrix = XMMatrixScaling(0.075f, 0.075f, 0.075f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Model_TrollQ"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Meshes/Bless/Troll/TrollQ.model"), PivotMatrix))))
			return E_FAIL;
	}

	// For. PLAYER
	{
			/* For.Prototype_Component_Model_HumanF */
			PivotMatrix = XMMatrixScaling(0.075f, 0.075f, 0.075f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
			if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Model_HumanF"),
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Meshes/Bless/Glove/Glove.model"), PivotMatrix))))
				return E_FAIL;

			/* For.Prototype_Component_Model_Upper */
			PivotMatrix = XMMatrixScaling(0.075f, 0.075f, 0.075f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
			if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Model_Upper"),
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Meshes/Bless/Upper/Upper.model"), PivotMatrix))))
				return E_FAIL;

			/* For.Prototype_Component_Model_Lower */
			PivotMatrix = XMMatrixScaling(0.075f, 0.075f, 0.075f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
			if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Model_Lower"),
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Meshes/Bless/Lower/Lower.model"), PivotMatrix))))
				return E_FAIL;

			/* For.Prototype_Component_Model_Glove */
			PivotMatrix = XMMatrixScaling(0.075f, 0.075f, 0.075f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
			if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Model_Glove"),
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Meshes/Bless/Face/Face.model"), PivotMatrix))))
				return E_FAIL;

			/* For.Prototype_Component_Model_Shoulder */
			PivotMatrix = XMMatrixScaling(0.075f, 0.075f, 0.075f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
			if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Model_Shoulder"),
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Meshes/Bless/Shoulder/Shoulder.model"), PivotMatrix))))
				return E_FAIL;

			/* For.Prototype_Component_Model_Boots */
			PivotMatrix = XMMatrixScaling(0.075f, 0.075f, 0.075f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
			if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Model_Boots"),
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Meshes/Bless/Boots/Boots.model"), PivotMatrix))))
				return E_FAIL;

			/* For.Prototype_Component_Model_Belt */
			PivotMatrix = XMMatrixScaling(0.075f, 0.075f, 0.075f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
			if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Model_Belt"),
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Meshes/Bless/Belt/Belt.model"), PivotMatrix))))
				return E_FAIL;

			/* For.Prototype_Component_Model_Helmet */
			PivotMatrix = XMMatrixScaling(0.075f, 0.075f, 0.075f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
			if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Model_Helmet"),
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Meshes/Bless/Helmet/Helmet.model"), PivotMatrix))))
				return E_FAIL;

			{
				/* For.Prototype_Component_Model_HumanF */
				PivotMatrix = XMMatrixScaling(0.075f, 0.075f, 0.075f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
				if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Model_HumanF_B"),
					CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Meshes/Bless/Human/B/Glove.model"), PivotMatrix))))
					return E_FAIL;

				/* For.Prototype_Component_Model_Upper */
				PivotMatrix = XMMatrixScaling(0.075f, 0.075f, 0.075f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
				if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Model_Upper_B"),
					CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Meshes/Bless/Human/B/Upper.model"), PivotMatrix))))
					return E_FAIL;

				/* For.Prototype_Component_Model_Lower */
				PivotMatrix = XMMatrixScaling(0.075f, 0.075f, 0.075f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
				if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Model_Lower_B"),
					CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Meshes/Bless/Human/B/Lower.model"), PivotMatrix))))
					return E_FAIL;

				/* For.Prototype_Component_Model_Glove */
				PivotMatrix = XMMatrixScaling(0.075f, 0.075f, 0.075f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
				if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Model_Glove_B"),
					CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Meshes/Bless/Face/Face.model"), PivotMatrix))))
					return E_FAIL;

				/* For.Prototype_Component_Model_Shoulder */
				PivotMatrix = XMMatrixScaling(0.075f, 0.075f, 0.075f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
				if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Model_Shoulder_B"),
					CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Meshes/Bless/Human/B/Shoulder.model"), PivotMatrix))))
					return E_FAIL;

/* For.Prototype_Component_Model_Boots */
PivotMatrix = XMMatrixScaling(0.075f, 0.075f, 0.075f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Model_Boots_B"),
	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Meshes/Bless/Human/B/Boots.model"), PivotMatrix))))
	return E_FAIL;

/* For.Prototype_Component_Model_Belt */
PivotMatrix = XMMatrixScaling(0.075f, 0.075f, 0.075f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Model_Belt_B"),
	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Meshes/Bless/Human/B/Belt.model"), PivotMatrix))))
	return E_FAIL;

/* For.Prototype_Component_Model_Helmet */
PivotMatrix = XMMatrixScaling(0.075f, 0.075f, 0.075f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Model_Helmet_B"),
	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Meshes/Bless/Human/B/Helmet.model"), PivotMatrix))))
	return E_FAIL;
			}

		{
		/* For.Prototype_Component_Model_HumanF */
		PivotMatrix = XMMatrixScaling(0.075f, 0.075f, 0.075f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Model_HumanF_A"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Meshes/Bless/Human/A/Glove.model"), PivotMatrix))))
			return E_FAIL;

		/* For.Prototype_Component_Model_Upper */
		PivotMatrix = XMMatrixScaling(0.075f, 0.075f, 0.075f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Model_Upper_A"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Meshes/Bless/Human/A/Upper.model"), PivotMatrix))))
			return E_FAIL;

		/* For.Prototype_Component_Model_Lower */
		PivotMatrix = XMMatrixScaling(0.075f, 0.075f, 0.075f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Model_Lower_A"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Meshes/Bless/Human/A/Lower.model"), PivotMatrix))))
			return E_FAIL;

		/* For.Prototype_Component_Model_Glove */
		PivotMatrix = XMMatrixScaling(0.075f, 0.075f, 0.075f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Model_Glove_A"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Meshes/Bless/Face/Face.model"), PivotMatrix))))
			return E_FAIL;

		/* For.Prototype_Component_Model_Shoulder */
		PivotMatrix = XMMatrixScaling(0.075f, 0.075f, 0.075f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Model_Shoulder_A"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Meshes/Bless/Human/A/Shoulder.model"), PivotMatrix))))
			return E_FAIL;

		/* For.Prototype_Component_Model_Boots */
		PivotMatrix = XMMatrixScaling(0.075f, 0.075f, 0.075f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Model_Boots_A"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Meshes/Bless/Human/A/Boots.model"), PivotMatrix))))
			return E_FAIL;

		/* For.Prototype_Component_Model_Belt */
		PivotMatrix = XMMatrixScaling(0.075f, 0.075f, 0.075f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Model_Belt_A"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Meshes/Bless/Human/A/Belt.model"), PivotMatrix))))
			return E_FAIL;

		/* For.Prototype_Component_Model_Helmet */
		PivotMatrix = XMMatrixScaling(0.075f, 0.075f, 0.075f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Model_Helmet_A"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Meshes/Bless/Human/A/Helmet.model"), PivotMatrix))))
			return E_FAIL;
		}


		///* For.Prototype_Component_Model_Sword*/
		//PivotMatrix = XMMatrixScaling(0.075f, 0.075f, 0.075f)
		//	* XMMatrixRotationX(XMConvertToRadians(97.5f))
		//	* XMMatrixRotationY(XMConvertToRadians(78.7f))
		//	* XMMatrixRotationZ(XMConvertToRadians(1.f));
		//if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Model_Sword"),
		//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Resources/Meshes/Bless/TAX/TAX.model"), PivotMatrix))))
		//	return E_FAIL;

		/* For.Prototype_Component_Model_Sword*/
		PivotMatrix = XMMatrixScaling(0.075f, 0.075f, 0.075f)
			* XMMatrixRotationX(XMConvertToRadians(97.5f))
			* XMMatrixRotationY(XMConvertToRadians(78.7f))
			* XMMatrixRotationZ(XMConvertToRadians(1.f));
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Model_Sword"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Resources/Meshes/Bless/Weapon/A/TAX1007.model"), PivotMatrix))))
			return E_FAIL;

		///* For.Prototype_Component_Model_Sword*/
		//PivotMatrix = XMMatrixScaling(0.075f, 0.075f, 0.075f)
		//	* XMMatrixRotationX(XMConvertToRadians(97.5f))
		//	* XMMatrixRotationY(XMConvertToRadians(78.7f))
		//	* XMMatrixRotationZ(XMConvertToRadians(1.f));
		//if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Model_Sword"),
		//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Resources/Meshes/Bless/Weapon/B/TAX0015.model"), PivotMatrix))))
		//	return E_FAIL;

	}

	// For. Ally
	{
		/* For.Prototype_Component_Model_Abelardo */
		PivotMatrix = XMMatrixScaling(0.075f, 0.075f, 0.075f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Model_Abelardo"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Meshes/Bless/Abelardo/Abelardo.model"), PivotMatrix))))
			return E_FAIL;

		/* For.Prototype_Component_Model_BalianBollwerk */
		PivotMatrix = XMMatrixScaling(0.075f, 0.075f, 0.075f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Model_BalianBollwerk"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Meshes/Bless/BalianBollwerk/BalianBollwerk.model"), PivotMatrix))))
			return E_FAIL;

		/* For.Prototype_Component_Model_Chinuwa */
		PivotMatrix = XMMatrixScaling(0.075f, 0.075f, 0.075f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Model_Chinuwa"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Meshes/Bless/Chinuwa/Chinuwa.model"), PivotMatrix))))
			return E_FAIL;

		/* For.Prototype_Component_Model_Chitata */
		PivotMatrix = XMMatrixScaling(0.075f, 0.075f, 0.075f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Model_Chitata"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Meshes/Bless/Chitata/Chitata.model"), PivotMatrix))))
			return E_FAIL;

		/* For.Prototype_Component_Model_Delilah */
		PivotMatrix = XMMatrixScaling(0.075f, 0.075f, 0.075f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Model_Delilah"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Meshes/Bless/Delilah/Delilah.model"), PivotMatrix))))
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

	// For.Effect
	{
		/* For.NormalAtk1_Trail */
		PivotMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("NormalAtk1_Trail"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Resources/Meshes/Bless/WeaponTrail/NormalAtk1/NormalAtk1_Trail.model"), PivotMatrix))))
			return E_FAIL;

		/* For.NormalAtk2_Trail */
		PivotMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("NormalAtk2_Trail"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Resources/Meshes/Bless/WeaponTrail/NormalAtk2/NormalAtk2_Trail.model"), PivotMatrix))))
			return E_FAIL;

		/* For.NormalAtk3_Trail */
		PivotMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("NormalAtk3_Trail"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Resources/Meshes/Bless/WeaponTrail/NormalAtk3/NormalAtk3_Trail.model"), PivotMatrix))))
			return E_FAIL;

		/* For.Wing */
		PivotMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Wing"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Resources/Meshes/Bless/Effect/Wing.model"), PivotMatrix))))
			return E_FAIL;

		/* For.Line_Aura */
		PivotMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Line_Aura"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Resources/Meshes/Bless/Effect/Line_Aura.model"), PivotMatrix))))
			return E_FAIL;

		/* For. ThunderWave */
		PivotMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("ThunderWave"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Resources/Meshes/Bless/Effect/ThunderWave.model"), PivotMatrix))))
			return E_FAIL;

		/* For. FireBallLine */
		PivotMatrix = XMMatrixScaling(0.2f, 0.2f, 0.2f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("FireBallLine"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Resources/Meshes/Bless/Effect/FireBallLine.model"), PivotMatrix))))
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

	// PLAYER
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

	// MONSTER
	{
		/* For.Prototype_GameObject_Demon*/
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Demon"), CDemon::Create(m_pDevice, m_pContext))))
			return E_FAIL;
		
		/* For.Prototype_GameObject_SkeletonWarrior*/
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SkeletonWarrior"), CSkeletonWarrior::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		/* For.Prototype_GameObject_ZombieA*/
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ZombieA"), CZombieA::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		/* For.Prototype_GameObject_TrollA*/
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TrollA"), CTrollA::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		/* For.Prototype_GameObject_TrollQ*/
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TrollQ"), CTrollQ::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}

	/* For. Ally*/
	{
		/* For.Prototype_GameObject_Abelardo*/
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Abelardo"), CAbelardo::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		/* For.Prototype_GameObject_BalianBollwerk*/
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BalianBollwerk"), CBalianBollwerk::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		/* For.Prototype_GameObject_Chinuwa*/
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Chinuwa"), CChinuwa::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		/* For.Prototype_GameObject_Chitata*/
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Chitata"), CChitata::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		/* For.Prototype_GameObject_Delilah*/
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Delilah"), CDelilah::Create(m_pDevice, m_pContext))))
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

	/* For. Prototype_GameObject_NorAtk_Trail1*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_NorAtk_Trail1"), CNorAtk_Trail1::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_NorAtk_Trail2*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_NorAtk_Trail2"), CNorAtk_Trail2::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_NorAtk_Trail3*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_NorAtk_Trail3"), CNorAtk_Trail3::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Wing*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Wing"), CWing::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Line_Aura*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Line_Aura"), CLine_Aura::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_ThunderWave*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ThunderWave"), CThunderWave::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_FireBallLine*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FireBallLine"), CFireBallLine::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	{
		/* For. Prototype_GameObject_SkillChargingUI*/
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SkillChargingUI"), CSkillChargingUI::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		/* For. Prototype_GameObject_ProgressBarUI*/
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ProgressBarUI"), CProgressBarUI::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		/* For. Prototype_GameObject_AimUI*/
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_AimUI"), CEffect_Rect::Create(m_pDevice, m_pContext))))
			return E_FAIL;
		
		/* For. Prototype_GameObject_DamageFontUI*/
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_DamageFontUI"), CDamageFontUI::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		/* For. Prototype_GameObject_MonsterNameFontUI*/
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MonsterNameFontUI"), CMonsterNameUI::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}

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
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Newfilter_Chap2_Change.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Sky*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP2, TEXT("Prototype_Component_Texture_Sky"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Bless/Sky/Chap2_Sky.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Ocean_Height*/
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP2, TEXT("Prototype_Component_Texture_Ocean_Height"),
	//	CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Meshes/Ocean/T_LargeWaves_H.png"), 1))))
	//	return E_FAIL;

	m_strLoadingText = TEXT("버퍼를 로딩중입니다. ");

	/* For.Prototype_Component_VIBuffer_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP2, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height_Chap2.bmp")))))
		return E_FAIL;

	m_strLoadingText = TEXT("모델을 로딩중입니다. ");

	_matrix			PivotMatrix = XMMatrixIdentity();

	/* For.Medieval_City */
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(15.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP2, TEXT("Medieval_City"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Resources/Meshes/Bless/Maps/Medieval_City/Medieval_City.model"), PivotMatrix))))
		return E_FAIL;

	m_strLoadingText = TEXT("셰이더를 로딩중입니다. ");

	m_strLoadingText = TEXT("네비게이션 정보 로딩중입니다. ");
	
	/* For.Prototype_Component_Navigation */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP2, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/Data/Navigation_CHAP2.dat")))))
		return E_FAIL;

	// 스카이박스 수정해야하고 터레인 수정해야함 스테이지2 넘어가서는 터레인과 스카이박스 바꿔줄 예정

	///* For.Prototype_GameObject_Sky*/
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sky_Chap2"), CSky::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

	///* For. Prototype_GameObject_Terrain*/
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain_Chap2"), CTerrain::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

	///* For. Prototype_GameObject_PlayerCamera*/
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PlayerCamera_Chap2"), CPlayerCamera::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

	///* For. Prototype_GameObject_Effect_Rect*/
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Rect_Chap2"), CEffect_Rect::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

	///* For. Prototype_GameObject_Effect_Point*/
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Point_Chap2"), Effect_Point::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

	///* For. Prototype_GameObject_Cube*/
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Cube_Chap2"), CTestCube::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

	/* For. Prototype_GameObject_Medieval_City*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Medieval_City"), CMedieval_City::Create(m_pDevice, m_pContext))))
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

	/* For.Prototype_Component_Texture_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP3, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/BMI_Terrain/Level1/Diffuse/TextureD_%d.dds"), 7))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Brush*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP3, TEXT("Prototype_Component_Texture_Brush"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Brush.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Filter*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP3, TEXT("Prototype_Component_Texture_Filter"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Newfilter_Chap3_Change.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Sky*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP3, TEXT("Prototype_Component_Texture_Sky"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Bless/Sky/Chap3_Sky.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Weapon_Effect*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP3, TEXT("Prototype_Component_Texture_Weapon_Effect"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Bless/Effect/FX_blizzardDecal_01_TEX_KJS.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Ocean_Height*/
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP3, TEXT("Prototype_Component_Texture_Ocean_Height"),
	//	CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Meshes/Ocean/T_LargeWaves_H.png"), 1))))
	//	return E_FAIL;

	m_strLoadingText = TEXT("버퍼를 로딩중입니다. ");

	/* For.Prototype_Component_VIBuffer_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP3, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height.bmp")))))
		return E_FAIL;

	m_strLoadingText = TEXT("모델을 로딩중입니다. ");

	_matrix			PivotMatrix = XMMatrixIdentity();

	/* For.Anor */
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(90.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP3, TEXT("Anor"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Resources/Meshes/Bless/Maps/Anor_Londo/Anor.model"), PivotMatrix))))
		return E_FAIL;

	m_strLoadingText = TEXT("객체원형을 로딩중입니다. ");

	/* For. Prototype_GameObject_Anor*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Anor"), CAnor::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	m_strLoadingText = TEXT("네비게이션 정보 로딩중입니다. ");

	/* For.Prototype_Component_Navigation */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP3, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/Data/Navigation_CHAP3.dat")))))
		return E_FAIL;

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
