#include "stdafx.h"
#include "..\Public\ZombieA.h"
#include "GameInstance.h"
#include "Animation.h"
#include "FSMComponent.h"

CZombieA::CZombieA(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)	
	:CMonster(pDevice, pContext)
{
}

CZombieA::CZombieA(const CZombieA & rhs)
	:CMonster(rhs)
{
}

HRESULT CZombieA::Init_Prototype()
{
	if (FAILED(__super::Init_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CZombieA::Init(void * pArg)
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

	m_strObjName = L"ZombieA";

	m_iHp = 100;
	m_iAttack = 10;
	m_iDefence = 5;

	return S_OK;
}

void CZombieA::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	AdditiveAnim(TimeDelta);
}

void CZombieA::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (m_bDeadAnim)
		return;

	Adjust_Collision(TimeDelta);
	CollisionToMonster(TimeDelta);
}

HRESULT CZombieA::Render()
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

#ifdef _DEBUG
	m_pAttackColCom->Render();
	m_pSwordColCom->Render();
#endif
	return S_OK;
}

void CZombieA::Imgui_RenderProperty()
{
	if (ImGui::Button("Navi~"))
	{
		m_pNavigationCom->Set_CurreuntIndex(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	}

	m_pSwordColCom->FixedSizeForImgui(1);
}

void CZombieA::SetUp_FSM()
{
	m_pFSM = CFSMComponentBuilder()
		.InitState("Idle")
		.AddState("Idle")
		.Tick([this](_double TimeDelta)
	{
		m_pModelCom->Set_AnimationIndex(ZOMBIEA_Idle_P_01);
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
			m_pModelCom->Set_AnimationIndex(ZOMBIEA_Run_F);
			m_pTransformCom->ChaseAndLookAt(vPlayerPos, TimeDelta, 0.1f, m_pNavigationCom);
		}
		else
		{
			m_pModelCom->Set_AnimationIndex(ZOMBIEA_Idle_C);
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
		m_pModelCom->Reset_AnimPlayTime(ZOMBIEA_Passout);
		m_pModelCom->Set_AnimationIndex(ZOMBIEA_Passout);
	})
		.Tick([this](_double TimeDelta)
	{
		m_GroggyDelayTime += TimeDelta;
		m_pModelCom->Set_AnimationIndex(ZOMBIEA_Passout);
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
		m_pModelCom->Reset_AnimPlayTime(ZOMBIEA_Hit_Down);
		m_pModelCom->Set_AnimationIndex(ZOMBIEA_Hit_Down);
	})
		.AddTransition("HitDown to HitDownLoop", "HitDownLoop")
		.Predicator([this]()
	{
		return AnimFinishChecker(ZOMBIEA_Hit_Down);
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
		m_pModelCom->Reset_AnimPlayTime(ZOMBIEA_Hit_Loop);
		m_pModelCom->Set_AnimationIndex(ZOMBIEA_Hit_Loop);
	})
		.Tick([this](_double TimeDelta)
	{
		m_HitDownDelayTime += TimeDelta;
		m_pModelCom->Set_AnimationIndex(ZOMBIEA_Hit_Loop);
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
		m_pModelCom->Reset_AnimPlayTime(ZOMBIEA_Get_Up);
		m_pModelCom->Set_AnimationIndex(ZOMBIEA_Get_Up);
	})
		.OnExit([this]()
	{
		m_bHitDown = false;
	})
		.AddTransition("Getup to Chase", "Chase")
		.Predicator([this]()
	{
		return  AnimFinishChecker(ZOMBIEA_Get_Up);
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
		m_pModelCom->Reset_AnimPlayTime(ZOMBIEA_ATK_01);
		m_pModelCom->Set_AnimationIndex(ZOMBIEA_ATK_01);
	})
		.Tick([this](_double TimeDelta)
	{
		if (AnimIntervalChecker(ZOMBIEA_ATK_01, 0.5, 0.6))
		{
			m_bRealAttack = true;
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
		return AnimFinishChecker(ZOMBIEA_ATK_01);
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
		m_pModelCom->Reset_AnimPlayTime(ZOMBIEA_Die);
		m_pModelCom->Set_AnimationIndex(ZOMBIEA_Die);
	})
		.AddTransition("Dead to DeadBody", "DeadBody")
		.Predicator([this]()
	{
		return AnimFinishChecker(ZOMBIEA_Die);
	})
		.AddState("DeadBody")
		.Tick([this](_double TimeDelta)
	{
		m_pModelCom->Set_AnimationIndex(ZOMBIEA_DeadBody);
		m_dDeadTime += TimeDelta;

		if (m_dDeadTime > 3.0)
			m_bDead = true;
	})

		.Build();
}

void CZombieA::Adjust_Collision(_double TimeDelta)
{
	if (m_pAttackColCom)
		m_pAttackColCom->Update(m_pTransformCom->Get_WorldMatrix());

	if (m_pSwordColCom)
		m_pSwordColCom->Update(m_pTransformCom->Get_WorldMatrix());

	CollisionToPlayer(TimeDelta);
	CollisionToAttack(TimeDelta);

	if (m_bPlayerAttackCommand)
		CollisionToWeapon(TimeDelta);

	if (m_bPlayerSkill02Command)
		CollisionToWeaponSkill02(TimeDelta);

	if (m_bPlayerSkill04Command)
		CollisionToWeaponSkill04(TimeDelta);

	if (!m_bPlayerSkill02Command && !m_bPlayerSkill04Command)
		m_bPossibleSkillDamaged = false;
}

void CZombieA::CollisionToPlayer(_double TimeDelta)
{
	CGameInstance*			pGameInstance = GET_INSTANCE(CGameInstance);

	CCollider*		pTargetCollider = nullptr;

	if (g_LEVEL == LEVEL_CHAP1)
		pTargetCollider = (CCollider*)pGameInstance->Get_ComponentPtr(LEVEL_CHAP1, TEXT("Layer_Player"), TEXT("Com_SPHERE"));
	else if (g_LEVEL == LEVEL_CHAP2)
		pTargetCollider = (CCollider*)pGameInstance->Get_ComponentPtr(LEVEL_CHAP2, TEXT("Layer_Player"), TEXT("Com_SPHERE"));
	else if (g_LEVEL == LEVEL_CHAP3)
		pTargetCollider = (CCollider*)pGameInstance->Get_ComponentPtr(LEVEL_CHAP3, TEXT("Layer_Player"), TEXT("Com_SPHERE"));

	if (nullptr == pTargetCollider)
		return;

	if (m_pColliderCom[COLLTYPE_SPHERE]->Collision(pTargetCollider) == true)
		m_bPlayerChase = true;
	else
		m_bPlayerChase = false;

	RELEASE_INSTANCE(CGameInstance);
}

void CZombieA::CollisionToAttack(_double TimeDelta)
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
				m_pPlayer->FrontDamagedToMonster();
			else
				m_pPlayer->BackDamagedToMonster();
		}
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CZombieA::CollisionToWeapon(_double TimeDelta)
{
	CCollider* pTargetCollider = m_pPlayer->Get_WeaponCollider();

	if (nullptr == pTargetCollider)
		return;

	if (m_pColliderCom[COLLTYPE_AABB]->Collision(pTargetCollider) == true)
	{
		_vector			vTargetLook = m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_LOOK);
		_vector			vDot = XMVector3Dot(m_pTransformCom->Get_State(CTransform::STATE_LOOK), vTargetLook);
		_float			fDot = XMVectorGetX(vDot);

		if (fDot < 0)
			m_bFrontDamaged = true;
		else
			m_bBackDamaged = true;
	}
}

