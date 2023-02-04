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

class CPrincePlayer final : public CGameObject
{
public:
	enum COLLIDERTYPE { COLLTYPE_AABB, /*COLLTYPE_OBB, COLLTYPE_SPHERE,*/ COLLTYPE_END };

	enum PLAYER_DIRECTION
	{
		PLAYER_FM, PLAYER_BM, PLAYER_RM, PLAYER_LM,
		PLAYER_FR, PLAYER_FL, PLAYER_BR, PLAYER_BL,
		PLAYER_END
	};

	enum ANIMATION
	{
		PRINCE_ADD_DMG_B,		
		PRINCE_ADD_DMG_F,
		PRINCE_ADD_MOUNT_BRAKE_HORSE,
		PRINCE_ADD_MOUNT_IDLE_ARM_DOWNWARD,
		PRINCE_ADD_MOUNT_IDLE_FOLD_HORSE,
		PRINCE_ADD_MOUNT_IDLE_Arm_Upward_Horse,
		PRINCE_ADD_mount_idle_horse_ani,
		PRINCE_ADD_Mount_Idle_Leg_Narrower_Horse,
		PRINCE_ADD_Mount_Idle_Leg_Wider_Horse,
		PRINCE_ADD_Mount_Idle_Pose_horse,
		PRINCE_ADD_Mount_Idle_Spine_Backward_Horse,
		PRINCE_ADD_Mount_Idle_Spine_Forward_Horse,
		PRINCE_ADD_mount_ROT_L_horse,
		PRINCE_ADD_mount_ROT_pose_L_horse,
		PRINCE_ADD_mount_ROT_pose_R_horse,
		PRINCE_ADD_mount_ROT_R_horse,
		PRINCE_ADD_Mount_Run_F_Arm_Downward_Horse,
		PRINCE_ADD_Mount_Run_F_Arm_Fold_Horse,
		PRINCE_ADD_Mount_Run_F_Arm_Upward_Horse,
		PRINCE_ADD_Mount_Run_F_Leg_Narrower_Horse,
		PRINCE_ADD_Mount_Run_F_Leg_Wider_Horse,
		PRINCE_ADD_Mount_Run_F_Spine_Backward_Horse,
		PRINCE_ADD_Mount_Run_F_Spine_Forward_Horse,
		PRINCE_ATK_01,
		PRINCE_ATK_01_Upper,	
		PRINCE_ATK_02,
		PRINCE_ATK_02_Upper,		
		PRINCE_Atk_ALL,
		PRINCE_DeadBody,
		PRINCE_DeadBody_WP,		
		PRINCE_Die,
		PRINCE_Die_WP,		
		PRINCE_DMG_B_upper_WP,	
		PRINCE_DMG_B_WP,
		PRINCE_DMG_F_upper_WP,		
		PRINCE_DMG_F_WP,
		PRINCE_Down,
		PRINCE_down_deadbody,		
		PRINCE_Down_Loop,		
		PRINCE_Get_Up,
		PRINCE_Hit_bounce,	
		PRINCE_Hit_Down,	
		PRINCE_Hit_Loop,		
		PRINCE_Hit_Up,
		PRINCE_Idle,
		PRINCE_Idle_P_02,	
		PRINCE_Idle_P_03,		
		PRINCE_idle_ready,	
		PRINCE_Idle_WP,	
		PRINCE_Jump_Land,
		PRINCE_Jump_Land_WP,	
		PRINCE_jump_loop,
		PRINCE_jump_loop_WP,	
		PRINCE_Jump_Up,
		PRINCE_Jump_Up_WP,	
		PRINCE_Mount_Brake_human,
		PRINCE_mount_flying_b_horse,
		PRINCE_mount_flying_f_flutter_horse,
		PRINCE_mount_flying_idle_horse,
		PRINCE_Mount_Idle_Arm_Downward_Horse,
		PRINCE_Mount_Idle_Arm_Fold_Horse,
		PRINCE_Mount_Idle_Arm_Upward_Horse,
		PRINCE_mount_idle_horse,
		PRINCE_mount_Idle_Horse01,
		PRINCE_mount_idle_horse_ani,
		PRINCE_mount_idle_horse_pose,
		PRINCE_mount_idle_human,
		PRINCE_mount_idle_hyaen_ani,
		PRINCE_mount_idle_hyaen_pose,
		PRINCE_Mount_Idle_Leg_Narrower_Horse,
		PRINCE_Mount_Idle_Leg_Wider_Horse,
		PRINCE_mount_Idle_ROT_L_Loop_horse,
		PRINCE_mount_Idle_ROT_L_Start_horse,
		PRINCE_mount_Idle_ROT_R_Loop_horse,
		PRINCE_mount_Idle_ROT_R_Start_horse,
		PRINCE_Mount_Idle_Spine_Backward_Horse,
		PRINCE_Mount_Idle_Spine_Forward_Horse,
		PRINCE_mount_jump_land_horse,
		PRINCE_Mount_ROT_L_Horse,
		PRINCE_Mount_ROT_pose_L_Horse,
		PRINCE_Mount_ROT_pose_R_Horse,
		PRINCE_Mount_ROT_R_Horse,
		PRINCE_mount_run_b_horse,
		PRINCE_Mount_Run_F,
		PRINCE_Mount_Run_F_Arm_Downward_Horse,
		PRINCE_Mount_Run_F_Arm_Fold_Horse,
		PRINCE_Mount_Run_F_Arm_Upward_Horse,
		PRINCE_mount_run_f_horse,
		PRINCE_Mount_Run_F_Leg_Narrower_Horse,
		PRINCE_Mount_Run_F_Leg_Wider_Horse,
		PRINCE_Mount_Run_F_Spine_Backward_Horse,
		PRINCE_Mount_Run_F_Spine_Forward_Horse,
		PRINCE_mount_run_f_stop_horse,
		PRINCE_mount_run_jump_land_horse,
		PRINCE_mount_run_jump_loop_horse,
		PRINCE_mount_run_jump_up_horse,
		PRINCE_mount_run_start_horse,
		PRINCE_mount_special_horse,
		PRINCE_Passout,
		PRINCE_Passout_WP,	
		PRINCE_pick_up,
		PRINCE_put_down,
		PRINCE_Return_CST_end,	
		PRINCE_Return_CST_loop,		
		PRINCE_Return_CST_start,
		PRINCE_Revival,		
		PRINCE_Ro_statue_pose,
		PRINCE_ROT_L,	
		PRINCE_ROT_R,
		PRINCE_Run_B,
		PRINCE_Run_B_WP,	
		PRINCE_Run_BL_WP,		
		PRINCE_Run_BR_WP,	
		PRINCE_Run_F,
		PRINCE_Run_F_33,		
		PRINCE_Run_F_39,	
		PRINCE_Run_F_Stop,
		PRINCE_run_f_stop_010,		
		PRINCE_run_f_stop_011,	
		PRINCE_Run_F_Stop_WP,
		PRINCE_Run_F_WP,		
		PRINCE_Run_Fall_B,	
		PRINCE_Run_Fall_B_WP,	
		PRINCE_Run_Fall_F,	
		PRINCE_Run_Fall_F_WP,		
		PRINCE_Run_FL_WP,		
		PRINCE_Run_FR_WP,
		PRINCE_run_jump_land_b,
		PRINCE_run_jump_land_b_WP,	
		PRINCE_run_jump_land_f,
		PRINCE_run_jump_land_f_WP,	
		PRINCE_run_jump_loop_b,
		PRINCE_run_jump_loop_b_WP,	
		PRINCE_run_jump_loop_f,
		PRINCE_run_jump_loop_f_WP,	
		PRINCE_Run_Jump_Up_B,
		PRINCE_run_jump_up_B_WP,	
		PRINCE_Run_Jump_Up_F,
		PRINCE_run_jump_up_F_WP,	
		PRINCE_Run_L_WP,		
		PRINCE_Run_R_WP,
		PRINCE_sewing_End,
		PRINCE_sewing_Loop,
		PRINCE_sewing_Start,
		PRINCE_Sit_Down,
		PRINCE_Sitting,
		PRINCE_SK_01_all,
		PRINCE_sK_01_CST_end,
		PRINCE_sK_01_CST_loop,
		PRINCE_sK_01_CST_start,
		PRINCE_sk_02_all,
		PRINCE_sk_02_CST_end,
		PRINCE_sk_02_CST_loop,
		PRINCE_sk_02_CST_start,
		PRINCE_SK_03,
		PRINCE_SK_04_A,
		PRINCE_SK_04,
		PRINCE_sk_05,
		PRINCE_sk_06_CST_end,
		PRINCE_sk_06_CST_loop,
		PRINCE_sk_06_CST_start,
		PRINCE_sk_08,
		PRINCE_sk_09,
		PRINCE_SK_10,
		PRINCE_sk_11_cst_end,
		PRINCE_sk_11_cst_loop,
		PRINCE_sk_11_cst_start,
		PRINCE_sk_12_all,
		PRINCE_sk_12_CST_end,
		PRINCE_sk_12_CST_loop,
		PRINCE_sk_12_CST_start,
		PRINCE_SK_13,
		PRINCE_SK_14,
		PRINCE_SK_15,
		PRINCE_sk_16_cst_end,
		PRINCE_sk_16_cst_loop,
		PRINCE_sk_16_cst_start,
		PRINCE_SK_17,
		PRINCE_SK_18,
		PRINCE_SK_19,
		PRINCE_SK_20,
		PRINCE_sk_21_cst_end,
		PRINCE_sk_21_cst_loop,
		PRINCE_sk_21_cst_start,
		PRINCE_SK_22,
		PRINCE_sk_23_end,
		PRINCE_sk_23_start,
		PRINCE_SK_24,
		PRINCE_SK_25,
		PRINCE_SK_26,
		PRINCE_sk_27_end,
		PRINCE_sk_27_start,
		PRINCE_sk_28_cst_end,
		PRINCE_sk_28_cst_loop,
		PRINCE_sk_28_cst_start,
		PRINCE_SK_29,
		PRINCE_SK_30,
		PRINCE_SK_UseItem_A,
		PRINCE_SK_UseItem_B,
		PRINCE_Sprint,
		PRINCE_sprint_fall,
		PRINCE_sprint_jump_land,
		PRINCE_sprint_jump_loop,
		PRINCE_Sprint_Jump_Up,
		PRINCE_sprint_start,
		PRINCE_sprint_stop,
		PRINCE_Stand_Up,
		PRINCE_UnWeapon,
		PRINCE_UnWeapon_Ready,
		PRINCE_UnWeapon_Run,
		PRINCE_UseItem_throw,
		PRINCE_V_def_01,
		PRINCE_Wait_Loop,
		PRINCE_Walk_B,
		PRINCE_Walk_B_WP,
		PRINCE_Walk_F,
		PRINCE_Walk_F_WP,
		PRINCE_Weapon,
		PRINCE_Weapon_Ready,
		PRINCE_Weapon_Run,
		PRINCE_STATE_END
	};

public:
		CPrincePlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
		CPrincePlayer(const CPrincePlayer& rhs);
		virtual ~CPrincePlayer() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT RenderShadow() override;

