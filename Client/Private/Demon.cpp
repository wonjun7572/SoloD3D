#include "stdafx.h"
#include "..\Public\Demon.h"
#include "GameInstance.h"
#include "Weapon.h"
#include "Animation.h"
#include "FSMComponent.h"
#include "MathUtils.h"
#include "Terrain.h"
#include "Bone.h"
#include "Effect_Mesh.h"
#include "Tree.h"
#include "ProgressBarUI.h"
#include "EffectManager.h"
#include "ConversationUI.h"
#include "DemonSkillCircular.h"
#include "RockMada.h"
#include "RockNorm04.h"
#include "Particle.h"

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

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	if (g_LEVEL == LEVEL_CHAP1)
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, _float4(35.f, 0.f, 60.f, 1.f));

	if (g_LEVEL == LEVEL_CHAP2)
	{
		SetUp_UI();
		m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, _float4(270.f, 0.f, 280.f , 1.f));
	}

	if (g_LEVEL == LEVEL_CHAP3)
	{
		m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, _float4(65.f, 25.f, 90.f, 1.f));
	}

	m_pNavigationCom->Set_CurreuntIndex(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

	m_strObjName = L"DEMON";

	m_fHp = 2000.f;
	m_fMaxHp = 2000.f;
	m_fAttack = 30;
	m_fDefence = 5;

	XMStoreFloat4x4(&m_Mat, m_pTransformCom->Get_WorldMatrix());

	m_vMonsterNamePos = _float2(720.f, 40.f);
	m_vMonsterNameScale = _float2(1.f, 1.f);

	if (FAILED(__super::SetUP_UI()))
		return E_FAIL;

	m_vRimColor = _float4(0.3f, 0.1f, 0.1f, 1.f);

	// texture
	CEffectManager::GetInstance()->Add_Effects(L"Prototype_GameObject_DemonSKillFloor", L"DEMON_SKILL_FLOOR");
	CEffectManager::GetInstance()->Add_Effects(L"Prototype_GameObject_FireE_0", L"DEMON_SKILL2_FIRE0");
	CEffect_Rect::RECTEFFECTDESC rectDesc;
	_matrix pivotMat = XMMatrixScaling(20.f, 20.f, 1.f) *
		XMMatrixRotationX(XMConvertToRadians(-90.f)) *
		XMMatrixRotationY(XMConvertToRadians(-110.f)) *
		XMMatrixRotationZ(XMConvertToRadians(-180.f)) *
		XMMatrixTranslation(0.f, 0.6f, 0.f);

	XMStoreFloat4x4(&rectDesc.PivotMatrix, pivotMat);
	rectDesc.pTargetTransform = m_pTransformCom;
	Safe_AddRef(m_pTransformCom);
	if (FAILED(CEffectManager::GetInstance()->Add_Effects(L"Prototype_GameObject_DemonSKillCircular", L"DEMON_SKILL_CIRCULAR", &rectDesc)))
	{
		static_cast<CEffect_Mesh*>(CEffectManager::GetInstance()->Find_Effects(L"DEMON_SKILL_CIRCULAR"))->Set_Target(m_pTransformCom);
		Safe_AddRef(m_pTransformCom);
	}
	
	pivotMat = XMMatrixScaling(3.f, 3.f, 1.f) *
		XMMatrixRotationX(XMConvertToRadians(90.f)) *
		XMMatrixRotationY(XMConvertToRadians(-90.f)) *
		XMMatrixRotationZ(XMConvertToRadians(0.f)) *
		XMMatrixTranslation(0.f, 0.6f, 0.f);

	XMStoreFloat4x4(&rectDesc.PivotMatrix, pivotMat);
	rectDesc.pTargetTransform = m_pTransformCom;
	Safe_AddRef(m_pTransformCom);
	if (FAILED(CEffectManager::GetInstance()->Add_Effects(L"Prototype_GameObject_DemonSKillStraight", L"DEMON_SKILL_Straight", &rectDesc)))
	{
		static_cast<CEffect_Mesh*>(CEffectManager::GetInstance()->Find_Effects(L"DEMON_SKILL_Straight"))->Set_Target(m_pTransformCom);
		Safe_AddRef(m_pTransformCom);
	}

	CEffect_Mesh::EFFECTDESC effectDesc;
	ZeroMemory(&effectDesc, sizeof(CEffect_Mesh::EFFECTDESC));

	pivotMat = XMMatrixScaling(0.1f, 0.1f, 0.5f) *
		XMMatrixRotationX(XMConvertToRadians(-140.f)) *
		XMMatrixRotationY(XMConvertToRadians(0.f)) *
		XMMatrixRotationZ(XMConvertToRadians(0.f)) *
		XMMatrixTranslation(0.f, 5.5f, 0.f);

	XMStoreFloat4x4(&effectDesc.PivotMatrix, pivotMat);
	effectDesc.iPassIndex = 3;
	effectDesc.iDiffuseTex = 42;
	effectDesc.iMaskTex = 28;
	effectDesc.pTargetTransform = m_pTransformCom;
	Safe_AddRef(m_pTransformCom);

	if (FAILED(CEffectManager::GetInstance()->Add_Effects(L"Prototype_GameObject_Twister_Line", L"DEMON_SKILL3_LINE", &effectDesc)))
	{
		static_cast<CEffect_Mesh*>(CEffectManager::GetInstance()->Find_Effects(L"DEMON_SKILL3_LINE"))->Set_Target(m_pTransformCom);
		Safe_AddRef(m_pTransformCom);
	}

	CEffectManager::GetInstance()->SetUp_Effects(L"DEMON_SKILL3_LINE", 1.f, 1.f, _float2(0.f, 0.f));

	return S_OK;
}

