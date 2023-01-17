#include "stdafx.h"
#include "..\Public\Ally.h"
#include "GameInstance.h"
#include "FSMComponent.h"
#include "Player.h"
#include "Monster.h"

CAlly::CAlly(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CAlly::CAlly(const CAlly & rhs)
	: CGameObject(rhs)
{
}

HRESULT CAlly::Init_Prototype()
{
	if (FAILED(__super::Init_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CAlly::Init(void * pArg)
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

	if (g_LEVEL == LEVEL_CHAP1)
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

void CAlly::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	if (m_pNavigationCom)
	{
		_float yPos = 0.f;
		m_pNavigationCom->isHeighit_OnNavigation(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION), &yPos);
		m_pTransformCom->Set_Height(yPos);
	}
}

void CAlly::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	for (_uint i = 0; i < COLLTYPE_END; ++i)
	{
		if (m_pColliderCom)
			m_pColliderCom[i]->Update(m_pTransformCom->Get_WorldMatrix());
	}

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (nullptr != m_pRendererCom &&
		true == pGameInstance->isInFrustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION), 2.f))
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

#ifdef _DEBUG
		for (auto& pCollider : m_pColliderCom)
			m_pRendererCom->Add_DebugRenderGroup(pCollider);
#endif
	}

	CollisionToAlly(TimeDelta);
	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CAlly::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CAlly::CollisionToAlly(_double TimeDelta)
{
	CGameInstance*			pGameInstance = GET_INSTANCE(CGameInstance);

	if (g_LEVEL == LEVEL_CHAP1)
	{
		const list<CGameObject*>* AllyList = pGameInstance->Find_LayerList(LEVEL_CHAP1, TEXT("Layer_Ally"));

		if (AllyList != nullptr)
		{
			for (auto& pAlly : *AllyList)
			{
				CCollider*	pTargetCollider = static_cast<CAlly*>(pAlly)->Get_AABB();

				_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
				_float fDistance = CMathUtils::Distance(vPos, pTargetCollider->Get_CollisionCenter());

				if (pTargetCollider == nullptr ||
					pTargetCollider == this->m_pColliderCom[COLLTYPE_AABB] ||
					fabsf(fDistance) > 5.f)
					continue;

				Safe_AddRef(pTargetCollider);
				m_AllyColliders.push_back(pTargetCollider);
			}
		}
	}
	else if (g_LEVEL == LEVEL_CHAP2)
	{
		const list<CGameObject*>* AllyList = pGameInstance->Find_LayerList(LEVEL_CHAP2, TEXT("Layer_Ally"));

		if (AllyList != nullptr)
		{
			for (auto& pAlly : *AllyList)
			{
				CCollider*	pTargetCollider = static_cast<CAlly*>(pAlly)->Get_AABB();

				_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
				_float fDistance = CMathUtils::Distance(vPos, pTargetCollider->Get_CollisionCenter());

				if (pTargetCollider == nullptr ||
					pTargetCollider == this->m_pColliderCom[COLLTYPE_AABB] ||
					fabsf(fDistance) > 5.f)
					continue;

				Safe_AddRef(pTargetCollider);
				m_AllyColliders.push_back(pTargetCollider);
			}
		}
	}
	else if (g_LEVEL == LEVEL_CHAP3)
	{
		const list<CGameObject*>* AllyList = pGameInstance->Find_LayerList(LEVEL_CHAP3, TEXT("Layer_Ally"));

		if (AllyList != nullptr)
		{
			for (auto& pAlly : *AllyList)
			{
				CCollider*	pTargetCollider = static_cast<CAlly*>(pAlly)->Get_AABB();

				_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
				_float fDistance = CMathUtils::Distance(vPos, pTargetCollider->Get_CollisionCenter());

				if (pTargetCollider == nullptr ||
					pTargetCollider == this->m_pColliderCom[COLLTYPE_AABB] ||
					fabsf(fDistance) > 5.f)
					continue;

				Safe_AddRef(pTargetCollider);
				m_AllyColliders.push_back(pTargetCollider);
			}
		}
	}

	// 이 콜라이더는 sphere여야만함
	_float3 sphereCenter = m_pColliderCom[COLLTYPE_SPHERE]->Get_CollisionCenter();
	_float sphereRadius = m_pColliderCom[COLLTYPE_SPHERE]->Get_SphereRadius();

	_uint iAllyColliderSize = static_cast<_uint>(m_AllyColliders.size());

	for (_uint i = 0; i < iAllyColliderSize; ++i)
	{
		// AABB -> m_AllyColliders
		_float3	p;
		ClosestPtPointAABB(sphereCenter, m_AllyColliders[i], p);

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

	for (auto pCollider : m_AllyColliders)
		Safe_Release(pCollider);

	m_AllyColliders.clear();

	RELEASE_INSTANCE(CGameInstance);
}

void CAlly::ClosestPtPointAABB(_float3 sphereCenter, CCollider * pAABB, _float3 & p)
{
	_float x = sphereCenter.x;
	_float y = sphereCenter.y;
	_float z = sphereCenter.z;

	_float3 vMin, vMax;
	vMin = pAABB->Compute_Min();
	vMax = pAABB->Compute_Max();

	x = max(x, vMin.x);
	x = min(x, vMax.x);
	p.x = x;

	y = max(y, vMin.y);
	y = min(y, vMax.y);
	p.y = y;

	z = max(z, vMin.z);
	z = min(z, vMax.z);
	p.z = z;
}

void CAlly::AdjustSetDamage()
{
	if (!m_bImpossibleDamaged)
	{
		m_bImpossibleDamaged = true;
	}
}

void CAlly::AdjustSetDamageToSkill()
{
	if (!m_bImpossibleSkillDamaged)
	{
		m_bImpossibleSkillDamaged = true;
	}
}

void CAlly::Free()
{
	__super::Free();

	for (_uint i = 0; i < COLLTYPE_END; ++i)
		Safe_Release(m_pColliderCom[i]);

	for (auto& pCollider : m_AllyColliders)
		Safe_Release(pCollider);

	m_AllyColliders.clear();

	Safe_Release(m_pPlayer);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pFSM);
}
