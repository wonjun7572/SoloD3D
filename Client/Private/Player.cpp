#include "stdafx.h"
#include "..\public\Player.h"
#include "GameInstance.h"
#include "Weapon.h"
#include "Bone.h"
#include "Navigation.h"
#include "Animation.h"
#include "PlayerCamera.h"
#include "Effect_Rect.h"
#include "Upper.h"
#include "Lower.h"
#include "Glove.h"
#include "Monster.h"
#include "FSMComponent.h"

#include "Trail.h"

CPlayer::CPlayer(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CPlayer::CPlayer(const CPlayer & rhs)
	: CGameObject(rhs)
{

}

HRESULT CPlayer::Init_Prototype()
{
	if (FAILED(__super::Init_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Init(void * pArg)
{
	CGameObject::GAMEOBJECTDESC			GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof GameObjectDesc);

	GameObjectDesc.TransformDesc.fSpeedPerSec = 7.0f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Init(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	if (FAILED(SetUp_Parts()))
		return E_FAIL;

	m_strObjName = L"Player";

	m_PartSize = static_cast<_uint>(m_PlayerParts.size());

	if(g_LEVEL == LEVEL_CHAP1)
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(45.f, 0.f, 75.f, 1.f));
	else if(g_LEVEL == LEVEL_CHAP2)
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(320.f, 0.f, 30.f, 1.f));
	else if (g_LEVEL == LEVEL_CHAP3)
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(65.f, 0.f, 10.f, 1.f));
	
	/* ~~~~ 공격력 체력 수치!!! ~~~~*/
	m_iHp = 100;
	m_iAttack = 20;
	m_iDefence = 100;

	m_pNormalAtkTrail->Get_TransformCom()->Set_State(CTransform::STATE_TRANSLATION, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

	SetUp_FSM();
	
	return S_OK;
}

void CPlayer::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	Movement(TimeDelta);

	m_pFSM->Tick(TimeDelta);
	
	for (_uint i = PART_UPPER; i < PART_END; ++i)
	{
		m_PlayerParts[i]->Tick(TimeDelta);
		static_cast<CParts*>(m_PlayerParts[i])->LinkPlayer(m_pTransformCom);
	}

	m_pModelCom[m_eModelState]->Play_Animation(TimeDelta);

	AdditiveAnim(TimeDelta);

	m_PlayerParts[PART_WEAPON]->Tick(TimeDelta);
	
	LinkObject(TimeDelta);
}

void CPlayer::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	for (_uint i = 0; i < m_PartSize; ++i)
		m_PlayerParts[i]->Late_Tick(TimeDelta);

	for (_uint i = 0; i < COLLTYPE_END; ++i)
		m_pColliderCom[i]->Update(m_pTransformCom->Get_WorldMatrix());

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

	// 모델 원래 상태로 돌아가기

	if (m_eModelState == MODEL_A)
		m_dModelATime += TimeDelta;

	if (m_dModelATime > 20.0)
	{
		m_dModelATime += TimeDelta;
		ChangeModel(CPlayer::MODEL_NOMAL);
		if (m_dModelATime > 10.2)
			static_cast<CParts*>(m_PlayerParts[PART_BOOTS])->ChangeModel(CParts::MODEL_NOMAL);
		if (m_dModelATime > 10.4)
			static_cast<CParts*>(m_PlayerParts[PART_BELT])->ChangeModel(CParts::MODEL_NOMAL);
		if (m_dModelATime > 10.6)
			static_cast<CParts*>(m_PlayerParts[PART_LOWER])->ChangeModel(CParts::MODEL_NOMAL);
		if (m_dModelATime > 10.8)
			static_cast<CParts*>(m_PlayerParts[PART_GLOVE])->ChangeModel(CParts::MODEL_NOMAL);
		if (m_dModelATime > 11.0)
			static_cast<CParts*>(m_PlayerParts[PART_UPPER])->ChangeModel(CParts::MODEL_NOMAL);
		if (m_dModelATime > 11.2)
			static_cast<CParts*>(m_PlayerParts[PART_SHOULDER])->ChangeModel(CParts::MODEL_NOMAL);
		if (m_dModelATime > 11.4)
		{
			static_cast<CParts*>(m_PlayerParts[PART_HELMET])->ChangeModel(CParts::MODEL_NOMAL);
			m_dModelATime = 0.0;
		}
	}
}

HRESULT CPlayer::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom[m_eModelState]->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		/* 이 모델을 그리기위한 셰이더에 머테리얼 텍스쳐를 전달하낟. */
		m_pModelCom[m_eModelState]->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");

		m_pModelCom[m_eModelState]->Render(m_pShaderCom, i, 2, "g_BoneMatrices");
	}

	for (_uint i = CPlayer::PART_UPPER; i < CPlayer::PART_END; ++i)
	{
		if (FAILED(static_cast<CParts*>(m_PlayerParts[i])->SetUp_ShaderResources()))
			return E_FAIL;

		if (FAILED(static_cast<CParts*>(m_PlayerParts[i])->PartsRender(2)))
			return E_FAIL;
	}

	// 트레일 매트릭스 수정해줘야함

	if (m_fNormalAtk1TrailMove <= 1.f && m_fNormalAtk1TrailMove >= -1.f)
	{
		_matrix mat = XMMatrixScaling(1.3f, 1.3f, 1.3f)
			* XMMatrixRotationX(XMConvertToRadians(0.f))
			* XMMatrixRotationY(XMConvertToRadians(0.f))
			* XMMatrixRotationZ(XMConvertToRadians(-20.f))
			* XMMatrixTranslation(0.f, 0.4f, 0.f);

		mat *= m_pTransformCom->Get_WorldMatrix();

		_float4x4 mat4x4;
		XMStoreFloat4x4(&mat4x4, mat);

		if (FAILED(static_cast<CTrail*>(m_pNormalAtkTrail)->Trail1_Render(m_fNormalAtk1TrailMove, mat4x4)))
			return E_FAIL;
	}
	
	if (m_fNormalAtk2TrailMove <= 1.f && m_fNormalAtk2TrailMove >= -1.f)
	{
		_matrix mat = XMMatrixScaling(1.3f, 1.3f, 1.3f)
			* XMMatrixRotationX(XMConvertToRadians(0.f))
			* XMMatrixRotationY(XMConvertToRadians(0.f))
			* XMMatrixRotationZ(XMConvertToRadians(0.f))
			* XMMatrixTranslation(0.f, 0.4f, 0.f);

		mat *= m_pTransformCom->Get_WorldMatrix();

		_float4x4 mat4x4;
		XMStoreFloat4x4(&mat4x4, mat);

		if (FAILED(static_cast<CTrail*>(m_pNormalAtkTrail)->Trail2_Render(m_fNormalAtk2TrailMove, mat4x4)))
			return E_FAIL;
	}

	if (m_fNormalAtk3TrailMove <= 1.f && m_fNormalAtk3TrailMove >= -1.f)
	{
		_matrix mat = XMMatrixScaling(1.3f, 1.3f, 1.3f)
			* XMMatrixRotationX(XMConvertToRadians(0.f))
			* XMMatrixRotationY(XMConvertToRadians(0.f))
			* XMMatrixRotationZ(XMConvertToRadians(-20.f))
			* XMMatrixTranslation(0.f, 0.4f, 0.f);

		mat *= m_pTransformCom->Get_WorldMatrix();

		_float4x4 mat4x4;
		XMStoreFloat4x4(&mat4x4, mat);

		if (FAILED(static_cast<CTrail*>(m_pNormalAtkTrail)->Trail3_Render(m_fNormalAtk3TrailMove, mat4x4)))
			return E_FAIL;
	}

#ifdef _DEBUG
	for (_uint i = 0; i < COLLTYPE_END; ++i)
	{
		if (nullptr != m_pColliderCom[i])
			m_pColliderCom[i]->Render();
	}
	if(m_pNavigationCom)
		m_pNavigationCom->Render();
#endif

	return S_OK;
}

