#include "stdafx.h"
#include "..\Public\MomoiFSM.h"
#include "GameInstance.h"
#include "Timer.h"
#include "Momoi.h"
#include "Model.h"

HRESULT CMomoiFSM::Init(void * pArg)
{
	CFSMComponentBuilder builder = CFSMComponentBuilder()
		.InitState(TEXT("Idle"))

		.AddState(TEXT("Idle"))
		.Tick(this, &CMomoiFSM::Idle_Tick)
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
		.Tick(this, &CMomoiFSM::Run_Tick)
		.OnExit(this, &CMomoiFSM::Run_End)
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
		.Tick(this, &CMomoiFSM::Jump_Ing)
		.Transition(TEXT("Run"), FSM_TRANSITION(TEXT("Jump to Run"), [this]()
	{
		return m_pTarget->Get_IsJumping() == false;
	}))
		.Transition(TEXT("ExSkill"), FSM_TRANSITION(TEXT("Jump To ExSkill"), [this]()
	{
		return m_pTarget->Get_IsExSkilling() == true;
	}))

		.AddState(L"Attack")
		.OnStart(this, &CMomoiFSM::Attack_Start)
		.Tick(this, &CMomoiFSM::Attack_Ing)
		.OnExit(this, &CMomoiFSM::Attack_End)
		.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("Attack to Idle"), [this]()
	{
		return m_pTarget->Get_IsAttack() == false;
	}))
		.Transition(TEXT("ExSkill"), FSM_TRANSITION(TEXT("Attack To ExSkill"), [this]()
	{
		return m_pTarget->Get_IsExSkilling() == true;
	}))

		.AddState(L"ExSkill")
		.Tick(this, &CMomoiFSM::ExSkill_Ing)
		.OnExit(this, &CMomoiFSM::ExSkill_End)
		.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("ExSkill to Idle"), [this]()
	{
		return m_pTarget->Get_IsExSkilling() == false;
	}))

		.Build();

	__super::Init(&builder);

	return S_OK;
}

void CMomoiFSM::Idle_Tick(_double TimeDelta)
{
	if (m_pTarget->Get_StateType() == CMomoi::STATE_STAND)
		m_pTarget->Get_ModelComponent()->Set_AnimationIndex(7);
	else if (m_pTarget->Get_StateType() == CMomoi::STATE_NORMAL)
		m_pTarget->Get_ModelComponent()->Set_AnimationIndex(14);
	else if (m_pTarget->Get_StateType() == CMomoi::STATE_KNEEL)
		m_pTarget->Get_ModelComponent()->Set_AnimationIndex(26);
}

void CMomoiFSM::Run_Tick(_double TimeDelta)
{
	m_pTarget->Get_ModelComponent()->Set_AnimationIndex(21);
}

void CMomoiFSM::Run_End()
{
	if (m_pTarget->Get_StateType() == CMomoi::STATE_STAND)
	{
		m_pTarget->Get_ModelComponent()->Set_AnimationIndex(22);
	}
	else if (m_pTarget->Get_StateType() == CMomoi::STATE_NORMAL)
	{
		m_pTarget->Get_ModelComponent()->Set_AnimationIndex(23);
	}
	else if (m_pTarget->Get_StateType() == CMomoi::STATE_KNEEL)
	{
		m_pTarget->Get_ModelComponent()->Set_AnimationIndex(24);
	}
}

void CMomoiFSM::Jump_Ing(_double TimeDelta)
{
	m_pTarget->Get_ModelComponent()->Set_AnimationIndex(20);
}

void CMomoiFSM::Attack_Start()
{
	if (m_pTarget->Get_StateType() == CMomoi::STATE_STAND)
	{
		m_pTarget->Get_ModelComponent()->Set_AnimationIndex(8);
	}
	else if (m_pTarget->Get_StateType() == CMomoi::STATE_NORMAL)
	{
		m_pTarget->Get_ModelComponent()->Set_AnimationIndex(16);
	}
	else if (m_pTarget->Get_StateType() == CMomoi::STATE_KNEEL)
	{
		m_pTarget->Get_ModelComponent()->Set_AnimationIndex(27);
	}
}

void CMomoiFSM::Attack_Ing(_double TimeDelta)
{
	if (m_pTarget->Get_StateType() == CMomoi::STATE_STAND)
	{
		m_pTarget->Get_ModelComponent()->Set_AnimationIndex(9);
	}
	else if (m_pTarget->Get_StateType() == CMomoi::STATE_NORMAL)
	{
		m_pTarget->Get_ModelComponent()->Set_AnimationIndex(17);
	}
	else if (m_pTarget->Get_StateType() == CMomoi::STATE_KNEEL)
	{
		m_pTarget->Get_ModelComponent()->Set_AnimationIndex(28);
	}
}

void CMomoiFSM::Attack_End()
{
	if (m_pTarget->Get_StateType() == CMomoi::STATE_STAND)
	{
		m_pTarget->Get_ModelComponent()->Set_AnimationIndex(10);
	}
	else if (m_pTarget->Get_StateType() == CMomoi::STATE_NORMAL)
	{
		m_pTarget->Get_ModelComponent()->Set_AnimationIndex(18);
	}
	else if (m_pTarget->Get_StateType() == CMomoi::STATE_KNEEL)
	{
		m_pTarget->Get_ModelComponent()->Set_AnimationIndex(29);
	}
}

void CMomoiFSM::ExSkill_Ing(_double TimeDelta)
{
	CGameInstance* pinst = GET_INSTANCE(CGameInstance);

	pinst->Find_Timer(L"Timer_60")->Set_SlowTimer(0.2f);

	RELEASE_INSTANCE(CGameInstance);
}

void CMomoiFSM::ExSkill_End()
{
	CGameInstance* pinst = GET_INSTANCE(CGameInstance);

	pinst->Find_Timer(L"Timer_60")->Set_SlowTimer(1.f);

	RELEASE_INSTANCE(CGameInstance);
}

CMomoiFSM * CMomoiFSM::Create(CMomoi * pTarget)
{
	CMomoiFSM* inst = new CMomoiFSM();
	inst->m_pTarget = pTarget;
	inst->Init(nullptr);

	return inst;
}
