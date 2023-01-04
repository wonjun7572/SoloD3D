#pragma once

#include "Monster.h"

BEGIN(Client)

class CZombieA final : public CMonster
{
	enum ANIMATION
	{
		ZOMBIEA_ADD_DMG_B
		, ZOMBIEA_ADD_DMG_F
		, ZOMBIEA_ATK_01
		, ZOMBIEA_ATK_02
		, ZOMBIEA_ATK_03
		, ZOMBIEA_DeadBody
		, ZOMBIEA_Die
		, ZOMBIEA_DMG_B
		, ZOMBIEA_DMG_F
		, ZOMBIEA_down_deadbody
		, ZOMBIEA_Down_Loop
		, ZOMBIEA_Down
		, ZOMBIEA_Get_Up
		, ZOMBIEA_Hit_bounce
		, ZOMBIEA_Hit_Down
		, ZOMBIEA_Hit_Loop
		, ZOMBIEA_Hit_Up
		, ZOMBIEA_Idle_C
		, ZOMBIEA_Idle_P_01
		, ZOMBIEA_Idle_P_02
		, ZOMBIEA_Passout
		, ZOMBIEA_Run_F
		, ZOMBIEA_SK_Firing_01
		, ZOMBIEA_SK_Firing_02
		, ZOMBIEA_Spawn
		, ZOMBIEA_Walk_F
		, ZOMBIEA_END
	};

private:
	CZombieA(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CZombieA(const CZombieA& rhs);
	virtual ~CZombieA() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	void	Imgui_RenderProperty() override;

private:
	void	SetUp_FSM() override;

public:
	void Adjust_Collision(_double TimeDelta);

	void CollisionToPlayer(_double TimeDelta);
	void CollisionToAttack(_double TimeDelta);
	void CollisionToWeapon(_double TimeDelta);
	void CollisionToWeaponSkill02(_double TimeDelta);
	void CollisionToWeaponSkill04(_double TimeDelta);

	void CollisionToMonster(_double TimeDelta);

private:
	void AdditiveAnim(_double TimeDelta);

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

	_bool	AnimFinishChecker(ANIMATION eAnim, _double FinishRate = 0.95);
	_bool   AnimIntervalChecker(ANIMATION eAnim, _double StartRate, _double FinishRate);

private:
	_bool	m_bPlayerChase = false;
	_bool	m_bPlayerAttack = true;
	_bool	m_bRealAttack = false;

	_double m_AttackDelayTime = 0.0;
	_double m_HitDownDelayTime = 0.0;
	_double m_GroggyDelayTime = 0.0;

private:
	CCollider* m_pAttackColCom = nullptr;
	CCollider* m_pSwordColCom = nullptr;

public:
	static CZombieA* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END