void CPlayer::SetUp_FSM()
{
	m_pFSM = CFSMComponentBuilder()
		.InitState("Idle")
		.AddState("Idle")
		.OnStart([this]()
	{
		m_bCamTurn = true;
		Reset_Anim(CPlayer::PLAYER_IDLE_READY);
	})
		.Tick(this, &CPlayer::Idle_Tick)
		.OnExit([this]()
	{
		m_bCamTurn = false;
	})
		.AddTransition("Idle to Move", "Move")
		.Predicator([this]()
	{
		return m_bMove && !m_bAction;
	})
		.AddTransition("Idle to Attack_1", "Attack_1")
		.Predicator([this]()
	{
		return m_bAction && !m_bMove && m_bNormalAttack_1;
	})
		.AddTransition("Idle to Skill_1", "Skill_1")
		.Predicator([this]()
	{
		return m_bAction && !m_bMove && m_bSK01;
	})
		.AddTransition("Idle to Skill_2", "Skill_2")
		.Predicator([this]()
	{
		return m_bAction && !m_bMove && m_bSK02;
	})
		.AddTransition("Idle to Skill_3", "Skill_3")
		.Predicator([this]()
	{
		return m_bAction && !m_bMove && m_bSK03;
	})
		.AddTransition("Idle to Skill_4_Charging", "Skill_4_Charging")
		.Predicator([this]()
	{
		return m_bAction && !m_bMove && m_bSK04_Charging;
	})
		.AddTransition("Idle to Skill_5", "Skill_5")
		.Predicator([this]()
	{
		return m_bAction && !m_bMove && m_bSK05;
	})
		.AddTransition("Idle to Skill_6", "Skill_6")
		.Predicator([this]()
	{
		return m_bAction && !m_bMove && m_bSK06;
	})
		.AddTransition("Idle to Groggy" , "Groggy")
		.Predicator([this]() 
	{
		return m_bGroggy;
	})
		.AddTransition("Idle to HitDown", "HitDown")
		.Predicator([this]()
	{
		return m_bHitDown;
	})
		.AddTransition("Idle to JumpUp", "JumpUp")
		.Predicator([this]()
	{
		return m_bJump;
	})

		// Groggy
		.AddState("Groggy")
		.OnStart([this]() 
	{
		m_bCamTurn = true;
		Reset_Anim(PLAYER_PASSOUT);
		Set_Anim(PLAYER_PASSOUT);
	})
		.Tick([this](_double TimeDelta) 
	{
		if(AnimIntervalChecker(PLAYER_PASSOUT,0, 0.4))
			m_pTransformCom->Go_Backward(TimeDelta * m_fKnockBackPower, m_pNavigationCom);
	})
		.OnExit([this]() 
	{
		m_bGroggy = false;
		m_bCamTurn = false; 
		Reset_Action();
	})
		.AddTransition("Groggy to Idle" , "Idle")
		.Predicator([this]() 
	{	
		return AnimFinishChecker(PLAYER_PASSOUT);
	})
		
		// HitDown
		.AddState("HitDown")
		.OnStart([this]()
	{
		m_bJump = true;
		m_bCamTurn = true;

		Reset_Anim(PLAYER_DOWN);
		Set_Anim(PLAYER_DOWN);
	})
		.Tick([this](_double TimeDelta) 
	{
		m_pTransformCom->Go_Backward(TimeDelta * m_fKnockBackPower, m_pNavigationCom);
	})
		.AddTransition("HitDown to HitDownLoop", "HitDownLoop")
		.Predicator([this]()
	{
		return AnimFinishChecker(PLAYER_DOWN, 0.7);
	})

		.AddState("HitDownLoop")
		.OnStart([this]()
	{
		m_HitDownDelayTime = 0.0;
		Reset_Anim(PLAYER_DOWN_LOOP);
		Set_Anim(PLAYER_DOWN_LOOP);
	})
		.Tick([this](_double TimeDelta)
	{
		m_HitDownDelayTime += TimeDelta;
		
		if(m_HitDownDelayTime <= m_HitDownDurationTime)
			m_pTransformCom->Go_Backward(TimeDelta * m_fKnockBackPower, m_pNavigationCom);

		Set_Anim(PLAYER_DOWN_LOOP);
	})
		.AddTransition("HitDownLoop to Getup", "Getup")
		.Predicator([this]()
	{
		return m_HitDownDelayTime >= 3.0;
	})

		.AddState("Getup")
		.OnStart([this]()
	{
		Reset_Anim(PLAYER_GETUP);
		Set_Anim(PLAYER_GETUP);
	})
		.OnExit([this]()
	{
		m_bCamTurn = false;
		m_bHitDown = false;
		Reset_Action();
	})
		.AddTransition("Getup to Idle", "Idle")
		.Predicator([this]()
	{
		return  AnimFinishChecker(PLAYER_GETUP, 0.85);
	})
		
		/* 움직임을 위한 */
		.AddState("Move")
		.AddTransition("Move to Walk", "Walk")
		.Predicator([this]() 
	{
		return m_bWalking && m_bMove;
	})	
		.AddTransition("Move to Run", "Run")
		.Predicator([this]()
	{
		return m_bRunning && m_bMove;
	})
		.AddTransition("Move to Idle", "Idle")
		.Predicator([this]()
	{
		return !m_bMove;
	})
		.AddTransition("Move to Groggy", "Groggy")
		.Predicator([this]()
	{
		return m_bGroggy;
	})
		.AddTransition("Move to HitDown", "HitDown")
		.Predicator([this]()
	{
		return m_bHitDown;
	})

		/* For. walk */
		.AddState("Walk")
		.Tick(this, &CPlayer::Walk_Tick)
		.AddTransition("Walk to Move", "Move")
		.Predicator([this]()
	{
		return !m_bMove;
	})
		.AddTransition("Walk to Run", "Run")
		.Predicator([this]()
	{
		return m_bRunning && m_bMove;
	})

		/* For. run */
		.AddState("Run")
		.Tick(this, &CPlayer::Run_Tick)
		.AddTransition("Run to Move", "Move")
		.Predicator([this]()
	{
		return !m_bMove;
	})
		.AddTransition("Run to Walk", "Walk")
		.Predicator([this]()
	{
		return m_bWalking && m_bMove;
	})
		.AddTransition("Run to V_DEF", "V_DEF")
		.Predicator([this]()
	{
		return m_bAction && m_bV_DEF;
	})
		.AddTransition("Run to Groggy", "Groggy")
		.Predicator([this]()
	{
		return m_bGroggy;
	})
		.AddTransition("Run to HitDown", "HitDown")
		.Predicator([this]()
	{
		return m_bHitDown;
	})
		.AddTransition("Run to F_RunJumpUp", "F_RunJumpUp")
		.Predicator([this]()
	{
		return m_bJump && m_eState == PLAYER_FM;
	})
		.AddTransition("Run to B_RunJumpUp", "B_RunJumpUp")
		.Predicator([this]()
	{
		return m_bJump && m_eState == PLAYER_BM;
	})
		

		/* For. Jump*/
		.AddState("JumpUp")
		.OnStart([this]()
	{
		m_fJumpPower = 0.5f;
		Reset_Anim(PLAYER_JUMP_UP);
		Set_Anim(PLAYER_JUMP_UP);
	})
		.AddTransition("JumpUp to JumpLand", "JumpLand")
		.Predicator([this]()
	{
		return AnimFinishChecker(PLAYER_JUMP_UP, 0.2);
	})

		.AddState("JumpLand")
		.OnStart([this]()
	{
		Reset_Anim(PLAYER_JUMP_LAND);
		Set_Anim(PLAYER_JUMP_LAND);
	})
		.AddTransition("JumpLand to Move", "Move")
		.Predicator([this]()
	{
		return AnimFinishChecker(PLAYER_JUMP_LAND, 0.8);
	})

		.AddState("F_RunJumpUp")
		.OnStart([this]() 
	{
		m_fJumpPower = 0.5f;
		Reset_Anim(PLAYER_RUN_JUMP_UP_F);
		Set_Anim(PLAYER_RUN_JUMP_UP_F);
	})
		.AddTransition("F_RunJumpUp to F_RunJumpLand" , "F_RunJumpLand")
		.Predicator([this]() 
	{
		return AnimFinishChecker(PLAYER_RUN_JUMP_UP_F, 0.2);
	})

		.AddState("F_RunJumpLand")
		.OnStart([this]()
	{
		Reset_Anim(PLAYER_RUN_JUMP_LAND_F);
		Set_Anim(PLAYER_RUN_JUMP_LAND_F);
	})
		.AddTransition("F_RunJumpLand to Move", "Move")
		.Predicator([this]()
	{
		return AnimFinishChecker(PLAYER_RUN_JUMP_LAND_F, 0.8);
	})

		.AddState("B_RunJumpUp")
		.OnStart([this]()
	{
		m_fJumpPower = 0.5f;
		Reset_Anim(PLAYER_RUN_JUMP_UP_B);
		Set_Anim(PLAYER_RUN_JUMP_UP_B);
	})
		.AddTransition("B_RunJumpUp to B_RunJumpLand", "B_RunJumpLand")
		.Predicator([this]()
	{
		return AnimFinishChecker(PLAYER_RUN_JUMP_UP_B, 0.2);
	})

		.AddState("B_RunJumpLand")
		.OnStart([this]()
	{
		Reset_Anim(PLAYER_RUN_JUMP_LAND_B);
		Set_Anim(PLAYER_RUN_JUMP_LAND_B);
	})
		.AddTransition("B_RunJumpLand to Move", "Move")
		.Predicator([this]()
	{
		return AnimFinishChecker(PLAYER_RUN_JUMP_LAND_B, 0.8);
	})

		/* For. V_DEF */
		.AddState("V_DEF")
		.OnStart([this]()
	{
		Reset_Anim(PLAYER_V_DEF);
		Set_Anim(PLAYER_V_DEF);
	})
		.Tick([this](_double TimeDelta) 
	{
		m_pTransformCom->Go_Straight(TimeDelta, m_pNavigationCom);
	})
		.AddTransition("V_DEF to Run", "Run")
		.Predicator([this]()
	{
		return !m_bAction && !m_bV_DEF;
	})
		
		/* For. Attack */
		.AddState("Attack_1")
		.OnStart([this]()
	{
		Reset_Anim(PLAYER_ATK_01);
		Set_Anim(PLAYER_ATK_01);
	})
		.Tick([this](_double TimeDelta)
	{
		if (AnimIntervalChecker(PLAYER_ATK_01, 0.1, 1.0))
			m_fNormalAtk1TrailMove += static_cast<float>(TimeDelta) * 2.5f;

		if (AnimIntervalChecker(PLAYER_ATK_01, 0.1, 0.3))
			MonsterNormalAttack(true);
		else
			MonsterNormalAttack(false);
	})
		.OnExit([this]()
	{
		MonsterNormalAttack(false);
		m_fNormalAtk1TrailMove = -1.001f;
	})
		.AddTransition("Attack_1 to Idle", "Idle")
		.Predicator([this]()
	{
		return !m_bAction && CheckFinish_Attack1();
	}) 
			.AddTransition("Attack_1 to Attack_2", "Attack_2")
		.Predicator([this]()
	{
		return m_bAction &&	m_bNormalAttack_2;
	}).
		AddTransition("Attack_1 to Groggy", "Groggy")
		.Predicator([this]()
	{
		return m_bGroggy;
	})
		.AddTransition("Attack_1 to HitDown", "HitDown")
		.Predicator([this]()
	{
		return m_bHitDown;
	})
		
		.AddState("Attack_2")
		.OnStart([this]()
	{
		Reset_Anim(PLAYER_ATK_02);
		Set_Anim(PLAYER_ATK_02);
	})
		.Tick([this](_double TimeDelta)
	{
		if (AnimIntervalChecker(PLAYER_ATK_02, 0.1, 1.0))
			m_fNormalAtk2TrailMove += static_cast<float>(TimeDelta) * 2.5f;

		if (AnimIntervalChecker(PLAYER_ATK_02, 0.1, 0.3))
			MonsterNormalAttack(true);
		else
			MonsterNormalAttack(false);
	})
		.OnExit([this]()
	{
		MonsterNormalAttack(false);
		m_fNormalAtk2TrailMove = -1.001f;
	})
		.AddTransition("Attack_2 to Idle", "Idle")
		.Predicator([this]()
	{
		return !m_bAction && CheckFinish_Attack2();
	})
		.AddTransition("Attack_2 to Attack_3", "Attack_3")
		.Predicator([this]()
	{
		return m_bAction &&	m_bNormalAttack_3;
	})
		.AddTransition("Attack_2 to Groggy", "Groggy")
		.Predicator([this]()
	{
		return m_bGroggy;
	})
		.AddTransition("Attack_2 to HitDown", "HitDown")
		.Predicator([this]()
	{
		return m_bHitDown;
	})


		.AddState("Attack_3")
		.OnStart([this]()
	{
		Reset_Anim(PLAYER_ATK_03);
		Set_Anim(PLAYER_ATK_03);
	})
		.Tick([this](_double TimeDelta)
	{
		if (AnimIntervalChecker(PLAYER_ATK_03, 0.1, 1.0))
			m_fNormalAtk3TrailMove += static_cast<float>(TimeDelta) * 2.5f;

		if (AnimIntervalChecker(PLAYER_ATK_03, 0.1, 0.3))
			MonsterNormalAttack(true);
		else
			MonsterNormalAttack(false);
	})
		.OnExit([this]()
	{
		MonsterNormalAttack(false);
		m_fNormalAtk3TrailMove = -1.001f;
	})
		.AddTransition("Attack_3 to Idle", "Idle")
		.Predicator([this]()
	{
		return !m_bAction && CheckFinish_Attack3();
	})
		.AddTransition("Attack_3 to Attack_1", "Attack_1")
		.Predicator([this]()
	{
		return m_bAction &&	m_bNormalAttack_1;
	})
		.AddTransition("Attack_3 to Groggy", "Groggy")
		.Predicator([this]()
	{
		return m_bGroggy;
	})
		.AddTransition("Attack_3 to HitDown", "HitDown")
		.Predicator([this]()
	{
		return m_bHitDown;
	})


		/*For. Skill*/

		// KEY E
		.AddState("Skill_1")
		.OnStart([this]()
	{
		Get_WeaponCollider()->FixedSphereSize(1.f, -0.15f, 0.1f, 1.f);
		Reset_Anim(PLAYER_SK24);
		Set_Anim(PLAYER_SK24);
	})
		.Tick([this](_double TimeDelta)
	{
		if (AnimIntervalChecker(PLAYER_SK24, 0.1, 0.9))
			MonsterNormalAttack(true);
		else
			MonsterNormalAttack(false);
	})
		.OnExit([this]() 
	{
		MonsterNormalAttack(false);
		Get_WeaponCollider()->FixedSphereSize(1.f, -0.15f, 0.1f, 0.65f);
	})
		.AddTransition("Skill_1 to Idle", "Idle")
		.Predicator([this]()
	{
		return !m_bAction && !m_bSK01 && CheckFinish_Skill1();
	})
		.AddTransition("Skill_1 to Groggy", "Groggy")
		.Predicator([this]()
	{
		return m_bGroggy;
	})
		.AddTransition("Skill_1 to HitDown", "HitDown")
		.Predicator([this]()
	{
		return m_bHitDown;
	})


		// KEY Q
		.AddState("Skill_2")
		.OnStart([this]()
	{
		Get_WeaponCollider()->FixedSphereSize(1.f, -0.15f, 0.1f, 5.f);
		Reset_Anim(PLAYER_SK09);
		Set_Anim(PLAYER_SK09);
	})
		.Tick([this](_double TimeDelta)
	{
		if (!AnimFinishChecker(PLAYER_SK09, 0.3))
			m_pTransformCom->Go_Straight(TimeDelta * 2.f, m_pNavigationCom);

		if (AnimIntervalChecker(PLAYER_SK09, 0.2, 0.5))
			MonsterSkill02(true);
		else
			MonsterSkill02(false);
	})
		.OnExit([this]()
	{
		Get_WeaponCollider()->FixedSphereSize(1.f, -0.15f, 0.1f, 0.5f);
		MonsterSkill02(false);
	})
		.AddTransition("Skill_2 to Idle", "Idle")
		.Predicator([this]()
	{
		return !m_bAction && !m_bSK02 && CheckFinish_Skill2();
	})
		.AddTransition("Skill_2 to Groggy", "Groggy")
		.Predicator([this]()
	{
		return m_bGroggy;
	})
		.AddTransition("Skill_2 to HitDown", "HitDown")
		.Predicator([this]()
	{
		return m_bHitDown;
	})

		// MOUSE RB
		.AddState("Skill_3")
		.OnStart([this]()
	{
		Reset_Anim(PLAYER_SK35);
		Set_Anim(PLAYER_SK35);
	})
		.Tick([this](_double TimeDelta)
	{
		if (AnimIntervalChecker(PLAYER_SK35, 0, 0.3) || AnimIntervalChecker(PLAYER_SK35, 0.7, 0.9))
			MonsterNormalAttack(true);
		else
			MonsterNormalAttack(false);
	})
		.OnExit([this]() 
	{
		MonsterNormalAttack(false);
	})
		.AddTransition("Skill_3 to Idle", "Idle")
		.Predicator([this]()
	{
		return !m_bAction &&  !m_bSK03 && CheckFinish_Skill3();
	})
		.AddTransition("Skill_3 to Groggy", "Groggy")
		.Predicator([this]()
	{
		return m_bGroggy;
	})
		.AddTransition("Skill_3 to HitDown", "HitDown")
		.Predicator([this]()
	{
		return m_bHitDown;
	})
		
		// KEY F
		.AddState("Skill_4_Charging")
		.Tick([this](_double TimeDelta)
	{
		Set_Anim(PLAYER_SK29_CHARGING);
	})
		.AddTransition("Skill_4_Charging to Skill_4_Attack", "Skill_4_Attacking")
		.Predicator([this]()
	{
		return !m_bSK04_Charging;
	})
		.AddTransition("Skill_4_Charging to Groggy", "Groggy")
		.Predicator([this]()
	{
		return m_bGroggy;
	})
		.AddTransition("Skill_4_Charging to HitDown", "HitDown")
		.Predicator([this]()
	{
		return m_bHitDown;
	})

		.AddState("Skill_4_Attacking")
		.OnStart([this]()
	{
		Get_WeaponCollider()->FixedSphereSize(1.f, -0.15f, 0.1f, 2.f);
		Reset_Anim(PLAYER_SK27_FIRING);
		Set_Anim(PLAYER_SK27_FIRING);
	})
		.Tick([this](_double TimeDelta)
	{
		if (AnimIntervalChecker(PLAYER_SK27_FIRING, 0.1, 0.8))
			MonsterSkill04(true);
		else
			MonsterSkill04(false);
	})
		.OnExit([this]()
	{
		Get_WeaponCollider()->FixedSphereSize(1.f, -0.15f, 0.1f, 0.65f);
		MonsterSkill04(false);
	})
		.AddTransition("Skill_4_Attacking to Idle", "Idle")
		.Predicator([this]()
	{
		return CheckFinish_Skill4();
	})
		.AddTransition("Skill_4_Attacking to Groggy", "Groggy")
		.Predicator([this]()
	{
		return m_bGroggy;
	})
		.AddTransition("Skill_4_Attacking to HitDown", "HitDown")
		.Predicator([this]()
	{
		return m_bHitDown;
	})

		// Key_1
		.AddState("Skill_5")
		.OnStart([this]()
	{
		Reset_Anim(PLAYER_SK11);
		Set_Anim(PLAYER_SK11);
		ChangeModel(CPlayer::MODEL_A);
	})
		.Tick([this](_double TimeDelta)
	{
		m_bCamTurn = false;

		if (AnimIntervalChecker(PLAYER_SK11, 0, 0.14))
			static_cast<CParts*>(m_PlayerParts[PART_BOOTS])->ChangeModel(CParts::MODEL_A);
		if (AnimIntervalChecker(PLAYER_SK11, 0.14, 0.28))
			static_cast<CParts*>(m_PlayerParts[PART_BELT])->ChangeModel(CParts::MODEL_A);
		if (AnimIntervalChecker(PLAYER_SK11, 0.28, 0.42))
			static_cast<CParts*>(m_PlayerParts[PART_LOWER])->ChangeModel(CParts::MODEL_A);
		if (AnimIntervalChecker(PLAYER_SK11, 0.42, 0.64))
			static_cast<CParts*>(m_PlayerParts[PART_GLOVE])->ChangeModel(CParts::MODEL_A);
		if (AnimIntervalChecker(PLAYER_SK11, 0.64, 0.76))
			static_cast<CParts*>(m_PlayerParts[PART_UPPER])->ChangeModel(CParts::MODEL_A);
		if (AnimIntervalChecker(PLAYER_SK11, 0.76, 0.88))
			static_cast<CParts*>(m_PlayerParts[PART_SHOULDER])->ChangeModel(CParts::MODEL_A);
		if (AnimIntervalChecker(PLAYER_SK11, 0.88, 1.0))
			static_cast<CParts*>(m_PlayerParts[PART_HELMET])->ChangeModel(CParts::MODEL_A);
	})
		.AddTransition("Skill_5 to Idle", "Idle")
		.Predicator([this]()
	{
		return !m_bAction && !m_bSK05 && CheckFinish_Skill5();
	})


		// Key_2
		.AddState("Skill_6")
		.OnStart([this]()
	{
		Reset_Anim(PLAYER_SK11_1);
		Set_Anim(PLAYER_SK11_1);
		ChangeModel(CPlayer::MODEL_B);
	})
		.Tick([this](_double TimeDelta)
	{
		m_bCamTurn = false;

		if (AnimIntervalChecker(PLAYER_SK11_1, 0, 0.14))
			static_cast<CParts*>(m_PlayerParts[PART_BOOTS])->ChangeModel(CParts::MODEL_B);
		if (AnimIntervalChecker(PLAYER_SK11_1, 0.14, 0.28))
			static_cast<CParts*>(m_PlayerParts[PART_BELT])->ChangeModel(CParts::MODEL_B);
		if (AnimIntervalChecker(PLAYER_SK11_1, 0.28, 0.42))
			static_cast<CParts*>(m_PlayerParts[PART_LOWER])->ChangeModel(CParts::MODEL_B);
		if (AnimIntervalChecker(PLAYER_SK11_1, 0.42, 0.64))
			static_cast<CParts*>(m_PlayerParts[PART_GLOVE])->ChangeModel(CParts::MODEL_B);
		if (AnimIntervalChecker(PLAYER_SK11_1, 0.64, 0.76))
			static_cast<CParts*>(m_PlayerParts[PART_UPPER])->ChangeModel(CParts::MODEL_B);
		if (AnimIntervalChecker(PLAYER_SK11_1, 0.76, 0.88))
			static_cast<CParts*>(m_PlayerParts[PART_SHOULDER])->ChangeModel(CParts::MODEL_B);
		if (AnimIntervalChecker(PLAYER_SK11_1, 0.88, 1.0))
			static_cast<CParts*>(m_PlayerParts[PART_HELMET])->ChangeModel(CParts::MODEL_B);
	})
		.AddTransition("Skill_6 to Idle", "Idle")
		.Predicator([this]()
	{
		return !m_bAction && !m_bSK05 && CheckFinish_Skill6();
	})

		.Build();
}

