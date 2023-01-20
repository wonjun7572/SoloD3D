#pragma once

#include "Monster.h"

BEGIN(Client)

class CSkeletonWarrior final : public CMonster
{
	enum ANIMATION
	{
		SKELETON_WARRIOR_ADD_DMG_B
		,SKELETON_WARRIOR_ADD_DMG_F
		,SKELETON_WARRIOR_ATK_01
		,SKELETON_WARRIOR_ATK_02
		,SKELETON_WARRIOR_DeadBody
		,SKELETON_WARRIOR_Die
		,SKELETON_WARRIOR_DMG_B
		,SKELETON_WARRIOR_DMG_F
		,SKELETON_WARRIOR_Down
		,SKELETON_WARRIOR_down_deadbody
		,SKELETON_WARRIOR_Down_Loop
		,SKELETON_WARRIOR_Get_Up
		,SKELETON_WARRIOR_Groggy
		,SKELETON_WARRIOR_Hit_bounce
		,SKELETON_WARRIOR_Hit_Down
		,SKELETON_WARRIOR_Hit_Loop
		,SKELETON_WARRIOR_Hit_Up
		,SKELETON_WARRIOR_Idle_C
		,SKELETON_WARRIOR_Idle_P_01
		,SKELETON_WARRIOR_Idle_P_02
		,SKELETON_WARRIOR_Passout
		,SKELETON_WARRIOR_Run_F
		,SKELETON_WARRIOR_SK_Firing_01
		,SKELETON_WARRIOR_SK_Firing_02
		,SKELETON_WARRIOR_SK_Firing_03
		,SKELETON_WARRIOR_Walk_F
		,SKELETON_WARRIOR_END
	};

public:
	typedef struct tagSkeletonQDesc
	{
		_uint	iGroup;
		_float4 vPos;
		_float	fRadian;
	}SKELETONDESC;

private:
	CSkeletonWarrior(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkeletonWarrior(const CSkeletonWarrior& rhs);
	virtual ~CSkeletonWarrior() = default;

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

private:
	void AdditiveAnim(_double TimeDelta);

private:
	HRESULT SetUp_Components() override;
	HRESULT SetUp_ShaderResources() override;
	
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

private:
	_uint		m_iRandAttack = 0;

public:
	static CSkeletonWarrior* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END