void CZombieA::CollisionToWeaponSkill02(_double TimeDelta)
{
	CCollider* pTargetCollider = m_pPlayer->Get_WeaponCollider();

	if (nullptr == pTargetCollider)
		return;

	if (m_pColliderCom[COLLTYPE_AABB]->Collision(pTargetCollider) == true)
	{
		AdjustSetDamageToSkill();
		m_bHitDown = true;
	}
}

void CZombieA::CollisionToWeaponSkill04(_double TimeDelta)
{
	CCollider* pTargetCollider = m_pPlayer->Get_WeaponCollider();

	if (nullptr == pTargetCollider)
		return;

	if (m_pColliderCom[COLLTYPE_AABB]->Collision(pTargetCollider) == true)
	{
		AdjustSetDamageToSkill();
		m_bGroggy = true;
	}
}

void CZombieA::CollisionToMonster(_double TimeDelta)
{
	CGameInstance*			pGameInstance = GET_INSTANCE(CGameInstance);

	if (g_LEVEL == LEVEL_CHAP1)
	{
		_uint iLayerSize = static_cast<_uint>(pGameInstance->Find_LayerList(LEVEL_CHAP1, TEXT("Layer_Monster")).size());

		for (_uint i = 0; i < iLayerSize; ++i)
		{
			CCollider*	pTargetCollider = (CCollider*)pGameInstance->Get_ComponentPtr(LEVEL_CHAP1, TEXT("Layer_Monster"), TEXT("Com_AABB"), i);
			// 서로의 길이를 비교해서 어느 정도 근처에 있으면 이 콜리젼을 실행 할 수 있게하자.

			if (pTargetCollider == nullptr || pTargetCollider == this->m_pColliderCom[COLLTYPE_AABB])
				continue;

			Safe_AddRef(pTargetCollider);
			m_MonsterColliders.push_back(pTargetCollider);
		}
	}
	else if (g_LEVEL == LEVEL_CHAP2)
	{
		_uint iLayerSize = static_cast<_uint>(pGameInstance->Find_LayerList(LEVEL_CHAP2, TEXT("Layer_Monster")).size());

		for (_uint i = 0; i < iLayerSize; ++i)
		{
			CCollider*	pTargetCollider = (CCollider*)pGameInstance->Get_ComponentPtr(LEVEL_CHAP2, TEXT("Layer_Monster"), TEXT("Com_AABB"), i);
			// 서로의 길이를 비교해서 어느 정도 근처에 있으면 이 콜리젼을 실행 할 수 있게하자.

			if (pTargetCollider == nullptr || pTargetCollider == this->m_pColliderCom[COLLTYPE_AABB])
				continue;

			Safe_AddRef(pTargetCollider);
			m_MonsterColliders.push_back(pTargetCollider);
		}
	}
	else if (g_LEVEL == LEVEL_CHAP3)
	{
		_uint iLayerSize = static_cast<_uint>(pGameInstance->Find_LayerList(LEVEL_CHAP3, TEXT("Layer_Monster")).size());

		for (_uint i = 0; i < iLayerSize; ++i)
		{
			CCollider*	pTargetCollider = (CCollider*)pGameInstance->Get_ComponentPtr(LEVEL_CHAP3, TEXT("Layer_Monster"), TEXT("Com_AABB"), i);
			// 서로의 길이를 비교해서 어느 정도 근처에 있으면 이 콜리젼을 실행 할 수 있게하자.

			if (pTargetCollider == nullptr || pTargetCollider == this->m_pColliderCom[COLLTYPE_AABB])
				continue;

			Safe_AddRef(pTargetCollider);
			m_MonsterColliders.push_back(pTargetCollider);
		}
	}


	// 이 콜라이더는 sphere여야만함
	_float3 sphereCenter = m_pAttackColCom->Get_CollisionCenter();
	_float sphereRadius = m_pAttackColCom->Get_SphereRadius();

	_uint iMonsterColliderSize = static_cast<_uint>(m_MonsterColliders.size());

	for (_uint i = 0; i < iMonsterColliderSize; ++i)
	{
		// sphere -> AttackColCom
		// AABB -> m_MonsterColliders
		_float3	p;
		ClosestPtPointAABB(sphereCenter, m_MonsterColliders[i], p);

		_vector v = p - sphereCenter;

		_float fDistance_Squared = XMVectorGetX(XMVector3Dot(v, v));

		if (fDistance_Squared <= sphereRadius * sphereRadius)
		{
			if (false == XMVector3NearEqual(v, _float4::Zero, XMVectorSet(0.001f, 0.001f, 0.001f, 0.001f)))
			{
				v = XMVector3Normalize(v);
			}
			_vector		vPosition = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
			_float4		vOldPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

			vPosition -= v * (sphereRadius - XMVectorGetX(XMVector3Length(p - sphereCenter)));

			if (true == m_pNavigationCom->isMove_OnNavigation(vPosition, &vOldPos))
			{
				m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vPosition);
			}
		}
	}

	for (auto pCollider : m_MonsterColliders)
		Safe_Release(pCollider);

	m_MonsterColliders.clear();

	RELEASE_INSTANCE(CGameInstance);
}

