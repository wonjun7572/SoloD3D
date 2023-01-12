#include "stdafx.h"
#include "..\Public\TrollQ.h"
#include "GameInstance.h"
#include "Animation.h"
#include "FSMComponent.h"

CTrollQ::CTrollQ(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CMonster(pDevice, pContext)
{
}

CTrollQ::CTrollQ(const CTrollQ & rhs)
	: CMonster(rhs)
{
}

HRESULT CTrollQ::Init_Prototype()
{
	if (FAILED(__super::Init_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTrollQ::Init(void * pArg)
{
	CGameObject::GAMEOBJECTDESC			GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof GameObjectDesc);

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.0f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Init(&GameObjectDesc)))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(rand() % 10 + 25.f, 0.f, rand() % 3 + 19.f, 1.f));

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pNavigationCom->Set_CurreuntIndex(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

	m_strObjName = L"TrollQ";

	m_fHp = 100;
	m_fMaxHp = 100.f;
	m_fAttack = 10;
	m_fDefence = 5;
	m_vMonsterNamePos = _float2(720.f, 40.f);
	m_vMonsterNameScale = _float2(1.f, 1.f);

	if (FAILED(SetUP_UI()))
		return E_FAIL;

	return S_OK;
}

void CTrollQ::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	AdditiveAnim(TimeDelta);
}

void CTrollQ::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (m_bDeadAnim)
		return;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (nullptr != m_pRendererCom &&
		true == pGameInstance->isInFrustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION), 2.f))
	{
#ifdef _DEBUG
		m_pRendererCom->Add_DebugRenderGroup(m_pAttackColCom);
		m_pRendererCom->Add_DebugRenderGroup(m_pSwordColCom);
#endif
	}

	RELEASE_INSTANCE(CGameInstance);

	Adjust_Collision(TimeDelta);
}

HRESULT CTrollQ::Render()
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

		m_pModelCom->Render(m_pShaderCom, i, 0, "g_BoneMatrices");
	}

	return S_OK;
}

