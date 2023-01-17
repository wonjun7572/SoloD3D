#pragma once

#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
class CRenderer;
class CNavigation;
class CAnimation;
class CFSMComponent;
END

BEGIN(Client)

class CPlayer final : public CGameObject
{
public:
	enum COLLIDERTYPE { COLLTYPE_AABB, /*COLLTYPE_OBB, COLLTYPE_SPHERE,*/ COLLTYPE_END };
	
	enum PLAYER_PARTS {PART_WEAPON,  
		PART_UPPER, PART_LOWER,  
		PART_SHOULDER, PART_GLOVE , 
		PART_BELT, PART_BOOTS, PART_HELMET, PART_END};

	enum PLAYER_DIRECTION
	{
		PLAYER_FM, PLAYER_BM, PLAYER_RM, PLAYER_LM,
		PLAYER_FR, PLAYER_FL , PLAYER_BR, PLAYER_BL,
		PLAYER_END
	};

	enum PLAYER_EFFECT
	{
		NORATK1, NORATK2, NORATK3, WING_SKILL2, LINE_AURA, THUNDERWAVE, EFFECT_END
	};

	enum ANIMATION
	{
		PLAYER_ADD_DMG_B, PLAYER_ADD_DMG_F,
		PLAYER_ATK_01, PLAYER_ATK_01_UPPER,
		PLAYER_ATK_02, PLAYER_ATK_02_UPPER,
		PLAYER_ATK_03, PLAYER_ATK_03_UPPER,

		PLAYER_DEAD_BODY, PLAYER_DIE,
		PLAYER_DMG_B_UPPER, PLAYER_DMG_B,
		PLAYER_DMG_F_UPPER, PLAYER_DMG_F,

		PLAYER_DOWN, PLAYER_DOWN_ALL, PLAYER_DOWN_DEADBODY,
		PLAYER_DOWN_LOOP, PLAYER_GETUP,
		PLAYER_HIT_BOUNCE, PLAYER_HIT_DOWN, PLAYER_HIT_LOOP,
		PLAYER_HIT_UP, PLAYER_HIT_UP_ALL, PLAYER_IDLE_READY,
		PLAYER_IDLE_WEAPON, PLAYER_JUMP_LAND, PLAYER_JUMP_LOOP,
		PLAYER_JUMP_UP, PLAYER_PASSOUT, PLAYER_RUN_B, PLAYER_RUN_BL,
		PLAYER_RUN_BR, PLAYER_RUN_F_STOP, PLAYER_RUN_F, PLAYER_RUN_FALL_B,
		PLAYER_RUN_FALL_F, PLAYER_RUN_FL, PLAYER_RUN_FR,
		PLAYER_RUN_JUMP_LAND_B, PLAYER_RUN_JUMP_LAND_F,
		PLAYER_RUN_JUMP_LOOP_B, PLAYER_RUN_JUMP_LOOP_F,
		PLAYER_RUN_JUMP_UP_B, PLAYER_RUN_JUMP_UP_F,
		PLAYER_RUN_L, PLAYER_RUN_R,
		PLAYER_SK01, PLAYER_SK01_UPPER,
		PLAYER_SK02, PLAYER_SK02_UPPER, PLAYER_SK03, PLAYER_SK04,
		PLAYER_SK05, PLAYER_SK05_02, PLAYER_SK05_UPPER,
		PLAYER_SK08, PLAYER_SK08_UPPER, PLAYER_SK09,
		PLAYER_SK10, PLAYER_SK10_UPPER, PLAYER_SK11_1, PLAYER_SK11_2,
		PLAYER_SK11, PLAYER_SK11_UPPER, PLAYER_SK12 ,PLAYER_SK12_A, 
		PLAYER_SK12_B, PLAYER_SK12_UPPER, PLAYER_SK13, PLAYER_SK15,
		PLAYER_SK17, PLAYER_SK19, PLAYER_SK21, PLAYER_SK21_UPPER,
		PLAYER_SK22, PLAYER_SK23, PLAYER_SK23_UPPER, PLAYER_SK24,
		PLAYER_SK25, PLAYER_SK25_1, PLAYER_SK25_2,  PLAYER_SK27_CHARGING,
		PLAYER_SK27_CHARGING_UPPER, PLAYER_SK27_FIRING, PLAYER_SK28_CHARGING, PLAYER_SK28_FIRING,
		PLAYER_SK29_CHARGING, PLAYER_SK29_CHARGING_UPPER, PLAYER_SK29_FIRING,
		PLAYER_SK30_CHARGING, PLAYER_SK30_CHARGING_UPPER, PLAYER_SK30_FIRING,
		PLAYER_SK31_FIRING,
		PLAYER_SK32_CHARGING, PLAYER_SK32_CHARGING_UPPER, PLAYER_SK32_FIRING,
		PLAYER_SK33, PLAYER_SK34, PLAYER_SK35, PLAYER_SK36, PLAYER_SK37,
		PLAYER_SK38, PLAYER_SK39, PLAYER_SK40, PLAYER_SK41, PLAYER_SK42,
		PLAYER_SK43, PLAYER_USEITEM_A, PLAYER_USEITEM_B, PLAYER_UNWEAPON, PLAYER_UNWEAPON_READY,
		PLAYER_UNWEAPON_RUN,  PLAYER_V_DEF, PLAYER_WALK_B, PLAYER_WALK_F,
		PLAYER_WEAPON, PLAYER_WEAPON_READY, PLAYER_WEAPON_RUN, PLAYER_STATE_END
	};

