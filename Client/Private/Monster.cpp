#include "stdafx.h"
#include "..\public\Monster.h"
#include "GameInstance.h"
#include "FSMComponent.h"

CMonster::CMonster(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CMonster::CMonster(const CMonster & rhs)
	: CGameObject(rhs)
{

}

HRESULT CMonster::Init_Prototype()
{
	if (FAILED(__super::Init_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster::Init(void * pArg)
{
	CGameObject::GAMEOBJECTDESC			GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof GameObjectDesc);

	if (pArg != nullptr)
		memcpy(&GameObjectDesc, pArg, sizeof(CGameObject::GAMEOBJECTDESC));
	else
	{
		GameObjectDesc.TransformDesc.fSpeedPerSec = 7.0f;
		GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	}

	if (FAILED(__super::Init(&GameObjectDesc)))
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if(g_LEVEL == LEVEL_CHAP1)
		m_pPlayer = static_cast<CPlayer*>(pGameInstance->Find_GameObject(LEVEL_CHAP1, L"Layer_Player", L"Player"));
	else if (g_LEVEL == LEVEL_CHAP2)
		m_pPlayer = static_cast<CPlayer*>(pGameInstance->Find_GameObject(LEVEL_CHAP2, L"Layer_Player", L"Player"));
	else if (g_LEVEL == LEVEL_CHAP3)
		m_pPlayer = static_cast<CPlayer*>(pGameInstance->Find_GameObject(LEVEL_CHAP3, L"Layer_Player", L"Player"));

	Safe_AddRef(m_pPlayer);

	RELEASE_INSTANCE(CGameInstance);

	SetUp_FSM();

	return S_OK;
}

void CMonster::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	if (m_pFSM)
		m_pFSM->Tick(TimeDelta);

	m_pModelCom->Play_Animation(TimeDelta);

	if (m_pNavigationCom)
	{
		_float yPos = 0.f;
		m_pNavigationCom->isHeighit_OnNavigation(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION), &yPos);
		m_pTransformCom->Set_Height(yPos);
	}
}

void CMonster::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	for (_uint i = 0; i < COLLTYPE_END; ++i)
	{
		if(m_pColliderCom)
			m_pColliderCom[i]->Update(m_pTransformCom->Get_WorldMatrix());
	}

	if (m_iHp <= 0)
		m_bDeadAnim = true;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (nullptr != m_pRendererCom && 
		 true == pGameInstance->isInFrustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION), 2.f))
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CMonster::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

#ifdef _DEBUG
	for (_uint i = 0; i < COLLTYPE_END; ++i)
	{
		if (nullptr != m_pColliderCom[i])
			m_pColliderCom[i]->Render();
	}
#endif

	return S_OK;
}

void CMonster::Set_PlayerAttackCommand(_bool bAttack, _int iAttack)
{
	m_bPlayerAttackCommand = bAttack; 
	m_iPlayerAttack = iAttack;
}

void CMonster::Set_PlayerSkill02Command(_bool bAttack, _int iAttack)
{
	m_bPlayerSkill02Command = bAttack;
	m_iPlayerAttack = iAttack;
}

void CMonster::Set_PlayerSkiil04Command(_bool bAttack, _int iAttack)
{
	m_bPlayerSkill04Command = bAttack;
	m_iPlayerAttack = iAttack;
}

void CMonster::AdjustSetDamage()
{
	if (!m_bPossibleDamaged)
	{
		_int iRealDamage = m_iPlayerAttack - m_iDefence;

		if (iRealDamage < 0)
			iRealDamage = 0;

		m_iHp -= iRealDamage;

		if (m_iHp < 0)
			m_iHp = 0;
		// 0보다 아래면 죽음

		m_bPossibleDamaged = true;
	}
}

void CMonster::AdjustSetDamageToSkill()
{
	if (!m_bPossibleSkillDamaged)
	{
		_int iRealDamage = m_iPlayerAttack - m_iDefence;

		if (iRealDamage < 0)
			iRealDamage = 0;

		m_iHp -= iRealDamage;

		if (m_iHp < 0)
			m_iHp = 0;
		// 0보다 아래면 죽음

		m_bPossibleSkillDamaged = true;
	}
}

