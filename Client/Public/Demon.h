#pragma once

#include "Monster.h"

BEGIN(Client)

class CDemon final : public CMonster
{
	enum ANIMATION
	{
		DEMON_ADD_DMG_B
		, DEMON_ADD_DMG_F
		, DEMON_ATK_01
		, DEMON_ATK_02
		, DEMON_DeadBody
		, DEMON_Die
		, DEMON_DMG_B
		, DEMON_DMG_F
		, DEMON_Down_Loop
		, DEMON_down_deadbody
		, DEMON_Down
		, DEMON_Get_Up
		, DEMON_Hit_bounce
		, DEMON_Hit_Down
		, DEMON_Hit_Loop
		, DEMON_Hit_Up
		, DEMON_Idle_C
		, DEMON_Idle_P_01
		, DEMON_Idle_P_02
		, DEMON_Passout
		, DEMON_Run_F
		, DEMON_SK_Firing_01
		, DEMON_SK_Firing_02
		, DEMON_SK_Firing_03
		, DEMON_SK_Firing_04
		, DEMON_SK_Firing_05
		, DEMON_SK_Firing_06
		, DEMON_SK_Firing_06_END
		, DEMON_Walk_F
		, DEMON_END
	};

private:
	CDemon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDemon(const CDemon& rhs);
	virtual ~CDemon() = default;

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
	static CDemon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END