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
		return m_pTarget->IsRunning() == true || m_pTarget->IsAttack() == true;
	}))
		.Transition(TEXT("Jump"), FSM_TRANSITION(TEXT("Idle to Jump"), [this]()
	{
		return m_pTarget->IsJumping() == true;
	}))
		
		.AddState(L"Run")
		.Tick(this, &CPlayerFSM::Run_Tick)
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
	m_pTarget->Get_ModelCom()->Set_AnimationIndex(24);
}

void CPlayerFSM::Walk_Tick(_double TimeDelta)
{
	switch (m_pTarget->Get_State())
	{
	case CPlayer::PLAYER_FM:
		m_pTarget->Get_ModelCom()->TurnOn_UpperAnim(false);
		m_pTarget->Get_ModelCom()->Set_AnimationIndex(115);
		m_pTarget->Get_TransformCom()->Go_Straight(TimeDelta * 0.7);
		break;
	case CPlayer::PLAYER_BM:
		m_pTarget->Get_ModelCom()->TurnOn_UpperAnim(false);
		m_pTarget->Get_ModelCom()->Set_AnimationIndex(114);
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
	case CPlayer::PLAYER_FM:
		m_pTarget->Get_ModelCom()->TurnOn_UpperAnim(false);
		m_pTarget->Get_ModelCom()->Set_AnimationIndex(34);
		m_pTarget->Get_TransformCom()->Go_Straight(TimeDelta);
		break;
	case CPlayer::PLAYER_BM:
		m_pTarget->Get_ModelCom()->TurnOn_UpperAnim(false);
		m_pTarget->Get_ModelCom()->Set_AnimationIndex(30);
		m_pTarget->Get_TransformCom()->Go_Backward(TimeDelta);
		break;
	case CPlayer::PLAYER_LM:
		m_pTarget->Get_ModelCom()->TurnOn_UpperAnim(false);
		m_pTarget->Get_ModelCom()->Set_AnimationIndex(45);
		m_pTarget->Get_TransformCom()->Go_Left(TimeDelta);
		break;
	case CPlayer::PLAYER_RM:
		m_pTarget->Get_ModelCom()->TurnOn_UpperAnim(false);
		m_pTarget->Get_ModelCom()->Set_AnimationIndex(46);
		m_pTarget->Get_TransformCom()->Go_Right(TimeDelta);
		break;
	case CPlayer::PLAYER_FL:
		m_pTarget->Get_ModelCom()->TurnOn_UpperAnim(false);
		m_pTarget->Get_ModelCom()->Set_AnimationIndex(37);
		m_pTarget->Get_TransformCom()->Go_Left(TimeDelta);
		m_pTarget->Get_TransformCom()->Go_Straight(TimeDelta);
		break;
	case CPlayer::PLAYER_FR:
		m_pTarget->Get_ModelCom()->TurnOn_UpperAnim(false);
		m_pTarget->Get_ModelCom()->Set_AnimationIndex(38);
		m_pTarget->Get_TransformCom()->Go_Right(TimeDelta);
		m_pTarget->Get_TransformCom()->Go_Straight(TimeDelta);
		break;
	case CPlayer::PLAYER_BL:
		m_pTarget->Get_ModelCom()->TurnOn_UpperAnim(false);
		m_pTarget->Get_ModelCom()->Set_AnimationIndex(31);
		m_pTarget->Get_TransformCom()->Go_Left(TimeDelta);
		m_pTarget->Get_TransformCom()->Go_Backward(TimeDelta);
		break;
	case CPlayer::PLAYER_BR:
		m_pTarget->Get_ModelCom()->TurnOn_UpperAnim(false);
		m_pTarget->Get_ModelCom()->Set_AnimationIndex(32);
		m_pTarget->Get_TransformCom()->Go_Right(TimeDelta);
		m_pTarget->Get_TransformCom()->Go_Backward(TimeDelta);
		break;
	case CPlayer::PLAYER_ATTACK1:
		m_pTarget->Get_ModelCom()->TurnOn_UpperAnim(false);
		m_pTarget->Get_ModelCom()->Set_AnimationIndex(2);
		break;
	case CPlayer::PLAYER_ATTACK2:
		m_pTarget->Get_ModelCom()->TurnOn_UpperAnim(false);
		m_pTarget->Get_ModelCom()->Set_AnimationIndex(4);
		break;
	case CPlayer::PLAYER_ATTACK3:
		m_pTarget->Get_ModelCom()->TurnOn_UpperAnim(false);
		m_pTarget->Get_ModelCom()->Set_AnimationIndex(6);
		break;
	}
}

void CPlayerFSM::Jump_Ing(_double TimeDelta)
{
	//m_pTarget->Get_ModelCom()->Set_AnimationIndex(25);
}

CPlayerFSM * CPlayerFSM::Create(CPlayer * pTarget)
{
	CPlayerFSM* inst = new CPlayerFSM();
	inst->m_pTarget = pTarget;
	inst->Init(nullptr);

	return inst;
}
