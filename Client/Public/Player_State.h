#pragma once

#include "Client_Define.h"
#include "Base.h"

BEGIN(Client)

class CPlayer_State : public CBase
{
//public:
//	enum DIRECTION
//	{
//		DIR_FL, DIR_F, DIR_FR, DIR_R, DIR_BR, DIR_B, DIR_BL, DIR_L, DIR_END
//	};
//
//	enum ANIMATION
//	{
//		PLAYER_ADD_DMG_B, PLAYER_ADD_DMG_F,
//		PLAYER_ATK_01_UPPER, PLAYER_ATK_01,
//		PLAYER_ATK_02_UPPER, PLAYER_ATK_02,
//		PLAYER_ATK_03_UPPER, PLAYER_ATK_03,
//
//		PLAYER_DEAD_BODY, PLAYER_DIE,
//		PLAYER_DMG_B_UPPER, PLAYER_DMG_B,
//		PLAYER_DMG_F_UPPER, PLAYER_DMG_F,
//
//		PLAYER_DOWN_ALL, PLAYER_DOWN_DEADBODY,
//		PLAYER_DOWN_LOOP, PLAYER_DOWN, PLAYER_GETUP,
//		PLAYER_HIT_BOUNCE, PLAYER_HIT_DOWN, PLAYER_HIT_LOOP,
//		PLAYER_HIT_UP_ALL, PLAYER_HIT_UP, PLAYER_IDLE_READY,
//		PLAYER_IDLE_WEAPON, PLAYER_JUMP_LAND, PLAYER_JUMP_LOOP,
//		PLAYER_JUMP_UP, PLAYER_PASSOUT, PLAYER_RUN_B, PLAYER_RUN_BL,
//		PLAYER_RUN_BR, PLAYER_RUN_F_STOP, PLAYER_RUN_F, PLAYER_RUN_FALL_B,
//		PLAYER_RUN_FALL_F, PLAYER_RUN_FL, PLAYER_RUN_FR,
//		PLAYER_RUN_JUMP_LAND_B, PLAYER_RUN_JUMP_LAND_F,
//		PLAYER_RUN_JUMP_LOOP_B, PLAYER_RUN_JUMP_LOOP_F,
//		PLAYER_RUN_JUMP_UP_B, PLAYER_RUN_JUMP_UP_F,
//		PLAYER_RUN_L, PLAYER_RUN_R,
//		PLAYER_SK01_UPPER, PLAYER_SK01,
//		PLAYER_SK02_UPPER, PLAYER_SK02, PLAYER_SK03, PLAYER_SK04,
//		PLAYER_SK05_02, PLAYER_SK05_UPPER, PLAYER_SK05,
//		PLAYER_SK08_UPPER, PLAYER_SK08, PLAYER_SK09,
//		PLAYER_SK10_UPPER, PLAYER_SK10, PLAYER_SK11_1, PLAYER_SK11_2,
//		PLAYER_SK11_UPPER, PLAYER_SK11, PLAYER_SK12_A, PLAYER_SK12_B,
//		PLAYER_SK12_UPPER, PLAYER_SK12, PLAYER_SK13, PLAYER_SK15,
//		PLAYER_SK17, PLAYER_SK19, PLAYER_SK21_UPPER, PLAYER_SK21,
//		PLAYER_SK22, PLAYER_SK23_UPPER, PLAYER_SK23, PLAYER_SK24,
//		PLAYER_SK25_1, PLAYER_SK25_2, PLAYER_SK25, PLAYER_SK27_CHARGING_,
//		PLAYER_SK27_CHARGING, PLAYER_SK27_FIRING, PLAYER_SK28_CHARGING, PLAYER_SK28_FIRING,
//		PLAYER_SK29_CHARGING_, PLAYER_SK29_CHARGING, PLAYER_SK29_FIRING,
//		PLAYER_SK30_CHARGING_, PLAYER_SK30_CHARGING, PLAYER_SK30_FIRING,
//		PLAYER_SK31_FIRING,
//		PLAYER_SK32_CHARGING_, PLAYER_SK32_CHARGING, PLAYER_SK32_FIRING,
//		PLAYER_SK33, PLAYER_SK34, PLAYER_SK35, PLAYER_SK36, PLAYER_SK37,
//		PLAYER_SK38, PLAYER_SK39, PLAYER_SK40, PLAYER_SK41, PLAYER_SK42,
//		PLAYER_SK43, PLAYER_USEITEM_A, PLAYER_USEITEM_B, PLAYER_UNWEAPON_READY,
//		PLAYER_UNWEAPON_RUN, PLAYER_UNWEAPON, PLAYER_V_DEF, PLAYER_WALK_B, PLAYER_WALK_F,
//		PLAYER_WEAPON_READY, PLAYER_WEAPON_RUN, PLAYER_WEAPON, PLAYER_STATE_END
//	};
//
//public:
//	CPlayer_State();
//	virtual	~CPlayer_State() = default;
//
//public:
//	HRESULT Initialize(class CPlayer* pPlayer);
//
//	HRESULT Setup_State_Idle();
//	HRESULT Setup_State_Walk();
//	HRESULT Setup_State_Run();
//	HRESULT	Setup_State_Attack();
//
//	void Start_Idle(_double TimeDelta);
//
//	void Start_Walk(_double TimeDelta);
//	void Start_Walk_B(_double TimeDelta);
//	void Start_Walk_F(_double TimeDelta);
//
//	void Start_Run(_double TimeDelta);
//	void Start_Run_B(_double TimeDelta);
//	void Start_Run_BL(_double TimeDelta);
//	void Start_Run_BR(_double TimeDelta);
//	void Start_Run_F(_double TimeDelta);
//	void Start_Run_FL(_double TimeDelta);
//	void Start_Run_FR(_double TimeDelta);
//	void Start_Run_L(_double TimeDelta);
//	void Start_Run_R(_double TimeDelta);
//
//	void Start_V_DEF(_double TimeDelta);
//
//	void Start_Attack_01(_double TimeDelta);
//	void Start_Attack_02(_double TimeDelta);
//	void Start_Attack_03(_double TimeDelta);
//
//public:
//	void Tick_Idle(_double TimeDelta);
//
//	void Tick_Walk(_double TimeDelta);
//	void Tick_Walk_B(_double TimeDelta);
//	void Tick_Walk_F(_double TimeDelta);
//
//	void Tick_Run(_double TimeDelta);
//	void Tick_Run_B(_double TimeDelta);
//	void Tick_Run_BL(_double TimeDelta);
//	void Tick_Run_BR(_double TimeDelta);
//	void Tick_Run_F(_double TimeDelta);
//	void Tick_Run_FL(_double TimeDelta);
//	void Tick_Run_FR(_double TimeDelta);
//	void Tick_Run_L(_double TimeDelta);
//	void Tick_Run_R(_double TimeDelta);
//
//	void Tick_V_DEF(_double TimeDelta);
//
//	void Tick_Attack_01(_double TimeDelta);
//	void Tick_Attack_02(_double TimeDelta);
//	void Tick_Attack_03(_double TimeDelta);
//
//public:
//	void End_Common(_double TimeDelta);
//
//public:
//	_bool	KeyInput_None(void);
//
//	_bool	KeyInputForMove();
//
//	_bool	KeyInput_W(void);
//	_bool	KeyInput_A(void);
//	_bool	KeyInput_S(void);
//	_bool	KeyInput_D(void);
//	_bool	KeyInput_WA(void);
//	_bool	KeyInput_WD(void);
//	_bool	KeyInput_SA(void);
//	_bool	KeyInput_SD(void);
//	_bool	KeyInput_WC();
//	_bool	KeyInput_SC();
//
//	_bool	KeyInput_E();
//	_bool	KeyInput_Q();
//	_bool   KeyCharge_F();
//	_bool   KeyInput_F();
//
//	_bool	MouseDown_RB();
//
//	_bool	KeyInput_Shift(void);
//
//	_bool	KeyUp_Shift(void);
//	_bool	KeyInput_Space(void);
//	_bool	MouseDown_LB(void);
//	_bool	MouseCharge_LB(void);
//
//	_bool	isFinish(void);
//
//	_bool	CheckFinish_V_DEF();
//
//public:
//	_bool AnimFinishChecker(ANIMATION eAnim, _double FinishRate = 0.95);
//
//private:
//	class CPlayer* m_pPlayer = nullptr;
//
//public:
//	static CPlayer_State* Create(class CPlayer* pPlayer);
//	virtual void Free();
};

END