void CZombieA::AdditiveAnim(_double TimeDelta)
{
	if (m_bFrontDamaged)
	{
		AdjustSetDamage();
		m_pModelCom->Set_AdditiveAnimIndex(ZOMBIEA_ADD_DMG_F);
		m_pModelCom->Play_AddtivieAnim(TimeDelta, 1.f);
	}

	if (AnimFinishChecker(ZOMBIEA_ADD_DMG_F))
	{
		m_bFrontDamaged = false;
		m_bPossibleDamaged = false;
		m_pModelCom->Reset_AnimPlayTime(ZOMBIEA_ADD_DMG_F);
	}

	///////////////////////////////////////

	if (m_bBackDamaged)
	{
		AdjustSetDamage();
		m_pModelCom->Set_AdditiveAnimIndex(ZOMBIEA_ADD_DMG_B);
		m_pModelCom->Play_AddtivieAnim(TimeDelta, 1.f);
	}

	if (AnimFinishChecker(ZOMBIEA_ADD_DMG_B))
	{
		m_bBackDamaged = false;
		m_bPossibleDamaged = false;
		m_pModelCom->Reset_AnimPlayTime(ZOMBIEA_ADD_DMG_B);
	}
}

HRESULT CZombieA::SetUp_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Model_ZombieA"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC			ColliderDesc;

	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vCenter = _float3(0.f, 1.4f, 0.f);
	ColliderDesc.vSize = _float3(0.4f, 1.4f, 0.4f);

	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_AABB"),
		(CComponent**)&m_pColliderCom[COLLTYPE_AABB], &ColliderDesc)))
		return E_FAIL;

	/* For.Com_SPHERE */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vSize = _float3(12.f, 12.f, 12.f);

	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Collider_SPHERE"), TEXT("Com_SPHERE"),
		(CComponent**)&m_pColliderCom[COLLTYPE_SPHERE], &ColliderDesc)))
		return E_FAIL;

	/* For.Com_Attack */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vCenter = _float3(0.f, 1.4f, 0.f);
	ColliderDesc.vSize = _float3(1.4f, 1.4f, 1.4f);

	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Collider_SPHERE"), TEXT("Com_SPHERE_Attack"),
		(CComponent**)&m_pAttackColCom, &ColliderDesc)))
		return E_FAIL;

	/* For.Com_Sword */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vCenter = _float3(0.f, 1.6f, 1.45f);
	ColliderDesc.vSize = _float3(0.6f, 0.6f, 0.6f);

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

