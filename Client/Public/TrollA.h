#pragma once

#include "Monster.h"

BEGIN(Client)

class CTrollA final : public CMonster
{
	enum ANIMATION
	{
		TROLLA_ADD_DMG_B
		, TROLLA_ADD_DMG_F
		, TROLLA_ATK_01
		, TROLLA_ATK_02
		, TROLLA_ATK_03
		, TROLLA_DeadBody
		, TROLLA_Die
		, TROLLA_DMG_B
		, TROLLA_DMG_F
		, TROLLA_Down
		, TROLLA_down_deadbody
		, TROLLA_Down_Loop
		, TROLLA_Get_Up
		, TROLLA_Hit_bounce
		, TROLLA_Hit_Down
		, TROLLA_Hit_Loop
		, TROLLA_Hit_Up
		, TROLLA_Idle_C
		, TROLLA_Idle_P_01
		, TROLLA_Idle_P_02
		, TROLLA_Passout
		, TROLLA_Run_F
		, TROLLA_SK_Firing_01
		, TROLLA_Walk_F
		, TROLLA_END
	};

public:
	typedef struct tagTrollADesc
	{
		_uint	iGroup;
		_float4 vPos;
		_float	fRadian;
	}TROLLADESC;

private:
	CTrollA(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTrollA(const CTrollA& rhs);
	virtual ~CTrollA() = default;

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

private:
	_uint m_iRandAttack = 0;
	_float3		m_vSecondCheckPoint;
	_bool		m_bSecondCheckPoint = false;

public:
	static CTrollA* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END