	void    SetUp_FSM();
	void	  SetUp_HorseFSM();
	void	  Movement(_double TimeDelta);
	void	  AdditiveAnim(_double TimeDelta);
	void	  LinkObject(_double TimeDelta);

	void    Reset_Anim(ANIMATION eAnim);
	void    Set_Anim(ANIMATION eAnim);
	CFSMComponent*			Get_FSMtoHorse() { return m_pHorseFSM; }
	_bool			Get_Ride() {		return m_bRideHorse;	}
	void	Set_Move(_bool bSetMove) { m_bSetMove = bSetMove; }

private:
	void	Imgui_RenderProperty() override;

private:
	_bool								m_bSetMove = false;

	CModel*							m_pModelCom = nullptr;
	CShader*							m_pShaderCom = nullptr;
	CCollider*							m_pColliderCom[COLLTYPE_END] = { nullptr };
	CRenderer*						m_pRendererCom = nullptr;
	CNavigation*					m_pNavigationCom = nullptr;

	CFSMComponent*			m_pFSM = nullptr;
	CFSMComponent*			m_pHorseFSM = nullptr;

	PLAYER_DIRECTION		m_eState = PLAYER_FM;
	PLAYER_DIRECTION		m_ePreState = PLAYER_END;

	class CPlayerCamera*		m_pCam = nullptr;
	class CHorse*					m_pHorse = nullptr;

