#include "stdafx.h"
#include "..\Public\DemonFSM.h"
#include "GameInstance.h"
#include "Demon.h"
#include "Model.h"

HRESULT CDemonFSM::Init(void * pArg)
{
	CFSMComponentBuilder builder = CFSMComponentBuilder()
		.InitState(TEXT("Idle"))

		.AddState(TEXT("Idle"))
		.Tick(this, &CDemonFSM::Idle_Tick)
		.Transition(TEXT("Walk"), FSM_TRANSITION(TEXT("Idle To Walk"), [this]()
	{
		return m_pTarget->IsWalking() == true;
	}))
		.Transition(TEXT("Run"), FSM_TRANSITION(TEXT("Idle To Run"), [this]()
	{
		return m_pTarget->IsRunning() == true;
	}))
		.Transition(TEXT("Attack"), FSM_TRANSITION(TEXT("Idle To Attack"), [this]()
	{
		return m_pTarget->IsAttacking() == true;
	}))
		.Transition(TEXT("Hit"), FSM_TRANSITION(TEXT("Idle to Hit"), [this]()
	{
		return m_pTarget->IsHitting() == true;
	}))

		.AddState(L"Run")
		.Tick(this, &CDemonFSM::Run_Tick)
		.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("Run to Idle"), [this]()
	{
		return m_pTarget->IsRunning() == false;
	}))
		.Transition(TEXT("Hit"), FSM_TRANSITION(TEXT("Run to Hit"), [this]()
	{
		return m_pTarget->IsHitting() == true;
	}))
		.Transition(TEXT("Attack"), FSM_TRANSITION(TEXT("Run to Attack"), [this]()
	{
		return m_pTarget->IsAttacking() == true;
	}))
		
		.AddState(L"Walk")
		.Tick(this, &CDemonFSM::Walk_Tick)
		.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("Walk to Idle"), [this]()
	{
		return m_pTarget->IsWalking() == false;
	}))
		.Transition(TEXT("Hit"), FSM_TRANSITION(TEXT("Walk to Hit"), [this]()
	{
		return m_pTarget->IsHitting() == true;
	}))

		.AddState(L"Attack")
		.Tick(this, &CDemonFSM::Attack_Ing)
		.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("Attack to Idle"), [this]()
	{
		return m_pTarget->IsAttacking() == false;
	}))
		.Transition(TEXT("Hit"), FSM_TRANSITION(TEXT("Attack to Hit"), [this]()
	{
		return m_pTarget->IsHitting() == true;
	}))


		.AddState(L"Hit")
		.Tick(this, &CDemonFSM::Hit_Ing)
		.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("Hit to Idle"), [this]()
	{
		return m_pTarget->IsHitting() == false;
	}))
		.Transition(TEXT("KnockDown"), FSM_TRANSITION(TEXT("Hit to KnockDown"), [this]()
	{
		return m_pTarget->IsKnockDowning() == true;
	}))
		
		.AddState(L"KnockDown")
		.Tick(this, &CDemonFSM::KnockDown_Ing)
		.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("KnockDown to Idle"), [this]()
	{
		return m_pTarget->IsKnockDowning() == false;
	}))

		.Build();

	__super::Init(&builder);

	return S_OK;
}

void CDemonFSM::Idle_Tick(_double TimeDelta)
{
	m_pTarget->Get_ModelCom()->Set_AnimationIndex(18);
	// 싸우는중이라면?
	//m_pTarget->Get_ModelCom()->Set_AnimationIndex(16);
}

void CDemonFSM::Walk_Tick(_double TimeDelta)
{
	m_pTarget->Get_ModelCom()->Set_AnimationIndex(28);
}

void CDemonFSM::Run_Tick(_double TimeDelta)
{
	m_pTarget->Get_ModelCom()->Set_AnimationIndex(20);
}

void CDemonFSM::KnockDown_Ing(_double TimeDelta)
{
	m_KnockDownTime += TimeDelta;

	if (m_KnockDownTime > 2)
	{
		m_KnockDownTime = 0.0;
		m_pTarget->Set_IsKnockDowning(false);
	}

	m_pTarget->Get_ModelCom()->Set_AnimationIndex(14);
}

void CDemonFSM::Hit_Ing(_double TimeDelta)
{
	if (m_pTarget->Get_State() == CDemon::PLAYER_FRONT)
	{
		m_pTarget->Get_ModelCom()->Set_AnimationIndex(7);
	}
	else
	{
		m_pTarget->Get_ModelCom()->Set_AnimationIndex(6);
	}
}

void CDemonFSM::Attack_Ing(_double TimeDelta)
{
	m_pTarget->Get_ModelCom()->Set_AnimationIndex(2);

	//m_pTarget->Get_ModelCom()->Set_AnimationIndex(3);
}

CDemonFSM * CDemonFSM::Create(CDemon * pTarget)
{
	CDemonFSM* inst = new CDemonFSM();
	inst->m_pTarget = pTarget;
	inst->Init(nullptr);

	return inst;
}
