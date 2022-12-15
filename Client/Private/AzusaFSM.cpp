#include "stdafx.h"
#include "..\public\AzusaFSM.h"
#include "GameInstance.h"
#include "Timer.h"
#include "Azusa.h"
#include "Model.h"

HRESULT CAzusaFSM::Init(void * pArg)
{
	CFSMComponentBuilder builder = CFSMComponentBuilder()
		.InitState(TEXT("Idle"))

		.AddState(TEXT("Idle"))
		.Tick(this, &CAzusaFSM::Idle_Tick)
		.Transition(TEXT("ExSkill"), FSM_TRANSITION(TEXT("Idle To ExSkill"), [this]()
	{
		return m_pTarget->Get_IsExSkilling() == true;
	}))
		.Transition(TEXT("Run"), FSM_TRANSITION(TEXT("Idle To Run"), [this]()
	{
		return m_pTarget->Get_IsRun() == true;
	}))
		.Transition(TEXT("Attack"), FSM_TRANSITION(TEXT("Idle To Attack"), [this]()
	{
		return m_pTarget->Get_IsAttack() == true;
	}))

		.AddState(L"Run")
		.Tick(this, &CAzusaFSM::Run_Tick)
		.OnExit(this, &CAzusaFSM::Run_End)
		.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("Run to Idle"), [this]()
	{
		return m_pTarget->Get_IsRun() == false;
	}))
		.Transition(TEXT("ExSkill"), FSM_TRANSITION(TEXT("Run To ExSkill"), [this]()
	{
		return m_pTarget->Get_IsExSkilling() == true;
	}))
		.Transition(TEXT("Jump"), FSM_TRANSITION(TEXT("Run to Jump"), [this]()
	{
		return m_pTarget->Get_IsJumping() == true;
	}))

		.AddState(L"Jump")
		.Tick(this, &CAzusaFSM::Jump_Ing)
		.Transition(TEXT("Run"), FSM_TRANSITION(TEXT("Jump to Run"), [this]()
	{
		return m_pTarget->Get_IsJumping() == false;
	}))
		.Transition(TEXT("ExSkill"), FSM_TRANSITION(TEXT("Jump To ExSkill"), [this]()
	{
		return m_pTarget->Get_IsExSkilling() == true;
	}))

		.AddState(L"Attack")
		.OnStart(this, &CAzusaFSM::Attack_Start)
		.Tick(this, &CAzusaFSM::Attack_Ing)
		.OnExit(this, &CAzusaFSM::Attack_End)
		.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("Attack to Idle"), [this]()
	{
		return m_pTarget->Get_IsAttack() == false;
	}))
		.Transition(TEXT("ExSkill"), FSM_TRANSITION(TEXT("Attack To ExSkill"), [this]()
	{
		return m_pTarget->Get_IsExSkilling() == true;
	}))

		.AddState(L"ExSkill")
		.Tick(this, &CAzusaFSM::ExSkill_Ing)
		.OnExit(this, &CAzusaFSM::ExSkill_End)
		.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("ExSkill to Idle"), [this]()
	{
		return m_pTarget->Get_IsExSkilling() == false;
	}))

		.Build();

	__super::Init(&builder);

	return S_OK;
}

void CAzusaFSM::Idle_Tick(_double TimeDelta)
{
	if (m_pTarget->Get_StateType() == CAzusa::STATE_STAND)
		m_pTarget->Get_ModelComponent()->Set_AnimationIndex(7);
	else if (m_pTarget->Get_StateType() == CAzusa::STATE_NORMAL)
		m_pTarget->Get_ModelComponent()->Set_AnimationIndex(37);
	else if (m_pTarget->Get_StateType() == CAzusa::STATE_KNEEL)
		m_pTarget->Get_ModelComponent()->Set_AnimationIndex(32);
}

void CAzusaFSM::Run_Tick(_double TimeDelta)
{
	m_pTarget->Get_ModelComponent()->Set_AnimationIndex(6);
}

void CAzusaFSM::Run_End()
{
	if (m_pTarget->Get_StateType() == CAzusa::STATE_STAND)
	{
		m_pTarget->Get_ModelComponent()->Set_AnimationIndex(8);
	}
	else if (m_pTarget->Get_StateType() == CAzusa::STATE_NORMAL)
	{
		m_pTarget->Get_ModelComponent()->Set_AnimationIndex(29);
	}
	else if (m_pTarget->Get_StateType() == CAzusa::STATE_KNEEL)
	{
		m_pTarget->Get_ModelComponent()->Set_AnimationIndex(20);
	}
}

void CAzusaFSM::Jump_Ing(_double TimeDelta)
{
	m_pTarget->Get_ModelComponent()->Set_AnimationIndex(31);
}

void CAzusaFSM::Attack_Start()
{
	if (m_pTarget->Get_StateType() == CAzusa::STATE_STAND)
	{
		m_pTarget->Get_ModelComponent()->Set_AnimationIndex(23);
	}
	else if (m_pTarget->Get_StateType() == CAzusa::STATE_NORMAL)
	{
		m_pTarget->Get_ModelComponent()->Set_AnimationIndex(25);
	}
	else if (m_pTarget->Get_StateType() == CAzusa::STATE_KNEEL)
	{
		m_pTarget->Get_ModelComponent()->Set_AnimationIndex(18);
	}
}

void CAzusaFSM::Attack_Ing(_double TimeDelta)
{
	if (m_pTarget->Get_StateType() == CAzusa::STATE_STAND)
	{
		m_pTarget->Get_ModelComponent()->Set_AnimationIndex(4);
	}
	else if (m_pTarget->Get_StateType() == CAzusa::STATE_NORMAL)
	{
		m_pTarget->Get_ModelComponent()->Set_AnimationIndex(10);
	}
	else if (m_pTarget->Get_StateType() == CAzusa::STATE_KNEEL)
	{
		m_pTarget->Get_ModelComponent()->Set_AnimationIndex(17);
	}
}

void CAzusaFSM::Attack_End()
{
	if (m_pTarget->Get_StateType() == CAzusa::STATE_STAND)
	{
		m_pTarget->Get_ModelComponent()->Set_AnimationIndex(30);
	}
	else if (m_pTarget->Get_StateType() == CAzusa::STATE_NORMAL)
	{
		m_pTarget->Get_ModelComponent()->Set_AnimationIndex(2);
	}
	else if (m_pTarget->Get_StateType() == CAzusa::STATE_KNEEL)
	{
		m_pTarget->Get_ModelComponent()->Set_AnimationIndex(7);
	}
}

void CAzusaFSM::ExSkill_Ing(_double TimeDelta)
{
	CGameInstance* pinst = GET_INSTANCE(CGameInstance);

	pinst->Find_Timer(L"Timer_60")->Set_SlowTimer(0.2f);

	RELEASE_INSTANCE(CGameInstance);
}

void CAzusaFSM::ExSkill_End()
{
	CGameInstance* pinst = GET_INSTANCE(CGameInstance);

	pinst->Find_Timer(L"Timer_60")->Set_SlowTimer(1.f);

	RELEASE_INSTANCE(CGameInstance);
}

CAzusaFSM * CAzusaFSM::Create(CAzusa * pTarget)
{
	CAzusaFSM* inst = new CAzusaFSM();
	inst->m_pTarget = pTarget;
	inst->Init(nullptr);

	return inst;
}
