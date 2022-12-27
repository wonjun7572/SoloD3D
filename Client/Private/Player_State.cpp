#include "stdafx.h"
#include "..\Public\Player_State.h"
//#include "Parts.h"
//#include "Player.h"
//#include "GameInstance.h"
//#include "Animation.h"
//
//CPlayer_State::CPlayer_State()
//{
//}
//
//HRESULT CPlayer_State::Initialize(CPlayer * pPlayer)
//{
//	m_pPlayer = pPlayer;
//
//	if (FAILED(Setup_State_Idle())) return E_FAIL;
//	if (FAILED(Setup_State_Walk())) return E_FAIL;
//	if (FAILED(Setup_State_Run())) return E_FAIL;
//	if (FAILED(Setup_State_Attack())) return E_FAIL;
//
//	return S_OK;
//}
//
//HRESULT CPlayer_State::Setup_State_Idle()
//{
//	if (m_pPlayer->m_pState == nullptr)
//		return E_FAIL;
//
//	m_pPlayer->m_pState->Set_Root(L"IDLE_READY")
//		.Add_State(L"IDLE_READY")
//		.Init_Start(this, &CPlayer_State::Start_Idle)
//		.Init_Tick(this, &CPlayer_State::Tick_Idle)
//		.Init_End(this, &CPlayer_State::End_Common)
//		
//		.Init_Changer(L"RUN", this, &CPlayer_State::KeyInputForMove)
//
//		.Init_Changer(L"NORMAL_ATTACK_1", this, &CPlayer_State::MouseDown_LB)
//
//		.Finish_Setting();
//	
//	return S_OK;
//}
//
//HRESULT CPlayer_State::Setup_State_Walk()
//{
//	if (m_pPlayer->m_pState == nullptr)
//		return E_FAIL;
//
//	m_pPlayer->m_pState
//		->Add_State(L"WALK")
//		.Init_Start(this, &CPlayer_State::Start_Walk)
//		.Init_Tick(this, &CPlayer_State::Tick_Walk)
//		.Init_End(this, &CPlayer_State::End_Common)
//
//		.Init_Changer(L"IDLE_READY", this, &CPlayer_State::KeyInput_None)
//
//		.Init_Changer(L"RUN", this, &CPlayer_State::KeyInput_S)
//		.Init_Changer(L"NORMAL_ATTACK_1", this, &CPlayer_State::MouseDown_LB)
//
//		.Finish_Setting();
//
//	return S_OK;
//}
//
//HRESULT CPlayer_State::Setup_State_Run()
//{
//	if (m_pPlayer->m_pState == nullptr)
//		return E_FAIL;
//
//	m_pPlayer->m_pState
//		->Add_State(L"RUN")
//		.Init_Start(this, &CPlayer_State::Start_Run)
//		.Init_Tick(this, &CPlayer_State::Tick_Run)
//		.Init_End(this, &CPlayer_State::End_Common)
//
//		.Init_Changer(L"IDLE_READY", this, &CPlayer_State::KeyInput_None)
//
//		.Init_Changer(L"Run_F", this, &CPlayer_State::KeyInput_W)
//		.Init_Changer(L"Run_FL", this, &CPlayer_State::KeyInput_WA)
//		.Init_Changer(L"Run_FR", this, &CPlayer_State::KeyInput_WD)
//		.Init_Changer(L"Run_R", this, &CPlayer_State::KeyInput_D)
//		.Init_Changer(L"Run_BL", this, &CPlayer_State::KeyInput_SA)
//		.Init_Changer(L"Run_BR", this, &CPlayer_State::KeyInput_SD)
//		.Init_Changer(L"Run_B", this, &CPlayer_State::KeyInput_S)
//		.Init_Changer(L"Run_L", this, &CPlayer_State::KeyInput_A)
//		
//		.Finish_Setting();
//
//	m_pPlayer->m_pState
//		->Add_State(L"RUN_F")
//		.Init_Start(this, &CPlayer_State::Start_Run_F)
//		.Init_Tick(this, &CPlayer_State::Tick_Run_F)
//		.Init_End(this, &CPlayer_State::End_Common)
//		.Init_Changer(L"RUN", this, &CPlayer_State::KeyInput_None)
//		.Finish_Setting();
//
//	m_pPlayer->m_pState
//		->Add_State(L"RUN_FL")
//		.Init_Start(this, &CPlayer_State::Start_Run_FL)
//		.Init_Tick(this, &CPlayer_State::Tick_Run_FL)
//		.Init_End(this, &CPlayer_State::End_Common)
//		.Init_Changer(L"RUN", this, &CPlayer_State::KeyInput_None)
//		.Finish_Setting();
//
//	m_pPlayer->m_pState
//		->Add_State(L"RUN_FR")
//		.Init_Start(this, &CPlayer_State::Start_Run_FR)
//		.Init_Tick(this, &CPlayer_State::Tick_Run_FR)
//		.Init_End(this, &CPlayer_State::End_Common)
//		.Init_Changer(L"RUN", this, &CPlayer_State::KeyInput_None)
//		.Finish_Setting();
//
//	m_pPlayer->m_pState
//		->Add_State(L"RUN_R")
//		.Init_Start(this, &CPlayer_State::Start_Run_R)
//		.Init_Tick(this, &CPlayer_State::Tick_Run_R)
//		.Init_End(this, &CPlayer_State::End_Common)
//		.Init_Changer(L"RUN", this, &CPlayer_State::KeyInput_None)
//		.Finish_Setting();
//
//	m_pPlayer->m_pState
//		->Add_State(L"RUN_BR")
//		.Init_Start(this, &CPlayer_State::Start_Run_BR)
//		.Init_Tick(this, &CPlayer_State::Tick_Run_BR)
//		.Init_End(this, &CPlayer_State::End_Common)
//		.Init_Changer(L"RUN", this, &CPlayer_State::KeyInput_None)
//		.Finish_Setting();
//
//	m_pPlayer->m_pState
//		->Add_State(L"RUN_B")
//		.Init_Start(this, &CPlayer_State::Start_Run_B)
//		.Init_Tick(this, &CPlayer_State::Tick_Run_B)
//		.Init_End(this, &CPlayer_State::End_Common)
//		.Init_Changer(L"RUN", this, &CPlayer_State::KeyInput_None)
//		.Finish_Setting();
//
//	m_pPlayer->m_pState
//		->Add_State(L"RUN_BL")
//		.Init_Start(this, &CPlayer_State::Start_Run_BL)
//		.Init_Tick(this, &CPlayer_State::Tick_Run_BL)
//		.Init_End(this, &CPlayer_State::End_Common)
//		.Init_Changer(L"RUN", this, &CPlayer_State::KeyInput_None)
//		.Finish_Setting();
//
//	m_pPlayer->m_pState
//		->Add_State(L"RUN_L")
//		.Init_Start(this, &CPlayer_State::Start_Run_L)
//		.Init_Tick(this, &CPlayer_State::Tick_Run_L)
//		.Init_End(this, &CPlayer_State::End_Common)
//		.Init_Changer(L"RUN", this, &CPlayer_State::KeyInput_None)
//		.Finish_Setting();
//
//	m_pPlayer->m_pState
//		->Add_State(L"V_DEF")
//		.Init_Start(this, &CPlayer_State::Start_V_DEF)
//		.Init_Tick(this, &CPlayer_State::Tick_V_DEF)
//		.Init_End(this, &CPlayer_State::End_Common)
//
//		.Init_Changer(L"IDLE_READY", this, &CPlayer_State::CheckFinish_V_DEF)
//
//		.Finish_Setting();
//
//	return S_OK;
//}
//
//HRESULT CPlayer_State::Setup_State_Attack()
//{
//	if (m_pPlayer->m_pState == nullptr)
//		return E_FAIL;
//
//	m_pPlayer->m_pState
//		->Add_State(L"NORMAL_ATTACK_1")
//		.Init_Start(this, &CPlayer_State::Start_Attack_01)
//		.Init_Tick(this, &CPlayer_State::Tick_Attack_01)
//		.Init_End(this, &CPlayer_State::End_Common)
//
//		.Init_Changer(L"IDLE_READY", this, &CPlayer_State::KeyInput_None)
//		.Init_Changer(L"NORMAL_ATTACK_2", this, &CPlayer_State::MouseDown_LB)
//
//		.Finish_Setting();
//
//	m_pPlayer->m_pState
//		->Add_State(L"NORMAL_ATTACK_2")
//		.Init_Start(this, &CPlayer_State::Start_Attack_02)
//		.Init_Tick(this, &CPlayer_State::Tick_Attack_02)
//		.Init_End(this, &CPlayer_State::End_Common)
//
//		.Init_Changer(L"IDLE_READY", this, &CPlayer_State::KeyInput_None)
//
//		.Init_Changer(L"NORMAL_ATTACK_3", this, &CPlayer_State::MouseDown_LB)
//
//		.Finish_Setting();
//
//	m_pPlayer->m_pState
//		->Add_State(L"NORMAL_ATTACK_3")
//		.Init_Start(this, &CPlayer_State::Start_Attack_02)
//		.Init_Tick(this, &CPlayer_State::Tick_Attack_02)
//		.Init_End(this, &CPlayer_State::End_Common)
//
//		.Init_Changer(L"IDLE_READY", this, &CPlayer_State::KeyInput_None)
//
//		.Init_Changer(L"NORMAL_ATTACK_1", this, &CPlayer_State::MouseDown_LB)
//
//		.Finish_Setting();
//
//
//	return S_OK;
//}
//
//void CPlayer_State::Start_Idle(_double TimeDelta)
//{
//	m_pPlayer->m_pModelCom->Set_AnimationIndex(PLAYER_IDLE_READY);
//	
//	for (_uint i = CPlayer::PART_UPPER; i < CPlayer::PART_END; ++i)
//		static_cast<CParts*>(m_pPlayer->m_PlayerParts[i])->Set_AnimIndex(PLAYER_IDLE_READY);
//}
//
//void CPlayer_State::Start_Walk(_double TimeDelta)
//{
//}
//
//void CPlayer_State::Start_Walk_B(_double TimeDelta)
//{
//}
//
//void CPlayer_State::Start_Walk_F(_double TimeDelta)
//{
//}
//
//void CPlayer_State::Start_Run(_double TimeDelta)
//{
//}
//
//void CPlayer_State::Start_Run_B(_double TimeDelta)
//{
//	m_pPlayer->m_pModelCom->Set_AnimationIndex(PLAYER_RUN_B);
//
//	for (_uint i = CPlayer::PART_UPPER; i < CPlayer::PART_END; ++i)
//		static_cast<CParts*>(m_pPlayer->m_PlayerParts[i])->Set_AnimIndex(PLAYER_RUN_B);
//}
//
//void CPlayer_State::Start_Run_BL(_double TimeDelta)
//{
//	m_pPlayer->m_pModelCom->Set_AnimationIndex(PLAYER_RUN_BL);
//
//	for (_uint i = CPlayer::PART_UPPER; i < CPlayer::PART_END; ++i)
//		static_cast<CParts*>(m_pPlayer->m_PlayerParts[i])->Set_AnimIndex(PLAYER_RUN_BL);
//}
//
//void CPlayer_State::Start_Run_BR(_double TimeDelta)
//{
//	m_pPlayer->m_pModelCom->Set_AnimationIndex(PLAYER_RUN_BR);
//
//	for (_uint i = CPlayer::PART_UPPER; i < CPlayer::PART_END; ++i)
//		static_cast<CParts*>(m_pPlayer->m_PlayerParts[i])->Set_AnimIndex(PLAYER_RUN_BR);
//}
//
//void CPlayer_State::Start_Run_F(_double TimeDelta)
//{
//	m_pPlayer->m_pModelCom->Set_AnimationIndex(PLAYER_RUN_F);
//
//	for (_uint i = CPlayer::PART_UPPER; i < CPlayer::PART_END; ++i)
//		static_cast<CParts*>(m_pPlayer->m_PlayerParts[i])->Set_AnimIndex(PLAYER_RUN_F);
//}
//
//void CPlayer_State::Start_Run_FL(_double TimeDelta)
//{
//	m_pPlayer->m_pModelCom->Set_AnimationIndex(PLAYER_RUN_FL);
//
//	for (_uint i = CPlayer::PART_UPPER; i < CPlayer::PART_END; ++i)
//		static_cast<CParts*>(m_pPlayer->m_PlayerParts[i])->Set_AnimIndex(PLAYER_RUN_FL);
//}
//
//void CPlayer_State::Start_Run_FR(_double TimeDelta)
//{
//	m_pPlayer->m_pModelCom->Set_AnimationIndex(PLAYER_RUN_FR);
//
//	for (_uint i = CPlayer::PART_UPPER; i < CPlayer::PART_END; ++i)
//		static_cast<CParts*>(m_pPlayer->m_PlayerParts[i])->Set_AnimIndex(PLAYER_RUN_FR);
//}
//
//void CPlayer_State::Start_Run_L(_double TimeDelta)
//{
//	m_pPlayer->m_pModelCom->Set_AnimationIndex(PLAYER_RUN_L);
//
//	for (_uint i = CPlayer::PART_UPPER; i < CPlayer::PART_END; ++i)
//		static_cast<CParts*>(m_pPlayer->m_PlayerParts[i])->Set_AnimIndex(PLAYER_RUN_L);
//}
//
//void CPlayer_State::Start_Run_R(_double TimeDelta)
//{
//	m_pPlayer->m_pModelCom->Set_AnimationIndex(PLAYER_RUN_R);
//
//	for (_uint i = CPlayer::PART_UPPER; i < CPlayer::PART_END; ++i)
//		static_cast<CParts*>(m_pPlayer->m_PlayerParts[i])->Set_AnimIndex(PLAYER_RUN_R);
//}
//
//void CPlayer_State::Start_V_DEF(_double TimeDelta)
//{
//	m_pPlayer->m_pModelCom->Set_AnimationIndex(PLAYER_V_DEF);
//
//	for (_uint i = CPlayer::PART_UPPER; i < CPlayer::PART_END; ++i)
//		static_cast<CParts*>(m_pPlayer->m_PlayerParts[i])->Set_AnimIndex(PLAYER_V_DEF);
//}
//
//void CPlayer_State::Start_Attack_01(_double TimeDelta)
//{
//	m_pPlayer->m_pModelCom->Set_AnimationIndex(PLAYER_ATK_01);
//
//	for (_uint i = CPlayer::PART_UPPER; i < CPlayer::PART_END; ++i)
//		static_cast<CParts*>(m_pPlayer->m_PlayerParts[i])->Set_AnimIndex(PLAYER_ATK_01);
//}
//
//void CPlayer_State::Start_Attack_02(_double TimeDelta)
//{
//	m_pPlayer->m_pModelCom->Set_AnimationIndex(PLAYER_ATK_02);
//
//	for (_uint i = CPlayer::PART_UPPER; i < CPlayer::PART_END; ++i)
//		static_cast<CParts*>(m_pPlayer->m_PlayerParts[i])->Set_AnimIndex(PLAYER_ATK_02);
//}
//
//void CPlayer_State::Start_Attack_03(_double TimeDelta)
//{
//	m_pPlayer->m_pModelCom->Set_AnimationIndex(PLAYER_ATK_03);
//
//	for (_uint i = CPlayer::PART_UPPER; i < CPlayer::PART_END; ++i)
//		static_cast<CParts*>(m_pPlayer->m_PlayerParts[i])->Set_AnimIndex(PLAYER_ATK_03);
//}
//
//void CPlayer_State::Tick_Idle(_double TimeDelta)
//{
//	m_pPlayer->m_pModelCom->Set_AnimationIndex(PLAYER_IDLE_READY);
//
//	for (_uint i = CPlayer::PART_UPPER; i < CPlayer::PART_END; ++i)
//		static_cast<CParts*>(m_pPlayer->m_PlayerParts[i])->Set_AnimIndex(PLAYER_IDLE_READY);
//}
//
//void CPlayer_State::Tick_Walk(_double TimeDelta)
//{
//}
//
//void CPlayer_State::Tick_Walk_B(_double TimeDelta)
//{
//}
//
//void CPlayer_State::Tick_Walk_F(_double TimeDelta)
//{
//}
//
//void CPlayer_State::Tick_Run(_double TimeDelta)
//{
//}
//
//void CPlayer_State::Tick_Run_B(_double TimeDelta)
//{
//}
//
//void CPlayer_State::Tick_Run_BL(_double TimeDelta)
//{
//}
//
//void CPlayer_State::Tick_Run_BR(_double TimeDelta)
//{
//}
//
//void CPlayer_State::Tick_Run_F(_double TimeDelta)
//{
//	m_pPlayer->m_pTransformCom->Go_Straight(TimeDelta);
//}
//
//void CPlayer_State::Tick_Run_FL(_double TimeDelta)
//{
//}
//
//void CPlayer_State::Tick_Run_FR(_double TimeDelta)
//{
//}
//
//void CPlayer_State::Tick_Run_L(_double TimeDelta)
//{
//}
//
//void CPlayer_State::Tick_Run_R(_double TimeDelta)
//{
//}
//
//void CPlayer_State::Tick_V_DEF(_double TimeDelta)
//{
//}
//
//void CPlayer_State::Tick_Attack_01(_double TimeDelta)
//{
//}
//
//void CPlayer_State::Tick_Attack_02(_double TimeDelta)
//{
//}
//
//void CPlayer_State::Tick_Attack_03(_double TimeDelta)
//{
//}
//
//void CPlayer_State::End_Common(_double TimeDelta)
//{
//	m_pPlayer->m_vAnimationMove = _float4(0.f, 0.f, 0.f, 1.f);
//}
//
//_bool CPlayer_State::KeyInput_None(void)
//{
//	CGameInstance* pInst = GET_INSTANCE(CGameInstance);
//
//	_bool bResult = !pInst->Key_Pressing(DIK_W) &&
//		!pInst->Key_Pressing(DIK_A) &&
//		!pInst->Key_Pressing(DIK_S) &&
//		!pInst->Key_Pressing(DIK_D);
//
//	RELEASE_INSTANCE(CGameInstance);
//
//	return bResult;
//}
//
//_bool CPlayer_State::KeyInputForMove()
//{
//	CGameInstance* pInst = GET_INSTANCE(CGameInstance);
//
//	_bool bResult = pInst->Key_Pressing(DIK_W) ||
//		pInst->Key_Pressing(DIK_A) ||
//		pInst->Key_Pressing(DIK_S) ||
//		pInst->Key_Pressing(DIK_D);
//
//	RELEASE_INSTANCE(CGameInstance);
//
//	return bResult;
//}
//
//_bool CPlayer_State::KeyInput_W(void)
//{
//	CGameInstance* pInst = GET_INSTANCE(CGameInstance);
//
//	_bool bResult = pInst->Key_Pressing(DIK_W) &&
//		!pInst->Key_Pressing(DIK_A) &&
//		!pInst->Key_Pressing(DIK_S) &&
//		!pInst->Key_Pressing(DIK_D) &&
//		!KeyInput_WA() && !KeyInput_WD() && !KeyInput_WC();
//
//	RELEASE_INSTANCE(CGameInstance);
//
//	return bResult;
//}
//
//_bool CPlayer_State::KeyInput_A(void)
//{
//	CGameInstance* pInst = GET_INSTANCE(CGameInstance);
//
//	_bool bResult = !pInst->Key_Pressing(DIK_W) &&
//		pInst->Key_Pressing(DIK_A) &&
//		!pInst->Key_Pressing(DIK_S) &&
//		!pInst->Key_Pressing(DIK_D) &&
//		!KeyInput_WA() && !KeyInput_SA();
//
//	RELEASE_INSTANCE(CGameInstance);
//
//	return bResult;
//}
//
//_bool CPlayer_State::KeyInput_S(void)
//{
//	CGameInstance* pInst = GET_INSTANCE(CGameInstance);
//
//	_bool bResult = !pInst->Key_Pressing(DIK_W) &&
//		!pInst->Key_Pressing(DIK_A) &&
//		pInst->Key_Pressing(DIK_S) &&
//		!pInst->Key_Pressing(DIK_D) &&
//		!KeyInput_SA() && !KeyInput_SD() && !KeyInput_SC();
//
//	RELEASE_INSTANCE(CGameInstance);
//
//	return bResult;
//}
//
//_bool CPlayer_State::KeyInput_D(void)
//{
//	CGameInstance* pInst = GET_INSTANCE(CGameInstance);
//
//	_bool bResult = !pInst->Key_Pressing(DIK_W) &&
//		!pInst->Key_Pressing(DIK_A) &&
//		!pInst->Key_Pressing(DIK_S) &&
//		pInst->Key_Pressing(DIK_D) &&
//		!KeyInput_WD() && !KeyInput_SD();
//
//	RELEASE_INSTANCE(CGameInstance);
//
//	return bResult;
//}
//
//_bool CPlayer_State::KeyInput_WA(void)
//{
//	CGameInstance* pInst = GET_INSTANCE(CGameInstance);
//
//	_bool bResult = pInst->Key_Pressing(DIK_W) &&
//		pInst->Key_Pressing(DIK_A) &&
//		!pInst->Key_Pressing(DIK_S) &&
//		!pInst->Key_Pressing(DIK_D);
//
//	RELEASE_INSTANCE(CGameInstance);
//
//	return bResult;
//}
//
//_bool CPlayer_State::KeyInput_WD(void)
//{
//	CGameInstance* pInst = GET_INSTANCE(CGameInstance);
//
//	_bool bResult = pInst->Key_Pressing(DIK_W) &&
//		!pInst->Key_Pressing(DIK_A) &&
//		!pInst->Key_Pressing(DIK_S) &&
//		pInst->Key_Pressing(DIK_D);
//
//	RELEASE_INSTANCE(CGameInstance);
//
//	return bResult;
//}
//
//_bool CPlayer_State::KeyInput_SA(void)
//{
//	CGameInstance* pInst = GET_INSTANCE(CGameInstance);
//
//	_bool bResult = !pInst->Key_Pressing(DIK_W) &&
//		pInst->Key_Pressing(DIK_A) &&
//		pInst->Key_Pressing(DIK_S) &&
//		!pInst->Key_Pressing(DIK_D);
//
//	RELEASE_INSTANCE(CGameInstance);
//
//	return bResult;
//}
//
//_bool CPlayer_State::KeyInput_SD(void)
//{
//	CGameInstance* pInst = GET_INSTANCE(CGameInstance);
//
//	_bool bResult = !pInst->Key_Pressing(DIK_W) &&
//		!pInst->Key_Pressing(DIK_A) &&
//		pInst->Key_Pressing(DIK_S) &&
//		pInst->Key_Pressing(DIK_D);
//
//	RELEASE_INSTANCE(CGameInstance);
//
//	return bResult;
//}
//
//_bool CPlayer_State::KeyInput_WC()
//{
//	CGameInstance* pInst = GET_INSTANCE(CGameInstance);
//
//	_bool bResult = pInst->Key_Pressing(DIK_W) &&
//		!pInst->Key_Pressing(DIK_A) &&
//		!pInst->Key_Pressing(DIK_S) &&
//		!pInst->Key_Pressing(DIK_D) &&
//		pInst->Key_Pressing(DIK_C);
//
//	RELEASE_INSTANCE(CGameInstance);
//
//	return bResult;
//}
//
//_bool CPlayer_State::KeyInput_SC()
//{
//	CGameInstance* pInst = GET_INSTANCE(CGameInstance);
//
//	_bool bResult = !pInst->Key_Pressing(DIK_W) &&
//		!pInst->Key_Pressing(DIK_A) &&
//		pInst->Key_Pressing(DIK_S) &&
//		!pInst->Key_Pressing(DIK_D) &&
//		pInst->Key_Pressing(DIK_C);
//
//	RELEASE_INSTANCE(CGameInstance);
//
//	return bResult;
//}
//
//_bool CPlayer_State::KeyInput_E()
//{
//	CGameInstance* pInst = GET_INSTANCE(CGameInstance);
//
//	_bool bResult = pInst->Key_Pressing(DIK_E);
//
//	RELEASE_INSTANCE(CGameInstance);
//
//	return bResult;
//}
//
//_bool CPlayer_State::KeyInput_Q()
//{
//	CGameInstance* pInst = GET_INSTANCE(CGameInstance);
//
//	_bool bResult = pInst->Key_Pressing(DIK_Q);
//
//	RELEASE_INSTANCE(CGameInstance);
//
//	return bResult;
//}
//
//_bool CPlayer_State::KeyCharge_F()
//{
//	CGameInstance* pInst = GET_INSTANCE(CGameInstance);
//
//	_bool bResult = pInst->Key_Pressing(DIK_F);
//
//	RELEASE_INSTANCE(CGameInstance);
//
//	return bResult;
//}
//
//_bool CPlayer_State::KeyInput_F()
//{
//	CGameInstance* pInst = GET_INSTANCE(CGameInstance);
//
//	_bool bResult = pInst->Key_Up(DIK_E);
//
//	RELEASE_INSTANCE(CGameInstance);
//
//	return bResult;
//}
//
//_bool CPlayer_State::MouseDown_RB()
//{
//	return _bool();
//}
//
//_bool CPlayer_State::KeyInput_Shift(void)
//{
//	CGameInstance* pInst = GET_INSTANCE(CGameInstance);
//
//	_bool bResult = pInst->Get_DIKeyState(DIK_LSHIFT);
//	
//	RELEASE_INSTANCE(CGameInstance);
//
//	return bResult;
//}
//
//_bool CPlayer_State::KeyUp_Shift(void)
//{
//	CGameInstance* pInst = GET_INSTANCE(CGameInstance);
//
//	_bool bResult = pInst->Key_Up(DIK_LSHIFT);
//
//	RELEASE_INSTANCE(CGameInstance);
//
//	return bResult;
//}
//
//_bool CPlayer_State::KeyInput_Space(void)
//{
//	return _bool();
//}
//
//_bool CPlayer_State::MouseDown_LB(void)
//{
//	CGameInstance* pInst = GET_INSTANCE(CGameInstance);
//
//	_bool bResult = pInst->Mouse_Down(DIM_LB);
//
//	RELEASE_INSTANCE(CGameInstance);
//
//	return bResult;
//}
//
//_bool CPlayer_State::MouseCharge_LB(void)
//{
//	return _bool();
//}
//
//_bool CPlayer_State::isFinish(void)
//{
//	return _bool();
//}
//
//_bool CPlayer_State::CheckFinish_V_DEF()
//{
//	return AnimFinishChecker(PLAYER_V_DEF);
//}
//
//_bool CPlayer_State::AnimFinishChecker(ANIMATION eAnim, _double FinishRate)
//{
//	return m_pPlayer->m_pModelCom->Get_IndexAnim(eAnim)->Get_PlayRate() > FinishRate;
//}
//
//CPlayer_State* CPlayer_State::Create(CPlayer* pPlayer)
//{
//	CPlayer_State* pInstance = new CPlayer_State();
//
//	if (FAILED(pInstance->Initialize(pPlayer)))
//	{
//		MSG_BOX("CPlayer_State Create Fail");
//		Safe_Release(pInstance);
//		return nullptr;
//	}
//
//	return pInstance;
//}
//
//void CPlayer_State::Free(void)
//{
//}