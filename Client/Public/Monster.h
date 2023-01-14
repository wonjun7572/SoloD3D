#pragma once

#include "Client_Define.h"
#include "GameObject.h"
#include "Player.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CCollider;
class CFSMComponent;
END

BEGIN(Client)

class CMonster abstract : public CGameObject
{
public:
	enum COLLIDERTYPE { COLLTYPE_AABB, COLLTYPE_SPHERE, COLLTYPE_END };
	enum UI { HP, TARGET_AIM, UI_END };

protected:
	CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster(const CMonster& rhs);
	virtual ~CMonster() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	virtual	void    SetUp_FSM() = 0;
	HRESULT SetUP_UI();
	void	UI_Tick(_double TimeDelta);
	void	UI_Switch(_double TimeDelta);
	CModel* Get_ModelCom() { return m_pModelCom; }

	/* 피격을 위해서 */
	void	Set_PlayerAttackCommand(_bool bAttack, _float fAttack);
	void	Set_PlayerSkill02Command(_bool bAttack, _float fAttack);
	void    Set_PlayerSkiil04Command(_bool bAttack, _float fAttack);

	void	AdjustSetDamage();
	void	AdjustSetDamageToSkill();

	void	Imgui_RenderProperty() override;

	/* 충돌 관련 */
	void CollisionToMonster(_double TimeDelta);
	
	virtual	void CollisionToWeapon(_double TimeDelta);
	virtual	void CollisionToWeaponSkill02(_double TimeDelta);
	virtual void CollisionToWeaponSkill04(_double TimeDelta);

	void    ClosestPtPointAABB(_float3 sphereCenter, CCollider* pAABB, _float3& p);

	CCollider* Get_AABB() { return m_pColliderCom[COLLTYPE_AABB]; }

	_bool		Get_DeadAnim() { return m_bDeadAnim; }
	_bool		Get_UIOn() { return m_bUIOn; }

protected:
	virtual	HRESULT SetUp_Components() = 0;
	virtual HRESULT SetUp_ShaderResources() = 0;

protected:
	/*피격을 위한 변수*/
	_bool					m_bFrontDamaged	= false;
	_bool					m_bBackDamaged	= false;
	_bool					m_bGroggy		= false;
	_bool					m_bHitDown		= false;
	_bool					m_bDeadAnim		= false;

	// 데미지를 입을 수 있는 상황인가?
	_bool					m_bImpossibleSkillDamaged		= false; 
	_bool					m_bImpossibleDamaged			= false;
	/****************/

	_bool					m_bPlayerAttackCommand = false;
	_bool					m_bPlayerSkill02Command = false;
	_bool					m_bPlayerSkill04Command = false;

protected:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CCollider*				m_pColliderCom[COLLTYPE_END] = { nullptr };
	CNavigation*			m_pNavigationCom = nullptr;
	CFSMComponent*			m_pFSM = nullptr;

	/* 플레이어를 찾기위한 멤버 변수 */
protected:
	CPlayer*				m_pPlayer = nullptr;
	_float					m_fPlayerAttack = 0;
	/*****************************/

	/* 체력 및 공격력 세팅! */
	_float					m_fHp = 0;
	_float					m_fMaxHp = 0.f;
	_float					m_fAttack = 0;
	_float					m_fDefence = 0;

	_double					m_dDeadTime = 0;

	vector<CCollider*>		m_MonsterColliders;
	
	vector<CGameObject*>	m_MonsterUI;

	vector<CGameObject*>	m_MonsterDamageFontUI;

	_float2					m_vMonsterNamePos;
	_float2					m_vMonsterNameScale;

	_bool					m_bUIOn = false;

	vector<CMonster*>		m_Monsters;

public:
	virtual CGameObject* Clone(void* pArg = nullptr) = 0;
	virtual void Free() override;
};

END