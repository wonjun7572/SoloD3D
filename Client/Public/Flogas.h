#pragma once

#include "Monster.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

class CFlogas final : public CMonster
{
	enum ANIMATION
	{
		FLOGAS_ADD_DMG_B
		, FLOGAS_ADD_DMG_F
		, FLOGAS_ATK_01
		, FLOGAS_ATK_02
		, FLOGAS_DeadBody
		, FLOGAS_Die
		, FLOGAS_DMG_B
		, FLOGAS_DMG_F
		, FLOGAS_Idle_C
		, FLOGAS_Idle_P_01
		, FLOGAS_Idle_P_02
		, FLOGAS_Passout
		, FLOGAS_Run_F
		, FLOGAS_SK_Firing_01
		, FLOGAS_SK_Firing_02
		, FLOGAS_SK_Firing_02_Casting
		, FLOGAS_SK_Firing_03
		, FLOGAS_SK_Firing_04
		, FLOGAS_SK_Firing_05
		, FLOGAS_SK_Firing_06
		, FLOGAS_SK_Firing_06_END
		, FLOGAS_Walk_F
		, FLOGAS_END
	};

	enum UI { UI_CONVERSATION, UI_END };

private:
	CFlogas(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFlogas(const CFlogas& rhs);
	virtual ~CFlogas() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT RenderShadow() override;

	void	Imgui_RenderProperty() override;
	void	Conversation(_double TimeDelta);

private:
	void	SetUp_FSM() override;
	void	SetUp_UI();

public:
	void Adjust_Collision(_double TimeDelta);

	void CollisionToPlayer(_double TimeDelta);
	void CollisionToAttack(_double TimeDelta);
	void CollisionToSkill(_double TimeDelta);
	void Play_Skill(_double TimeDelta);
	_bool	Get_Conversation() { return m_bConversation; }

private:
	void AdditiveAnim(_double TimeDelta);

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();
	HRESULT	SetUP_ShadowShaderResources();

	_bool	AnimFinishChecker(ANIMATION eAnim, _double FinishRate = 0.95);
	_bool   AnimIntervalChecker(ANIMATION eAnim, _double StartRate, _double FinishRate);

private:
	_bool	m_bPlayerChase = false;
	_bool	m_bPlayerAttack = true;
	_bool	m_bRealAttack = false;

	/* SKILL */
	_bool m_bRealSkill = false;

	_bool  m_bSkill_1ToPlayer = false;
	_bool  m_bSkill_2ToPlayer = false;
	_bool  m_bSkill_5ToPlayer = false;
	 
	_double	m_SkillDelayTime = 0.0;

	_double	m_Skill5Time = 0.0;
	_double	TimeConversation = 0.0;
	wstring		m_strConversation;

private:
	_uint m_iRandAttack = 0;

	vector<CGameObject*>	m_UI;

	_bool m_bConversation = false;
	_bool m_bSpawnToIdle = false;

public:
	static CFlogas* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

