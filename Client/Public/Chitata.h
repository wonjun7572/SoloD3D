#pragma once
#include "Ally.h"

BEGIN(Client)

class CChitata final : public CAlly
{
	enum ANIMATION
	{
		CHITATA_ADD_DMG_B
		, CHITATA_ADD_DMG_F
		, CHITATA_ATK_01
		, CHITATA_ATK_02
		, CHITATA_DeadBody
		, CHITATA_Die
		, CHITATA_DMG_B_Upper
		, CHITATA_DMG_B
		, CHITATA_DMG_F_Upper
		, CHITATA_DMG_F
		, CHITATA_down_deadbody
		, CHITATA_Down_Loop
		, CHITATA_Down
		, CHITATA_Hit_bounce
		, CHITATA_Hit_Down
		, CHITATA_Hit_Loop
		, CHITATA_Hit_Up
		, CHITATA_Idle_C
		, CHITATA_Idle_P_01
		, CHITATA_Passout
		, CHITATA_Run_F
		, CHITATA_SK_Firing_01
		, CHITATA_UnWeapon_Ready
		, CHITATA_UnWeapon_Run
		, CHITATA_UnWeapon
		, CHITATA_Walk_F
		, CHITATA_Weapon_Ready
		, CHITATA_Weapon_Run
		, CHITATA_Weapon
		, CHITATA_END
	};

private:
	CChitata(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CChitata(const CChitata& rhs);
	virtual ~CChitata() = default;

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
	static CChitata* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END