void CDemon::Tick(_double TimeDelta)
{
	if (g_LEVEL == LEVEL_CHAP3 || g_LEVEL == LEVEL_CHAP1)
	{
		__super::Tick(TimeDelta);
		AdditiveAnim(TimeDelta);
		Play_Skill(TimeDelta);
	}
	else if (g_LEVEL == LEVEL_CHAP2)
	{
		Level_Chap2Tick(TimeDelta);
	
		m_pModelCom->Play_Animation(TimeDelta);

		if (m_pNavigationCom)
		{
			_float yPos = 0.f;
			m_pNavigationCom->isHeighit_OnNavigation(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION), &yPos);
			m_pTransformCom->Set_Height(yPos);
		}

		for (auto& pUI : m_MonsterUI)
			pUI->Tick(TimeDelta);

		if (m_bConversation)
		{
			Conversation(TimeDelta);
			m_UI[UI_CONVERSATION]->Late_Tick(TimeDelta);
		}

		for (auto& pDamageFont : m_MonsterDamageFontUI)
			pDamageFont->Tick(TimeDelta);

		for (auto& pDamageEffect : m_MonsterDamageEffect)
			pDamageEffect->Tick(TimeDelta);

		UI_Tick(TimeDelta);
	}
}

void CDemon::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOW, this);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
#ifdef _DEBUG
		m_pRendererCom->Add_DebugRenderGroup(m_pAttackColCom);
		m_pRendererCom->Add_DebugRenderGroup(m_pSwordColCom);
		m_pRendererCom->Add_DebugRenderGroup(m_pSkillHitDownColCom);
		m_pRendererCom->Add_DebugRenderGroup(m_pSkillKnockBackColCom);
#endif
	}

	if (m_bDeadAnim)
		return;

	Adjust_Collision(TimeDelta);

	if (!m_bDeadAnim)
		CollisionToAABBPlayer(TimeDelta);
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
		/* 이 모델을 그리기위한 셰이더에 머테리얼 텍스쳐를 전달하낟. */
		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");

		bool HasNormal = false;
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_NORMALS, "g_NormalTexture")))
			HasNormal = false;
		else
			HasNormal = true;

		m_pShaderCom->Set_RawValue("g_HasNormal", &HasNormal, sizeof(bool));

		bool HasSpecular = false;
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_SPECULAR, "g_SpecularTexture")))
			HasSpecular = false;
		else
			HasSpecular = true;

		m_pShaderCom->Set_RawValue("g_HasSpecular", &HasSpecular, sizeof(bool));

		if (FAILED(m_pDissolveTexCom->Bind_ShaderResource(m_pShaderCom, "g_DissolveTexture")))
			return E_FAIL;

		m_pModelCom->Render(m_pShaderCom, i, 0, "g_BoneMatrices");
	}	

	return S_OK;
}