// 로컬 애니메이션 움직임 때문에 넣어놨으나 그냥 조종해주는 것이 더 낫다고 판단

/*void CPlayer::MoveToAnim(_double TimeDelta)
{
	_vector   vMovePos;
	ZeroMemory(&vMovePos, sizeof(_vector));

	vMovePos = XMLoadFloat4(&m_pModelCom->Get_MovePos());

	_float4 vIsZero;
	XMStoreFloat4(&vIsZero, vMovePos);

	if (0 == vIsZero.x &&
		0 == vIsZero.z)
		return;

	_matrix mat = XMMatrixRotationY(XMConvertToRadians(270.0f));
	XMVector3TransformCoord(vMovePos, mat);
	vMovePos = XMVectorSetW(vMovePos, 1.f);

	_vector	  vDifferent = vMovePos - XMLoadFloat4(&m_vAnimationMove);
	vDifferent = XMVectorSetW(vDifferent, 1.f);

	XMStoreFloat4(&m_vAnimationMove, vMovePos);

	_float4   vMoving;
	XMStoreFloat4(&vMoving, vDifferent);

	m_pTransformCom->Go_Straight(TimeDelta * vMoving.x * 40, m_pNavigationCom);
}*/

void CPlayer::Movement(_double TimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (m_pNavigationCom != nullptr)
	{
		_float yPos = 0.f;
		m_pNavigationCom->isHeighit_OnNavigation(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION), &yPos);
		
		if (m_bJump)
		{
			static _float fGravity = 8.81f;
			_float4 vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
			_float	fJumpPower = yPos + m_fJumpPower;
			_float fVelocity = sqrtf(fabsf(fJumpPower * 2.f * fGravity));
			
			if (vPos.y >= fVelocity)
				fGravity *= -1.f;
			
			_float y = max(vPos.y + fGravity * static_cast<_float>(TimeDelta), yPos);
			
			if (y - yPos <= 0.00001f)
			{
				fGravity *= -1.f;
				m_bJump = false;
			}

			m_pTransformCom->Set_Height(y);
		}
		else
			m_pTransformCom->Set_Height(yPos);
	}

	if (!pGameInstance->Key_Pressing(DIK_W) &&
		!pGameInstance->Key_Pressing(DIK_A) &&
		!pGameInstance->Key_Pressing(DIK_S) &&
		!pGameInstance->Key_Pressing(DIK_D))
	{
		m_bMove = false;
	}
	else
	{
		m_bMove = true;
	}

	if (m_bMove && !m_bAction && !m_bGroggy && !m_bHitDown)
	{
		if (pGameInstance->Get_DIKeyState(DIK_W) & 0x80)
		{
			m_bRunning = true;
			m_eState = PLAYER_FM;
			m_pTransformCom->Go_Straight(TimeDelta * m_fVelocity, m_pNavigationCom);
		}

		if (m_eState == PLAYER_FM)
		{
			if (pGameInstance->Key_Up(DIK_W))
			{
				m_bRunning = false;
			}
		}

		if (pGameInstance->Get_DIKeyState(DIK_S) & 0x80)
		{
			m_bRunning = true;
			m_eState = PLAYER_BM;
			m_pTransformCom->Go_Backward(TimeDelta * m_fVelocity, m_pNavigationCom);
		}

		if (m_eState == PLAYER_BM)
		{
			if (pGameInstance->Key_Up(DIK_S))
			{
				m_bRunning = false;
			}
		}

		if (pGameInstance->Get_DIKeyState(DIK_D) & 0x80)
		{
			m_bRunning = true;
			m_eState = PLAYER_RM;
			m_pTransformCom->Go_Right(TimeDelta, m_pNavigationCom);
		}

		if (m_eState == PLAYER_RM)
		{
			if (pGameInstance->Key_Up(DIK_D))
			{
				m_bRunning = false;
			}
		}

		if (pGameInstance->Get_DIKeyState(DIK_A) & 0x80)
		{
			m_bRunning = true;
			m_eState = PLAYER_LM;
			m_pTransformCom->Go_Left(TimeDelta, m_pNavigationCom);
		}

		if (m_eState == PLAYER_LM)
		{
			if (pGameInstance->Key_Up(DIK_A))
			{
				m_bRunning = false;
			}
		}

		if (pGameInstance->Get_DIKeyState(DIK_W) & 0x80 && pGameInstance->Get_DIKeyState(DIK_D) & 0x80)
		{
			m_bRunning = true;
			m_eState = PLAYER_FR;
		}

		if (m_eState == PLAYER_FR)
		{
			if (pGameInstance->Key_Up(DIK_W))
				m_eState = PLAYER_RM;

			if (pGameInstance->Key_Up(DIK_D))
				m_eState = PLAYER_FM;
		}

		if (pGameInstance->Get_DIKeyState(DIK_W) & 0x80 && pGameInstance->Get_DIKeyState(DIK_A) & 0x80)
		{
			m_bRunning = true;
			m_eState = PLAYER_FL;
		}

		if (m_eState == PLAYER_FL)
		{
			if (pGameInstance->Key_Up(DIK_W))
				m_eState = PLAYER_LM;

			if (pGameInstance->Key_Up(DIK_A))
				m_eState = PLAYER_FM;
		}

		if (pGameInstance->Get_DIKeyState(DIK_S) & 0x80 && pGameInstance->Get_DIKeyState(DIK_D) & 0x80)
		{
			m_bRunning = true;
			m_eState = PLAYER_BR;
		}

		if (m_eState == PLAYER_BR)
		{
			if (pGameInstance->Key_Up(DIK_S))
				m_eState = PLAYER_RM;

			if (pGameInstance->Key_Up(DIK_D))
				m_eState = PLAYER_BM;
		}

		if (pGameInstance->Get_DIKeyState(DIK_S) & 0x80 && pGameInstance->Get_DIKeyState(DIK_A) & 0x80)
		{
			m_bRunning = true;
			m_eState = PLAYER_BL;
		}

		if (m_eState == PLAYER_BL)
		{
			if (pGameInstance->Key_Up(DIK_W))
				m_eState = PLAYER_LM;

			if (pGameInstance->Key_Up(DIK_D))
				m_eState = PLAYER_BM;
		}

		if (pGameInstance->Get_DIKeyState(DIK_W) & 0x80 && pGameInstance->Get_DIKeyState(DIK_C) & 0x80)
		{
			m_bRunning = false;
			m_bWalking = true;
			m_eState = PLAYER_FM;
			m_fVelocity = 0.7f;
		}

		if (m_eState == PLAYER_FM)
		{
			if (pGameInstance->Key_Up(DIK_C))
			{
				m_bRunning = true;
				m_bWalking = false;
				m_eState = PLAYER_FM;
				m_fVelocity = 1.f;
			}
		}

		if (pGameInstance->Get_DIKeyState(DIK_S) & 0x80 && pGameInstance->Get_DIKeyState(DIK_C) & 0x80)
		{
			m_bRunning = false;
			m_bWalking = true;
			m_eState = PLAYER_BM;
			m_fVelocity = 0.7f;
		}

		if (m_eState == PLAYER_BM)
		{
			if (pGameInstance->Key_Up(DIK_C))
			{
				m_bRunning = true;
				m_bWalking = false;
				m_eState = PLAYER_BM;
				m_fVelocity = 1.f;
			}
		}

	}

	if (pGameInstance->Mouse_Down(DIM_LB))
	{
		m_bAction = true;
		
		if (!strcmp(m_pFSM->GetCurStateName(), "Idle"))
		{
			m_bNormalAttack_1 = true;
		}
		else if (!strcmp(m_pFSM->GetCurStateName(), "Attack_1"))
		{
			m_bAttack = true;
		}
		else if (!strcmp(m_pFSM->GetCurStateName(), "Attack_2"))
		{
			m_bAttack = true;
		}
		else if (!strcmp(m_pFSM->GetCurStateName(), "Attack_3"))
		{
			m_bAttack = true;
		}
	}

	if (m_bAction)
	{
		if (!strcmp(m_pFSM->GetCurStateName(), "Attack_1") && CheckFinish_Attack1())
		{
			if (m_bAttack)
			{
				m_bNormalAttack_1 = false;
				m_bNormalAttack_2 = true;
				m_bNormalAttack_3 = false;
				m_bAttack = false;
				RELEASE_INSTANCE(CGameInstance);
				return;
			}
			m_bNormalAttack_1 = false;
			m_bNormalAttack_2 = false;
			m_bNormalAttack_3 = false;
			m_bAttack = false;
			m_bAction = false;
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		if (!strcmp(m_pFSM->GetCurStateName(), "Attack_2") && CheckFinish_Attack2())
		{
			if (m_bAttack)
			{
				m_bNormalAttack_1 = false;
				m_bNormalAttack_2 = false;
				m_bNormalAttack_3 = true;
				m_bAttack = false;
				RELEASE_INSTANCE(CGameInstance);
				return;
			}
			m_bNormalAttack_1 = false;
			m_bNormalAttack_2 = false;
			m_bNormalAttack_3 = false;
			m_bAttack = false;
			m_bAction = false;
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		if (!strcmp(m_pFSM->GetCurStateName(), "Attack_3") && CheckFinish_Attack3())
		{
			if (m_bAttack)
			{
				m_bNormalAttack_1 = true;
				m_bNormalAttack_2 = false;
				m_bNormalAttack_3 = false;
				m_bAttack = false;
				RELEASE_INSTANCE(CGameInstance);
				return;
			}
			m_bNormalAttack_1 = false;
			m_bNormalAttack_2 = false;
			m_bNormalAttack_3 = false;
			m_bAttack = false;
			m_bAction = false;
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
	}
	
	/* SKILL1 !!*/
	if (pGameInstance->Key_Down(DIK_E))
	{
		m_bAction = true;
		m_bSK01 = true;
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	if (!strcmp(m_pFSM->GetCurStateName(), "Skill_1") && CheckFinish_Skill1())
	{
		m_bAction = false;
		m_bSK01 = false;
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	/* SKILL2 !!*/
	if (pGameInstance->Key_Down(DIK_Q))
	{
		m_bAction = true;
		m_bSK02 = true;
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	if (!strcmp(m_pFSM->GetCurStateName(), "Skill_2") && CheckFinish_Skill2())
	{
		m_bAction = false;
		m_bSK02 = false;
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	/* SKILL3 !!*/
	if (pGameInstance->Mouse_Down(DIM_RB))
	{
		m_bAction = true;
		m_bSK03 = true;
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	if (!strcmp(m_pFSM->GetCurStateName(), "Skill_3") && CheckFinish_Skill3())
	{
		m_bAction = false;
		m_bSK03 = false;
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	/* SKILL4 !!*/
	if (pGameInstance->Key_Pressing(DIK_F))
	{
		m_bAction = true;
		m_bSK04_Charging = true;
	}

	if (pGameInstance->Key_Up(DIK_F))
	{
		m_bSK04_Charging = false;
		RELEASE_INSTANCE(CGameInstance);
		return;
	}
	
	/* SKILL5 !! 변신*/
	if (pGameInstance->Key_Down(DIK_1))
	{
		m_bAction = true;
		m_bSK05 = true;
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	if (!strcmp(m_pFSM->GetCurStateName(), "Skill_5") && CheckFinish_Skill5())
	{
		m_bAction = false;
		m_bSK05 = false;
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	/* SKILL6 !! 변신*/
	if (pGameInstance->Key_Down(DIK_2))
	{
		m_bAction = true;
		m_bSK06 = true;
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	if (!strcmp(m_pFSM->GetCurStateName(), "Skill_6") && CheckFinish_Skill6())
	{
		m_bAction = false;
		m_bSK06 = false;
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	if (pGameInstance->Key_Down(DIK_LSHIFT))
	{
		m_bAction = true;
		m_bV_DEF = true;
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	if (m_bV_DEF &&	CheckFinish_V_DEF())
	{
		m_bAction = false;
		m_bV_DEF = false;
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	if (!m_bJump && pGameInstance->Key_Down(DIK_SPACE) &&
		(!strcmp(m_pFSM->GetCurStateName(), "Idle") || 
		 (!strcmp(m_pFSM->GetCurStateName(), "Run") && 
			(m_eState == CPlayer::PLAYER_FM || m_eState == CPlayer::PLAYER_BM)))) // 점프를 할 수 있는 조건을 줘
	{
		m_bJump = true;
		RELEASE_INSTANCE(CGameInstance);
		return;
	}
		
	if (m_bCamTurn == false)
	{
		_long MouseMove_X = 0;

		if (MouseMove_X = pGameInstance->Get_DIMouseMove(DIMS_X))
		{
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), MouseMove_X * m_MouseSensity * TimeDelta);
		}
	}
		
	RELEASE_INSTANCE(CGameInstance);
	return;
}

void CPlayer::AdditiveAnim(_double TimeDelta)
{
	if (m_bFrontDamagedToMonster)
	{
		Reset_Anim(PLAYER_ADD_DMG_F);
		m_bFrontDamage = true;
	}

	if (m_bFrontDamage)
	{
		m_pModelCom[m_eModelState]->Set_AdditiveAnimIndex(PLAYER_ADD_DMG_F);
		m_pModelCom[m_eModelState]->Play_AddtivieAnim(TimeDelta, 1.f);

		for (_uint i = CPlayer::PART_UPPER; i < CPlayer::PART_END; ++i)
		{
			static_cast<CParts*>(m_PlayerParts[i])->Set_AdditiveAnimIndex(PLAYER_ADD_DMG_F);
			static_cast<CParts*>(m_PlayerParts[i])->Play_AdditiveAnim(TimeDelta, 1.f);
		}
	}

	if (AnimFinishChecker(CPlayer::PLAYER_ADD_DMG_F))
	{
		m_bFrontDamage = false;
		m_bFrontDamagedToMonster = false;
	}

	///////////////////////////////////////
	
	if (m_bBackDamagedToMonster)
	{
		Reset_Anim(PLAYER_ADD_DMG_B);
		m_bBackDamaged = true;
	}

	if (m_bBackDamaged)
	{
		m_pModelCom[m_eModelState]->Set_AdditiveAnimIndex(PLAYER_ADD_DMG_B);
		m_pModelCom[m_eModelState]->Play_AddtivieAnim(TimeDelta, 1.f);

		for (_uint i = CPlayer::PART_UPPER; i < CPlayer::PART_END; ++i)
		{
			static_cast<CParts*>(m_PlayerParts[i])->Set_AdditiveAnimIndex(PLAYER_ADD_DMG_B);
			static_cast<CParts*>(m_PlayerParts[i])->Play_AdditiveAnim(TimeDelta, 1.f);
		}
	}

	if (AnimFinishChecker(CPlayer::PLAYER_ADD_DMG_B))
	{
		m_bBackDamaged = false;
		m_bBackDamagedToMonster = false;
	}
}

void CPlayer::LinkObject(_double TimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	
	m_pCam = static_cast<CPlayerCamera*>(pGameInstance->Find_GameObject(pGameInstance->GetCurLevelIdx(), L"Layer_Camera", L"PlayerCamera"));
	
	if (pGameInstance->Key_Down(DIK_F2))
		m_bCamChange = !m_bCamChange;
		
	if (m_pCam != nullptr && !m_bCamChange)
		m_pCam->LinkPlayer(TimeDelta, m_pTransformCom, m_bCamTurn);
	else if (m_pCam != nullptr && m_bCamChange)
		m_pCam->DynamicCamera(TimeDelta);

	// 이펙트 연결인데 이거 수정해야겠다
	RELEASE_INSTANCE(CGameInstance);
}

void CPlayer::Reset_Anim(ANIMATION eAnim)
{
	m_pModelCom[m_eModelState]->Reset_AnimPlayTime(eAnim);
	for (_uint i = CPlayer::PART_UPPER; i < CPlayer::PART_END; ++i)
		static_cast<CParts*>(m_PlayerParts[i])->Get_ModelCom()->Reset_AnimPlayTime(eAnim);
}

void CPlayer::Set_Anim(ANIMATION eAnim)
{
	m_pModelCom[m_eModelState]->Set_AnimationIndex(eAnim);
	for (_uint i = CPlayer::PART_UPPER; i < CPlayer::PART_END; ++i)
		static_cast<CParts*>(m_PlayerParts[i])->Set_AnimIndex(eAnim);
}

void CPlayer::ChangeModel(MODEL eModelIndex)
{
	if (m_eModelState == eModelIndex)
		return;

	m_ePreModelState = m_eModelState;
	m_eModelState = eModelIndex;

	_uint AnimNum = m_pModelCom[m_ePreModelState]->Get_AnimationsNum();
	_uint iCurrentIndex = m_pModelCom[m_ePreModelState]->Get_CurrentAnimIndex();
	_uint iPreIndex = m_pModelCom[m_ePreModelState]->Get_PreAnimIndex();
	_uint iAdditiveAnimNum = m_pModelCom[m_ePreModelState]->Get_AdditiveAnimIndex();

	m_pModelCom[m_eModelState]->Model_IndexChange(iCurrentIndex, iPreIndex, iAdditiveAnimNum);

	for (_uint i = 0; i < AnimNum; ++i)
	{
		_double time = m_pModelCom[m_ePreModelState]->Get_AnimPlayTime(i);
		m_pModelCom[m_eModelState]->Model_Change(i, time);
	}

	CWeapon::WEAPONDESC			WeaponDesc;
	ZeroMemory(&WeaponDesc, sizeof(CWeapon::WEAPONDESC));

	WeaponDesc.PivotMatrix = m_pModelCom[m_eModelState]->Get_PivotMatrix();
	WeaponDesc.pSocket = m_pModelCom[m_eModelState]->Get_BonePtr("Bip01-R-Hand");
	WeaponDesc.pTargetTransform = m_pTransformCom;
	Safe_AddRef(WeaponDesc.pSocket);
	Safe_AddRef(m_pTransformCom);

	static_cast<CWeapon*>(m_PlayerParts[PART_WEAPON])->Set_TargetTrasform(WeaponDesc);
}

void CPlayer::Imgui_RenderProperty()
{
	m_pFSM->Imgui_RenderProperty();

	/*ImGui::DragFloat("TrailMove", &m_fNormalAtk1TrailMove, 0.1f, -10.f, 10.f);

	ImGui::DragFloat("CX", &m_CX, 0.1f, -10.f, 10.f);
	ImGui::DragFloat("CY", &m_CY, 0.1f, -10.f, 10.f);
	ImGui::DragFloat("CZ", &m_CZ, 0.1f, -10.f, 10.f);
	
	ImGui::DragFloat("RX", &m_RX, 0.01f, 0.f, 360.f);
	ImGui::DragFloat("RY", &m_RY, 0.01f, 0.f, 360.f);
	ImGui::DragFloat("RZ", &m_RZ, 0.01f, 0.f, 360.f);
	
	ImGui::DragFloat("X", &m_X, 0.01f, -10.f, 10.f);
	ImGui::DragFloat("Y", &m_Y, 0.01f, -10.f, 10.f);
	ImGui::DragFloat("Z", &m_Z, 0.01f, -10.f, 10.f);*/
}

void CPlayer::Idle_Tick(_double TimeDelta)
{
	m_pModelCom[m_eModelState]->Set_AnimationIndex(PLAYER_IDLE_READY);

	for (_uint i = CPlayer::PART_UPPER; i < CPlayer::PART_END; ++i)
		static_cast<CParts*>(m_PlayerParts[i])->Set_AnimIndex(PLAYER_IDLE_READY);
}

void CPlayer::Walk_Tick(_double TImeDelat)
{
	if (m_ePreState != m_eState)
	{
		switch (m_eState)
		{
		case Client::CPlayer::PLAYER_FM:
			Reset_Anim(PLAYER_WALK_F);
			break;
		case Client::CPlayer::PLAYER_BM:
			Reset_Anim(PLAYER_WALK_B);
			break;
		}

		m_ePreState = m_eState;
	}

	switch (m_eState)
	{
	case Client::CPlayer::PLAYER_FM:
		m_pModelCom[m_eModelState]->Set_AnimationIndex(PLAYER_WALK_F);

		for (_uint i = CPlayer::PART_UPPER; i < CPlayer::PART_END; ++i)
			static_cast<CParts*>(m_PlayerParts[i])->Set_AnimIndex(PLAYER_WALK_F);
		break;
	case Client::CPlayer::PLAYER_BM:
		m_pModelCom[m_eModelState]->Set_AnimationIndex(PLAYER_WALK_B);

		for (_uint i = CPlayer::PART_UPPER; i < CPlayer::PART_END; ++i)
			static_cast<CParts*>(m_PlayerParts[i])->Set_AnimIndex(PLAYER_WALK_B);
		break;
	}
}

void CPlayer::Run_Tick(_double TimeDelta)
{
	if (m_ePreState != m_eState)
	{
		switch (m_eState)
		{
		case Client::CPlayer::PLAYER_FM:
			Reset_Anim(PLAYER_RUN_F);
			break;
		case Client::CPlayer::PLAYER_BM:
			Reset_Anim(PLAYER_RUN_B);
			break;
		case Client::CPlayer::PLAYER_RM:
			Reset_Anim(PLAYER_RUN_R);
			break;
		case Client::CPlayer::PLAYER_LM:
			Reset_Anim(PLAYER_RUN_L);
			break;
		case Client::CPlayer::PLAYER_FR:
			Reset_Anim(PLAYER_RUN_FR);
			break;
		case Client::CPlayer::PLAYER_FL:
			Reset_Anim(PLAYER_RUN_FL);
			break;
		case Client::CPlayer::PLAYER_BR:
			Reset_Anim(PLAYER_RUN_BR);
			break;
		case Client::CPlayer::PLAYER_BL:
			Reset_Anim(PLAYER_RUN_BL);
			break;
		}
			
		m_ePreState = m_eState;
	}


	switch (m_eState)
	{
	case Client::CPlayer::PLAYER_FM:
		m_pModelCom[m_eModelState]->Set_AnimationIndex(PLAYER_RUN_F);

		for (_uint i = CPlayer::PART_UPPER; i < CPlayer::PART_END; ++i)
			static_cast<CParts*>(m_PlayerParts[i])->Set_AnimIndex(PLAYER_RUN_F);
		break;
	case Client::CPlayer::PLAYER_BM:
		m_pModelCom[m_eModelState]->Set_AnimationIndex(PLAYER_RUN_B);

		for (_uint i = CPlayer::PART_UPPER; i < CPlayer::PART_END; ++i)
			static_cast<CParts*>(m_PlayerParts[i])->Set_AnimIndex(PLAYER_RUN_B);
		break;
	case Client::CPlayer::PLAYER_RM:
		m_pModelCom[m_eModelState]->Set_AnimationIndex(PLAYER_RUN_R);

		for (_uint i = CPlayer::PART_UPPER; i < CPlayer::PART_END; ++i)
			static_cast<CParts*>(m_PlayerParts[i])->Set_AnimIndex(PLAYER_RUN_R);
		break;
	case Client::CPlayer::PLAYER_LM:
		m_pModelCom[m_eModelState]->Set_AnimationIndex(PLAYER_RUN_L);

		for (_uint i = CPlayer::PART_UPPER; i < CPlayer::PART_END; ++i)
			static_cast<CParts*>(m_PlayerParts[i])->Set_AnimIndex(PLAYER_RUN_L);
		break;
	case Client::CPlayer::PLAYER_FR:
		m_pModelCom[m_eModelState]->Set_AnimationIndex(PLAYER_RUN_FR);

		for (_uint i = CPlayer::PART_UPPER; i < CPlayer::PART_END; ++i)
			static_cast<CParts*>(m_PlayerParts[i])->Set_AnimIndex(PLAYER_RUN_FR);
		break;
	case Client::CPlayer::PLAYER_FL:
		m_pModelCom[m_eModelState]->Set_AnimationIndex(PLAYER_RUN_FL);

		for (_uint i = CPlayer::PART_UPPER; i < CPlayer::PART_END; ++i)
			static_cast<CParts*>(m_PlayerParts[i])->Set_AnimIndex(PLAYER_RUN_FL);
		break;
	case Client::CPlayer::PLAYER_BR:
		m_pModelCom[m_eModelState]->Set_AnimationIndex(PLAYER_RUN_BR);

		for (_uint i = CPlayer::PART_UPPER; i < CPlayer::PART_END; ++i)
			static_cast<CParts*>(m_PlayerParts[i])->Set_AnimIndex(PLAYER_RUN_BR);
		break;
	case Client::CPlayer::PLAYER_BL:
		m_pModelCom[m_eModelState]->Set_AnimationIndex(PLAYER_RUN_BL);

		for (_uint i = CPlayer::PART_UPPER; i < CPlayer::PART_END; ++i)
			static_cast<CParts*>(m_PlayerParts[i])->Set_AnimIndex(PLAYER_RUN_BL);
		break;
	}
}

CCollider* CPlayer::Get_WeaponCollider()
{
	return static_cast<CWeapon*>(m_PlayerParts[PART_WEAPON])->Get_Collider();
}

void CPlayer::BackDamagedToMonster()
{
	m_pCam->Shake(0.5f);
	m_bBackDamaged = true;
}

void CPlayer::FrontDamagedToMonster()
{
	m_pCam->Shake(0.5f);
	m_bFrontDamage = true;
}

void CPlayer::PassOutToMonster(_float fKnockBackPower)
{
	m_bGroggy = true;
	m_fKnockBackPower = fKnockBackPower;
}

void CPlayer::HitDownToMonster(_float fJumpPower, _float fKnockBackPower, _float fDuration)
{
	m_bHitDown = true;
	m_fJumpPower = fJumpPower;
	m_fKnockBackPower = fKnockBackPower;
	m_HitDownDurationTime = fDuration;
}

void CPlayer::MonsterNormalAttack(_bool bAttack)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (g_LEVEL == LEVEL_CHAP1)
	{
		const list<CGameObject*>* monlist = pGameInstance->Find_LayerList(LEVEL_CHAP1, L"Layer_Monster");

		if (monlist == nullptr)
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		for (auto& pMonster : *(monlist))
		{
			if (pMonster != nullptr)
			{
				_float3 vMonsterPos = pMonster->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION);
				_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
				_float fDistance =	CMathUtils::Distance(vMonsterPos, vPos);

				if(fabsf(fDistance) < 10.f)
					static_cast<CMonster*>(pMonster)->Set_PlayerAttackCommand(bAttack, 20);
			}
		}
	}
	else if (g_LEVEL == LEVEL_CHAP2)
	{
		const list<CGameObject*>* monlist = pGameInstance->Find_LayerList(LEVEL_CHAP2, L"Layer_Monster");

		if (monlist == nullptr)
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		for (auto& pMonster : *(monlist))
		{
			if (pMonster != nullptr)
			{
				_float3 vMonsterPos = pMonster->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION);
				_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
				_float fDistance = CMathUtils::Distance(vMonsterPos, vPos);

				if (fabsf(fDistance) < 10.f)
					static_cast<CMonster*>(pMonster)->Set_PlayerAttackCommand(bAttack, 20);
			}
		}
	}
	else if (g_LEVEL == LEVEL_CHAP3)
	{
		const list<CGameObject*>* monlist = pGameInstance->Find_LayerList(LEVEL_CHAP3, L"Layer_Monster");

		if (monlist == nullptr)
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		for (auto& pMonster : *(monlist))
		{
			if (pMonster != nullptr)
			{
				_float3 vMonsterPos = pMonster->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION);
				_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
				_float fDistance = CMathUtils::Distance(vMonsterPos, vPos);

				if (fabsf(fDistance) < 10.f)
					static_cast<CMonster*>(pMonster)->Set_PlayerAttackCommand(bAttack, 20);
			}
		}
	}	

	RELEASE_INSTANCE(CGameInstance);
}

void CPlayer::MonsterSkill02(_bool bAttack)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (g_LEVEL == LEVEL_CHAP1)
	{
		const list<CGameObject*>* monlist = pGameInstance->Find_LayerList(LEVEL_CHAP1, L"Layer_Monster");

		if (monlist == nullptr)
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		for (auto& pMonster : *(monlist))
		{
			if (pMonster != nullptr)
			{
				_float3 vMonsterPos = pMonster->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION);
				_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
				_float fDistance = CMathUtils::Distance(vMonsterPos, vPos);

				if (fabsf(fDistance) < 10.f)
					static_cast<CMonster*>(pMonster)->Set_PlayerSkill02Command(bAttack, 35);
			}
		}
	}
	else if (g_LEVEL == LEVEL_CHAP2)
	{
		const list<CGameObject*>* monlist = pGameInstance->Find_LayerList(LEVEL_CHAP2, L"Layer_Monster");

		if (monlist == nullptr)
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		for (auto& pMonster : *(monlist))
		{
			if (pMonster != nullptr)
			{
				_float3 vMonsterPos = pMonster->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION);
				_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
				_float fDistance = CMathUtils::Distance(vMonsterPos, vPos);

				if (fabsf(fDistance) < 10.f)
					static_cast<CMonster*>(pMonster)->Set_PlayerSkill02Command(bAttack, 35);
			}
		}
	}
	else if (g_LEVEL == LEVEL_CHAP3)
	{
		const list<CGameObject*>* monlist = pGameInstance->Find_LayerList(LEVEL_CHAP3, L"Layer_Monster");

		if (monlist == nullptr)
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		for (auto& pMonster : *(monlist))
		{
			if (pMonster != nullptr)
			{
				_float3 vMonsterPos = pMonster->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION);
				_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
				_float fDistance = CMathUtils::Distance(vMonsterPos, vPos);

				if (fabsf(fDistance) < 10.f)
					static_cast<CMonster*>(pMonster)->Set_PlayerSkill02Command(bAttack, 35);
			}
		}
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CPlayer::MonsterSkill04(_bool bAttack)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (g_LEVEL == LEVEL_CHAP1)
	{
		const list<CGameObject*>* monlist = pGameInstance->Find_LayerList(LEVEL_CHAP1, L"Layer_Monster");

		if (monlist == nullptr)
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		for (auto& pMonster : *(monlist))
		{
			if (pMonster != nullptr)
			{
				_float3 vMonsterPos = pMonster->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION);
				_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
				_float fDistance = CMathUtils::Distance(vMonsterPos, vPos);

				if (fabsf(fDistance) < 10.f)
					static_cast<CMonster*>(pMonster)->Set_PlayerSkiil04Command(bAttack, 35);
			}
		}
	}
	else if (g_LEVEL == LEVEL_CHAP2)
	{
		const list<CGameObject*>* monlist = pGameInstance->Find_LayerList(LEVEL_CHAP2, L"Layer_Monster");

		if (monlist == nullptr)
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		for (auto& pMonster : *(monlist))
		{
			if (pMonster != nullptr)
			{
				_float3 vMonsterPos = pMonster->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION);
				_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
				_float fDistance = CMathUtils::Distance(vMonsterPos, vPos);

				if (fabsf(fDistance) < 10.f)
					static_cast<CMonster*>(pMonster)->Set_PlayerSkiil04Command(bAttack, 35);
			}
		}
	}
	else if (g_LEVEL == LEVEL_CHAP3)
	{
		const list<CGameObject*>* monlist = pGameInstance->Find_LayerList(LEVEL_CHAP3, L"Layer_Monster");

		if (monlist == nullptr)
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		for (auto& pMonster : *(monlist))
		{
			if (pMonster != nullptr)
			{
				_float3 vMonsterPos = pMonster->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION);
				_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
				_float fDistance = CMathUtils::Distance(vMonsterPos, vPos);

				if (fabsf(fDistance) < 10.f)
					static_cast<CMonster*>(pMonster)->Set_PlayerSkiil04Command(bAttack, 35);
			}
		}
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CPlayer::Reset_Action()
{

	m_bAction = false;
	m_bAttack = false;
	m_bNormalAttack_1 = false;
	m_bNormalAttack_2 = false;
	m_bNormalAttack_3 = false;
	m_bSK01 = false;
	m_bSK02 = false;
	m_bSK03 = false;
	m_bSK04_Charging = false;
	m_bSK05 = false;
	m_bSK06 = false;
}

_bool CPlayer::AnimFinishChecker(ANIMATION eAnim, _double FinishRate)
{
	return m_pModelCom[m_eModelState]->Get_IndexAnim(eAnim)->Get_PlayRate() >= FinishRate;
}

_bool CPlayer::AnimIntervalChecker(ANIMATION eAnim, _double StartRate, _double FinishRate)
{
	if (m_pModelCom[m_eModelState]->Get_IndexAnim(eAnim)->Get_PlayRate() > StartRate &&
		m_pModelCom[m_eModelState]->Get_IndexAnim(eAnim)->Get_PlayRate() <= FinishRate)
	{
		return true;
	}

	return false;
}

void CPlayer::AnimEditPlayTime(ANIMATION eAnim, _double PlayTime)
{
	m_pModelCom[m_eModelState]->Get_IndexAnim(eAnim)->Set_MulSecond(PlayTime);
}

_bool CPlayer::CheckFinish_Attack1()
{
	return AnimFinishChecker(PLAYER_ATK_01, 0.9);
}

_bool CPlayer::CheckFinish_Attack2()
{
	return AnimFinishChecker(PLAYER_ATK_02, 0.9);
}

_bool CPlayer::CheckFinish_Attack3()
{
	return AnimFinishChecker(PLAYER_ATK_03, 0.9);
}

_bool CPlayer::CheckFinish_Skill1()
{
	return AnimFinishChecker(PLAYER_SK24, 0.9);
}

_bool CPlayer::CheckFinish_Skill2()
{
	return AnimFinishChecker(PLAYER_SK09, 0.9);
}

_bool CPlayer::CheckFinish_Skill3()
{
	return AnimFinishChecker(PLAYER_SK35, 0.9);
}

_bool CPlayer::CheckFinish_Skill4()
{
	if (AnimFinishChecker(PLAYER_SK27_FIRING, 0.9))
	{
		m_bAction = false;
		return true;
	}
	return false;
}

_bool CPlayer::CheckFinish_Skill5()
{
	return AnimFinishChecker(PLAYER_SK11, 0.9);
}

_bool CPlayer::CheckFinish_Skill6()
{
	return AnimFinishChecker(PLAYER_SK11_1, 0.9);
}

_bool CPlayer::CheckFinish_V_DEF()
{
	return AnimFinishChecker(PLAYER_V_DEF, 0.8);
}

HRESULT CPlayer::SetUp_Parts()
{
	CGameObject*		pPartObject = nullptr;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CWeapon::WEAPONDESC			WeaponDesc;
	ZeroMemory(&WeaponDesc, sizeof(CWeapon::WEAPONDESC));

	WeaponDesc.PivotMatrix = m_pModelCom[m_eModelState]->Get_PivotMatrix();
	WeaponDesc.pSocket = m_pModelCom[m_eModelState]->Get_BonePtr("Bip01-R-Hand");
	WeaponDesc.pTargetTransform = m_pTransformCom;
	Safe_AddRef(WeaponDesc.pSocket);
	Safe_AddRef(m_pTransformCom);

	pPartObject = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Weapon"), &WeaponDesc);
	
	if (nullptr == pPartObject)
		return E_FAIL;

	m_PlayerParts.push_back(pPartObject);

	pPartObject = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Upper"), nullptr);

	if (nullptr == pPartObject)
		return E_FAIL;

	m_PlayerParts.push_back(pPartObject);

	pPartObject = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Lower"), nullptr);

	if (nullptr == pPartObject)
		return E_FAIL;

	m_PlayerParts.push_back(pPartObject);

	// glove가 얼굴임 본체가 글로브임
	pPartObject = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Glove"), nullptr);

	if (nullptr == pPartObject)
		return E_FAIL;

	m_PlayerParts.push_back(pPartObject);

	pPartObject = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Shoulder"), nullptr);

	if (nullptr == pPartObject)
		return E_FAIL;

	m_PlayerParts.push_back(pPartObject);

	pPartObject = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Belt"), nullptr);

	if (nullptr == pPartObject)
		return E_FAIL;

	m_PlayerParts.push_back(pPartObject);

	pPartObject = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Boots"), nullptr);

	if (nullptr == pPartObject)
		return E_FAIL;

	m_PlayerParts.push_back(pPartObject);

	pPartObject = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Helmet"), nullptr);

	if (nullptr == pPartObject)
		return E_FAIL;

	m_PlayerParts.push_back(pPartObject);

	m_pNormalAtkTrail = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Trail"), nullptr);
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CPlayer::SetUp_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Model_HumanF"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom[MODEL_NOMAL])))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Model_HumanF_A"), TEXT("Com_Model_A"),
		(CComponent**)&m_pModelCom[MODEL_A])))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Model_HumanF_B"), TEXT("Com_Model_B"),
		(CComponent**)&m_pModelCom[MODEL_B])))
		return E_FAIL;

	CCollider::COLLIDERDESC			ColliderDesc;

	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(0.3f, 1.1f, 0.3f);
	ColliderDesc.vCenter = _float3(0.f, 1.3f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_AABB"),
		(CComponent**)&m_pColliderCom[COLLTYPE_AABB], &ColliderDesc)))
		return E_FAIL;

	///* For.Com_OBB */
	//ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	//ColliderDesc.vSize = _float3(0.3f, 1.1f, 0.3f);
	//ColliderDesc.vCenter = _float3(0.f, 1.3f, 0.f);

	//if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_OBB"),
	//	(CComponent**)&m_pColliderCom[COLLTYPE_OBB], &ColliderDesc)))
	//	return E_FAIL;

	///* For.Com_SPHERE */
	//ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	//ColliderDesc.vSize = _float3(15.f, 15.f, 15.f);
	//ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);

	//if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Collider_SPHERE"), TEXT("Com_SPHERE"),
	//	(CComponent**)&m_pColliderCom[COLLTYPE_SPHERE], &ColliderDesc)))
	//	return E_FAIL;

	if (g_LEVEL == LEVEL_CHAP1)
	{
		/* For.Com_Navigation */
		CNavigation::NAVIDESC			NaviDesc;
		ZeroMemory(&NaviDesc, sizeof(CNavigation::NAVIDESC));

		NaviDesc.iCurrentIndex = 0;

		if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"),
			(CComponent**)&m_pNavigationCom, &NaviDesc)))
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

HRESULT CPlayer::SetUp_ShaderResources()
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

CPlayer * CPlayer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayer*		pInstance = new CPlayer(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Created : CPlayer");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPlayer::Clone(void * pArg)
{
	CPlayer*		pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPlayer");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();

	for (_uint i = 0; i < COLLTYPE_END; ++i)
		Safe_Release(m_pColliderCom[i]);

	for (auto& pPart : m_PlayerParts)
		Safe_Release(pPart);

	m_PlayerParts.clear();

	Safe_Release(m_pNormalAtkTrail);

	for (_uint i = 0; i < MODEL_END; ++i)
		Safe_Release(m_pModelCom[i]);
	
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pNavigationCom);

	Safe_Release(m_pFSM);
}
