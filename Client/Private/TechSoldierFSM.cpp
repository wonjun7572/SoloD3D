#include "stdafx.h"
#include "..\Public\TechSoldierFSM.h"
#include "GameInstance.h"
#include "Timer.h"
#include "TechSoldier.h"
#include "Model.h"

HRESULT CTechSoldierFSM::Init(void * pArg)
{
	CFSMComponentBuilder builder = CFSMComponentBuilder()
		.InitState(TEXT("Appear"))

		.AddState(TEXT("Appear")) // 적이 어느정도 가까이 왔을때 실행
		.OnStart(this, &CTechSoldierFSM::Apper_Start)
		.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("Appear To Idle"), [this]()
	{
		return true;
	}))
		.AddState(TEXT("Idle"))
		.Tick(this, &CTechSoldierFSM::Idle_Tick)
		.Transition(TEXT("Run"), FSM_TRANSITION(TEXT("Idle To Run"), [this]()
	{
		return m_pTarget->Get_IsRun() == true;
	}))
		.AddState(L"Run")
		.Tick(this, &CTechSoldierFSM::Move_Tick)
		.OnExit(this, &CTechSoldierFSM::Move_End)
		.Transition(TEXT("Attack"), FSM_TRANSITION(TEXT("Run to Attack"), [this]()
	{
		return m_pTarget->Get_IsAttack() == true;
	}))
		.AddState(L"Attack")
		.OnStart(this, &CTechSoldierFSM::Attack_Start)
		.Tick(this,&CTechSoldierFSM::Attack_Tick)
		.Transition(TEXT("Death"), FSM_TRANSITION(TEXT("Attack to Deatch"), [this]()
	{
		return m_pTarget->Get_IsDeath() == true;
	}))
		// 여기서 만약에 플레이어가 이기는 상황이 만들어진다면? 다른 애니메이션 추가 해야함
		.AddState(L"Death")
		.Tick(this, &CTechSoldierFSM::Death_Tick)
		.Build();

	__super::Init(&builder);

	return S_OK;
}

void CTechSoldierFSM::Apper_Start()
{
	m_pTarget->Get_ModelComponent()->Set_AnimationIndex(14);
}

void CTechSoldierFSM::Idle_Tick(_double TimeDelta)
{
	m_pTarget->Get_ModelComponent()->Set_AnimationIndex(8);
}

void CTechSoldierFSM::Move_Tick(_double TimeDelta)
{
	m_pTarget->Get_ModelComponent()->Set_AnimationIndex(13);
	m_pTarget->Get_TransformCom()->Go_Straight(TimeDelta);
}

void CTechSoldierFSM::Move_End()
{
	m_pTarget->Get_ModelComponent()->Set_AnimationIndex(2);
}

void CTechSoldierFSM::Attack_Start()
{
	m_pTarget->Get_ModelComponent()->Set_AnimationIndex(0);
}

void CTechSoldierFSM::Attack_Tick(_double TimeDelta)
{
	// 중간에 가끔 총알 장전할까? 7번 인덱스
	m_pTarget->Get_ModelComponent()->Set_AnimationIndex(6);
}

void CTechSoldierFSM::Death_Tick(_double TimeDelta)
{
	m_pTarget->Get_ModelComponent()->Set_AnimationIndex(9);
}

CTechSoldierFSM * CTechSoldierFSM::Create(CTechSoldier * pTarget)
{
	CTechSoldierFSM* inst = new CTechSoldierFSM();
	inst->m_pTarget = pTarget;
	inst->Init(nullptr);

	return inst;
}
