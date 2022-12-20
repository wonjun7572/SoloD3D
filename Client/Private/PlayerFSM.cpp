#include "stdafx.h"
#include "..\Public\PlayerFSM.h"
#include "GameInstance.h"
#include "Player.h"
#include "Model.h"

HRESULT CPlayerFSM::Init(void * pArg)
{
	CFSMComponentBuilder builder = CFSMComponentBuilder()
		.InitState(TEXT("Idle"))

		.AddState(TEXT("Idle"))
		.Tick(this, &CPlayerFSM::Idle_Tick)
		.Transition(TEXT("Walk"), FSM_TRANSITION(TEXT("Idle To Walk"), [this]()
	{
		return m_pTarget->IsWalking() == true;
	}))
		.Transition(TEXT("Run"), FSM_TRANSITION(TEXT("Idle To Run"), [this]()
	{
		return m_pTarget->IsRunning() == true;
	}))
		.Transition(TEXT("Jump"), FSM_TRANSITION(TEXT("Run to Jump"), [this]()
	{
		return m_pTarget->IsJumping() == true;
	}))
		.Transition(TEXT("Attack_0"), FSM_TRANSITION(TEXT("Idle To Attack_0"), [this]()
	{
		return m_pTarget->IsAttack_0() == true;
	}))

		.AddState(L"Run")
		.Tick(this, &CPlayerFSM::Run_Tick)
		.OnExit(this, &CPlayerFSM::Run_End)
		.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("Run to Idle"), [this]()
	{
		return m_pTarget->IsRunning() == false;
	}))
		.Transition(TEXT("Jump"), FSM_TRANSITION(TEXT("Run to Jump"), [this]()
	{
		return m_pTarget->IsJumping() == true;
	}))

		.AddState(L"Jump")
		.Tick(this, &CPlayerFSM::Jump_Ing)
		.Transition(TEXT("Run"), FSM_TRANSITION(TEXT("Jump to Run"), [this]()
	{
		return m_pTarget->IsJumping() == false;
	}))
		.AddState(L"Attack_0")
		.Tick(this, &CPlayerFSM::Attack_Ing)
		.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("Attack_0 to Idle"), [this]()
	{
		return m_pTarget->IsAttack_0() == false;
	}))
		.Transition(TEXT("Attack_1"), FSM_TRANSITION(TEXT("Attack_0 to Attack_1"), [this]()
	{
		return m_pTarget->IsAttack_1() == true;
	}))

		.AddState(L"Attack_1")
		.Tick(this, &CPlayerFSM::Attack1_Ing)
		.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("Attack_1 to Idle"), [this]()
	{
		return m_pTarget->IsAttack_1() == false;
	}))
		.Transition(TEXT("Attack_2"), FSM_TRANSITION(TEXT("Attack_1 to Attack_2"), [this]()
	{
		return m_pTarget->IsAttack_2() == true;
	}))

		.AddState(L"Attack_2")
		.Tick(this, &CPlayerFSM::Attack2_Ing)
		.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("Attack_2 to Idle"), [this]()
	{
		return m_pTarget->IsAttack_2() == false;
	}))

		.AddState(L"Walk")
		.Tick(this, &CPlayerFSM::Walk_Tick)
		.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("Walk to Idle"), [this]()
	{
		return m_pTarget->IsWalking() == false;
	}))
		.Transition(TEXT("Jump"), FSM_TRANSITION(TEXT("Walk to Jump"), [this]()
	{
		return m_pTarget->IsJumping() == true;
	}))

		.Build();

	__super::Init(&builder);

	return S_OK;
}

void CPlayerFSM::Idle_Tick(_double TimeDelta)
{
	m_pTarget->Get_ModelCom()->Set_AnimationIndex(23);
}