	enum MODEL { MODEL_NOMAL, MODEL_A, MODEL_END };

	enum UI	{ SKILL_ICON_1, SKILL_ICON_2, SKILL_ICON_3, 
		SKILL_ICON_4, SKILL_ICON_5, SKILL_MODELATIME, V_DEF,
		HP , MP, UI_END };

private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	void Effect_Tick(_double TimeDelta);
	void UI_Tick(_double TimeDelta);

	CModel*					Get_ModelCom() { return m_pModelCom[m_eModelState]; }
	CNavigation*			Get_NaviCom() { return m_pNavigationCom; }
	vector<CGameObject*>	Get_PlayerParts() { return m_PlayerParts; }

	void    SetUp_FSM();
	//void	MoveToAnim(_double TimeDelta);
	void	Movement(_double TimeDelta);
	void	AdditiveAnim(_double TimeDelta);
	void	LinkObject(_double TimeDelta);

	void	Reset_Anim(ANIMATION eAnim);
	void	Set_Anim(ANIMATION eAnim);

	void	ChangeModel(MODEL eModelIndex);

private:
	void	Imgui_RenderProperty() override;

private:
	CModel*					m_pModelCom[MODEL_END] = { nullptr };

	CCollider*				m_pColliderCom[COLLTYPE_END] = { nullptr };
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CNavigation*			m_pNavigationCom = nullptr;

private:
	vector<CGameObject*>	m_PlayerParts;
	
	vector<CGameObject*>	m_PlayerEffects;

	vector<CGameObject*>	m_PlayerUI;

	_uint					m_PartSize = 0;
	_float					m_MouseSensity = 0.1f;
	_bool					Get_CamTurn() { return m_bCamTurn; }

public:
	void					Idle_Tick(_double TimeDelta);
	void					Walk_Tick(_double TImeDelat);
	void					Run_Tick(_double TimeDelta);

public:
	CCollider*				Get_WeaponCollider();

private:
	_bool					m_bCamTurn = false;

	_float					m_fVelocity = 1.f;
	/* 로컬 애니메이션 움직임때문에 넣었으나 현재 그냥 조종해주는것이 더 낫다고 판단 */
	//_float4					m_vAnimationMove = _float4(0.f, 0.f, 0.f, 1.f);

	/* 피격을 위해서 */
public:
	void					BackDamagedToMonster(_float fDamage);
	void					FrontDamagedToMonster(_float fDamage);
	void					PassOutToMonster(_float fDamage, _float fKnockBackPower = 0.f);
	void					HitDownToMonster(_float fDamage, _float fJumpPower = 1.5f, _float fKnockBackPower = 2.f, _float fDuration = 1.f);

	/* 타격을 위해서*/
public:
	void					MonsterNormalAttack(_bool bAttack);
	
	void					MonsterSkill02(_bool bAttack);
	void					MonsterSkill04(_bool bAttack);

public:
	void					Reset_Action();
	class CPlayerCamera*	Get_PlayerCam() { return m_pCam; }
	void					CamLockOn(CGameObject* pGameObject, OUT _bool& bLock);
	
