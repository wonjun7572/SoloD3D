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

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(5.f, 0.f, 3.f, 1.f));


	/* ~~~~ 공격력 체력 수치!!! ~~~~*/
	m_iHp = 100;
	m_iAttack = 20;
	m_iDefence = 100;

	SetUp_FSM();
	
	return S_OK;
}

void CPlayer::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	_float yPos = 0.f;
	m_pNavigationCom->isHeighit_OnNavigation(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION), &yPos);
	m_pTransformCom->Set_Height(yPos);

	Movement(TimeDelta);

	m_pFSM->Tick(TimeDelta);
	
	for (_uint i = PART_UPPER; i < PART_END; ++i)
	{
		m_PlayerParts[i]->Tick(TimeDelta);
		static_cast<CParts*>(m_PlayerParts[i])->LinkPlayer(m_pTransformCom);
	}
	m_pModelCom->Play_Animation(TimeDelta);
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
}

HRESULT CPlayer::Render()
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
	for (_uint i = 0; i < COLLTYPE_END; ++i)
	{
		if (nullptr != m_pColliderCom[i])
			m_pColliderCom[i]->Render();
	}

	m_pNavigationCom->Render();
#endif

	return S_OK;
}

void CPlayer::SetUp_FSM()
{
	m_pFSM = CFSMComponentBuilder()
		.InitState("Idle")
		.AddState("Idle")
		.Tick(this, &CPlayer::Idle_Tick)
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
		if (AnimIntervalChecker(PLAYER_ATK_01, 0.1, 0.3))
			MonsterNormalAttack(true);
		else
			MonsterNormalAttack(false);
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
	})
	
			.AddState("Attack_2")
		.OnStart([this]()
	{
		Reset_Anim(PLAYER_ATK_02);
		Set_Anim(PLAYER_ATK_02);
	})
		.Tick([this](_double TimeDelta)
	{
		if (AnimIntervalChecker(PLAYER_ATK_02, 0.1, 0.3))
			MonsterNormalAttack(true);
		else
			MonsterNormalAttack(false);
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

		.AddState("Attack_3")
		.OnStart([this]()
	{
		Reset_Anim(PLAYER_ATK_03);
		Set_Anim(PLAYER_ATK_03);
	})
		.Tick([this](_double TimeDelta)
	{
		if (AnimIntervalChecker(PLAYER_ATK_03, 0.1, 0.3))
			MonsterNormalAttack(true);
		else
			MonsterNormalAttack(false);
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

		/*For. Skill*/

		// KEY E
		.AddState("Skill_1")
		.OnStart([this]()
	{
		Get_WeaponCollider()->FixedSphereSize(1.5f, -0.2f, 0.28f, 1.f);
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
		Get_WeaponCollider()->FixedSphereSize(1.5f, -0.2f, 0.28f, 0.5f);
	})
		.AddTransition("Skill_1 to Idle", "Idle")
		.Predicator([this]()
	{
		return !m_bAction && !m_bSK01 && CheckFinish_Skill1();
	})
		// KEY Q
		.AddState("Skill_2")
		.OnStart([this]()
	{
		m_bCamTurn = true;
		Get_WeaponCollider()->FixedSphereSize(1.5f, -0.2f, 0.28f, 5.f);
		Reset_Anim(PLAYER_SK09);
		Set_Anim(PLAYER_SK09);
	})
		.Tick([this](_double TimeDelta)
	{
		if (!AnimFinishChecker(PLAYER_SK09, 0.3))
			m_pTransformCom->Go_Straight(TimeDelta * 2.f, m_pNavigationCom);

		if (AnimIntervalChecker(PLAYER_SK09, 0.1, 0.9))
			MonsterSkill02(true);
		else
			MonsterSkill02(false);
	})
		.OnExit([this]()
	{
		Get_WeaponCollider()->FixedSphereSize(1.5f, -0.2f, 0.28f, 0.5f);
		MonsterSkill02(false);
		m_bCamTurn = false;
	})
		.AddTransition("Skill_2 to Idle", "Idle")
		.Predicator([this]()
	{
		return !m_bAction && !m_bSK01 && CheckFinish_Skill2();
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
		if (AnimIntervalChecker(PLAYER_SK35, 0, 0.2) || AnimIntervalChecker(PLAYER_SK35, 0.7, 0.9))
			MonsterNormalAttack(true);
		else
			MonsterNormalAttack(false);
	})
		.AddTransition("Skill_3 to Idle", "Idle")
		.Predicator([this]()
	{
		return !m_bAction &&  !m_bSK03 && CheckFinish_Skill3();
	})
		
		// KEY F
		.AddState("Skill_4_Charging")
		.Tick([this](_double TimeDelta)
	{
		Set_Anim(PLAYER_SK29_CHARGING_);
	})
		.AddTransition("Skill_4_Charging to Skill_4_Attack", "Skill_4_Attacking")
		.Predicator([this]()
	{
		return !m_bSK04_Charging;
	})

		.AddState("Skill_4_Attacking")
		.OnStart([this]()
	{
		Get_WeaponCollider()->FixedSphereSize(1.5f, -0.2f, 0.28f, 2.f);
		Reset_Anim(PLAYER_SK27_FIRING);
		Set_Anim(PLAYER_SK27_FIRING);
	})
		.Tick([this](_double TimeDelta)
	{
		if (AnimIntervalChecker(PLAYER_SK27_FIRING, 0.1, 0.9))
			MonsterSkill04(true);
		else
			MonsterSkill04(false);
	})
		.OnExit([this]()
	{
		Get_WeaponCollider()->FixedSphereSize(1.5f, -0.2f, 0.28f, 0.5f);
		MonsterSkill04(false);
	})
		.AddTransition("Skill_4_Attacking to Idle", "Idle")
		.Predicator([this]()
	{
		return CheckFinish_Skill4();
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

	if (m_bMove && !m_bAction)
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
		m_pModelCom->Set_AdditiveAnimIndex(PLAYER_ADD_DMG_F);
		m_pModelCom->Play_AddtivieAnim(TimeDelta, 1.f);

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
		m_pModelCom->Set_AdditiveAnimIndex(PLAYER_ADD_DMG_B);
		m_pModelCom->Play_AddtivieAnim(TimeDelta, 1.f);

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
	
	CPlayerCamera* pCam = (CPlayerCamera*)pGameInstance->Find_GameObject(pGameInstance->GetCurLevelIdx(), L"Layer_Camera", L"PlayerCamera");
	
	if (pGameInstance->Key_Down(DIK_F2))
		m_bCamChange = !m_bCamChange;
		
	if (pCam != nullptr && !m_bCamChange)
		pCam->LinkPlayer(TimeDelta, m_pTransformCom, m_bCamTurn);
	else if (pCam != nullptr && m_bCamChange)
		pCam->DynamicCamera(TimeDelta);

	CEffect_Rect* pEffect = (CEffect_Rect*)pGameInstance->Find_GameObject(pGameInstance->GetCurLevelIdx(), L"Layer_Effect", L"Effect_Rect");
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	pEffect->LinkPlayer(TimeDelta, XMVectorSet(XMVectorGetX(vPos), XMVectorGetY(vPos) + 2.f, XMVectorGetZ(vPos), 1.f));

	if (m_bSK01 && !CheckFinish_Skill1())
		pEffect->Set_Tick(true);
	else
		pEffect->Set_Tick(false);

	RELEASE_INSTANCE(CGameInstance);
}

void CPlayer::Reset_Anim(ANIMATION eAnim)
{
	m_pModelCom->Reset_AnimPlayTime(eAnim);
	for (_uint i = CPlayer::PART_UPPER; i < CPlayer::PART_END; ++i)
		static_cast<CParts*>(m_PlayerParts[i])->Get_ModelCom()->Reset_AnimPlayTime(eAnim);
}

void CPlayer::Set_Anim(ANIMATION eAnim)
{
	m_pModelCom->Set_AnimationIndex(eAnim);
	for (_uint i = CPlayer::PART_UPPER; i < CPlayer::PART_END; ++i)
		static_cast<CParts*>(m_PlayerParts[i])->Set_AnimIndex(eAnim);
}

void CPlayer::Imgui_RenderProperty()
{
	m_pFSM->Imgui_RenderProperty();
}

void CPlayer::Idle_Tick(_double TimeDelta)
{
	m_pModelCom->Set_AnimationIndex(PLAYER_IDLE_READY);

	for (_uint i = CPlayer::PART_UPPER; i < CPlayer::PART_END; ++i)
		static_cast<CParts*>(m_PlayerParts[i])->Set_AnimIndex(PLAYER_IDLE_READY);
}

void CPlayer::Walk_Tick(_double TImeDelat)
{
	switch (m_eState)
	{
	case Client::CPlayer::PLAYER_FM:
		m_pModelCom->Set_AnimationIndex(PLAYER_WALK_F);

		for (_uint i = CPlayer::PART_UPPER; i < CPlayer::PART_END; ++i)
			static_cast<CParts*>(m_PlayerParts[i])->Set_AnimIndex(PLAYER_WALK_F);
		break;
	case Client::CPlayer::PLAYER_BM:
		m_pModelCom->Set_AnimationIndex(PLAYER_WALK_B);

		for (_uint i = CPlayer::PART_UPPER; i < CPlayer::PART_END; ++i)
			static_cast<CParts*>(m_PlayerParts[i])->Set_AnimIndex(PLAYER_WALK_B);
		break;
	}
}

void CPlayer::Run_Tick(_double TimeDelta)
{
	switch (m_eState)
	{
	case Client::CPlayer::PLAYER_FM:
		m_pModelCom->Set_AnimationIndex(PLAYER_RUN_F);

		for (_uint i = CPlayer::PART_UPPER; i < CPlayer::PART_END; ++i)
			static_cast<CParts*>(m_PlayerParts[i])->Set_AnimIndex(PLAYER_RUN_F);
		break;
	case Client::CPlayer::PLAYER_BM:
		m_pModelCom->Set_AnimationIndex(PLAYER_RUN_B);

		for (_uint i = CPlayer::PART_UPPER; i < CPlayer::PART_END; ++i)
			static_cast<CParts*>(m_PlayerParts[i])->Set_AnimIndex(PLAYER_RUN_B);
		break;
	case Client::CPlayer::PLAYER_RM:
		m_pModelCom->Set_AnimationIndex(PLAYER_RUN_R);

		for (_uint i = CPlayer::PART_UPPER; i < CPlayer::PART_END; ++i)
			static_cast<CParts*>(m_PlayerParts[i])->Set_AnimIndex(PLAYER_RUN_R);
		break;
	case Client::CPlayer::PLAYER_LM:
		m_pModelCom->Set_AnimationIndex(PLAYER_RUN_L);

		for (_uint i = CPlayer::PART_UPPER; i < CPlayer::PART_END; ++i)
			static_cast<CParts*>(m_PlayerParts[i])->Set_AnimIndex(PLAYER_RUN_L);
		break;
	case Client::CPlayer::PLAYER_FR:
		m_pModelCom->Set_AnimationIndex(PLAYER_RUN_FR);

		for (_uint i = CPlayer::PART_UPPER; i < CPlayer::PART_END; ++i)
			static_cast<CParts*>(m_PlayerParts[i])->Set_AnimIndex(PLAYER_RUN_FR);
		break;
	case Client::CPlayer::PLAYER_FL:
		m_pModelCom->Set_AnimationIndex(PLAYER_RUN_FL);

		for (_uint i = CPlayer::PART_UPPER; i < CPlayer::PART_END; ++i)
			static_cast<CParts*>(m_PlayerParts[i])->Set_AnimIndex(PLAYER_RUN_FL);
		break;
	case Client::CPlayer::PLAYER_BR:
		m_pModelCom->Set_AnimationIndex(PLAYER_RUN_BR);

		for (_uint i = CPlayer::PART_UPPER; i < CPlayer::PART_END; ++i)
			static_cast<CParts*>(m_PlayerParts[i])->Set_AnimIndex(PLAYER_RUN_BR);
		break;
	case Client::CPlayer::PLAYER_BL:
		m_pModelCom->Set_AnimationIndex(PLAYER_RUN_BL);

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
	m_bBackDamaged = true;
}

void CPlayer::FrontDamagedToMonster()
{
	m_bFrontDamage = true;
}

void CPlayer::MonsterNormalAttack(_bool bAttack)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	for (auto& pMonster : pGameInstance->Find_LayerList(LEVEL_CHAP1, L"Layer_Monster"))
	{
		static_cast<CMonster*>(pMonster)->Set_PlayerAttackCommand(bAttack, 20);
	}
	RELEASE_INSTANCE(CGameInstance);
}

void CPlayer::MonsterSkill02(_bool bAttack)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	for (auto& pMonster : pGameInstance->Find_LayerList(LEVEL_CHAP1, L"Layer_Monster"))
	{
		static_cast<CMonster*>(pMonster)->Set_PlayerSkill02Command(bAttack, 35);
	}
	RELEASE_INSTANCE(CGameInstance);
}

void CPlayer::MonsterSkill04(_bool bAttack)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	for (auto& pMonster : pGameInstance->Find_LayerList(LEVEL_CHAP1, L"Layer_Monster"))
	{
		static_cast<CMonster*>(pMonster)->Set_PlayerSkiil04Command(bAttack , 40);
	}
	RELEASE_INSTANCE(CGameInstance);
}

_bool CPlayer::AnimFinishChecker(ANIMATION eAnim, _double FinishRate)
{
	return m_pModelCom->Get_IndexAnim(eAnim)->Get_PlayRate() >= FinishRate;
}

_bool CPlayer::AnimIntervalChecker(ANIMATION eAnim, _double StartRate, _double FinishRate)
{
	if (m_pModelCom->Get_IndexAnim(eAnim)->Get_PlayRate() > StartRate &&
		m_pModelCom->Get_IndexAnim(eAnim)->Get_PlayRate() <= FinishRate)
	{
		return true;
	}

	return false;
}

void CPlayer::AnimEditPlayTime(ANIMATION eAnim, _double PlayTime)
{
	m_pModelCom->Get_IndexAnim(eAnim)->Set_MulSecond(PlayTime);
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

	WeaponDesc.PivotMatrix = m_pModelCom->Get_PivotMatrix();
	WeaponDesc.pSocket = m_pModelCom->Get_BonePtr("Bip01-R-Hand");
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
	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Shader_VtxAnimModel"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Model_HumanF"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC			ColliderDesc;

	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(0.5f, 2.f, 0.5f);
	ColliderDesc.vCenter = _float3(0.f, 1.5f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_AABB"),
		(CComponent**)&m_pColliderCom[COLLTYPE_AABB], &ColliderDesc)))
		return E_FAIL;

	/* For.Com_OBB */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(0.5f, 2.f, 0.5f);
	ColliderDesc.vCenter = _float3(0.f, 1.5f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_OBB"),
		(CComponent**)&m_pColliderCom[COLLTYPE_OBB], &ColliderDesc)))
		return E_FAIL;

	/* For.Com_SPHERE */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(20.f, 20.f, 20.f);
	ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Collider_SPHERE"), TEXT("Com_SPHERE"),
		(CComponent**)&m_pColliderCom[COLLTYPE_SPHERE], &ColliderDesc)))
		return E_FAIL;

	/* For.Com_Navigation */
	CNavigation::NAVIDESC			NaviDesc;
	ZeroMemory(&NaviDesc, sizeof(CNavigation::NAVIDESC));

	NaviDesc.iCurrentIndex = 0;

	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"),
		(CComponent**)&m_pNavigationCom, &NaviDesc)))
		return E_FAIL;

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

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pNavigationCom);

	Safe_Release(m_pFSM);
}