void CMonster::CollisionToMonster(_double TimeDelta)
{
	CGameInstance*			pGameInstance = GET_INSTANCE(CGameInstance);

	if (g_LEVEL == LEVEL_CHAP1)
	{
		const list<CGameObject*>* monList = pGameInstance->Find_LayerList(LEVEL_CHAP1, TEXT("Layer_Monster"));

		if (monList != nullptr)
		{
			for (auto& pMonster : *monList)
			{
				CCollider*	pTargetCollider = static_cast<CMonster*>(pMonster)->Get_AABB();

				_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
				_float fDistance = CMathUtils::Distance(vPos, pTargetCollider->Get_CollisionCenter());
				if (pTargetCollider == nullptr ||
					pTargetCollider == this->m_pColliderCom[COLLTYPE_AABB] ||
					fabsf(fDistance) > 5.f ||
					static_cast<CMonster*>(pMonster)->Get_DeadAnim())
					continue;

				Safe_AddRef(pTargetCollider);
				m_MonsterColliders.push_back(pTargetCollider);
			}
		}
	}
	else if (g_LEVEL == LEVEL_CHAP2)
	{
		const list<CGameObject*>* monList = pGameInstance->Find_LayerList(LEVEL_CHAP2, TEXT("Layer_Monster"));

		if (monList != nullptr)
		{
			for (auto& pMonster : *monList)
			{
				CCollider*	pTargetCollider = static_cast<CMonster*>(pMonster)->Get_AABB();

				_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
				_float fDistance = CMathUtils::Distance(vPos, pTargetCollider->Get_CollisionCenter());
				if (pTargetCollider == nullptr ||
					pTargetCollider == this->m_pColliderCom[COLLTYPE_AABB] ||
					fabsf(fDistance) > 5.f ||
					static_cast<CMonster*>(pMonster)->Get_DeadAnim())
					continue;

				Safe_AddRef(pTargetCollider);
				m_MonsterColliders.push_back(pTargetCollider);
			}
		}
	}
	else if (g_LEVEL == LEVEL_CHAP3)
	{
		const list<CGameObject*>* monList = pGameInstance->Find_LayerList(LEVEL_CHAP3, TEXT("Layer_Monster"));

		if (monList != nullptr)
		{
			for (auto& pMonster : *monList)
			{
				CCollider*	pTargetCollider = static_cast<CMonster*>(pMonster)->Get_AABB();

				_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
				_float fDistance = CMathUtils::Distance(vPos, pTargetCollider->Get_CollisionCenter());
				if (pTargetCollider == nullptr ||
					pTargetCollider == this->m_pColliderCom[COLLTYPE_AABB] ||
					fabsf(fDistance) > 5.f ||
					static_cast<CMonster*>(pMonster)->Get_DeadAnim())
					continue;

				Safe_AddRef(pTargetCollider);
				m_MonsterColliders.push_back(pTargetCollider);
			}
		}
	}

	// 이 콜라이더는 sphere여야만함
	_float3 sphereCenter = m_pColliderCom[COLLTYPE_SPHERE]->Get_CollisionCenter();
	_float sphereRadius = m_pColliderCom[COLLTYPE_SPHERE]->Get_SphereRadius();

	_uint iMonsterColliderSize = static_cast<_uint>(m_MonsterColliders.size());

	for (_uint i = 0; i < iMonsterColliderSize; ++i)
	{
		// sphere -> AttackColCom
		// AABB -> m_MonsterColliders
		_float3	p;
		ClosestPtPointAABB(sphereCenter, m_MonsterColliders[i], p);

		_vector v = p - sphereCenter;

		_float fDistance_Squared = XMVectorGetX(XMVector3Dot(v, v));

		if (fDistance_Squared <= sphereRadius * sphereRadius)
		{
			if (false == XMVector3NearEqual(v, _float4::Zero, XMVectorSet(0.001f, 0.001f, 0.001f, 0.001f)))
			{
				v = XMVector3Normalize(v);
			}
			_vector		vPosition = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
			_float4		vOldPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

			vPosition -= v * (sphereRadius - XMVectorGetX(XMVector3Length(p - sphereCenter)));

			if (true == m_pNavigationCom->isMove_OnNavigation(vPosition, &vOldPos))
			{
				m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vPosition);
			}
		}
	}

	for (auto pCollider : m_MonsterColliders)
		Safe_Release(pCollider);

	m_MonsterColliders.clear();

	RELEASE_INSTANCE(CGameInstance);
}

void CMonster::CollisionToWeapon(_double TimeDelta)
{
	CCollider* pTargetCollider = m_pPlayer->Get_WeaponCollider();

	if (nullptr == pTargetCollider)
		return;

	if (m_pColliderCom[COLLTYPE_AABB]->Collision(pTargetCollider) == true)
	{
		_vector			vTargetLook = m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_LOOK);
		_vector			vDot = XMVector3Dot(m_pTransformCom->Get_State(CTransform::STATE_LOOK), vTargetLook);
		_float			fDot = XMVectorGetX(vDot);

		if (fDot < 0)
			m_bFrontDamaged = true;
		else
			m_bBackDamaged = true;
	}
}

void CMonster::CollisionToWeaponSkill02(_double TimeDelta)
{
	CCollider* pTargetCollider = m_pPlayer->Get_WeaponCollider();

	if (nullptr == pTargetCollider)
		return;

	if (m_pColliderCom[COLLTYPE_AABB]->Collision(pTargetCollider) == true)
	{
		AdjustSetDamageToSkill();
		m_bHitDown = true;
	}
}

void CMonster::CollisionToWeaponSkill04(_double TimeDelta)
{
	CCollider* pTargetCollider = m_pPlayer->Get_WeaponCollider();

	if (nullptr == pTargetCollider)
		return;

	if (m_pColliderCom[COLLTYPE_AABB]->Collision(pTargetCollider) == true)
	{
		AdjustSetDamageToSkill();
		m_bGroggy = true;
	}
}

void CMonster::ClosestPtPointAABB(_float3 sphereCenter, CCollider* pAABB, _float3 & p)
{
	_float x = sphereCenter.x;
	_float y = sphereCenter.y;
	_float z = sphereCenter.z;
	
	_float3 vMin, vMax;
	vMin = pAABB->Compute_Min();
	vMax = pAABB->Compute_Max();

	x =	max(x, vMin.x);
	x = min(x, vMax.x);
	p.x = x;

	y = max(y, vMin.y);
	y = min(y, vMax.y);
	p.y = y;

	z = max(z, vMin.z);
	z = min(z, vMax.z);
	p.z = z;
}

void CMonster::Free()
{
	__super::Free();

	for (_uint i = 0; i < COLLTYPE_END; ++i)
		Safe_Release(m_pColliderCom[i]);

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pFSM);
	Safe_Release(m_pPlayer);
}
