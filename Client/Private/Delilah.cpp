#include "stdafx.h"
#include "..\Public\Delilah.h"
#include "GameInstance.h"
#include "Animation.h"
#include "FSMComponent.h"
#include "Player.h"

CDelilah::CDelilah(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CAlly(pDevice, pContext)
{
}

CDelilah::CDelilah(const CDelilah & rhs)
	: CAlly(rhs)
{
}

HRESULT CDelilah::Init_Prototype()
{
	if (FAILED(__super::Init_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDelilah::Init(void * pArg)
{
	CGameObject::GAMEOBJECTDESC			GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof GameObjectDesc);

	GameObjectDesc.TransformDesc.fSpeedPerSec = 6.0f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Init(&GameObjectDesc)))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(rand() % 10 + 32.f, 0.f, rand() % 3 + 70.f, 1.f));

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pNavigationCom->Set_CurreuntIndex(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

	m_strObjName = L"Delilah";

	m_vRimColor = _float4(0.1f, 0.1f, 1.f, 1.f);

	return S_OK;
}

void CDelilah::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	if (m_pFSM)
		m_pFSM->Tick(TimeDelta);

	m_pModelCom->Play_Animation(TimeDelta);

	AdditiveAnim(TimeDelta);
}

void CDelilah::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CDelilah::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		/* 이 모델을 그리기위한 셰이더에 머테리얼 텍스쳐를 전달하낟. */
		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
	
		bool HasSpecular = false;
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_SPECULAR, "g_SpecularTexture")))
			HasSpecular = false;
		else
			HasSpecular = true;

		m_pShaderCom->Set_RawValue("g_HasSpecular", &HasSpecular, sizeof(bool));
		m_pModelCom->Render(m_pShaderCom, i, 0, "g_BoneMatrices");
	}

	return S_OK;
}

void CDelilah::Imgui_RenderProperty()
{
	if (ImGui::Button("Navi~"))
	{
		m_pNavigationCom->Set_CurreuntIndex(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	}

	if (ImGui::Button("FrontDamage"))
		m_bFrontDamaged = true;

	if (ImGui::Button("BackDamage"))
		m_bBackDamaged = true;
	
	m_pColliderCom[COLLTYPE_AABB]->FixedSizeForImgui(COLLTYPE_AABB);
	m_pColliderCom[COLLTYPE_SPHERE]->FixedSizeForImgui(COLLTYPE_SPHERE);
}

void CDelilah::SetUp_FSM()
{
	m_pFSM = CFSMComponentBuilder()
		.InitState("Idle")
		.AddState("Idle")
		.Tick([this](_double TimeDelta)
	{
		m_pModelCom->Set_AnimationIndex(DELILAH_Idle_C);
	})
		.AddTransition("Idle to Player_Chase", "Player_Chase")
		.Predicator([this]()
	{
		return m_bChase && !m_bMonsterChase;
	})
		.AddTransition("Idel to Monster_Chase", "Monster_Chase")
		.Predicator([this]()
	{
		return m_bMonsterChase;
	})

		.AddState("Player_Chase")
		.Tick([this](_double TimeDelta)
	{
		m_pModelCom->Set_AnimationIndex(DELILAH_Run_F);
		m_pTransformCom->ChaseAndLookAt(m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION), TimeDelta, 5.f, m_pNavigationCom);

		if (_float3::Distance(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION), m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION)) > 15.f)
		{
			m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION));
			m_pNavigationCom->Set_CurreuntIndex(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
		}
	})
		.AddTransition("Player_Chase to Idle", "Idle")
		.Predicator([this]()
	{
		return !m_bChase && !m_bMonsterChase;
	})
		.AddTransition("Player_Chase to Monster_Chase", "Monster_Chase")
		.Predicator([this]()
	{
		return m_bMonsterChase;
	})

		.AddState("Monster_Chase")
		.Tick([this](_double TimeDelta)
	{
	})
		.AddTransition("Monster_Chase to Idle", "Idle")
		.Predicator([this]()
	{
		return !m_bMonsterChase;
	})
		.AddTransition("Monster_Chase to Attack", "Attack")
		.Predicator([this]()
	{
		return m_AttackDelayTime > 0.1;
	})

		.AddState("Attack")
		.OnStart([this]()
	{
		m_iRandAttack = rand() % 5;
		
		if (m_iRandAttack == 0)
		{
			m_pModelCom->Reset_AnimPlayTime(DELILAH_ATK_01);
			m_pModelCom->Set_AnimationIndex(DELILAH_ATK_01);
		}
		else if (m_iRandAttack == 1)
		{
			m_pModelCom->Reset_AnimPlayTime(DELILAH_ATK_02);
			m_pModelCom->Set_AnimationIndex(DELILAH_ATK_02);
		}
		else if (m_iRandAttack == 2)
		{
			m_pModelCom->Reset_AnimPlayTime(DELILAH_SK_Firing_01);
			m_pModelCom->Set_AnimationIndex(DELILAH_SK_Firing_01);
		}
		else if (m_iRandAttack == 3)
		{
			m_pModelCom->Reset_AnimPlayTime(DELILAH_SK_Firing_02);
			m_pModelCom->Set_AnimationIndex(DELILAH_SK_Firing_02);
		}
		else if (m_iRandAttack == 4)
		{
			m_pModelCom->Reset_AnimPlayTime(DELILAH_SK_Firing_03);
			m_pModelCom->Set_AnimationIndex(DELILAH_SK_Firing_03);
		}
	})
		.OnExit([this]()
	{
		m_AttackDelayTime = 0.0;
	})
		.AddTransition("Attack to Idle", "Idle")
		.Predicator([this]()
	{
		return AnimFinishChecker(DELILAH_SK_Firing_01) ||
			AnimFinishChecker(DELILAH_SK_Firing_02) || 
			AnimFinishChecker(DELILAH_SK_Firing_03) || 
			AnimFinishChecker(DELILAH_ATK_01) ||
			AnimFinishChecker(DELILAH_ATK_02);
	})

		.Build();
}