void CTrollQ::Imgui_RenderProperty()
{
	if (ImGui::Button("Navi~"))
	{
		m_pNavigationCom->Set_CurreuntIndex(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	}

	m_pAttackColCom->FixedSizeForImgui(COLLTYPE_SPHERE);
}

void CTrollQ::SetUp_FSM()
{
	m_pFSM = CFSMComponentBuilder()
		.InitState("Idle")
		.AddState("Idle")
		.Tick([this](_double TimeDelta)
	{
		m_pModelCom->Set_AnimationIndex(TROLLQ_Idle_P_01);
	})
		.AddTransition("Idle to Chase", "Chase")
		.Predicator([this]()
	{
		return m_bPlayerChase;
	})
		.AddTransition("Idle to HitDown", "HitDown")
		.Predicator([this]()
	{
		return m_bHitDown;
	})
		.AddTransition("Idle to Groggy", "Groggy")
		.Predicator([this]()
	{
		return m_bGroggy;
	})
		.AddTransition("Idle to Dead", "Dead")
		.Predicator([this]()
	{
		return m_bDeadAnim;
	})

		// Chase
		.AddState("Chase")
		.Tick([this](_double TimeDelta)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		_vector	vPlayerPos = m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION);

		if (!m_bPlayerAttack)
		{
			m_pModelCom->Set_AnimationIndex(TROLLQ_Run_F);
			m_pTransformCom->ChaseAndLookAt(vPlayerPos, TimeDelta, 0.1f, m_pNavigationCom);
		}
		else
		{
			m_pModelCom->Set_AnimationIndex(TROLLQ_Idle_C);
			m_pTransformCom->LookAt(vPlayerPos);
		}

		m_AttackDelayTime += TimeDelta;
		RELEASE_INSTANCE(CGameInstance);
	})
		.AddTransition("Chase to Idle", "Idle")
		.Predicator([this]()
	{
		return !m_bPlayerChase;
	})
		.AddTransition("Chase to Attack", "Attack")
		.Predicator([this]()
	{
		return m_bPlayerChase && m_bPlayerAttack && m_AttackDelayTime > 3.0;
	})
		.AddTransition("Chase to HitDown", "HitDown")
		.Predicator([this]()
	{
		return m_bHitDown;
	})
		.AddTransition("Chase to Groggy", "Groggy")
		.Predicator([this]()
	{
		return m_bGroggy;
	})
		.AddTransition("Chase to Dead", "Dead")
		.Predicator([this]()
	{
		return m_bDeadAnim;
	})

		// Groggy
		.AddState("Groggy")
		.OnStart([this]()
	{
		m_GroggyDelayTime = 0.0;
		m_pModelCom->Reset_AnimPlayTime(TROLLQ_Passout);
		m_pModelCom->Set_AnimationIndex(TROLLQ_Passout);
	})
		.Tick([this](_double TimeDelta)
	{
		m_GroggyDelayTime += TimeDelta;
		m_pModelCom->Set_AnimationIndex(TROLLQ_Passout);
	})
		.OnExit([this]()
	{
		m_bGroggy = false;
	})
		.AddTransition("Groggy to Chase", "Chase")
		.Predicator([this]()
	{
		return  m_GroggyDelayTime >= 5.0;
	})
		.AddTransition("Groggy to Dead", "Dead")
		.Predicator([this]()
	{
		return m_bDeadAnim;
	})

		// HitDown
		.AddState("HitDown")
		.OnStart([this]()
	{
		m_pModelCom->Reset_AnimPlayTime(TROLLQ_Down);
		m_pModelCom->Set_AnimationIndex(TROLLQ_Down);
	})
		.AddTransition("HitDown to HitDownLoop", "HitDownLoop")
		.Predicator([this]()
	{
		return AnimFinishChecker(TROLLQ_Down, 0.83);
	})
		.AddTransition("HitDown to Dead", "Dead")
		.Predicator([this]()
	{
		return m_bDeadAnim;
	})

		.AddState("HitDownLoop")
		.OnStart([this]()
	{
		m_HitDownDelayTime = 0.0;
		m_pModelCom->Reset_AnimPlayTime(TROLLQ_Down_Loop);
		m_pModelCom->Set_AnimationIndex(TROLLQ_Down_Loop);
	})
		.Tick([this](_double TimeDelta)
	{
		m_HitDownDelayTime += TimeDelta;
		m_pModelCom->Set_AnimationIndex(TROLLQ_Down_Loop);
	})
		.AddTransition("HitDownLoop to Getup", "Getup")
		.Predicator([this]()
	{
		return m_HitDownDelayTime >= 5.0;
	})
		.AddTransition("HitDownLoop to Dead", "Dead")
		.Predicator([this]()
	{
		return m_bDeadAnim;
	})

		.AddState("Getup")
		.OnStart([this]()
	{
		m_pModelCom->Reset_AnimPlayTime(TROLLQ_Get_Up);
		m_pModelCom->Set_AnimationIndex(TROLLQ_Get_Up);
	})
		.OnExit([this]()
	{
		m_bHitDown = false;
	})
		.AddTransition("Getup to Chase", "Chase")
		.Predicator([this]()
	{
		return  AnimFinishChecker(TROLLQ_Get_Up, 0.85);
	})
		.AddTransition("Getup to Dead", "Dead")
		.Predicator([this]()
	{
		return m_bDeadAnim;
	})


		// Attack
		.AddState("Attack")
		.OnStart([this]()
	{

		m_iRandAttack = rand() % 2;

		if (m_iRandAttack == 0)
		{
			m_pModelCom->Reset_AnimPlayTime(TROLLQ_ATK_01);
			m_pModelCom->Set_AnimationIndex(TROLLQ_ATK_01);
		}
		else if (m_iRandAttack == 1)
		{
			m_pModelCom->Reset_AnimPlayTime(TROLLQ_ATK_02);
			m_pModelCom->Set_AnimationIndex(TROLLQ_ATK_02);
		}
	})
		.Tick([this](_double TimeDelta)
	{
		if (m_iRandAttack == 0)
		{
			if (AnimIntervalChecker(TROLLQ_ATK_01, 0.5, 0.6))
				m_bRealAttack = true;
			else
				m_bRealAttack = false;
		}
		else if (m_iRandAttack == 1)
		{
			if (AnimIntervalChecker(TROLLQ_ATK_02, 0.5, 0.6))
				m_bRealAttack = true;
			else
				m_bRealAttack = false;
		}
	})
		.OnExit([this]()
	{
		m_bRealAttack = false;
		m_AttackDelayTime = 0.0;
	})
		.AddTransition("Attack to Idle", "Idle")
		.Predicator([this]()
	{
		return AnimFinishChecker(TROLLQ_ATK_01) || 
			AnimFinishChecker(TROLLQ_ATK_02);
	})
		.AddTransition("Attack to HitDown", "HitDown")
		.Predicator([this]()
	{
		return m_bHitDown;
	})
		.AddTransition("Attack to Groggy", "Groggy")
		.Predicator([this]()
	{
		return m_bGroggy;
	})
		.AddTransition("Attack to Dead", "Dead")
		.Predicator([this]()
	{
		return m_bDeadAnim;
	})

		// Dead
		.AddState("Dead")
		.OnStart([this]()
	{
		m_pModelCom->Reset_AnimPlayTime(TROLLQ_Die);
		m_pModelCom->Set_AnimationIndex(TROLLQ_Die);
	})
		.AddTransition("Dead to DeadBody", "DeadBody")
		.Predicator([this]()
	{
		return AnimFinishChecker(TROLLQ_Die, 0.85);
	})
		.AddState("DeadBody")
		.Tick([this](_double TimeDelta)
	{
		m_pModelCom->Set_AnimationIndex(TROLLQ_DeadBody);
		m_dDeadTime += TimeDelta;

		if (m_dDeadTime > 3.0)
			m_bDead = true;
	})

		.Build();
}

