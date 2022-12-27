#include "stdafx.h"
#include "..\Public\Demon_State.h"
#include "Demon.h"
#include "GameInstance.h"

CDemon_State::CDemon_State()
{
}

HRESULT CDemon_State::Initialize(CDemon * pDemon)
{
	m_pDemon = pDemon;

	if (FAILED(Setup_State_Idle()))		return E_FAIL;
	if (FAILED(Setup_State_Walk()))		return E_FAIL;
	if (FAILED(Setup_State_Run()))		return E_FAIL;
	if (FAILED(Setup_State_Attack()))	return E_FAIL;
	if (FAILED(Setup_State_Damaged()))	return E_FAIL;
	return S_OK;
}

HRESULT CDemon_State::Setup_State_Idle()
{
	if (m_pDemon->m_pState == nullptr)
		return E_FAIL;

	m_pDemon->m_pState
		->Set_Root(TEXT("STATE::IDLE"))
		.Add_State(TEXT("STATE::IDLE"))
		.Init_Start(this, &CDemon_State::Start_Idle)
		.Init_Tick(this, &CDemon_State::Tick_Idle)
		.Init_End(this, &CDemon_State::End_Common)
		.Init_Changer(TEXT("STATE::WALK_F"), this, &CDemon_State::IsWalk_F)
		.Init_Changer(TEXT("STATE::RUN_F"), this, &CDemon_State::IsRun_F)
		.Init_Changer(TEXT("STATE::ATTACK1"), this, &CDemon_State::IsAttack1)
		.Init_Changer(TEXT("STATE::ATTACK2"), this, &CDemon_State::IsAttack2)
		.Init_Changer(TEXT("STATE::HIT_DOWN"), this, &CDemon_State::IsHitDown)
		.Init_Changer(TEXT("STATE::DAMAGED_B"), this, &CDemon_State::IsDamaged_B)
		.Init_Changer(TEXT("STATE::DAMAGED_F"), this, &CDemon_State::IsDamaged_F)

.Finish_Setting();

	return S_OK;
}

HRESULT CDemon_State::Setup_State_Walk()
{
	if (m_pDemon->m_pState == nullptr)
		return E_FAIL;

	m_pDemon->m_pState
		->Add_State(TEXT("STATE::WALK_F"))
		.Init_Start(this, &CDemon_State::Start_Walk_F)
		.Init_Tick(this, &CDemon_State::Tick_Walk_F)
		.Init_End(this, &CDemon_State::End_Common)
		.Init_Changer(TEXT("STATE::RUN_F"), this, &CDemon_State::IsRun_F)
		.Init_Changer(TEXT("STATE::ATTACK1"), this, &CDemon_State::IsAttack1)
		.Init_Changer(TEXT("STATE::ATTACK2"), this, &CDemon_State::IsAttack2)
		.Init_Changer(TEXT("STATE::HIT_DOWN"), this, &CDemon_State::IsHitDown)
		.Init_Changer(TEXT("STATE::DAMAGED_B"), this, &CDemon_State::IsDamaged_B)
		.Init_Changer(TEXT("STATE::DAMAGED_F"), this, &CDemon_State::IsDamaged_F)
		.Init_Changer(TEXT("STATE::IDLE"),this ,&CDemon_State::IsIdle)

		.Finish_Setting();

	return S_OK;
}

HRESULT CDemon_State::Setup_State_Run()
{
	if (m_pDemon->m_pState == nullptr)
		return E_FAIL;

	m_pDemon->m_pState
		->Add_State(TEXT("STATE::RUN_F"))
		.Init_Start(this, &CDemon_State::Start_Run_F)
		.Init_Tick(this, &CDemon_State::Tick_Run_F)
		.Init_End(this, &CDemon_State::End_Common)
		.Init_Changer(TEXT("STATE::WALK_F"), this, &CDemon_State::IsWalk_F)
		.Init_Changer(TEXT("STATE::ATTACK1"), this, &CDemon_State::IsAttack1)
		.Init_Changer(TEXT("STATE::ATTACK2"), this, &CDemon_State::IsAttack2)
		.Init_Changer(TEXT("STATE::HIT_DOWN"), this, &CDemon_State::IsHitDown)
		.Init_Changer(TEXT("STATE::DAMAGED_B"), this, &CDemon_State::IsDamaged_B)
		.Init_Changer(TEXT("STATE::DAMAGED_F"), this, &CDemon_State::IsDamaged_F)
		.Init_Changer(TEXT("STATE::IDLE"), this, &CDemon_State::IsIdle)

		.Finish_Setting();

	return S_OK;
}