void CDelilah::AdditiveAnim(_double TimeDelta)
{
	if (m_bFrontDamaged)
	{
		AdjustSetDamage();
		m_pModelCom->Set_AdditiveAnimIndex(DELILAH_ADD_DMG_F);
		m_pModelCom->Play_AddtivieAnim(TimeDelta, 1.f);
	}

	if (AnimFinishChecker(DELILAH_ADD_DMG_F))
	{
		m_bFrontDamaged = false;
		m_bImpossibleDamaged = false;
		m_pModelCom->Reset_AnimPlayTime(DELILAH_ADD_DMG_F);
	}

	///////////////////////////////////////

	if (m_bBackDamaged)
	{
		AdjustSetDamage();
		m_pModelCom->Set_AdditiveAnimIndex(DELILAH_ADD_DMG_B);
		m_pModelCom->Play_AddtivieAnim(TimeDelta, 1.f);
	}

	if (AnimFinishChecker(DELILAH_ADD_DMG_B))
	{
		m_bBackDamaged = false;
		m_bImpossibleDamaged = false;
		m_pModelCom->Reset_AnimPlayTime(DELILAH_ADD_DMG_B);
	}
}

HRESULT CDelilah::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxAnimModel"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Model_Delilah"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC			ColliderDesc;

	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vCenter = _float3(0.f, 1.2f, 0.f);
	ColliderDesc.vSize = _float3(0.5f, 1.2f, 0.5f);

	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_AABB"),
		(CComponent**)&m_pColliderCom[COLLTYPE_AABB], &ColliderDesc)))
		return E_FAIL;

	/* For.Com_SPHERE */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vSize = _float3(0.5f, 0.5f, 0.5f);

	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Collider_SPHERE"), TEXT("Com_SPHERE"),
		(CComponent**)&m_pColliderCom[COLLTYPE_SPHERE], &ColliderDesc)))
		return E_FAIL;


	if (g_LEVEL == LEVEL_CHAP1)
	{
		/* For.Com_Navigation */
		CNavigation::NAVIDESC			NaviDesc;
		ZeroMemory(&NaviDesc, sizeof(CNavigation::NAVIDESC));

		NaviDesc.iCurrentIndex = 0;

		if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"),
			(CComponent**)&m_pNavigationCom, nullptr)))
			return E_FAIL;
	}
	else if (g_LEVEL == LEVEL_CHAP2)
	{
		/* For.Com_Navigation */
		CNavigation::NAVIDESC			NaviDesc;
		ZeroMemory(&NaviDesc, sizeof(CNavigation::NAVIDESC));

		NaviDesc.iCurrentIndex = 0;

		if (FAILED(__super::Add_Component(LEVEL_CHAP2, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"),
			(CComponent**)&m_pNavigationCom, &NaviDesc)))
			return E_FAIL;
	}
	else if (g_LEVEL == LEVEL_CHAP3)
	{
		/* For.Com_Navigation */
		CNavigation::NAVIDESC			NaviDesc;
		ZeroMemory(&NaviDesc, sizeof(CNavigation::NAVIDESC));

		NaviDesc.iCurrentIndex = 0;

		if (FAILED(__super::Add_Component(LEVEL_CHAP3, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"),
			(CComponent**)&m_pNavigationCom, &NaviDesc)))
			return E_FAIL;
	}
	return S_OK;
}

HRESULT CDelilah::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;
	_float3 vCamPos = _float3(pGameInstance->Get_CamPosition().x, pGameInstance->Get_CamPosition().y, pGameInstance->Get_CamPosition().z);
	if (_float3::Distance(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION), vCamPos) > 30.f)
		m_vRimColor = _float4(0.f, 0.f, 0.f, 0.f);
	else
		m_vRimColor = _float4(0.1f, 0.1f, 1.f, 1.f);
	if (FAILED(m_pShaderCom->Set_RawValue("g_vRimColor", &m_vRimColor, sizeof(_float4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

_bool CDelilah::AnimFinishChecker(ANIMATION eAnim, _double FinishRate)
{
	return m_pModelCom->Get_IndexAnim(eAnim)->Get_PlayRate() >= FinishRate;
}

_bool CDelilah::AnimIntervalChecker(ANIMATION eAnim, _double StartRate, _double FinishRate)
{
	if (m_pModelCom->Get_IndexAnim(eAnim)->Get_PlayRate() > StartRate &&
		m_pModelCom->Get_IndexAnim(eAnim)->Get_PlayRate() <= FinishRate)
	{
		return true;
	}

	return false;
}

CDelilah * CDelilah::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CDelilah*		pInstance = new CDelilah(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Created : CDelilah");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CDelilah::Clone(void * pArg)
{
	CDelilah*		pInstance = new CDelilah(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDelilah");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDelilah::Free()
{
	__super::Free();
}