	_bool					Get_Conversation() { return m_bConversation; }

private:
	CFSMComponent*			m_pFSM = nullptr;

	PLAYER_DIRECTION		m_eState = PLAYER_FM;
	PLAYER_DIRECTION		m_ePreState = PLAYER_END;

	MODEL					m_eModelState = MODEL_NOMAL;
	MODEL					m_ePreModelState = MODEL_END;

	/* 애니메이션 시간 체크를 위한 함수*/
	_bool					AnimFinishChecker(ANIMATION eAnim, _double FinishRate = 0.95);
	_bool					AnimIntervalChecker(ANIMATION eAnim, _double StartRate,	_double FinishRate);
	void					AnimEditPlayTime(ANIMATION eAnim, _double PlayTime);
	/*******************************/

	/* FSM을 위한 변수들 */
	_bool					m_bMove = true;
	_bool					m_bRunning = false;
	_bool					m_bWalking = false;

	_bool					m_bAction = false;

	_bool					m_bAttack = false;

	_bool					m_bGroggy = false;
	_bool					m_bHitDown = false;
	_double					m_HitDownDelayTime = 0.0;

	_bool					m_bNormalAttack_1 = false;
	_bool					m_bNormalAttack_2 = false;
	_bool					m_bNormalAttack_3 = false;

	_bool					m_bSK01 = false;
	_bool					m_bSK02 = false;
	_bool					m_bSK03 = false;
	_bool					m_bSK04_Charging = false;
	_bool					m_bSK05 = false;

	_bool					m_bV_DEF = false;
	_bool					m_bJump = false;
	_float					m_fJumpPower = 0.f;
	_float					m_fKnockBackPower = 0.f;
	_double					m_HitDownDurationTime = 0.f;

	/*******************/

	/*피격을 위한 변수*/
	_bool					m_bFrontDamagedToMonster = false;
	_bool					m_bFrontDamage = false;

	_bool					m_bBackDamagedToMonster = false;
	_bool					m_bBackDamaged = false;
	/****************/

	_bool					CheckFinish_Attack1();
	_bool					CheckFinish_Attack2();
	_bool					CheckFinish_Attack3();

	_bool					CheckFinish_Skill1();
	_bool					CheckFinish_Skill2();
	_bool					CheckFinish_Skill3();
	_bool					CheckFinish_Skill4();
	_bool					CheckFinish_Skill5();

	_bool					CheckFinish_V_DEF();
	
	_bool					m_bCamChange = false;

	_double					m_dModelATime = 0.0;

	/* for . Imgui*/
	_float					m_CX = 1.f, m_CY = 1.f, m_CZ = 1.f;
	_float					m_RX = 0.f, m_RY = 0.f, m_RZ = 0.f;
	_float					m_X = 0.f, m_Y = 0.f, m_Z = 0.f;
	
	class CPlayerCamera*	m_pCam = nullptr;

	/* 체력 및 공격력 세팅 값*/

	_float	m_fHp = 0.f;
	_float  m_fMp = 0.f;
	_float	m_fAttack = 0.f;
	_float	m_fDefence = 0.f;

	/********************/

	_double m_WingAlpha = 0.0;
	_float  m_fWingY = 1.f;

	//_float m_fTime = 0.2f;
	//_float m_fFinishTime = 0.4f;

	//_float m_fWingTestUPY = 1.f;
	//_float m_fWingTestDOWNY = 1.f;

	/* 스킬 쿨타임~! */
	_double m_Skill_1IconCollTime = 15.0;
	_double m_Skill_2IconCoolTime = 20.0;
	_double m_Skill_3IconCoolTime = 10.0;
	_double m_Skill_4IconCollTime = 20.0;
	_double m_Skill_5IconCoolTime = 30.0;

	_double m_Skill_VDefCoolTime = 7.0;

	// 데미지를 입을 수 있는 상황인가?
	_bool					m_bImpossibleSkillDamaged = false;
	_bool					m_bImpossibleDamaged = false;
	/****************/
	void	AdjustDamage(_float fDamage);
	void	AdjustSkillDamage(_float fDamage);

	_bool					m_bCamLock = false;

	_bool					m_bConversation = false;

private:
	HRESULT SetUp_Parts();
	HRESULT SetUp_Effects();
	HRESULT SetUp_UI();
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END