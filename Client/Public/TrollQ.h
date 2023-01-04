#pragma once

#include "Monster.h"

BEGIN(Client)

class CTrollQ final : public CMonster
{
	enum ANIMATION
	{
		TROLLQ_ADD_DMG_B
		, TROLLQ_ADD_DMG_F
		, TROLLQ_ATK_01
		, TROLLQ_ATK_02
		, TROLLQ_DeadBody
		, TROLLQ_Die
		, TROLLQ_DMG_B
		, TROLLQ_DMG_F
		, TROLLQ_down_deadbody
		, TROLLQ_Down_Loop
		, TROLLQ_Down
		, TROLLQ_Get_Up
		, TROLLQ_Hit_bounce
		, TROLLQ_Hit_Down
		, TROLLQ_Hit_Loop
		, TROLLQ_Hit_Up
		, TROLLQ_Idle_C
		, TROLLQ_Idle_P_01
		, TROLLQ_Idle_P_02
		, TROLLQ_Passout
		, TROLLQ_Run_F
		, TROLLQ_SK_Firing_01
		, TROLLQ_SK_Firing_02
		, TROLLQ_SK_Firing_03
		, TROLLQ_SK_Firing_04
		, TROLLQ_SK_Firing_05
		, TROLLQ_Walk_F
		, TROLLQ_END
	};

private:
	CTrollQ(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTrollQ(const CTrollQ& rhs);
	virtual ~CTrollQ() = default;

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
	static CTrollQ* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END