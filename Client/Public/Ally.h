#pragma once

#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CCollider;
class CFSMComponent;
END

BEGIN(Client)

class CAlly abstract : public CGameObject
{
public:
	enum UI { UI_CONVERSATION, UI_END };
	enum COLLIDERTYPE { COLLTYPE_AABB, COLLTYPE_SPHERE, COLLTYPE_END };

	typedef struct tagInitDesc
	{
		_float4 vPos;
		_float	fRadian;
	}ALLYDESC;

protected:
	CAlly(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAlly(const CAlly& rhs);
	virtual	~CAlly() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void	Tick(_double TimeDelta) override;
	virtual void	Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	/* 충돌 관련 */
	void CollisionToAlly(_double TimeDelta);
	void ClosestPtPointAABB(_float3 sphereCenter, CCollider* pAABB, _float3& p);

	void	AdjustSetDamage();
	void	AdjustSetDamageToSkill();

	CCollider* Get_AABB() { return m_pColliderCom[COLLTYPE_AABB]; }

protected:
	virtual	void    SetUp_FSM() = 0;
	virtual	HRESULT SetUp_Components() = 0;
	virtual HRESULT SetUp_ShaderResources() = 0;

protected:
	/*피격을 위한 변수*/
	_bool					m_bFrontDamaged = false;
	_bool					m_bBackDamaged = false;
	_bool					m_bGroggy = false;
	_bool					m_bHitDown = false;
	_bool					m_bDeadAnim = false;

	_bool					m_bChase = false;
	_bool					m_bMonsterChase = false;
	_bool					m_bPlayerChase = false;

	_bool					m_bSpeak = false;

	// 데미지를 입을 수 있는 상황인가?
	_bool					m_bImpossibleSkillDamaged = false;
	_bool					m_bImpossibleDamaged = false;
	/****************/

protected:
	class	CGameObject*		m_pPlayer = nullptr;
	class	CGameObject*		m_pSkeleton = nullptr;

protected:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CCollider*				m_pColliderCom[COLLTYPE_END] = { nullptr };
	CNavigation*			m_pNavigationCom = nullptr;
	CFSMComponent*			m_pFSM = nullptr;

	_float					m_fDeadTime = 0.f;

protected:
	vector<CCollider*>		m_AllyColliders;
	
public:
	virtual CGameObject* Clone(void* pArg = nullptr) = 0;
	virtual void Free() override;
};

END