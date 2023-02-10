#pragma once

#include "Monster.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

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
		, DEMON_Down
		, DEMON_down_deadbody
		, DEMON_Down_Loop
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

	enum MONSTER_EFFECT
	{
		FIRE_SKILL3, SKILLEND
	};

	enum UI { UI_CONVERSATION, UI_END };

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

	void	Level_Chap2Tick(_double TimeDelta);
	_bool	Get_LevelChap2Finished() { return m_bLevel2Finish; }
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
	_bool	m_bLevel2Finish = false;
	_bool	m_bPlayerChase = false;
	_bool	m_bPlayerAttack = true;
	_bool	m_bRealAttack = false;

	/* SKILL */
	_bool m_bRealSkill = false;

	_bool  m_bSkill_1ToPlayer = false;
	_bool  m_bSkill_2ToPlayer = false;
	_bool  m_bSkill_3ToPlayer = false;
	_bool  m_bSkill_4ToPlayer = false;
	_bool  m_bSkill_5ToPlayer = false;
	_bool  m_bSkill_6ToPlayer = false;

	_double m_AttackDelayTime = 0.0;
	_double m_HitDownDelayTime = 0.0;
	_double m_GroggyDelayTime = 0.0;
	_double	m_SkillDelayTime = 0.0;

	_float	m_fSkill3EffectTime = 0.f;
	_float	m_fSkill5EffectTime = 0.f;

	_double	TimeConversation = 0.0;
	wstring		m_strConversation;
	
	_int		m_iSkillOrder = 0;
	_bool	m_bSkillOrderFinish = false;

private:
	CCollider* m_pAttackColCom = nullptr;
	CCollider* m_pSwordColCom = nullptr;
	CCollider* m_pSkillHitDownColCom = nullptr;
	CCollider* m_pSkillKnockBackColCom = nullptr;

	_float m_fOBBX = 0.f;
	_float m_fOBBY = 0.f;
	_float m_fOBBZ = 0.f;
	_float m_fOBBCX = 1.f;
	_float m_fOBBCY = 1.f;
	_float m_fOBBCZ = 1.f;
	_float m_fRotationX = 0.f;
	_float m_fRotationY = 0.f;
	_float m_fRotationZ = 0.f;

	_float m_fSkillHitDownRange = 0.f;
	_float4x4 m_Mat;
	
private:
	_uint m_iRandAttack = 0;

	vector<CGameObject*>	m_UI;

	_bool m_bConversation = false;

	/* for . Imgui*/
	_float					m_CX = 1.f, m_CY = 1.f, m_CZ = 1.f;
	_float					m_RX = 0.f, m_RY = 0.f, m_RZ = 0.f;
	_float					m_X = 0.f, m_Y = 0.f, m_Z = 0.f;

	CGameObject*		m_pSkill3Particle = nullptr;
	CGameObject*		m_pSkill4Rock = nullptr;

public:
	static CDemon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END