	_float								m_MouseSensity = 0.1f;
	_float								m_fVelocity = 1.f;
private:
	void					Idle_Tick(_double TimeDelta);
	void					Walk_Tick(_double TImeDelat);
	void					Run_Tick(_double TimeDelta);

	/* 애니메이션 시간 체크를 위한 함수*/
	_bool					AnimFinishChecker(ANIMATION eAnim, _double FinishRate = 0.95);
	_bool					AnimIntervalChecker(ANIMATION eAnim, _double StartRate, _double FinishRate);
	void						AnimEditPlayTime(ANIMATION eAnim, _double PlayTime);
	/*******************************/
private:
	_bool					m_bCamTurn = false;
	_bool					m_bPlayerTurn = true;
	_bool					m_bCamChange = false;

	_bool					m_bMove = true;
	_bool					m_bRunning = false;
	_bool					m_bWalking = false;

	_bool					m_bAction = false;
	_bool					m_bAttack = false;

	_bool					m_bGroggy = false;
	_bool					m_bHitDown = false;
	_double				m_HitDownDelayTime = 0.0;

	_bool					m_bNormalAttack_1 = false;
	_bool					m_bNormalAttack_2 = false;

	_bool					m_bSK01 = false;
	_bool					m_bSK02 = false;
	_bool					m_bSK03 = false;
	_bool					m_bSK04_Charging = false;
	_bool					m_bSK05 = false;
	_bool					m_bSK06 = false;
	_bool					m_bSK07 = false;
	_bool					m_bSK08 = false;

	_bool					m_bV_DEF = false;
	_bool					m_bJump = false;
	_float					m_fJumpPower = 0.f;
	_float					m_fKnockBackPower = 0.f;
	_double				m_HitDownDurationTime = 0.f;

	_bool					m_bRideHorse = false;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();
	HRESULT	SetUP_ShadowShaderResources();

public:
	static CPrincePlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END