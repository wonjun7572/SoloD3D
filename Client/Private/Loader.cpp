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

#include "TargetAimEffect.h"

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
#include "FireWave.h"
#include "FireBallLine.h"
#include "ExplosionE.h"
#include "ArcaneE.h"
#include "FireE_0.h"
#include "FlameE.h"

#include "SkillChargingUI.h"
#include "ProgressBarUI.h"
#include "DamageFontUI.h"
#include "MonsterNameUI.h"

#include "Abelardo.h"
#include "BalianBollwerk.h"
#include "Chinuwa.h"
#include "Chitata.h"
#include "Delilah.h"
#include "HitE.h"
#include "SparkE.h"
#include "QskillCrackE.h"

#include "ConversationUI.h"
#include "MonsterHpUI.h"
#include "PortraitCircleUI.h"

#include "Tree.h"
#include "BoomWave_0.h"
#include "BoomWave_1.h"
#include "CMedieval_City_Down.h"
#include "CRainEffect.h"
#include "Twister_Cycle.h"
#include "Twister_Line.h"
#include "FSkillTrail.h"
#include "DashParticle.h"

#include "LightCamera.h"
#include "DemonSkillFloor.h"
#include "DemonSkillCircular.h"
#include "DemonSkillStraight.h"

#include "Rock_1.h"
#include "Rock_2.h"
#include "Rock_3.h"
#include "Rock_4.h"
#include "Torchlight.h"
#include "TorchFlame.h"

