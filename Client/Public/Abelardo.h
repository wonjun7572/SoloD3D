#pragma once

#include "Ally.h"

BEGIN(Client)

class CAbelardo final : public CAlly
{
	enum UI { UI_CONVERSATION, UI_END };

	enum ANIMATION
	{
		ABELARDO_ADD_DMG_B
		, ABELARDO_ADD_DMG_F
		, ABELARDO_ATK_01
		, ABELARDO_ATK_02
		, ABELARDO_DeadBody
		, ABELARDO_Die
		, ABELARDO_DMG_B_UnWeapon
		, ABELARDO_DMG_B
		, ABELARDO_DMG_F_UnWeapon
		, ABELARDO_DMG_F
		, ABELARDO_Down
		, ABELARDO_down_deadbody
		, ABELARDO_Down_Loop
		, ABELARDO_Get_Up
		, ABELARDO_Hit_bounce
		, ABELARDO_Hit_Down
		, ABELARDO_Hit_Loop
		, ABELARDO_Hit_Up
		, ABELARDO_Idle_C
		, ABELARDO_Idle_P_01
		, ABELARDO_Passout
		, ABELARDO_Run_F
		, ABELARDO_SK_Firing_01
		, ABELARDO_UnWeapon_Ready
		, ABELARDO_UnWeapon_Run
		, ABELARDO_UnWeapon
		, ABELARDO_Walk_F
		, ABELARDO_Weapon_Ready
		, ABELARDO_Weapon_Run
		, ABELARDO_Weapon
		, ABELARDO_END
	};

private:
	CAbelardo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAbelardo(const CAbelardo& rhs);
	virtual ~CAbelardo() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	void	Imgui_RenderProperty() override;

	void	Level_Chap2Tick(_double TimeDelta);
	void	Conversation(_double TimeDelta);
	_bool	DistancePointCheck(_float4 vTargetPos, _float4 vPos);

private:
	void	SetUp_FSM() override;
	void	SetUp_UI();

private:
	void	AdditiveAnim(_double TimeDelta);

private:
	HRESULT SetUp_Components() override;
	HRESULT SetUp_ShaderResources() override;

	_bool	AnimFinishChecker(ANIMATION eAnim, _double FinishRate = 0.95);
	_bool   AnimIntervalChecker(ANIMATION eAnim, _double StartRate, _double FinishRate);

	_double m_AttackDelayTime = 0.0;

	vector<CGameObject*> m_UI;

	_double TimeConversation = 0.0;
	_bool	m_bConversation = true;
	_bool	m_bSecondStageCheck = false;
	vector<_float4> m_CheckPoints;
	wstring	m_strConversation;

public:
	static CAbelardo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END