HRESULT CDemon_State::Setup_State_Attack()
{
	if (m_pDemon->m_pState == nullptr)
		return E_FAIL;

	m_pDemon->m_pState
		->Add_State(TEXT("STATE::ATTACK1"))
		.Init_Start(this, &CDemon_State::Start_Attack1)
		.Init_Tick(this, &CDemon_State::Tick_Attack1)
		.Init_End(this, &CDemon_State::End_Common)
		.Init_Changer(TEXT("STATE::WALK_F"), this, &CDemon_State::IsWalk_F)
		.Init_Changer(TEXT("STATE::RUN_F"), this, &CDemon_State::IsRun_F)
		.Init_Changer(TEXT("STATE::ATTACK2"), this, &CDemon_State::IsAttack2)
		.Init_Changer(TEXT("STATE::HIT_DOWN"), this, &CDemon_State::IsHitDown)
		.Init_Changer(TEXT("STATE::DAMAGED_B"), this, &CDemon_State::IsDamaged_B)
		.Init_Changer(TEXT("STATE::DAMAGED_F"), this, &CDemon_State::IsDamaged_F)
		.Init_Changer(TEXT("STATE::IDLE"), this, &CDemon_State::IsIdle)

		.Finish_Setting();

	m_pDemon->m_pState
		->Add_State(TEXT("STATE::ATTACK2"))
		.Init_Start(this, &CDemon_State::Start_Attack2)
		.Init_Tick(this, &CDemon_State::Tick_Attack2)
		.Init_End(this, &CDemon_State::End_Common)
		.Init_Changer(TEXT("STATE::WALK_F"), this, &CDemon_State::IsWalk_F)
		.Init_Changer(TEXT("STATE::RUN_F"), this, &CDemon_State::IsRun_F)
		.Init_Changer(TEXT("STATE::ATTACK1"), this, &CDemon_State::IsAttack1)
		.Init_Changer(TEXT("STATE::HIT_DOWN"),this,&CDemon_State::IsHitDown)
		.Init_Changer(TEXT("STATE::DAMAGED_B"), this, &CDemon_State::IsDamaged_B)
		.Init_Changer(TEXT("STATE::DAMAGED_F"), this, &CDemon_State::IsDamaged_F)
		.Init_Changer(TEXT("STATE::IDLE"), this, &CDemon_State::IsIdle)

		.Finish_Setting();

	return S_OK;
}

HRESULT CDemon_State::Setup_State_Damaged()
{
	if (m_pDemon->m_pState == nullptr)
		return E_FAIL;

	m_pDemon->m_pState
		->Add_State(TEXT("STATE::DAMAGED_B"))
		.Init_Start(this, &CDemon_State::Start_Damaged_B)
		.Init_Tick(this, &CDemon_State::Tick_Damaged_B)
		.Init_End(this, &CDemon_State::End_Common)
		.Init_Changer(TEXT("STATE::WALK_F"), this, &CDemon_State::IsWalk_F)
		.Init_Changer(TEXT("STATE::RUN_F"), this, &CDemon_State::IsRun_F)
		.Init_Changer(TEXT("STATE::ATTACK1"), this, &CDemon_State::IsAttack1)
		.Init_Changer(TEXT("STATE::ATTACK2"), this, &CDemon_State::IsAttack2)
		.Init_Changer(TEXT("STATE::DAMAGED_F"), this, &CDemon_State::IsDamaged_F)
		.Init_Changer(TEXT("STATE::IDLE"), this, &CDemon_State::IsIdle)

		.Finish_Setting();

	m_pDemon->m_pState
		->Add_State(TEXT("STATE::DAMAGED_F"))
		.Init_Start(this, &CDemon_State::Start_Damaged_F)
		.Init_Tick(this, &CDemon_State::Tick_Damaged_F)
		.Init_End(this, &CDemon_State::End_Common)
		.Init_Changer(TEXT("STATE::WALK_F"), this, &CDemon_State::IsWalk_F)
		.Init_Changer(TEXT("STATE::RUN_F"), this, &CDemon_State::IsRun_F)
		.Init_Changer(TEXT("STATE::ATTACK1"), this, &CDemon_State::IsAttack1)
		.Init_Changer(TEXT("STATE::ATTACK2"), this, &CDemon_State::IsAttack2)
		.Init_Changer(TEXT("STATE::DAMAGED_B"), this, &CDemon_State::IsDamaged_B)
		.Init_Changer(TEXT("STATE::IDLE"), this, &CDemon_State::IsIdle)

		.Finish_Setting();

	m_pDemon->m_pState
		->Add_State(TEXT("STATE::HIT_DOWN"))
		.Init_Start(this, &CDemon_State::Start_Hit_Down)
		.Init_Tick(this, &CDemon_State::Tick_Hit_Down)
		.Init_End(this, &CDemon_State::End_Hit_Down)
		.Init_Changer(TEXT("STATE::WALK_F"), this, &CDemon_State::IsWalk_F)
		.Init_Changer(TEXT("STATE::RUN_F"), this, &CDemon_State::IsRun_F)
		.Init_Changer(TEXT("STATE::ATTACK1"), this, &CDemon_State::IsAttack1)
		.Init_Changer(TEXT("STATE::ATTACK2"), this, &CDemon_State::IsAttack2)
		.Init_Changer(TEXT("STATE::DAMAGED_B"), this, &CDemon_State::IsDamaged_B)
		.Init_Changer(TEXT("STATE::DAMAGED_F"), this, &CDemon_State::IsDamaged_F)
		.Init_Changer(TEXT("STATE::IDLE"), this, &CDemon_State::IsIdle)

		.Finish_Setting();

	return S_OK;
}