void CTrollQ::Adjust_Collision(_double TimeDelta)
{
	if (m_pAttackColCom)
		m_pAttackColCom->Update(m_pTransformCom->Get_WorldMatrix());

	if (m_pSwordColCom)
		m_pSwordColCom->Update(m_pTransformCom->Get_WorldMatrix());

	CollisionToPlayer(TimeDelta);

	CollisionToMonster(TimeDelta);

	// 플레이어와 어느 정도거리가 되었을 때
	// 즉 chase 상태로 가는 것이 가능한 조건 일때임
	if (m_bPlayerChase)
		CollisionToAttack(TimeDelta);

	// 밑은 플레이어의 데미지 입었을 때 이벤트

	if (m_bPlayerAttackCommand)
		CollisionToWeapon(TimeDelta);

	if (m_bPlayerSkill02Command)
		CollisionToWeaponSkill02(TimeDelta);

	if (m_bPlayerSkill04Command)
		CollisionToWeaponSkill04(TimeDelta);

	if (!m_bPlayerSkill02Command && !m_bPlayerSkill04Command)
		m_bImpossibleSkillDamaged = false;
}

void CTrollQ::CollisionToPlayer(_double TimeDelta)
{
	// 이 부분 충돌을 해야할 이유가 없는 것 같다
	_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_float3 vPlayerPos = m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION);
	_float fDistance = CMathUtils::Distance(vPos, vPlayerPos);

	if (fabsf(fDistance) < 10.f)
		m_bPlayerChase = true;
	else
		m_bPlayerChase = false;
}

