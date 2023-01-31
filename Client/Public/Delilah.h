#pragma once
#include "Ally.h"

BEGIN(Client)

class CDelilah final : public CAlly
{
	enum UI { UI_CONVERSATION, UI_END };

	enum ANIMATION
	{
		DELILAH_ADD_DMG_B
		, DELILAH_ADD_DMG_F
		, DELILAH_ATK_01
		, DELILAH_ATK_02
		, DELILAH_DeadBody
		, DELILAH_Die
		, DELILAH_DMG_B_Upper
		, DELILAH_DMG_B
		, DELILAH_DMG_F_Upper
		, DELILAH_DMG_F
		, DELILAH_Down
		, DELILAH_down_deadbody
		, DELILAH_Down_Loop
		, DELILAH_Get_Up
		, DELILAH_Hit_bounce
		, DELILAH_Hit_Down
		, DELILAH_Hit_Loop
		, DELILAH_Hit_Up
		, DELILAH_Idle_C
		, DELILAH_Idle_P_01
		, DELILAH_Passout
		, DELILAH_Run_F
		, DELILAH_SK_Firing_01
		, DELILAH_SK_Firing_02
		, DELILAH_SK_Firing_03
		, DELILAH_UnWeapon_Ready
		, DELILAH_UnWeapon_Run
		, DELILAH_UnWeapon
		, DELILAH_Walk_F
		, DELILAH_Weapon_Ready
		, DELILAH_Weapon_Run
		, DELILAH_Weapon
		, DELILAH_END
	};

private:
	CDelilah(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDelilah(const CDelilah& rhs);
	virtual ~CDelilah() = default;

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
	_bool	Get_Conversation() { return m_bConversation; }

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
	
	_int	m_iRandAttack = 0;

	vector<CGameObject*> m_UI;

	_double TimeConversation = 0.0;
	_bool	m_bConversation = true;
	_bool	m_bSecondStageCheck = false;
	vector<_float4> m_CheckPoints;
	wstring	m_strConversation;

public:
	static CDelilah* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END