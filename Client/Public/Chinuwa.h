#pragma once
#include "Ally.h"

BEGIN(Client)

class CChinuwa final : public CAlly
{
	enum ANIMATION
	{
		CHINUWA_ADD_DMG_B
		, CHINUWA_ADD_DMG_F
		, CHINUWA_ATK_01
		, CHINUWA_ATK_02
		, CHINUWA_DeadBody
		, CHINUWA_Die
		, CHINUWA_DMG_B_Upper
		, CHINUWA_DMG_B
		, CHINUWA_DMG_F_Upper
		, CHINUWA_DMG_F
		, CHINUWA_down_deadbody
		, CHINUWA_Down_Loop
		, CHINUWA_Down
		, CHINUWA_Get_Up
		, CHINUWA_Hit_bounce
		, CHINUWA_Hit_Down
		, CHINUWA_Hit_Loop
		, CHINUWA_Hit_Up
		, CHINUWA_Idle_C
		, CHINUWA_Idle_P_01
		, CHINUWA_Passout
		, CHINUWA_Run_F
		, CHINUWA_SK_Firing_01
		, CHINUWA_SK_Firing_02
		, CHINUWA_UnWeapon_Ready
		, CHINUWA_UnWeapon_Run
		, CHINUWA_UnWeapon
		, CHINUWA_Walk_F
		, CHINUWA_Weapon_Ready
		, CHINUWA_Weapon_Run
		, CHINUWA_Weapon
		, CHINUWA_END
	};

private:
	CChinuwa(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CChinuwa(const CChinuwa& rhs);
	virtual ~CChinuwa() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	void	Imgui_RenderProperty() override;

private:
	void	SetUp_FSM() override;

private:
	void	AdditiveAnim(_double TimeDelta);

private:
	HRESULT SetUp_Components() override;
	HRESULT SetUp_ShaderResources() override;

	_bool	AnimFinishChecker(ANIMATION eAnim, _double FinishRate = 0.95);
	_bool   AnimIntervalChecker(ANIMATION eAnim, _double StartRate, _double FinishRate);

public:
	static CChinuwa* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END