HRESULT CZombieA::SetUp_ShaderResources()
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

	/* For.Lights */
	const LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(0);
	if (nullptr == pLightDesc)
		return E_FAIL;
	//
	//if (FAILED(m_pShaderCom->Set_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Set_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Set_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Set_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
	//	return E_FAIL;

	//if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4))))
	//	return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

_bool CZombieA::AnimFinishChecker(ANIMATION eAnim, _double FinishRate)
{
	return m_pModelCom->Get_IndexAnim(eAnim)->Get_PlayRate() >= FinishRate;
}

_bool CZombieA::AnimIntervalChecker(ANIMATION eAnim, _double StartRate, _double FinishRate)
{
	if (m_pModelCom->Get_IndexAnim(eAnim)->Get_PlayRate() > StartRate &&
		m_pModelCom->Get_IndexAnim(eAnim)->Get_PlayRate() <= FinishRate)
	{
		return true;
	}

	return false;
}

CZombieA * CZombieA::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CZombieA*		pInstance = new CZombieA(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Created : CZombieA");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CZombieA::Clone(void * pArg)
{
	CZombieA*		pInstance = new CZombieA(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Cloned : CZombieA");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CZombieA::Free()
{
	__super::Free();

	Safe_Release(m_pAttackColCom);
	Safe_Release(m_pSwordColCom);

	for (auto pCollider : m_MonsterColliders)
		Safe_Release(pCollider);

	m_MonsterColliders.clear();
}