void CPlayerFSM::Walk_Tick(_double TimeDelta)
{
	switch (m_pTarget->Get_State())
	{
	case CPlayer::FORWARD:
		m_pTarget->Get_ModelCom()->Set_AnimationIndex(103);
		m_pTarget->Get_TransformCom()->Go_Straight(TimeDelta * 0.7);
		break;
	case CPlayer::BACK:
		m_pTarget->Get_ModelCom()->Set_AnimationIndex(28);
		m_pTarget->Get_TransformCom()->Go_Backward(TimeDelta * 0.7);
		break;
	case CPlayer::LEFT:
		m_pTarget->Get_ModelCom()->Set_AnimationIndex(43);
		m_pTarget->Get_TransformCom()->Go_Left(TimeDelta * 0.7);
		break;
	case CPlayer::RIGHT:
		m_pTarget->Get_ModelCom()->Set_AnimationIndex(44);
		m_pTarget->Get_TransformCom()->Go_Right(TimeDelta * 0.7);
		break;
	case CPlayer::LF:
		m_pTarget->Get_ModelCom()->Set_AnimationIndex(35);
		m_pTarget->Get_TransformCom()->Go_Left(TimeDelta * 0.7);
		m_pTarget->Get_TransformCom()->Go_Straight(TimeDelta * 0.7);
		break;
	case CPlayer::RF:
		m_pTarget->Get_ModelCom()->Set_AnimationIndex(36);
		m_pTarget->Get_TransformCom()->Go_Right(TimeDelta * 0.7);
		m_pTarget->Get_TransformCom()->Go_Straight(TimeDelta * 0.7);
		break;
	case CPlayer::LB:
		m_pTarget->Get_ModelCom()->Set_AnimationIndex(29);
		m_pTarget->Get_TransformCom()->Go_Left(TimeDelta * 0.7);
		m_pTarget->Get_TransformCom()->Go_Backward(TimeDelta * 0.7);
		break;
	case CPlayer::RB:
		m_pTarget->Get_ModelCom()->Set_AnimationIndex(30);
		m_pTarget->Get_TransformCom()->Go_Right(TimeDelta * 0.7);
		m_pTarget->Get_TransformCom()->Go_Backward(TimeDelta * 0.7);
		break;
	default:
		break;
	}
}

void CPlayerFSM::Run_Tick(_double TimeDelta)
{
	switch (m_pTarget->Get_State())
	{
	case CPlayer::FORWARD:
		m_pTarget->Get_ModelCom()->Set_AnimationIndex(32);
		m_pTarget->Get_TransformCom()->Go_Straight(TimeDelta * 1.2);
		break;
	case CPlayer::LF:
		m_pTarget->Get_ModelCom()->Set_AnimationIndex(35);
		m_pTarget->Get_TransformCom()->Go_Left(TimeDelta * 1.2);
		m_pTarget->Get_TransformCom()->Go_Straight(TimeDelta * 1.2);
		break;
	case CPlayer::RF:
		m_pTarget->Get_ModelCom()->Set_AnimationIndex(36);
		m_pTarget->Get_TransformCom()->Go_Right(TimeDelta * 1.2);
		m_pTarget->Get_TransformCom()->Go_Straight(TimeDelta * 1.2);
		break;
	}
}

void CPlayerFSM::Run_End()
{
	m_pTarget->Get_ModelCom()->Set_AnimationIndex(31);
}

void CPlayerFSM::Jump_Ing(_double TimeDelta)
{
	m_pTarget->Get_ModelCom()->Set_AnimationIndex(25);
}

void CPlayerFSM::Attack_Ing(_double TimeDelta)
{
	m_pTarget->Set_State(CPlayer::ATTACK);
	m_pTarget->Set_CamTurn(false);
	m_pTarget->Get_ModelCom()->Set_AnimationIndex(2);
}

void CPlayerFSM::Attack1_Ing(_double TimeDelta)
{
	m_pTarget->Set_State(CPlayer::ATTACK);
	m_pTarget->Set_CamTurn(false);
	m_pTarget->Get_ModelCom()->Set_AnimationIndex(4);
}

void CPlayerFSM::Attack2_Ing(_double TimeDelta)
{
	m_pTarget->Set_State(CPlayer::ATTACK);
	m_pTarget->Set_CamTurn(false);
	m_pTarget->Get_ModelCom()->Set_AnimationIndex(6);
}

CPlayerFSM * CPlayerFSM::Create(CPlayer * pTarget)
{
	CPlayerFSM* inst = new CPlayerFSM();
	inst->m_pTarget = pTarget;
	inst->Init(nullptr);

	return inst;
}