void CDemon_State::Start_Idle(_double TimeDelta)
{
	m_pDemon->Get_Model()->Set_AnimationIndex(16);
}

void CDemon_State::Start_Walk_F(_double TimeDelta)
{
	m_pDemon->Get_Model()->Set_AnimationIndex(28);
}

void CDemon_State::Start_Run_F(_double TimeDelta)
{
	m_pDemon->Get_Model()->Set_AnimationIndex(20);
}

void CDemon_State::Start_Attack1(_double TImeDelta)
{
	m_pDemon->Get_Model()->Set_AnimationIndex(2);
}

void CDemon_State::Start_Attack2(_double TimeDelta)
{
	m_pDemon->Get_Model()->Set_AnimationIndex(3);
}

void CDemon_State::Start_Damaged_B(_double TimeDelta)
{
	m_pDemon->Get_Model()->Set_AnimationIndex(6);
}

void CDemon_State::Start_Damaged_F(_double TimeDelta)
{
	m_pDemon->Get_Model()->Set_AnimationIndex(7);
}

void CDemon_State::Start_Hit_Down(_double TimeDelta)
{
	m_pDemon->Get_Model()->Set_AnimationIndex(8);
}

void CDemon_State::Tick_Idle(_double TimeDelta)
{
}

void CDemon_State::Tick_Walk_F(_double TimeDelta)
{
	// 한량처럼 걷기
}

void CDemon_State::Tick_Run_F(_double TimeDelta)
{
	CGameInstance*			pGameInstance = GET_INSTANCE(CGameInstance);
	CPlayer*		pPlayer = static_cast<CPlayer*>(pGameInstance->Find_GameObject(LEVEL_CHAP1, L"Layer_Player", L"Player"));
	_vector			vPlayerPos = pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION);
	m_pDemon->Get_TransformCom()->LookAt(vPlayerPos);
	m_pDemon->Get_TransformCom()->Chase(vPlayerPos, TimeDelta);
	RELEASE_INSTANCE(CGameInstance);
}

void CDemon_State::Tick_Attack1(_double TImeDelta)
{
	CGameInstance*			pGameInstance = GET_INSTANCE(CGameInstance);
	CPlayer*		pPlayer = static_cast<CPlayer*>(pGameInstance->Find_GameObject(LEVEL_CHAP1, L"Layer_Player", L"Player"));
	_vector			vPlayerPos = pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION);
	m_pDemon->Get_TransformCom()->LookAt(vPlayerPos);
	RELEASE_INSTANCE(CGameInstance);
	// damage주기
}

void CDemon_State::Tick_Attack2(_double TimeDelta)
{
	// damage주기
}

void CDemon_State::Tick_Damaged_B(_double TimeDelta)
{
	// 맞기
}

void CDemon_State::Tick_Damaged_F(_double TimeDelta)
{
	// 맞기
}

void CDemon_State::Tick_Hit_Down(_double TimeDelta)
{
	m_pDemon->Get_Model()->Set_AnimationIndex(10);
}

void CDemon_State::End_Hit_Down(_double TimeDelta)
{
	m_pDemon->Get_Model()->Set_AnimationIndex(11);
}

void CDemon_State::End_Common(_double TimeDelta)
{
}

_bool CDemon_State::IsIdle()
{
	if (!m_pDemon->m_bMove &&
		!m_pDemon->m_bAttack1 &&
		!m_pDemon->m_bAttack2 &&
		!m_pDemon->m_bDamaged_B &&
		!m_pDemon->m_bDamaged_F &&
		!m_pDemon->m_bHitDown)
		return true;
	else
		return false;
}

_bool CDemon_State::IsWalk_F()
{
	return	false;
}

_bool CDemon_State::IsRun_F()
{
	return m_pDemon->m_bMove;
}

_bool CDemon_State::IsAttack1()
{
	return m_pDemon->m_bAttack1;
}

_bool CDemon_State::IsAttack2()
{
	return m_pDemon->m_bAttack2;
}

_bool CDemon_State::IsDamaged_B()
{
	return  m_pDemon->m_bDamaged_B;
}

_bool CDemon_State::IsDamaged_F()
{
	return  m_pDemon->m_bDamaged_F;
}

_bool CDemon_State::IsHitDown()
{
	return m_pDemon->m_bHitDown;
}

CDemon_State * CDemon_State::Create(CDemon * pDemon)
{
	CDemon_State* pInstance = new CDemon_State();

	if (FAILED(pInstance->Initialize(pDemon)))
	{
		MSG_BOX("CDemon_State Create Fail");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CDemon_State::Free()
{
}