HRESULT CDemon::RenderShadow()
{
	if (FAILED(__super::RenderShadow()))
		return E_FAIL;

	if (FAILED(SetUP_ShadowShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
		m_pModelCom->Render(m_pShaderCom, i, 1, "g_BoneMatrices");

	return S_OK;
}

void CDemon::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();

	if (ImGui::Button("Navi~"))
	{
		m_pNavigationCom->Set_CurreuntIndex(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	}
	
	ImGui::DragFloat("Dissolve", &m_fDissolveAmount, 0.01f, -10.f, 10.f);
	ImGui::DragFloat("Finge", &m_fFringeAmount, 0.01f, -10.f, 10.f);
	
	if (ImGui::Button("Skill1"))
		m_bSkill_1ToPlayer = true;

	if (ImGui::Button("Skill2"))
		m_bSkill_2ToPlayer = true;

	if (ImGui::Button("Skill3"))
		m_bSkill_3ToPlayer = true;

	if (ImGui::Button("Skill4"))
		m_bSkill_4ToPlayer = true;

	if (ImGui::Button("Skill5"))
		m_bSkill_5ToPlayer = true;

	if (ImGui::Button("Skill6"))
		m_bSkill_6ToPlayer = true;

	//ImGui::DragFloat("CX", &m_CX, 0.1f, -10.f, 10.f);
	//ImGui::DragFloat("CY", &m_CY, 0.1f, -10.f, 10.f);
	//ImGui::DragFloat("CZ", &m_CZ, 0.1f, -10.f, 10.f);
	//
	//ImGui::DragFloat("RX", &m_RX, 1.f, 0.f, 360.f);
	//ImGui::DragFloat("RY", &m_RY, 1.f, 0.f, 360.f);
	//ImGui::DragFloat("RZ", &m_RZ, 1.f, 0.f, 360.f);
	//
	//ImGui::DragFloat("X", &m_X, 0.01f, -10.f, 10.f);
	//ImGui::DragFloat("Y", &m_Y, 0.01f, -10.f, 10.f);
	//ImGui::DragFloat("Z", &m_Z, 0.01f, -10.f, 10.f);
}

void CDemon::Conversation(_double TimeDelta)
{
	if (g_LEVEL == LEVEL_CHAP2)
	{
		m_UI[UI_CONVERSATION]->Tick(TimeDelta);

		if (TimeConversation < 3.f)
		{
			m_strConversation = L"나무는 이제 우리 꺼다!!!";
			TimeConversation += TimeDelta;
		}
		else if (TimeConversation >= 3.f && TimeConversation < 6.f)
		{
			m_strConversation = L"좋았어! 이제 이 나무의 영혼을 들고 가자 트롤들아!";
			TimeConversation += TimeDelta;
		}
		else if (TimeConversation >= 6.f && TimeConversation < 9.f)
		{
			m_strConversation = L"앗 녀석이 우리를 따라온다 도망쳐!!!";
			TimeConversation += TimeDelta;
		}

		static_cast<CConversationUI*>(m_UI[UI_CONVERSATION])->SetConversation(m_strConversation);

		if (TimeConversation > 9.f)
			m_bConversation = false;
		else
			static_cast<CPlayer*>(m_pPlayer)->Set_PlayerUI(false);
	}
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
	
	// 플레이어와 어느 정도거리가 되었을 때
	// 즉 chase 상태로 가는 것이 가능한 조건 일때임
	if(m_bPlayerChase)
		CollisionToAttack(TimeDelta);

	// 플레이어 상태 이상 스킬
	if (m_bSkill_1ToPlayer	
		|| m_bSkill_2ToPlayer
		|| m_bSkill_3ToPlayer
		|| m_bSkill_4ToPlayer
		|| m_bSkill_5ToPlayer
		|| m_bSkill_6ToPlayer)
		CollisionToSkill(TimeDelta);

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

void CDemon::CollisionToPlayer(_double TimeDelta)
{
	_float3 vPos =	m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_float3 vPlayerPos = m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION);
	_float fDistance =	CMathUtils::Distance(vPos, vPlayerPos);

	if (fabsf(fDistance) < 20.f)
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

void CDemon::CollisionToSkill(_double TimeDelta)
{
	// 스킬을 활용해서 콜리전을 어떻게 할 것인가에 대해서 고민해봐야할듯
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
			m_pPlayer->PassOutToMonster(m_fAttack);

		if (m_bSkill_2ToPlayer && m_pSkillHitDownColCom->Collision(pTargetCollider) == true)
			m_pPlayer->PassOutToMonster(m_fAttack, 0.1f);

		if(m_bSkill_3ToPlayer && m_pSkillKnockBackColCom->Collision(pTargetCollider) == true)
			m_pPlayer->PassOutToMonster(m_fAttack, 0.1f);

		if (m_bSkill_4ToPlayer && m_pSkillKnockBackColCom->Collision(pTargetCollider) == true)
			m_pPlayer->PassOutToMonster(m_fAttack, 0.2f);

		if (m_bSkill_5ToPlayer && m_pSkillHitDownColCom->Collision(pTargetCollider) == true)
			m_pPlayer->HitDownToMonster(m_fAttack, 0.1f , 0.5f , 0.7f);

		if (m_bSkill_6ToPlayer && m_pSkillHitDownColCom->Collision(pTargetCollider) == true)
			m_pPlayer->HitDownToMonster(m_fAttack);
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CDemon::Play_Skill(_double TimeDelta)
{
	m_SkillDelayTime += TimeDelta;

	// 어떻게 순서대로 넣지?
	// 2 -> 3 -> 5 -> 1 -> 6 -> 4 그 이후에는 또 랜덤

	if (m_SkillDelayTime > 10.0 &&
		!m_bSkill_1ToPlayer &&
		!m_bSkill_2ToPlayer &&
		!m_bSkill_3ToPlayer &&
		!m_bSkill_4ToPlayer &&
		!m_bSkill_5ToPlayer &&
		!m_bSkill_6ToPlayer &&
		!m_bSkillOrderFinish)
	{
		if (m_iSkillOrder == 0)
			m_bSkill_2ToPlayer = true;
		if (m_iSkillOrder == 1)
			m_bSkill_3ToPlayer = true;
		if (m_iSkillOrder == 2)
			m_bSkill_5ToPlayer = true;
		if (m_iSkillOrder == 3)
			m_bSkill_1ToPlayer = true;
		if (m_iSkillOrder == 4)
			m_bSkill_6ToPlayer = true;
		if (m_iSkillOrder == 5)
		{
			m_bSkill_4ToPlayer = true;
			m_bSkillOrderFinish = true;
		}
			m_iSkillOrder++;
	}
	else	if (m_SkillDelayTime > 10.0 &&
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

void CDemon::Level_Chap2Tick(_double TimeDelta)
{
	_vector vTreePos = m_pTree->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION);
	_float fTreeToPos = _float4::Distance(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION), vTreePos);
	
	if (static_cast<CTree*>(m_pTree)->Get_Hp() <= 0)
	{
		// 화면 흔들리면서 다음 씬으로 넘어가기
		m_bConversation = true;
		m_pModelCom->Set_AnimationIndex(DEMON_Idle_C);
		m_pTransformCom->LookAt(vTreePos);
		return;
	}

	if (fTreeToPos > 11.f)
	{
		m_pModelCom->Set_AnimationIndex(DEMON_Run_F);
		m_pTransformCom->ChaseAndLookAt(m_pTree->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION), TimeDelta, 0.1f, m_pNavigationCom);
	}
	else
	{
		m_bLevel2Finish = true;
		m_pModelCom->Set_AnimationIndex(DEMON_ATK_02);
		m_pTransformCom->LookAt(vTreePos);
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
	else if (!m_bFrontDamaged)
	{
		m_pModelCom->Reset_AnimPlayTime(DEMON_ADD_DMG_F);
	}

	if (AnimFinishChecker(DEMON_ADD_DMG_F, 0.9))
	{
		m_bFrontDamaged = false;
		m_bImpossibleDamaged = false;
	}

	///////////////////////////////////////

	if (m_bBackDamaged)
	{
		AdjustSetDamage();
		m_pModelCom->Set_AdditiveAnimIndex(DEMON_ADD_DMG_B);
		m_pModelCom->Play_AddtivieAnim(TimeDelta, 1.f);
	}
	else if (!m_bBackDamaged)
	{
		m_pModelCom->Reset_AnimPlayTime(DEMON_ADD_DMG_B);
	}

	if (AnimFinishChecker(DEMON_ADD_DMG_B, 0.9))
	{
		m_bBackDamaged = false;
		m_bImpossibleDamaged = false;
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
			m_pModelCom->Set_AnimationIndex(DEMON_Walk_F);
			m_pTransformCom->ChaseAndLookAt(vPlayerPos, TimeDelta, 0.1f, m_pNavigationCom);
		}
		else
		{
			m_pModelCom->Set_AnimationIndex(DEMON_Idle_C);
			m_pTransformCom->LookAt(vPlayerPos);
		}
		RELEASE_INSTANCE(CGameInstance);
		
		m_AttackDelayTime += TimeDelta;
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

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance)
			pGameInstance->Play_Sound(L"Flogas_Attack_2.mp3", 1.f, false);
		
		CRockMada::DESC rockmadaDesc;
			ZeroMemory(&rockmadaDesc, sizeof(CRockMada::DESC));
			_float4 vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
			_float4 vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
			// 내 앞으로
		rockmadaDesc.vDestination = vPos + vLook * 5.f;
		rockmadaDesc.vDestination.y += 0.6f;
		pGameInstance->Clone_GameObject(g_LEVEL, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_RockMada"), &rockmadaDesc);
		
		ZeroMemory(&rockmadaDesc, sizeof(CRockMada::DESC));
		// 내 앞으로
		rockmadaDesc.vDestination = vPos + vLook * 10.f;
		rockmadaDesc.vDestination.y += 0.6f;
		pGameInstance->Clone_GameObject(g_LEVEL, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_RockMada"), &rockmadaDesc);

		ZeroMemory(&rockmadaDesc, sizeof(CRockMada::DESC));
		// 내 앞으로
		rockmadaDesc.vDestination = vPos + vLook * 15.f;
		rockmadaDesc.vDestination.y += 0.6f;
		pGameInstance->Clone_GameObject(g_LEVEL, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_RockMada"), &rockmadaDesc);

		vLook = XMVector3TransformNormal(vLook, XMMatrixRotationY(XMConvertToRadians(30.f)));

		ZeroMemory(&rockmadaDesc, sizeof(CRockMada::DESC));
		// 내 앞으로
		rockmadaDesc.vDestination = vPos + vLook * 10.f;
		rockmadaDesc.vDestination.y += 0.6f;
		pGameInstance->Clone_GameObject(g_LEVEL, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_RockMada"), &rockmadaDesc);

		ZeroMemory(&rockmadaDesc, sizeof(CRockMada::DESC));
		// 내 앞으로
		rockmadaDesc.vDestination = vPos + vLook * 15.f;
		rockmadaDesc.vDestination.y += 0.6f;
		pGameInstance->Clone_GameObject(g_LEVEL, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_RockMada"), &rockmadaDesc);

		vLook = XMVector3TransformNormal(vLook, XMMatrixRotationY(XMConvertToRadians(-60.f)));

		ZeroMemory(&rockmadaDesc, sizeof(CRockMada::DESC));
		// 내 앞으로
		rockmadaDesc.vDestination = vPos + vLook * 10.f;
		rockmadaDesc.vDestination.y += 0.6f;
		pGameInstance->Clone_GameObject(g_LEVEL, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_RockMada"), &rockmadaDesc);

		ZeroMemory(&rockmadaDesc, sizeof(CRockMada::DESC));
		// 내 앞으로
		rockmadaDesc.vDestination = vPos + vLook * 15.f;
		rockmadaDesc.vDestination.y += 0.6f;
		pGameInstance->Clone_GameObject(g_LEVEL, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_RockMada"), &rockmadaDesc);

		RELEASE_INSTANCE(CGameInstance)
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
		m_fSkillHitDownRange = 10.f;

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance)
			pGameInstance->Play_Sound(L"Flogas_Skill_06.mp3", 1.f, false);
		
		for (int i = 0; i < 5; ++i)
		{
			CRockNorm04::DESC rockmadaDesc;
			ZeroMemory(&rockmadaDesc, sizeof(CRockNorm04::DESC));
			_float4 vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
			vLook = XMVector3TransformNormal(vLook, XMMatrixRotationY(XMConvertToRadians(-25.f + (i * 12.5f))));
			_float4 vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
			// 내 앞으로
			rockmadaDesc.vDestination = vPos + vLook* 5.f;
			rockmadaDesc.vDestination.y += 2.5f;
			rockmadaDesc.vDir = vLook;
			rockmadaDesc.iProperty = CRockNorm04::DEMON;
			pGameInstance->Clone_GameObject(g_LEVEL, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_RockNorm04"), &rockmadaDesc);
		}
	
		RELEASE_INSTANCE(CGameInstance)
	})
		.Tick([this](_double TimeDelta)
	{
		_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		
		static_cast<CEffect*>(CEffectManager::GetInstance()->Find_Effects(L"DEMON_SKILL_FLOOR"))->LinkObject(TimeDelta, _float4(vPos.x, vPos.y + 0.6f, vPos.z, 1.f));
		CEffectManager::GetInstance()->Get_Transform(L"DEMON_SKILL_FLOOR")->Set_Scaled(_float3(m_fSkillHitDownRange * 2.5f, m_fSkillHitDownRange * 2.5f, 1.f));
		CEffectManager::GetInstance()->Render_Effects(L"DEMON_SKILL_FLOOR", TimeDelta);
		
		static_cast<CEffect*>(CEffectManager::GetInstance()->Find_Effects(L"DEMON_SKILL2_FIRE0"))->LinkObject(TimeDelta, _float4(vPos.x, vPos.y + 2.f, vPos.z, 1.f));
		CEffectManager::GetInstance()->Render_Effects(L"DEMON_SKILL2_FIRE0", TimeDelta);

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
		m_fSkill3EffectTime = 0.0;
		static_cast<CDemonSkillCircular*>(CEffectManager::GetInstance()->Find_Effects(L"DEMON_SKILL_CIRCULAR"))->Set_Linking(true);

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance)
			pGameInstance->Play_Sound(L"Balrog_Skill_07_1.mp3", 1.f, false);

		CParticle::DESC desc;
		ZeroMemory(&desc, sizeof(CParticle::DESC));

		desc.fShowTime = 2.f;
		desc.iProperty = CParticle::COCHLEA;
		desc.fSizeUV_X = 1.f / 6.f;
		desc.fSizeUV_Y = 1.f / 5.f;
		desc.iSpriteCount_X = 6;
		desc.iSpriteCount_Y = 5;
		desc.pTextureTag = L"FX_fireaura_001_TEX_KJS";
		desc.ViBufferDesc.fMinSpeed = 0.1f;
		desc.ViBufferDesc.fMaxSpeed = 1.f;
		desc.ViBufferDesc.iNumSprite = 30;
		desc.ViBufferDesc.vMaxSize = _float2(1.024f * 1.2f, 1.024f * 1.2f);
		desc.ViBufferDesc.vMinSize = _float2(1.024f * 0.7f, 1.024f * 0.7f);
		desc.ViBufferDesc.fMinTime = 0.1f;
		desc.ViBufferDesc.fMaxTime = 0.5f;
		desc.ViBufferDesc.fRange = 10.f;
		desc.ViBufferDesc.fRangeOffset = 6.f;
		desc.ViBufferDesc.fMinSpreadSizeOffset = 0.1f;
		desc.ViBufferDesc.fMaxSpreadSizeOffset = 2.f;
		desc.ViBufferDesc.iNumInstances = 0;
		desc.ViBufferDesc.iNumSpreadInstances = 500;
		pGameInstance->Clone_GameObject(g_LEVEL, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Particle"), &desc, &m_pSkill3Particle);

		RELEASE_INSTANCE(CGameInstance)
	})
		.Tick([this](_double TimeDelta) 
	{
		m_bRealSkill = true;
		_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_float3 vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		CGameObject* pSkillEffect = CEffectManager::GetInstance()->Find_Effects(L"DEMON_SKILL_CIRCULAR");
		m_fSkill3EffectTime += static_cast<float>(TimeDelta) * 0.25f;
		static_cast<CDemonSkillCircular*>(pSkillEffect)->Set_Amount(m_fSkill3EffectTime);
		CEffectManager::GetInstance()->Render_Effects(L"DEMON_SKILL_CIRCULAR", TimeDelta);
		if (m_fSkill3EffectTime >= 0.4f)
			m_fSkill3EffectTime = 0.4f;

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance)
			pGameInstance->Play_Sound(L"Balrog_Skill_06_1.mp3", 1.f, false, SOUND_BOSS);
		RELEASE_INSTANCE(CGameInstance)

		vPos += vLook;
		vPos.y += 5.f;
		m_pSkill3Particle->Get_TransformCom()->Set_State(CTransform::STATE_TRANSLATION, vPos);

		if (AnimIntervalChecker(DEMON_SK_Firing_03, 0.2, 0.9))
		{
			m_fRotationY += static_cast<_float>(TimeDelta) * 1.2f;
			_mat pivotMat = XMMatrixScaling(0.1f, 0.1f, 0.5f) *
				XMMatrixRotationX(XMConvertToRadians(-140.f)) *
				XMMatrixRotationY(XMConvertToRadians(m_fRotationY * 70.f)) *
				XMMatrixRotationZ(XMConvertToRadians(0.f)) *
				XMMatrixTranslation(0.f, 5.5f, 0.f);
			static_cast<CEffect_Mesh*>(CEffectManager::GetInstance()->Find_Effects(L"DEMON_SKILL3_LINE"))->Set_PivotMatrix(pivotMat);
			CEffectManager::GetInstance()->Render_Effects(L"DEMON_SKILL3_LINE", TimeDelta);
		}

		if (m_fRotationY >= 1.f)
			m_fRotationY = 1.f;
	})
		.OnExit([this]()
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance)
			pGameInstance->Play_Sound(L"Balrog_Skill_06_2.mp3", 1.f);
		RELEASE_INSTANCE(CGameInstance)

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

		Safe_Release(m_pSkill3Particle);
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
		m_fOBBCX = 1.f;
		m_fOBBCY = 1.f;
		m_fOBBCZ = 0.f;
		m_fRotationX = 0.f;
		m_fRotationY = 0.f;
		m_fRotationZ = 0.f;

		m_pModelCom->Reset_AnimPlayTime(DEMON_SK_Firing_04);
		m_pModelCom->Set_AnimationIndex(DEMON_SK_Firing_04);
		static_cast<CEffect_Rect*>(CEffectManager::GetInstance()->Find_Effects(L"DEMON_SKILL_Straight"))->Set_Linking(true);
		_float4 vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		static_cast<CEffect*>(CEffectManager::GetInstance()->Find_Effects(L"DEMON_SKILL2_FIRE0"))->LinkObject(0.0, _float4(vPos.x, vPos.y + 4.f, vPos.z, 1.f));
		m_fSkill5EffectTime = 0.f;
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance)
		CRockNorm04::DESC rockmadaDesc;
		ZeroMemory(&rockmadaDesc, sizeof(CRockNorm04::DESC));
		_float4 vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		rockmadaDesc.vDestination = vPos + vLook;
		rockmadaDesc.vDestination.y += 5.f;
		rockmadaDesc.vDir = vLook;
		rockmadaDesc.iProperty = CRockNorm04::DEMON_SKILL4;
		pGameInstance->Clone_GameObject(g_LEVEL, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_RockNorm04"), &rockmadaDesc, &m_pSkill4Rock);
		pGameInstance->Play_Sound(L"Balrog_Skill_07_1.mp3", 1.f, false);
		RELEASE_INSTANCE(CGameInstance)
	})
		.Tick([this](_double TimeDelta) 
	{
		m_fSkill5EffectTime += static_cast<float>(TimeDelta) * 10.f;
		if (m_fSkill5EffectTime >= 20.f)
			m_fSkill5EffectTime = 20.f;
		_matrix pivotMat = XMMatrixScaling(m_fSkill5EffectTime * 2.f, 4.f, 1.f) *
			XMMatrixRotationX(XMConvertToRadians(90.f)) *
			XMMatrixRotationY(XMConvertToRadians(-90.f)) *
			XMMatrixRotationZ(XMConvertToRadians(0.f)) *
			XMMatrixTranslation(0.f, 0.6f, 0.f);
		static_cast<CEffect_Rect*>(CEffectManager::GetInstance()->Find_Effects(L"DEMON_SKILL_Straight"))->Set_PivotMatrix(pivotMat);
		
		CEffectManager::GetInstance()->Render_Effects(L"DEMON_SKILL_Straight", TimeDelta);
		CEffectManager::GetInstance()->Render_Effects(L"DEMON_SKILL2_FIRE0", TimeDelta);
		static_cast<CEffect_Rect*>(CEffectManager::GetInstance()->Find_Effects(L"DEMON_SKILL2_FIRE0"))->Set_Size(m_fSkill5EffectTime * 0.7f);

		if (AnimIntervalChecker(DEMON_SK_Firing_04, 0.7, 0.9))
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance)
				pGameInstance->Play_Sound(L"Balrog_Skill_07_2.mp3", 1.f, false, SOUND_BOSS);
			RELEASE_INSTANCE(CGameInstance)

			CEffectManager::GetInstance()->Get_Transform(L"DEMON_SKILL2_FIRE0")->Go_Direction(TimeDelta * 7.f, m_pTransformCom->Get_State(CTransform::STATE_LOOK));
			m_bRealSkill = true;
			m_fOBBZ = 7.f;
			m_fOBBCZ = 7.f;
		}
		else
		{
			m_bRealSkill = false;
			m_pSkill4Rock->Get_TransformCom()->Set_Scaled(_float3(m_fSkill5EffectTime * 0.2f, m_fSkill5EffectTime* 0.2f, m_fSkill5EffectTime* 0.2f));
		}
	})
		.OnExit([this]()
	{
		m_fOBBZ = 0.f;
		m_fOBBCZ = 0.f;
		m_SkillDelayTime = 0.0;
		m_bSkill_4ToPlayer = false;
		Safe_Release(m_pSkill4Rock);
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
		m_fSkillHitDownRange = 8.f;

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance)
			pGameInstance->Play_Sound(L"Flogas_Skill_05_3.mp3", 1.f, false);
		RELEASE_INSTANCE(CGameInstance)
	})
		.Tick([this](_double TimeDelta) 
	{
		_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

		static_cast<CEffect*>(CEffectManager::GetInstance()->Find_Effects(L"DEMON_SKILL_FLOOR"))->LinkObject(TimeDelta, _float4(vPos.x, vPos.y + 0.6f, vPos.z, 1.f));
		CEffectManager::GetInstance()->Get_Transform(L"DEMON_SKILL_FLOOR")->Set_Scaled(_float3(m_fSkillHitDownRange * 2.5f, m_fSkillHitDownRange * 2.5f, 1.f));
		CEffectManager::GetInstance()->Render_Effects(L"DEMON_SKILL_FLOOR", TimeDelta);

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

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance)
			pGameInstance->Play_Sound(L"Balrog_Skill_02_2.mp3", 1.f, false);
		RELEASE_INSTANCE(CGameInstance)
	})
		.Tick([this](_double TimeDelta) 
	{
		_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		static_cast<CEffect*>(CEffectManager::GetInstance()->Find_Effects(L"DEMON_SKILL_FLOOR"))->LinkObject(TimeDelta, _float4(vPos.x, vPos.y + 0.6f, vPos.z, 1.f));
		m_fSkillHitDownRange += static_cast<_float>(TimeDelta) * 5.f;
		CEffectManager::GetInstance()->Get_Transform(L"DEMON_SKILL_FLOOR")->Set_Scaled(_float3(m_fSkillHitDownRange * 2.5f, m_fSkillHitDownRange * 2.5f, 1.f));
		m_bImpossibleDamaged = true;
		m_bImpossibleSkillDamaged = true;
		m_pModelCom->Set_AnimationIndex(DEMON_SK_Firing_06);
		CEffectManager::GetInstance()->Render_Effects(L"DEMON_SKILL_FLOOR", TimeDelta);
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
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance)
			pGameInstance->Play_Sound(L"Balrog_Skill_02_3.mp3", 1.f, false);
		RELEASE_INSTANCE(CGameInstance)
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
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance)
				pGameInstance->Play_Sound(L"Balrog_Attack_2.mp3", 1.f);
			RELEASE_INSTANCE(CGameInstance)
		}
		else if (m_iRandAttack == 1)
		{
			m_pModelCom->Reset_AnimPlayTime(DEMON_ATK_02);
			m_pModelCom->Set_AnimationIndex(DEMON_ATK_02);
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance)
				pGameInstance->Play_Sound(L"Balrog_Attack_1.mp3", 1.f);
			RELEASE_INSTANCE(CGameInstance)
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

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance)
			pGameInstance->Play_Sound(L"Balrog_Die_1.mp3", 1.f);
			pGameInstance->Play_Sound(L"Balrog_Die_2.mp3", 1.f);
		RELEASE_INSTANCE(CGameInstance)
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
		m_fDissolveAmount += static_cast<_float>(TimeDelta);
		
		if (m_dDeadTime > 3.0)
			m_bDead = true;
	})

		.Build();
}