void CTrollQ::CollisionToAttack(_double TimeDelta)
{
	CGameInstance*			pGameInstance = GET_INSTANCE(CGameInstance);

	CCollider*		pTargetCollider = nullptr;

	if (g_LEVEL == LEVEL_CHAP1)
		pTargetCollider = (CCollider*)pGameInstance->Get_ComponentPtr(LEVEL_CHAP1, TEXT("Layer_Player"), TEXT("Com_AABB"));
	else if (g_LEVEL == LEVEL_CHAP2)
		pTargetCollider = (CCollider*)pGameInstance->Get_ComponentPtr(LEVEL_CHAP2, TEXT("Layer_Player"), TEXT("Com_AABB"));
	else if (g_LEVEL == LEVEL_CHAP3)
		pTargetCollider = (CCollider*)pGameInstance->Get_ComponentPtr(LEVEL_CHAP3, TEXT("Layer_Player"), TEXT("Com_AABB"));

	if (nullptr == pTargetCollider)
		return;

	if (m_pAttackColCom->Collision(pTargetCollider) == true)
		m_bPlayerAttack = true;
	else
		m_bPlayerAttack = false;

	if (m_bRealAttack)
	{
		if (m_pSwordColCom->Collision(pTargetCollider) == true)
		{
			// 플레이어가 앞을 보고 있느냐 뒤를 보고 있느냐를 판단하자
			_vector			vTargetLook = m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_LOOK);
			_vector			vDot = XMVector3Dot(m_pTransformCom->Get_State(CTransform::STATE_LOOK), vTargetLook);
			_float			fDot = XMVectorGetX(vDot);

			// 0보다 작으면 내 앞에 있다.
			if (fDot < 0)
				m_pPlayer->FrontDamagedToMonster(m_fAttack);
			else
				m_pPlayer->BackDamagedToMonster(m_fAttack);
		}
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CTrollQ::AdditiveAnim(_double TimeDelta)
{
	if (m_bFrontDamaged)
	{
		AdjustSetDamage();
		m_pModelCom->Set_AdditiveAnimIndex(TROLLQ_ADD_DMG_F);
		m_pModelCom->Play_AddtivieAnim(TimeDelta, 1.f);
	}

	if (AnimFinishChecker(TROLLQ_ADD_DMG_F))
	{
		m_bFrontDamaged = false;
		m_bImpossibleDamaged = false;
		m_pModelCom->Reset_AnimPlayTime(TROLLQ_ADD_DMG_F);
	}

	///////////////////////////////////////

	if (m_bBackDamaged)
	{
		AdjustSetDamage();
		m_pModelCom->Set_AdditiveAnimIndex(TROLLQ_ADD_DMG_B);
		m_pModelCom->Play_AddtivieAnim(TimeDelta, 1.f);
	}

	if (AnimFinishChecker(TROLLQ_ADD_DMG_B))
	{
		m_bBackDamaged = false;
		m_bImpossibleDamaged = false;
		m_pModelCom->Reset_AnimPlayTime(TROLLQ_ADD_DMG_B);
	}
}

HRESULT CTrollQ::SetUp_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Model_TrollQ"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC			ColliderDesc;

	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vCenter = _float3(0.f, 1.7f, 0.f);
	ColliderDesc.vSize = _float3(0.7f, 1.7f, 0.9f);

	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_AABB"),
		(CComponent**)&m_pColliderCom[COLLTYPE_AABB], &ColliderDesc)))
		return E_FAIL;

	/* For.Com_SPHERE */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vSize = _float3(1.2f, 1.2f, 1.2f);

	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Collider_SPHERE"), TEXT("Com_SPHERE"),
		(CComponent**)&m_pColliderCom[COLLTYPE_SPHERE], &ColliderDesc)))
		return E_FAIL;

	/* For.Com_Attack */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vCenter = _float3(0.f, 1.7f, 0.f);
	ColliderDesc.vSize = _float3(2.2f, 2.2f, 2.2f);

	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Collider_SPHERE"), TEXT("Com_SPHERE_Attack"),
		(CComponent**)&m_pAttackColCom, &ColliderDesc)))
		return E_FAIL;

	/* For.Com_Sword */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vCenter = _float3(0.f, 2.f, 2.8f);
	ColliderDesc.vSize = _float3(0.7f, 0.7f, 0.7f);

	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Collider_SPHERE"), TEXT("Com_SPHERE_Sword"),
		(CComponent**)&m_pSwordColCom, &ColliderDesc)))
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

HRESULT CTrollQ::SetUp_ShaderResources()
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

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

_bool CTrollQ::AnimFinishChecker(ANIMATION eAnim, _double FinishRate)
{
	return m_pModelCom->Get_IndexAnim(eAnim)->Get_PlayRate() >= FinishRate;
}

_bool CTrollQ::AnimIntervalChecker(ANIMATION eAnim, _double StartRate, _double FinishRate)
{
	if (m_pModelCom->Get_IndexAnim(eAnim)->Get_PlayRate() > StartRate &&
		m_pModelCom->Get_IndexAnim(eAnim)->Get_PlayRate() <= FinishRate)
	{
		return true;
	}

	return false;
}

CTrollQ * CTrollQ::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTrollQ*		pInstance = new CTrollQ(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Created : CTrollQ");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CTrollQ::Clone(void * pArg)
{
	CTrollQ*		pInstance = new CTrollQ(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTrollQ");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTrollQ::Free()
{
	__super::Free();

	Safe_Release(m_pAttackColCom);
	Safe_Release(m_pSwordColCom);

	for (auto pCollider : m_MonsterColliders)
		Safe_Release(pCollider);

	m_MonsterColliders.clear();
}