#include "CRainEffect.h"
#include "BloodFrameUI.h"

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
	CLoader*		pLoader = static_cast<CLoader*>(pArg);

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

	m_hThread = reinterpret_cast<HANDLE>(_beginthreadex(nullptr, 0, LoadingThread, this, 0, nullptr));
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

	/* For.Prototype_Component_Texture_Sky*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Texture_Sky"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/SkyBox/Sky_%d.dds"), 3))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Dissolve*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Texture_Dissolve"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/53.png")))))
		return E_FAIL;

	// For. EffectTexture~
	{
		/* For.Texture_Trail*/
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Texture_Trail"),
			CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Bless/Effect/%d.png"), 133))))
			return E_FAIL;

		/* For.Texture_Lighting*/
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Texture_Lighting"),
			CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Bless/Effect/Lighting/%d.png"), 16))))
			return E_FAIL;

		/* For. Texture_DamageFont */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Texture_DamageFont"),
			CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Bless/UI/Damage/Damage_%d.png"), 12))))
			return E_FAIL;

		/* For. Texture_Explosion */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Texture_Explosion"),
			CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Explosion/%d.png"), 58))))
			return E_FAIL;

		/* For. Texture_Arcane */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Texture_Arcane"),
			CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Arcane/Arcane_%d.png"), 42))))
			return E_FAIL;

		/* For. Texture_Fire */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Texture_Fire"),
			CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Fire/%d.png"), 32))))
			return E_FAIL;

		/* For. Texture_FireTile */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Texture_FireTile"),
			CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Bless/Effect/FireTile/%d.png"), 19))))
			return E_FAIL;

		/* For. Texture_Flame */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Texture_Flame"),
			CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Flame/%d.png"), 32))))
			return E_FAIL;

		/* For. Texture_Flame_Filter */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Texture_Flame_Filter"),
			CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Flame/Filter0.png")))))
			return E_FAIL;

		/* For. Texture_Hit */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Texture_Hit"),
			CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Hit/%d.png"), 16))))
			return E_FAIL;

		/* For. Texture_Blood */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Texture_Blood"),
			CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Blood/Blood01_%d.png"), 17))))
			return E_FAIL;

		/* For. Texture_Spark */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Texture_Spark"),
			CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Spark/Spark%d.png"), 4))))
			return E_FAIL;

		/* For. Texture_Rain */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Texture_Rain"),
			CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Bless/Rain/Rain.png")))))
				return E_FAIL;
		
		/* For. Texture_QSkillCrack */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Texture_QSkillCrack"),
			CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/QskillCrack.png")))))
			return E_FAIL;

		/* For. Texture_DemonSkillFloork */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Texture_DemonSkillFloork"),
			CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/DemonSkillFloor.png")))))
			return E_FAIL;

		/* For. Texture_DemonSkillFloorD */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Texture_DemonSkillFloorD"),
			CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/DemonSkillFloorD.png")))))
			return E_FAIL;

		/* For. Texture_DemonSkillFoor_2 */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Texture_DemonSkillFoor_2"),
			CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/49.png")))))
			return E_FAIL;
	}

	// For. UI
	{
		/* For.Texture_SkillChargeGuage*/
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Texture_SkillIcon"),
			CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Bless/UI/SkillIcon/Lups_%d.png"), 12))))
			return E_FAIL;

		/* For.Texture_SkillIconBase*/
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Texture_SkillIconBase"),
			CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Bless/UI/SkillIcon/SkillIconBase/SkillIconBase_%d.png"), 2))))
			return E_FAIL;

		/* For.Texture_ProgressBar*/
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Texture_ProgressBar"),
			CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Bless/UI/HPGauge/HPGauge_%d.png"), 10))))
			return E_FAIL;

		/* For. Texture_MonsterAimUI*/
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Texture_MonsterAimUI"),
			CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Bless/UI/Aim/Aim_%d.png"), 2))))
			return E_FAIL;

		/* For. Texture_PortraitCircle*/
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Texture_PortraitCircle"),
			CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Bless/UI/PortraitCircle/PortraitCircle_%d.png"), 28))))
			return E_FAIL;

		/* For. Texture_BloodFrameBackGround*/
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Texture_BloodFrameBackGround"),
			CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Bless/UI/BloodFrameBackGround.png")))))
			return E_FAIL;

		/* For. Texture_Portrait*/
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Texture_Portrait"),
			CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Bless/UI/Portrait_1.png")))))
			return E_FAIL;

		/* For. Texture_SquareMask*/
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Texture_SquareMask"),
			CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Bless/SingleTexture/fx_SquareMask_001_TEX_KKJ.png")))))
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
		CVIBuffer_Point_Instancing::Create(m_pDevice, m_pContext, 100))))
		return E_FAIL;

	{
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
	}
	m_strLoadingText = TEXT("모델을 로딩중입니다. ");

	_matrix			PivotMatrix = XMMatrixIdentity();

	// For. MONSTER
	{
		/* For.Prototype_Component_Model_SkeletonWarrior */
		PivotMatrix = XMMatrixScaling(0.075f, 0.075f, 0.075f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Model_SkeletonWarrior"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Meshes/Bless/SkeletonWarrior/SkeletonWarrior.model"), PivotMatrix))))
			return E_FAIL;

		/* For.Prototype_Component_Model_Demon */
		PivotMatrix = XMMatrixScaling(0.075f, 0.075f, 0.075f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Model_Demon"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Meshes/Bless/Demon/Demon.model"), PivotMatrix))))
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
	}
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
		{
		/* For.Prototype_Component_Model_Sword*/
		PivotMatrix = XMMatrixScaling(0.075f, 0.075f, 0.075f)
			* XMMatrixRotationX(XMConvertToRadians(97.5f))
			* XMMatrixRotationY(XMConvertToRadians(78.7f))
			* XMMatrixRotationZ(XMConvertToRadians(1.f));
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Model_Sword"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Resources/Meshes/Bless/TAX/TAX.model"), PivotMatrix))))
			return E_FAIL;

		/* For.Prototype_Component_Model_Sword_A*/
		PivotMatrix = XMMatrixScaling(0.075f, 0.075f, 0.075f)
			* XMMatrixRotationX(XMConvertToRadians(97.5f))
			* XMMatrixRotationY(XMConvertToRadians(78.7f))
			* XMMatrixRotationZ(XMConvertToRadians(1.f));
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Model_Sword_A"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Resources/Meshes/Bless/Weapon/A/TAX1007.model"), PivotMatrix))))
			return E_FAIL;

		/* For.Prototype_Component_Model_Sword_B*/
		PivotMatrix = XMMatrixScaling(0.075f, 0.075f, 0.075f)
			* XMMatrixRotationX(XMConvertToRadians(97.5f))
			* XMMatrixRotationY(XMConvertToRadians(78.7f))
			* XMMatrixRotationZ(XMConvertToRadians(1.f));
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Model_Sword_B"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Resources/Meshes/Bless/Weapon/B/TAX0015.model"), PivotMatrix))))
			return E_FAIL;

	}

	// For. Ally
	{
		/* For.Prototype_Component_Model_BalianBollwerk */
		PivotMatrix = XMMatrixScaling(0.075f, 0.075f, 0.075f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Model_BalianBollwerk"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Meshes/Bless/BalianBollwerk/BalianBollwerk.model"), PivotMatrix))))
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

		/* For.Rock_1 */
		PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Rock_1"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Resources/Meshes/Bless/Maps/Rock/Rock_1.model"), PivotMatrix))))
			return E_FAIL;

		/* For.Rock_2 */
		PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Rock_2"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Resources/Meshes/Bless/Maps/Rock/Rock_2.model"), PivotMatrix))))
			return E_FAIL;

		/* For.Rock_3 */
		PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Rock_3"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Resources/Meshes/Bless/Maps/Rock/Rock_3.model"), PivotMatrix))))
			return E_FAIL;

		/* For.Rock_4 */
		PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Rock_4"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Resources/Meshes/Bless/Maps/Rock/Rock_4.model"), PivotMatrix))))
			return E_FAIL;

		/* For.Torchlight */
		PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Torchlight"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Resources/Meshes/Bless/Maps/Torchlight/Torchlight.model"), PivotMatrix))))
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

		/* For. RcTex_Long */
		PivotMatrix = XMMatrixScaling(0.001f, 0.001f, 0.001f);
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("RcTex_Long"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Resources/Meshes/Bless/Effect/RcTex_Long.model"), PivotMatrix))))
			return E_FAIL;

		/* For. BoomWave_0 */
		PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("BoomWave_0"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Resources/Meshes/Bless/Effect/BoomWave_0.model"), PivotMatrix))))
			return E_FAIL;

		/* For. BoomWave_1 */
		PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("BoomWave_1"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Resources/Meshes/Bless/Effect/BoomWave_1.model"), PivotMatrix))))
			return E_FAIL;

		/* For. Twister_Cycle */
		PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Twister_Cycle"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Resources/Meshes/Bless/Effect/Twister_Cycle.model"), PivotMatrix))))
			return E_FAIL;

		/* For. Twister_Line*/
		PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(-90.0f));
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Twister_Line"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Resources/Meshes/Bless/Effect/Twister_Line.model"), PivotMatrix))))
			return E_FAIL;

		/* For. FSkillTrail*/
		PivotMatrix = XMMatrixScaling(0.001f, 0.001f, 0.001f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("FSkillTrail"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Resources/Meshes/Bless/Effect/FSkillTrail.model"), PivotMatrix))))
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
		/* For.Prototype_GameObject_SkeletonWarrior*/
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SkeletonWarrior"), CSkeletonWarrior::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		/* For.Prototype_GameObject_Demon*/
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Demon"), CDemon::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}

	/* For. Ally*/
	{

		/* For.Prototype_GameObject_BalianBollwerk*/
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BalianBollwerk"), CBalianBollwerk::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}


	/* For. Prototype_GameObject_Terrain*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"), CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_PlayerCamera*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PlayerCamera"), CPlayerCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_LightCamera*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_LightCamera"), CLightCamera::Create(m_pDevice, m_pContext))))
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
	
	/* For. Prototype_GameObject_Rock_1*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Rock_1"), CRock_1::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Rock_2*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Rock_2"), CRock_2::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Rock_3*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Rock_3"), CRock_3::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Rock_4*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Rock_4"), CRock_4::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Torchlight*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Torchlight"), CTorchlight::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Torchlight_Flame*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Torchlight_Flame"), CTorchFlame::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// Effect
	{
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

		/* For. Prototype_GameObject_FireWave*/
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FireWave"), CFireWave::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		/* For. Prototype_GameObject_FireBallLine*/
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FireBallLine"), CFireBallLine::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		/* For. Prototype_GameObject_ExplosionE*/
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ExplosionE"), CExplosionE::Create(m_pDevice, m_pContext))))
			return E_FAIL;
		
		/* For. Prototype_GameObject_ArcaneE*/
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ArcaneE"), CArcaneE::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		/* For. Prototype_GameObject_FireE_0*/
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FireE_0"), CFireE_0::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		/* For. Prototype_GameObject_FlameE*/
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FlameE"), CFlameE::Create(m_pDevice, m_pContext))))
			return E_FAIL;
		
		/* For. Prototype_GameObject_HitE*/
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_HitE"), CHitE::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		/* For. Prototype_GameObject_SparkE*/
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SparkE"), CSparkE::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		/* For. Prototype_GameObject_QskillCrackE*/
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_QskillCrackE"), CQskillCrackE::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		/* For. Prototype_GameObject_BoomWave_0 */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BoomWave_0"), CBoomWave_0::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		/* For. Prototype_GameObject_BoomWave_1 */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BoomWave_1"), CBoomWave_1::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		/* For. Prototype_GameObject_Twister_Cycle */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Twister_Cycle"), CTwister_Cycle::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		/* For. Prototype_GameObject_Twister_Line */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Twister_Line"), CTwister_Line::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		/* For. Prototype_GameObject_FSKillTrail */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FSKillTrail"), CFSkillTrail::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		/* For. Prototype_GameObject_DashParticle */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_DashParticle"), CDashParticle::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		/* For. Prototype_GameObject_RainParticle */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_RainParticle"), CRainEffect::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}

	// UI
	{
		/* For. Prototype_GameObject_SkillChargingUI*/
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SkillChargingUI"), CSkillChargingUI::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		/* For. Prototype_GameObject_ProgressBarUI*/
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ProgressBarUI"), CProgressBarUI::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		/* For. Prototype_GameObject_AimUI*/
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_AimUI"), CTargetAimEffect::Create(m_pDevice, m_pContext))))
			return E_FAIL;
		
		/* For. Prototype_GameObject_DamageFontUI*/
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_DamageFontUI"), CDamageFontUI::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		/* For. Prototype_GameObject_ConversationUI*/
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ConversationUI"), CConversationUI::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		/* For. Prototype_GameObject_MonsterNameFontUI*/
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MonsterNameFontUI"), CMonsterNameUI::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		/* For. Prototype_GameObject_MonsterHPUI*/
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MonsterHPUI"), CMonsterHpUI::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		/* For. Prototype_GameObject_Portrait*/
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Portrait"), CPortraitCircleUI::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		/* For. Prototype_GameObject_DemonSKillFloor*/
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_DemonSKillFloor"), CDemonSkillFloor::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		/* For. Prototype_GameObject_DemonSKillCircular*/
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_DemonSKillCircular"), CDemonSkillCircular::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		/* For. Prototype_GameObject_DemonSKillStraight*/
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_DemonSKillStraight"), CDemonSkillStraight::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		/* For. Prototype_GameObject_BloodFrameUI*/
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BloodFrameUI"), CBloodFrameUI::Create(m_pDevice, m_pContext))))
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

	m_strLoadingText = TEXT("버퍼를 로딩중입니다. ");

	/* For.Prototype_Component_VIBuffer_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP2, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height_Chap2.bmp")))))
		return E_FAIL;

	m_strLoadingText = TEXT("모델을 로딩중입니다. ");

	_matrix			PivotMatrix = XMMatrixIdentity();

	/* For.Medieval_City_Up */
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(15.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP2, TEXT("Medieval_City_Up"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Resources/Meshes/Bless/Maps/Medieval_City/Medieval_City_Up.model"), PivotMatrix))))
		return E_FAIL;

	/* For.Medieval_City_Down */
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(15.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP2, TEXT("Medieval_City_Down"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Resources/Meshes/Bless/Maps/Medieval_City/Medieval_City_Down.model"), PivotMatrix))))
			return E_FAIL;

	/* For.Prototype_Component_Model_ZombieA */
	PivotMatrix = XMMatrixScaling(0.075f, 0.075f, 0.075f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP2, TEXT("Prototype_Component_Model_ZombieA"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Meshes/Bless/Zombie/ZombieA.model"), PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_TrollA */
	PivotMatrix = XMMatrixScaling(0.075f, 0.075f, 0.075f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP2, TEXT("Prototype_Component_Model_TrollA"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Meshes/Bless/Troll/TrollA.model"), PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_TrollQ */
	PivotMatrix = XMMatrixScaling(0.075f, 0.075f, 0.075f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP2, TEXT("Prototype_Component_Model_TrollQ"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Meshes/Bless/Troll/TrollQ.model"), PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Abelardo */
	PivotMatrix = XMMatrixScaling(0.075f, 0.075f, 0.075f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP2, TEXT("Prototype_Component_Model_Abelardo"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Meshes/Bless/Abelardo/Abelardo.model"), PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Chinuwa */
	PivotMatrix = XMMatrixScaling(0.075f, 0.075f, 0.075f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP2, TEXT("Prototype_Component_Model_Chinuwa"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Meshes/Bless/Chinuwa/Chinuwa.model"), PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Chitata */
	PivotMatrix = XMMatrixScaling(0.075f, 0.075f, 0.075f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP2, TEXT("Prototype_Component_Model_Chitata"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Meshes/Bless/Chitata/Chitata.model"), PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Delilah */
	PivotMatrix = XMMatrixScaling(0.075f, 0.075f, 0.075f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP2, TEXT("Prototype_Component_Model_Delilah"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Meshes/Bless/Delilah/Delilah.model"), PivotMatrix))))
		return E_FAIL;

	m_strLoadingText = TEXT("객체원형을 로딩중입니다. ");

	/* For. Prototype_GameObject_Medieval_City*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Medieval_City"), CMedieval_City::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Medieval_City_Down*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Medieval_City_Down"), CMedieval_City_Down::Create(m_pDevice, m_pContext))))
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

	/* For.Prototype_GameObject_Abelardo*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Abelardo"), CAbelardo::Create(m_pDevice, m_pContext))))
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

	/* For.Prototype_GameObject_Tree*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Tree"), CTree::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	m_strLoadingText = TEXT("네비게이션 정보 로딩중입니다. ");
	
	/* For.Prototype_Component_Navigation */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP2, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/Data/Navigation_CHAP2.dat")))))
		return E_FAIL;


	m_strLoadingText = TEXT("로딩끝. ");

	m_isFinished = true;

	RELEASE_INSTANCE(CGameInstance)

	return S_OK;
}

HRESULT CLoader::Loading_ForChapter_3()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance)

	m_strLoadingText = TEXT("텍스쳐를 로딩중입니다. ");

	m_strLoadingText = TEXT("버퍼를 로딩중입니다. ");

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
