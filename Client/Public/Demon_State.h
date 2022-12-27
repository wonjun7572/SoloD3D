#pragma once

#include "Client_Define.h"
#include "Base.h"

BEGIN(Client)

class CDemon_State : public CBase
{
public:
	enum ANIMATION
	{
		DEMON_READY/* 16 */, DEMON_PASSOUT/* 19 */, DEMON_WALK/* 28 */, DEMON_RUN_F/* 20 */,
		DEMON_ATTACK1/* 2 */, DEMON_ATTACK2/* 3 */,
		DEMON_HIT_B/* 6 */, DEMON_HIT_F/* 7 */, DEMON_DOWN/* 8 */, DEMON_DOWNLOOP/* 10 */,
		DEMON_GETUP/* 11 */, DEMON_HIT_BOUNCE /* 바닥에 넘어져 있을 때 12 */, DEMON_HIT_DOWN/* 13 */,
		DEMON_FIRING1/* 21 */, DEMON_FIRING2/* 22 */,
		DEMON_FIRING3/* 23 */, DEMON_FIRING4/* 24 */, DEMON_FIRING5/* 25 */,
		DEMON_DIE/* 5 */, DEMON_DEAD/* 4 */, STATE_END
	};

public:
	CDemon_State();
	virtual	~CDemon_State() = default;

public:
	HRESULT Initialize(class CDemon* pDemon);

private:
	HRESULT Setup_State_Idle();
	HRESULT Setup_State_Walk();
	HRESULT	Setup_State_Run();
	HRESULT Setup_State_Attack();
	HRESULT Setup_State_Damaged();

	void	Start_Idle(_double TimeDelta);
	
	void	Start_Walk_F(_double TimeDelta);

	void	Start_Run_F(_double TimeDelta);

	void	Start_Attack1(_double TImeDelta);
	void	Start_Attack2(_double TimeDelta);

	void	Start_Damaged_B(_double TimeDelta);
	void	Start_Damaged_F(_double TimeDelta);

	void	Start_Hit_Down(_double TimeDelta);

	void	Tick_Idle(_double TimeDelta);

	void	Tick_Walk_F(_double TimeDelta);

	void	Tick_Run_F(_double TimeDelta);

	void	Tick_Attack1(_double TImeDelta);
	void	Tick_Attack2(_double TimeDelta);

	void	Tick_Damaged_B(_double TimeDelta);
	void	Tick_Damaged_F(_double TimeDelta);

	void	Tick_Hit_Down(_double TimeDelta);

	void	End_Hit_Down(_double TimeDelta);
	void	End_Common(_double TimeDelta);

	_bool	IsIdle();

	_bool	IsWalk_F();
	_bool	IsRun_F();

	_bool	IsAttack1();
	_bool	IsAttack2();

	_bool	IsDamaged_B();
	_bool	IsDamaged_F();

	_bool	IsHitDown();

private:
	class CDemon* m_pDemon;

public:
	static CDemon_State* Create(class CDemon* pDemon);
	virtual void Free();
};

END