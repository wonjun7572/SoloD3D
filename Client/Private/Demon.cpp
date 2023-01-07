#include "stdafx.h"
#include "..\Public\Demon.h"
#include "GameInstance.h"
#include "Weapon.h"
#include "Animation.h"
#include "FSMComponent.h"
#include "MathUtils.h"
#include "Terrain.h"
#include "Bone.h"

CDemon::CDemon(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CMonster(pDevice, pContext)
{
}

CDemon::CDemon(const CDemon & rhs)
	: CMonster(rhs)
{
}

HRESULT CDemon::Init_Prototype()
{
	if (FAILED(__super::Init_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDemon::Init(void * pArg)
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

	m_strObjName = L"DEMON";

	m_iHp = 100;
	m_iAttack = 10;
	m_iDefence = 5;

	XMStoreFloat4x4(&m_Mat, m_pTransformCom->Get_WorldMatrix());

	return S_OK;
}

void CDemon::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	AdditiveAnim(TimeDelta);
	Play_Skill(TimeDelta);
}

void CDemon::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (m_bDeadAnim)
		return;

	Adjust_Collision(TimeDelta);
}

HRESULT CDemon::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		/* �� ���� �׸������� ���̴��� ���׸��� �ؽ��ĸ� �����ϳ�. */
		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");

		m_pModelCom->Render(m_pShaderCom, i, 0, "g_BoneMatrices");
	}	

#ifdef _DEBUG
	m_pAttackColCom->Render();
	m_pSwordColCom->Render();
	m_pSkillHitDownColCom->Render();
	m_pSkillKnockBackColCom->Render();
#endif
	return S_OK;
}

void CDemon::Imgui_RenderProperty()
{
	ImGui::Text("HP : %d", m_iHp);

	if (ImGui::Button("Navi~"))
	{
		m_pNavigationCom->Set_CurreuntIndex(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	}
	ImGui::DragFloat("RotationX", &m_fRotationX, 0.1f, -180.f, 180.f);
	ImGui::DragFloat("RotationY", &m_fRotationY, 0.1f, -180.f, 180.f);
	ImGui::DragFloat("RotationZ", &m_fRotationZ, 0.01f, -180.f, 180.f);
	m_pSkillKnockBackColCom->FixedSizeForImgui(2);
}

void CDemon::Adjust_Collision(_double TimeDelta)
{
	if (m_pAttackColCom)
		m_pAttackColCom->Update(m_pTransformCom->Get_WorldMatrix());

	if (m_pSwordColCom)
		m_pSwordColCom->Update(m_pTransformCom->Get_WorldMatrix());

	if (m_pSkillHitDownColCom)
	{
		m_pSkillHitDownColCom->FixedSphereSize(0.f, 0.f, 0.f, m_fSkillHitDownRange);
		m_pSkillHitDownColCom->Update(m_pTransformCom->Get_WorldMatrix());
	}

	if (m_pSkillKnockBackColCom)
	{
		m_pSkillKnockBackColCom->FixedOBBSize(m_fOBBX, m_fOBBY, m_fOBBZ, m_fOBBCX, m_fOBBCY, m_fOBBCZ);
		
		_matrix  mat = m_pTransformCom->Get_WorldMatrix();

		_matrix originMat =	XMLoadFloat4x4(&m_Mat);
		
		_matrix		RotationYMatrix =
			XMMatrixRotationX(m_fRotationX) *
			XMMatrixRotationY(m_fRotationY) *
			XMMatrixRotationZ(m_fRotationZ);

		mat = RotationYMatrix * mat;

		m_pSkillKnockBackColCom->Update(mat);
	}

	CollisionToPlayer(TimeDelta);

	CollisionToMonster(TimeDelta);
	
	// �÷��̾�� ��� �����Ÿ��� �Ǿ��� ��
	// �� chase ���·� ���� ���� ������ ���� �϶���
	if(m_bPlayerChase)
		CollisionToAttack(TimeDelta);

	// �÷��̾� ���� �̻� ��ų
	if (m_bSkill_1ToPlayer	
		|| m_bSkill_2ToPlayer
		|| m_bSkill_3ToPlayer
		|| m_bSkill_4ToPlayer
		|| m_bSkill_5ToPlayer
		|| m_bSkill_6ToPlayer)
		CollisionToSkill(TimeDelta);

	// ���� �÷��̾��� ������ �Ծ��� �� �̺�Ʈ

	if (m_bPlayerAttackCommand)
		CollisionToWeapon(TimeDelta);

	if (m_bPlayerSkill02Command)
		CollisionToWeaponSkill02(TimeDelta);

	if (m_bPlayerSkill04Command)
		CollisionToWeaponSkill04(TimeDelta);

	if (!m_bPlayerSkill02Command && !m_bPlayerSkill04Command)
		m_bImpossibleSkillDamaged = false;
}

void CDemon::CollisionToPlayer(_double TimeDelta)
{
	// �� �κ� �浹�� �ؾ��� ������ ���� �� ����
	_float3 vPos =	m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_float3 vPlayerPos = m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION);
	_float fDistance =	CMathUtils::Distance(vPos, vPlayerPos);

	if (fabsf(fDistance) < 10.f)
		m_bPlayerChase = true;
	else
		m_bPlayerChase = false;
}

