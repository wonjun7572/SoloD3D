#include "stdafx.h"
#include "..\Public\Loader.h"

#include "GameInstance.h"

/*Logo*/
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

#include "PlayerCamera.h"

#include "Effect_Rect.h"
#include "Effect_Point.h"

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

	/* For.Prototype_Component_Texture_Snow*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Texture_Snow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Snow/Snow.png")))))
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

	/* For.Prototype_Component_Model_Demon */
	PivotMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Model_Demon"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Meshes/Bless/Demon/Demon.model"), PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Model_ForkLift"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Resources/Meshes/ForkLift/ForkLift.model"), PivotMatrix))))
		return E_FAIL;
	
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

	// For. Maps
	{
		/* For.BlackForest_BCastle_Brick01 */
		PivotMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("BlackForest_BCastle_Brick01"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Resources/Meshes/Bless/Maps/BlackForest_BCastle/BlackForest_BCastle_Brick01.model"), PivotMatrix))))
			return E_FAIL;

		/* For.BlackForest_BCastle_Brick02 */
		PivotMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("BlackForest_BCastle_Brick02"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Resources/Meshes/Bless/Maps/BlackForest_BCastle/BlackForest_BCastle_Brick02.model"), PivotMatrix))))
			return E_FAIL;

		/* For.BlackForest_BCastle_Brick03 */
		PivotMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("BlackForest_BCastle_Brick03"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Resources/Meshes/Bless/Maps/BlackForest_BCastle/BlackForest_BCastle_Brick03.model"), PivotMatrix))))
			return E_FAIL;

		/* For.BlackForest_BCastle_Bridge01 */
		PivotMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("BlackForest_BCastle_Bridge01"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Resources/Meshes/Bless/Maps/BlackForest_BCastle/BlackForest_BCastle_Bridge01.model"), PivotMatrix))))
			return E_FAIL;

		/* For.BlackForest_BCastle_Buttress01 */
		PivotMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("BlackForest_BCastle_Buttress01"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Resources/Meshes/Bless/Maps/BlackForest_BCastle/BlackForest_BCastle_Buttress01.model"), PivotMatrix))))
			return E_FAIL;

		/* For.BlackForest_BCastle_Buttress02 */
		PivotMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("BlackForest_BCastle_Buttress02"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Resources/Meshes/Bless/Maps/BlackForest_BCastle/BlackForest_BCastle_Buttress02.model"), PivotMatrix))))
			return E_FAIL;

		/* For.BlackForest_BCastle_EntranceFloor01 */
		PivotMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("BlackForest_BCastle_EntranceFloor01"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Resources/Meshes/Bless/Maps/BlackForest_BCastle/BlackForest_BCastle_EntranceFloor01.model"), PivotMatrix))))
			return E_FAIL;

		/* For.BlackForest_BCastle_Gate01 */
		PivotMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("BlackForest_BCastle_Gate01"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Resources/Meshes/Bless/Maps/BlackForest_BCastle/BlackForest_BCastle_Gate01.model"), PivotMatrix))))
			return E_FAIL;

		/* For.BlackForest_BCastle_Gate02 */
		PivotMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("BlackForest_BCastle_Gate02"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Resources/Meshes/Bless/Maps/BlackForest_BCastle/BlackForest_BCastle_Gate02.model"), PivotMatrix))))
			return E_FAIL;

		/* For.BlackForest_BCastle_Gate03 */
		PivotMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("BlackForest_BCastle_Gate03"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Resources/Meshes/Bless/Maps/BlackForest_BCastle/BlackForest_BCastle_Gate03.model"), PivotMatrix))))
			return E_FAIL;

		/* For.BlackForest_BCastle_GatePillar01 */
		PivotMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("BlackForest_BCastle_GatePillar01"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Resources/Meshes/Bless/Maps/BlackForest_BCastle/BlackForest_BCastle_GatePillar01.model"), PivotMatrix))))
			return E_FAIL;

		/* For.BlackForest_BCastle_GatePillar02 */
		PivotMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("BlackForest_BCastle_GatePillar02"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Resources/Meshes/Bless/Maps/BlackForest_BCastle/BlackForest_BCastle_GatePillar02.model"), PivotMatrix))))
			return E_FAIL;
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

	/* For.Prototype_Component_Shader_VtxCubeTex*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Shader_VtxCubeTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCubeTex.hlsl"), VTXCUBETEX_DECLARATION::Elements, VTXCUBETEX_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxRectInstancing */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Shader_VtxRectInstancing"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxRectInstancing.hlsl"), VTXRECTINSTANCE_DECLARATION::Elements, VTXRECTINSTANCE_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxPointInstancing */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Shader_VtxPointInstancing"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPointInstancing.hlsl"), VTXPOINT_DECLARATION::Elements, VTXPOINT_DECLARATION::iNumElements))))
		return E_FAIL;

	m_strLoadingText = TEXT("네비게이션 정보 로딩중입니다. ");
	/* For.Prototype_Component_Navigation */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/Data/Navigation.dat")))))
		return E_FAIL;

	m_strLoadingText = TEXT("FSM 로딩중입니다. ");

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CHAP1, TEXT("Prototype_Component_STATE"),
		CState::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	m_strLoadingText = TEXT("객체원형을 생성중입니다. ");

	/* For.Prototype_GameObject_Sky*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sky"), CSky::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Player*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player"), CPlayer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_ForkLift*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ForkLift"), CForkLift::Create(m_pDevice, m_pContext))))
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

	/* For.Prototype_GameObject_Demon*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Demon"), CDemon::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
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
