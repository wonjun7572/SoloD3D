#include "stdafx.h"
#include "..\public\MidoriFSM.h"
#include "GameInstance.h"
#include "Timer.h"
#include "Midori.h"
#include "Model.h"

HRESULT CMidoriFSM::Init(void * pArg)
{
	CFSMComponentBuilder builder = CFSMComponentBuilder()
		.InitState(TEXT("Idle"))

		.AddState(TEXT("Idle"))
		.Tick(this, &CMidoriFSM::Idle_Tick)
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
		.Tick(this, &CMidoriFSM::Run_Tick)
		.OnExit(this, &CMidoriFSM::Run_End)
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
		.Tick(this, &CMidoriFSM::Jump_Ing)
		.Transition(TEXT("Run"), FSM_TRANSITION(TEXT("Jump to Run"), [this]()
	{
		return m_pTarget->Get_IsJumping() == false;
	}))
		.Transition(TEXT("ExSkill"), FSM_TRANSITION(TEXT("Jump To ExSkill"), [this]()
	{
		return m_pTarget->Get_IsExSkilling() == true;
	}))

		.AddState(L"Attack")
		.OnStart(this, &CMidoriFSM::Attack_Start)
		.Tick(this, &CMidoriFSM::Attack_Ing)
		.OnExit(this, &CMidoriFSM::Attack_End)
		.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("Attack to Idle"), [this]()
	{
		return m_pTarget->Get_IsAttack() == false;
	}))
		.Transition(TEXT("ExSkill"), FSM_TRANSITION(TEXT("Attack To ExSkill"), [this]()
	{
		return m_pTarget->Get_IsExSkilling() == true;
	}))

		.AddState(L"ExSkill")
		.Tick(this, &CMidoriFSM::ExSkill_Ing)
		.OnExit(this, &CMidoriFSM::ExSkill_End)
		.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("ExSkill to Idle"), [this]()
	{
		return m_pTarget->Get_IsExSkilling() == false;
	}))

		.Build();

	__super::Init(&builder);

	return S_OK;
}

void CMidoriFSM::Idle_Tick(_double TimeDelta)
{
	if (m_pTarget->Get_StateType() == CMidori::STATE_STAND)
		m_pTarget->Get_ModelComponent()->Set_AnimationIndex(7);
	else if (m_pTarget->Get_StateType() == CMidori::STATE_NORMAL)
		m_pTarget->Get_ModelComponent()->Set_AnimationIndex(8);
	else if (m_pTarget->Get_StateType() == CMidori::STATE_KNEEL)
		m_pTarget->Get_ModelComponent()->Set_AnimationIndex(26);
}

void CMidoriFSM::Run_Tick(_double TimeDelta)
{
	m_pTarget->Get_ModelComponent()->Set_AnimationIndex(17);
}

void CMidoriFSM::Run_End()
{
	if (m_pTarget->Get_StateType() == CMidori::STATE_STAND)
	{
		m_pTarget->Get_ModelComponent()->Set_AnimationIndex(28);
	}
	else if (m_pTarget->Get_StateType() == CMidori::STATE_NORMAL)
	{
		m_pTarget->Get_ModelComponent()->Set_AnimationIndex(15);
	}
	else if (m_pTarget->Get_StateType() == CMidori::STATE_KNEEL)
	{
		m_pTarget->Get_ModelComponent()->Set_AnimationIndex(31);
	}
}

void CMidoriFSM::Jump_Ing(_double TimeDelta)
{
	m_pTarget->Get_ModelComponent()->Set_AnimationIndex(7);
}

void CMidoriFSM::Attack_Start()
{
	if (m_pTarget->Get_StateType() == CMidori::STATE_STAND)
	{
		m_pTarget->Get_ModelComponent()->Set_AnimationIndex(8);
	}
	else if (m_pTarget->Get_StateType() == CMidori::STATE_NORMAL)
	{
		m_pTarget->Get_ModelComponent()->Set_AnimationIndex(30);
	}
	else if (m_pTarget->Get_StateType() == CMidori::STATE_KNEEL)
	{
		m_pTarget->Get_ModelComponent()->Set_AnimationIndex(33);
	}
}

void CMidoriFSM::Attack_Ing(_double TimeDelta)
{
	if (m_pTarget->Get_StateType() == CMidori::STATE_STAND)
	{
		m_pTarget->Get_ModelComponent()->Set_AnimationIndex(9);
	}
	else if (m_pTarget->Get_StateType() == CMidori::STATE_NORMAL)
	{
		m_pTarget->Get_ModelComponent()->Set_AnimationIndex(26);
	}
	else if (m_pTarget->Get_StateType() == CMidori::STATE_KNEEL)
	{
		m_pTarget->Get_ModelComponent()->Set_AnimationIndex(12);
	}
}

void CMidoriFSM::Attack_End()
{
	if (m_pTarget->Get_StateType() == CMidori::STATE_STAND)
	{
		m_pTarget->Get_ModelComponent()->Set_AnimationIndex(10);
	}
	else if (m_pTarget->Get_StateType() == CMidori::STATE_NORMAL)
	{
		m_pTarget->Get_ModelComponent()->Set_AnimationIndex(18);
	}
	else if (m_pTarget->Get_StateType() == CMidori::STATE_KNEEL)
	{
		m_pTarget->Get_ModelComponent()->Set_AnimationIndex(34);
	}
}

void CMidoriFSM::ExSkill_Ing(_double TimeDelta)
{
	CGameInstance* pinst = GET_INSTANCE(CGameInstance);

	pinst->Find_Timer(L"Timer_60")->Set_SlowTimer(0.2f);

	RELEASE_INSTANCE(CGameInstance);
}

void CMidoriFSM::ExSkill_End()
{
	CGameInstance* pinst = GET_INSTANCE(CGameInstance);

	pinst->Find_Timer(L"Timer_60")->Set_SlowTimer(1.f);

	RELEASE_INSTANCE(CGameInstance);
}

CMidoriFSM * CMidoriFSM::Create(CMidori * pTarget)
{
	CMidoriFSM* inst = new CMidoriFSM();
	inst->m_pTarget = pTarget;
	inst->Init(nullptr);

	return inst;
}