void CDemon::CollisionToAttack(_double TimeDelta)
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
			// �÷��̾ ���� ���� �ִ��� �ڸ� ���� �ִ��ĸ� �Ǵ�����
			_vector			vTargetLook = m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_LOOK);
			_vector			vDot = XMVector3Dot(m_pTransformCom->Get_State(CTransform::STATE_LOOK), vTargetLook);
			_float			fDot = XMVectorGetX(vDot);

			// 0���� ������ �� �տ� �ִ�.
			if (fDot < 0)
				m_pPlayer->FrontDamagedToMonster();
			else
				m_pPlayer->BackDamagedToMonster();
		}
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CDemon::CollisionToSkill(_double TimeDelta)
{
	// ��ų�� Ȱ���ؼ� �ݸ����� ��� �� ���ΰ��� ���ؼ� ����غ����ҵ�
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

	if (m_bRealSkill)
	{
		if (m_bSkill_1ToPlayer && m_pSwordColCom->Collision(pTargetCollider) == true)
			m_pPlayer->PassOutToMonster();

		if (m_bSkill_2ToPlayer && m_pSkillHitDownColCom->Collision(pTargetCollider) == true)
			m_pPlayer->PassOutToMonster(0.1f);

		if(m_bSkill_3ToPlayer && m_pSkillKnockBackColCom->Collision(pTargetCollider) == true)
			m_pPlayer->PassOutToMonster(0.1f);

		if (m_bSkill_4ToPlayer && m_pSkillKnockBackColCom->Collision(pTargetCollider) == true)
			m_pPlayer->PassOutToMonster(0.2f);

		if (m_bSkill_5ToPlayer && m_pSkillHitDownColCom->Collision(pTargetCollider) == true)
			m_pPlayer->HitDownToMonster(0.1f , 0.5f , 0.7f);

		if (m_bSkill_6ToPlayer && m_pSkillHitDownColCom->Collision(pTargetCollider) == true)
			m_pPlayer->HitDownToMonster();
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CDemon::Play_Skill(_double TimeDelta)
{
	m_SkillDelayTime += TimeDelta;

	if (m_SkillDelayTime > 10.0 &&
		!m_bSkill_1ToPlayer &&
		!m_bSkill_2ToPlayer &&
		!m_bSkill_3ToPlayer &&
		!m_bSkill_4ToPlayer &&
		!m_bSkill_5ToPlayer &&
		!m_bSkill_6ToPlayer)
	{
		_uint iSkill = rand() % 6;

		if (iSkill == 0)
			m_bSkill_1ToPlayer = true;
		if (iSkill == 1)
			m_bSkill_2ToPlayer = true;
		if (iSkill == 2)
			m_bSkill_3ToPlayer = true;
		if (iSkill == 3)
			m_bSkill_4ToPlayer = true;
		if (iSkill == 4)
			m_bSkill_5ToPlayer = true;
		if (iSkill == 5)
			m_bSkill_6ToPlayer = true;
	}
}

void CDemon::AdditiveAnim(_double TimeDelta)
{
	if (m_bFrontDamaged)
	{
		AdjustSetDamage();
		m_pModelCom->Set_AdditiveAnimIndex(DEMON_ADD_DMG_F);
		m_pModelCom->Play_AddtivieAnim(TimeDelta, 1.f);
	}

	if (AnimFinishChecker(DEMON_ADD_DMG_F))
	{
		m_bFrontDamaged = false;
		m_bImpossibleDamaged = false;
		m_pModelCom->Reset_AnimPlayTime(DEMON_ADD_DMG_F);
	}

	///////////////////////////////////////

	if (m_bBackDamaged)
	{
		AdjustSetDamage();
		m_pModelCom->Set_AdditiveAnimIndex(DEMON_ADD_DMG_B);
		m_pModelCom->Play_AddtivieAnim(TimeDelta, 1.f);
	}

	if (AnimFinishChecker(DEMON_ADD_DMG_B))
	{
		m_bBackDamaged = false;
		m_bImpossibleDamaged = false;
		m_pModelCom->Reset_AnimPlayTime(DEMON_ADD_DMG_B);
	}
}

void CDemon::SetUp_FSM()
{
	m_pFSM = CFSMComponentBuilder()
		.InitState("Idle")
		.AddState("Idle")
		.Tick([this](_double TimeDelta)
	{
		m_pModelCom->Set_AnimationIndex(DEMON_Idle_P_01);
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
			m_pModelCom->Set_AnimationIndex(DEMON_Run_F);
			m_pTransformCom->ChaseAndLookAt(vPlayerPos, TimeDelta, 0.1f, m_pNavigationCom);
		}
		else
		{
			m_pModelCom->Set_AnimationIndex(DEMON_Idle_C);
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
		return m_bPlayerChase
			&& m_bPlayerAttack
			&& m_AttackDelayTime > 3.0
			&& !m_bSkill_1ToPlayer
			&& !m_bSkill_2ToPlayer
			&& !m_bSkill_3ToPlayer
			&& !m_bSkill_4ToPlayer
			&& !m_bSkill_5ToPlayer
			&& !m_bSkill_6ToPlayer;
	})
		.AddTransition("Chase to Skill_1", "Skill_1")
		.Predicator([this]()
	{
		return m_bPlayerChase && m_bSkill_1ToPlayer;
	})
		.AddTransition("Chase to Skill_2", "Skill_2")
		.Predicator([this]()
	{
		return m_bPlayerChase && m_bSkill_2ToPlayer;
	})
		.AddTransition("Chase to Skill_3", "Skill_3")
		.Predicator([this]()
	{
		return m_bPlayerChase && m_bSkill_3ToPlayer;
	})
		.AddTransition("Chase to Skill_4", "Skill_4")
		.Predicator([this]()
	{
		return m_bPlayerChase && m_bSkill_4ToPlayer;
	})
		.AddTransition("Chase to Skill_5", "Skill_5")
		.Predicator([this]()
	{
		return m_bPlayerChase && m_bSkill_5ToPlayer;
	})
		.AddTransition("Chase to Skill_6_Charging", "Skill_6_Charging")
		.Predicator([this]()
	{
		return m_bPlayerChase && m_bSkill_6ToPlayer;
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

		.AddState("Skill_1")
		.OnStart([this]() 
	{
		m_pModelCom->Reset_AnimPlayTime(DEMON_SK_Firing_01);
		m_pModelCom->Set_AnimationIndex(DEMON_SK_Firing_01);
	})
		.Tick([this](_double TImeDelta) 
	{
		if (AnimIntervalChecker(DEMON_SK_Firing_01, 0.4, 0.6))
			m_bRealSkill = true;
		else
			m_bRealSkill = false;
	})
		.OnExit([this]()
	{
		m_bRealSkill = false;
		m_bSkill_1ToPlayer = false;
		m_SkillDelayTime = 0.0;
	})
		.AddTransition("Skill_1 to Idle", "Idle")
		.Predicator([this]()
	{
		return AnimFinishChecker(DEMON_SK_Firing_01);
	})
		.AddTransition("Skill_1 to HitDown", "HitDown")
		.Predicator([this]()
	{
		return m_bHitDown;
	})
		.AddTransition("Skill_1 to Groggy", "Groggy")
		.Predicator([this]()
	{
		return m_bGroggy;
	})
		.AddTransition("Skill_1 to Dead", "Dead")
		.Predicator([this]()
	{
		return m_bDeadAnim;
	})

		.AddState("Skill_2")
		.OnStart([this]()
	{
		m_pModelCom->Reset_AnimPlayTime(DEMON_SK_Firing_02);
		m_pModelCom->Set_AnimationIndex(DEMON_SK_Firing_02);
		m_fSkillHitDownRange = 4.f;
	})
		.Tick([this](_double TImeDelta)
	{
		if (AnimIntervalChecker(DEMON_SK_Firing_02, 0.6, 0.8))
			m_bRealSkill = true;
		else
			m_bRealSkill = false;
	})
		.OnExit([this]()
	{
		m_bSkill_2ToPlayer = false;
		m_fSkillHitDownRange = 0.f;
		m_SkillDelayTime = 0.0;
	})
		.AddTransition("Skill_2 to Idle", "Idle")
		.Predicator([this]()
	{
		return AnimFinishChecker(DEMON_SK_Firing_02);
	})
		.AddTransition("Skill_2 to Dead", "Dead")
		.Predicator([this]()
	{
		return m_bDeadAnim;
	})

		.AddState("Skill_3")
		.OnStart([this]()
	{
		m_pModelCom->Reset_AnimPlayTime(DEMON_SK_Firing_03);
		m_pModelCom->Set_AnimationIndex(DEMON_SK_Firing_03);

		m_fOBBX = 0.f;
		m_fOBBY = 4.f;
		m_fOBBZ = 2.5f;

		m_fOBBCX = 0.5f;
		m_fOBBCY = 1.f;
		m_fOBBCZ = 4.f;

		m_fRotationX = 0.5f;
		m_fRotationY = -1.f;
		m_fRotationZ = 0.f;
	})
		.Tick([this](_double TimeDelta) 
	{
		m_bRealSkill = true;
		
		if(AnimIntervalChecker(DEMON_SK_Firing_03, 0.2, 0.9))
			m_fRotationY += static_cast<_float>(TimeDelta);
		
		if (m_fRotationY >= 1.f)
			m_fRotationY = 1.f;
	})
		.OnExit([this]()
	{
		m_fOBBX = 0.f;
		m_fOBBY = 3.f;
		m_fOBBZ = 0.f;

		m_fOBBCX = 0.5f;
		m_fOBBCY = 1.f;
		m_fOBBCZ = 0.f;

		m_fRotationX = 0.f;
		m_fRotationY = 0.f;
		m_fRotationZ = 0.f;
		m_bRealSkill = false;
		m_bSkill_3ToPlayer = false;
		m_SkillDelayTime = 0.0;
	})
		.AddTransition("Skill_3 to Idle", "Idle")
		.Predicator([this]()
	{
		return AnimFinishChecker(DEMON_SK_Firing_03);
	})
		.AddTransition("Skill_3 to Dead", "Dead")
		.Predicator([this]()
	{
		return m_bDeadAnim;
	})

		.AddState("Skill_4")
		.OnStart([this]()
	{
		m_fOBBX = 0.f;
		m_fOBBY = 3.f;
		m_fOBBZ = 0.f;

		m_fOBBCX = 0.5f;
		m_fOBBCY = 1.f;
		m_fOBBCZ = 0.f;

		m_fRotationX = 0.f;
		m_fRotationY = 0.f;
		m_fRotationZ = 0.f;

		m_pModelCom->Reset_AnimPlayTime(DEMON_SK_Firing_04);
		m_pModelCom->Set_AnimationIndex(DEMON_SK_Firing_04);
	})
		.Tick([this](_double TimeDelta) 
	{
		m_fOBBZ  += static_cast<_float>(TimeDelta) * 2.f;
		m_fOBBCZ += static_cast<_float>(TimeDelta) * 2.f;
		if (AnimIntervalChecker(DEMON_SK_Firing_04, 0.7, 0.9))
			m_bRealSkill = true;
		else
			m_bRealSkill = false;
	})
		.OnExit([this]()
	{
		m_bSkill_4ToPlayer = false;
		m_SkillDelayTime = 0.0;
		m_fOBBZ = 0.f;
		m_fOBBCZ = 0.f;
	})
		.AddTransition("Skill_4 to Idle", "Idle")
		.Predicator([this]()
	{
		return AnimFinishChecker(DEMON_SK_Firing_04);
	})
		.AddTransition("Skill_4 to Dead", "Dead")
		.Predicator([this]()
	{
		return m_bDeadAnim;
	})

		.AddState("Skill_5")
		.OnStart([this]()
	{
		m_pModelCom->Reset_AnimPlayTime(DEMON_SK_Firing_05);
		m_pModelCom->Set_AnimationIndex(DEMON_SK_Firing_05);
		m_fSkillHitDownRange = 4.f;
	})
		.Tick([this](_double TimeDelta) 
	{
		if (AnimIntervalChecker(DEMON_SK_Firing_05, 0.6, 0.7))
			m_bRealSkill = true;
		else
			m_bRealSkill = false;
	})
		.OnExit([this]()
	{
		m_bRealSkill = false;
		m_bSkill_5ToPlayer = false;
		m_SkillDelayTime = 0.0;
		m_fSkillHitDownRange = 0.f;
	})
		.AddTransition("Skill_5", "Idle")
		.Predicator([this]()
	{
		return AnimFinishChecker(DEMON_SK_Firing_05);
	})
		.AddTransition("Skill_5 to Dead", "Dead")
		.Predicator([this]()
	{
		return m_bDeadAnim;
	})

		.AddState("Skill_6_Charging")
		.OnStart([this]()
	{
		m_pModelCom->Reset_AnimPlayTime(DEMON_SK_Firing_06);
		m_pModelCom->Set_AnimationIndex(DEMON_SK_Firing_06);
	})
		.Tick([this](_double TimeDelta) 
	{
		m_fSkillHitDownRange += static_cast<_float>(TimeDelta) * 2.5f;
		m_bImpossibleDamaged = true;
		m_bImpossibleSkillDamaged = true;
		m_pModelCom->Set_AnimationIndex(DEMON_SK_Firing_06);
	})
		.AddTransition("Skill_6_Charging to Skill_6", "Skill_6")
		.Predicator([this]()
	{
		return AnimFinishChecker(DEMON_SK_Firing_06);
	})

		.AddState("Skill_6")
		.OnStart([this]() 
	{
		m_bImpossibleDamaged = false;
		m_bImpossibleSkillDamaged = false;
		m_pModelCom->Reset_AnimPlayTime(DEMON_SK_Firing_06_END);
		m_pModelCom->Set_AnimationIndex(DEMON_SK_Firing_06_END);
	})
		.Tick([this](_double TimeDelta) 
	{
		m_bRealSkill = true;
	})
		.OnExit([this]()
	{
		m_fSkillHitDownRange = 0.f;
		m_bRealSkill = false;
		m_bSkill_6ToPlayer = false;
		m_SkillDelayTime = 0.0;
		m_bGroggy = false;
		m_bHitDown = false;
	})
		.AddTransition("Skill_6 to Idle", "Idle")
		.Predicator([this]()
	{
		return AnimFinishChecker(DEMON_SK_Firing_06_END , 0.85);
	})
		.AddTransition("Skill_6 to Dead", "Dead")
		.Predicator([this]()
	{
		return m_bDeadAnim;
	})



		// Groggy
		.AddState("Groggy")
		.OnStart([this]()
	{
		m_GroggyDelayTime = 0.0;
		m_pModelCom->Reset_AnimPlayTime(DEMON_Passout);
		m_pModelCom->Set_AnimationIndex(DEMON_Passout);
	})
		.Tick([this](_double TimeDelta)
	{
		m_GroggyDelayTime += TimeDelta;
		m_pModelCom->Set_AnimationIndex(DEMON_Passout);
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
		m_pModelCom->Reset_AnimPlayTime(DEMON_Down);
		m_pModelCom->Set_AnimationIndex(DEMON_Down);
	})
		.AddTransition("HitDown to HitDownLoop", "HitDownLoop")
		.Predicator([this]()
	{
		return AnimFinishChecker(DEMON_Down , 0.83);
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
		m_pModelCom->Reset_AnimPlayTime(DEMON_Down_Loop);
		m_pModelCom->Set_AnimationIndex(DEMON_Down_Loop);
	})
		.Tick([this](_double TimeDelta)
	{
		m_HitDownDelayTime += TimeDelta;
		m_pModelCom->Set_AnimationIndex(DEMON_Down_Loop);
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
		m_pModelCom->Reset_AnimPlayTime(DEMON_Get_Up);
		m_pModelCom->Set_AnimationIndex(DEMON_Get_Up);
	})
		.OnExit([this]()
	{
		m_bHitDown = false;
	})
		.AddTransition("Getup to Chase", "Chase")
		.Predicator([this]()
	{
		return  AnimFinishChecker(DEMON_Get_Up, 0.85);
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
			m_pModelCom->Reset_AnimPlayTime(DEMON_ATK_01);
			m_pModelCom->Set_AnimationIndex(DEMON_ATK_01);
		}
		else if (m_iRandAttack == 1)
		{
			m_pModelCom->Reset_AnimPlayTime(DEMON_ATK_02);
			m_pModelCom->Set_AnimationIndex(DEMON_ATK_02);
		}
	})
		.Tick([this](_double TimeDelta)
	{
		if (m_iRandAttack == 0)
		{
			if (AnimIntervalChecker(DEMON_ATK_01, 0.6, 0.7))
				m_bRealAttack = true;
			else
				m_bRealAttack = false;
		}
		else if (m_iRandAttack == 1)
		{
			if (AnimIntervalChecker(DEMON_ATK_02, 0.6, 0.7))
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
		return AnimFinishChecker(DEMON_ATK_01) || 
			AnimFinishChecker(DEMON_ATK_02);
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
		m_pModelCom->Reset_AnimPlayTime(DEMON_Die);
		m_pModelCom->Set_AnimationIndex(DEMON_Die);
	})
		.AddTransition("Dead to DeadBody", "DeadBody")
		.Predicator([this]()
	{
		return AnimFinishChecker(DEMON_Die, 0.85);
	})
		.AddState("DeadBody")
		.Tick([this](_double TimeDelta)
	{
		m_pModelCom->Set_AnimationIndex(DEMON_DeadBody);
		m_dDeadTime += TimeDelta;

		if (m_dDeadTime > 3.0)
			m_bDead = true;
	})

		.Build();
}

HRESULT CDemon::SetUp_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Model_Demon"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC			ColliderDesc;

	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vCenter = _float3(0.f, 2.f, 0.f);
	ColliderDesc.vSize = _float3(1.f, 2.f, 1.f);

	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_AABB"),
		(CComponent**)&m_pColliderCom[COLLTYPE_AABB], &ColliderDesc)))
		return E_FAIL;

	/* For.Com_OBB */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vCenter = _float3(0.f, 3.f, 0.f);
	//ColliderDesc.vRotation = _float3(XMConvertToRadians(45.0f), 0.f, 0.f);
	ColliderDesc.vSize = _float3(0.5f, 1.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_OBB_KnockBack"),
		(CComponent**)&m_pSkillKnockBackColCom, &ColliderDesc)))
		return E_FAIL;

	/* For.Com_SPHERE */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vSize = _float3(1.5f, 1.5f, 1.5f);

	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Collider_SPHERE"), TEXT("Com_SPHERE"),
		(CComponent**)&m_pColliderCom[COLLTYPE_SPHERE], &ColliderDesc)))
		return E_FAIL;

	/* For.Com_Attack */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vCenter = _float3(0.f, 2.f, 0.f);
	ColliderDesc.vSize = _float3(2.5f, 2.5f, 2.5f);

	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Collider_SPHERE"), TEXT("Com_SPHERE_Attack"),
		(CComponent**)&m_pAttackColCom, &ColliderDesc)))
		return E_FAIL;

	/* For.Com_Sword */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vCenter = _float3(0.f, 2.4f, 2.7f);
	ColliderDesc.vSize = _float3(0.7f, 0.7f, 0.7f);

	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Collider_SPHERE"), TEXT("Com_SPHERE_Sword"),
		(CComponent**)&m_pSwordColCom, &ColliderDesc)))
		return E_FAIL;

	/* For.Com_Skill6 */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vSize = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Collider_SPHERE"), TEXT("Com_SPHERE_Skil6"),
		(CComponent**)&m_pSkillHitDownColCom, &ColliderDesc)))
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

HRESULT CDemon::SetUp_ShaderResources()
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

_bool CDemon::AnimFinishChecker(ANIMATION eAnim, _double FinishRate)
{
	return m_pModelCom->Get_IndexAnim(eAnim)->Get_PlayRate() >= FinishRate;
}

_bool CDemon::AnimIntervalChecker(ANIMATION eAnim, _double StartRate, _double FinishRate)
{
	if (m_pModelCom->Get_IndexAnim(eAnim)->Get_PlayRate() > StartRate &&
		m_pModelCom->Get_IndexAnim(eAnim)->Get_PlayRate() <= FinishRate)
	{
		return true;
	}

	return false;
}

CDemon * CDemon::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CDemon*		pInstance = new CDemon(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Created : CDemon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CDemon::Clone(void * pArg)
{
	CDemon*		pInstance = new CDemon(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDemon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDemon::Free()
{
	__super::Free();

	Safe_Release(m_pAttackColCom);
	Safe_Release(m_pSwordColCom);
	Safe_Release(m_pSkillHitDownColCom);
	Safe_Release(m_pSkillKnockBackColCom);

	for (auto pCollider : m_MonsterColliders)
		Safe_Release(pCollider);

	m_MonsterColliders.clear();
}