void CDemon::SetUp_UI()
{
	CConversationUI::CONVERSATIONFONT conversationDesc;

	wcscpy_s(conversationDesc.szConversation, MAX_PATH, L"");
	wcscpy_s(conversationDesc.szFontName, MAX_PATH, L"");
	conversationDesc.vColor = _float4(0.1f, 0.1f, 0.1f, 1.f);

	conversationDesc.fX = 375.f;
	conversationDesc.fY = 800.f;
	conversationDesc.fSizeX = 1.f;
	conversationDesc.fSizeY = 1.f;
	conversationDesc.bTextureOn = true;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CGameObject* pUI = nullptr;
	pUI = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_ConversationUI"), &conversationDesc);

	if (nullptr != pUI)
		m_UI.push_back(pUI);

	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CDemon::SetUp_Components()
{
	if (FAILED(__super::SetUp_Components()))
		return E_FAIL;

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
	ColliderDesc.vSize = _float3(1.f, 4.f, 1.f);

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
	if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	if (Get_CamDistance() > 30.f)
		m_vRimColor = _float4(0.f, 0.f, 0.f, 0.f);
	
	if (FAILED(m_pShaderCom->Set_RawValue("g_vRimColor", &m_vRimColor, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("fDissolveAmount", &m_fDissolveAmount, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("fFringeAmount", &m_fFringeAmount, sizeof(_float))))
		return E_FAIL;

	_bool bTRUE = true;

	if (FAILED(m_pShaderCom->Set_RawValue("g_bDissolve", &bTRUE, sizeof(_bool))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CDemon::SetUP_ShadowShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_LIGHTVIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

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
	for (auto pUI : m_UI)
		Safe_Release(pUI);
	m_UI.clear();
}
