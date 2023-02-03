#pragma once
#include "Ally.h"

BEGIN(Client)

class CBalianBollwerk final : public CAlly
{
	enum UI{ UI_CONVERSATION, UI_END };

	enum ANIMATION
	{
		BALIANBOLLWERK_ADD_DMG_B
		, BALIANBOLLWERK_ADD_DMG_F
		, BALIANBOLLWERK_ATK_01
		, BALIANBOLLWERK_ATK_02
		, BALIANBOLLWERK_DeadBody
		, BALIANBOLLWERK_Die
		, BALIANBOLLWERK_DMG_B
		, BALIANBOLLWERK_DMG_F
		, BALIANBOLLWERK_down_deadbody
		, BALIANBOLLWERK_Down_Loop
		, BALIANBOLLWERK_Down
		, BALIANBOLLWERK_Get_Up
		, BALIANBOLLWERK_Hit_bounce
		, BALIANBOLLWERK_Hit_Down
		, BALIANBOLLWERK_Hit_Loop
		, BALIANBOLLWERK_Hit_Up
		, BALIANBOLLWERK_Idle_C
		, BALIANBOLLWERK_Idle_P_01
		, BALIANBOLLWERK_Passout
		, BALIANBOLLWERK_Run_F
		, BALIANBOLLWERK_UnWeapon_Ready
		, BALIANBOLLWERK_UnWeapon_Run
		, BALIANBOLLWERK_UnWeapon
		, BALIANBOLLWERK_Walk_F
		, BALIANBOLLWERK_Weapon_Ready
		, BALIANBOLLWERK_Weapon_Run
		, BALIANBOLLWERK_Weapon
		, BALIANBOLLWERK_END
	};

private:
	CBalianBollwerk(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBalianBollwerk(const CBalianBollwerk& rhs);
	virtual ~CBalianBollwerk() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT RenderShadow() override;


	void	Level_Chap1Tick(_double TimeDelta);
	void	Level_Chap2Tick(_double TimeDelta);

	void	Imgui_RenderProperty() override;

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
	HRESULT	SetUP_ShadowShaderResources();

	_bool	AnimFinishChecker(ANIMATION eAnim, _double FinishRate = 0.95);
	_bool   AnimIntervalChecker(ANIMATION eAnim, _double StartRate, _double FinishRate);

	_double m_AttackDelayTime = 0.0;

	_int	m_iRandAttack = 0;

	vector<CGameObject*> m_UI;

	_double TimeConversation = 0.0;
	_bool	m_bConversation = true;
	_bool	m_bSecondStageCheck = false;
	_bool	m_bChangeBGM = false;
	vector<_float4> m_CheckPoints;
	wstring	m_strConversation;

	_bool	m_bChap1Speak = false;
	_bool	m_bChap1FinSpeak = false;

public:
	static CBalianBollwerk* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END