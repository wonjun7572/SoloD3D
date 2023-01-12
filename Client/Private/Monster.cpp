#include "stdafx.h"
#include "..\public\Monster.h"
#include "GameInstance.h"
#include "FSMComponent.h"
#include "ProgressBarUI.h"
#include "PlayerCamera.h"
#include "Effect_Rect.h"

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

	if (FAILED(SetUP_UI()))
		return E_FAIL;

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

	for (auto& pUI : m_MonsterUI)
		pUI->Tick(TimeDelta);

	UI_Tick(TimeDelta);
}

void CMonster::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	for (_uint i = 0; i < COLLTYPE_END; ++i)
	{
		if(m_pColliderCom)
			m_pColliderCom[i]->Update(m_pTransformCom->Get_WorldMatrix());
	}

	if (m_fHp <= 0)
		m_bDeadAnim = true;

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

	// 내 주변에 있는 몬스터와 거리 비교 이후에 그녀석의 UI가 켜져있는가? 
	// 한명이라도 켜져있다면 내껀 꺼버리고 아니면 키고
	// 아닌데.. 흠 

	UI_Switch(TimeDelta);

	if (m_bUIOn && !m_bDeadAnim)
	{
		for (auto& pUI : m_MonsterUI)
			pUI->Late_Tick(TimeDelta);
	}

	m_Monsters.clear();

	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CMonster::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (m_bUIOn)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		
		pGameInstance->Render_Font(TEXT("Font_Comic"), m_strObjName.c_str(), m_vMonsterNamePos, 0.f, m_vMonsterNameScale, XMVectorSet(1.f, 1.f, 0.f, 1.f));
		
		pGameInstance->Render_Font(TEXT("Font_Comic"), m_strDamage.c_str(), _float2(m_vMonsterNamePos.x - 100.f, m_vMonsterNamePos.y + 200.f), 0.f, m_vMonsterNameScale, XMVectorSet(1.f, 0.f, 0.f, 1.f));

		RELEASE_INSTANCE(CGameInstance);
	}
	
	return S_OK;
}

HRESULT CMonster::SetUP_UI()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CGameObject* pUI = nullptr;

	CProgressBarUI::PROGRESSBARDESC progressBarDesc;
	ZeroMemory(&progressBarDesc, sizeof(CProgressBarUI::PROGRESSBARDESC));

	progressBarDesc.fAmount = 1.f;
	progressBarDesc.fSizeX = 400.f;
	progressBarDesc.fSizeY = 40.f;
	progressBarDesc.fX = 0.f;
	progressBarDesc.fY = 350.f;
	progressBarDesc.iAlbedoTexNum = 3;
	progressBarDesc.iFillTexNum = 3;
	progressBarDesc.iBackTexNum = 4;
	progressBarDesc.iPassIndex = 1;

	pUI = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_ProgressBarUI"), &progressBarDesc);

	if (nullptr == pUI)
		return E_FAIL;

	m_MonsterUI.push_back(pUI);

	pUI = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_AimUI"), &progressBarDesc);

	if (nullptr == pUI)
		return E_FAIL;

	m_MonsterUI.push_back(pUI);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CMonster::UI_Tick(_double TimeDelta)
{
	static_cast<CProgressBarUI*>(m_MonsterUI[HP])->Set_Amount((1.f + m_fHp / m_fMaxHp) - 1.f);
	static_cast<CEffect_Rect*>(m_MonsterUI[TARGET_AIM])->LinkObject(TimeDelta,m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
}

void CMonster::UI_Switch(_double TimeDelta)
{
	if (!m_bDeadAnim)
	{
		m_bUIOn = false;
		_float fDis = 0.f;

		if (m_pPlayer->Get_PlayerCam() != nullptr)
		{
			if (m_pColliderCom[COLLTYPE_AABB]->Collision(m_pPlayer->Get_PlayerCam()->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION),
				m_pPlayer->Get_PlayerCam()->Get_TransformCom()->Get_State(CTransform::STATE_LOOK), fDis))
			{
				if (fDis < 20.f)
				{
					_bool bUI = false;
					m_pPlayer->CamLockOn(this, bUI);

					if (bUI)
						m_bUIOn = true;
					
					for (auto pMonster : m_Monsters)
					{
						if (pMonster->Get_UIOn() == true)
						{
							m_bUIOn = false;
							return;
						}
					}
				}
				else
					m_bUIOn = false;
			}
			else
				m_bUIOn = false;
		}
	}
}

void CMonster::Set_PlayerAttackCommand(_bool bAttack, _float fAttack)
{
	m_bPlayerAttackCommand = bAttack; 
	m_fPlayerAttack = fAttack;
}

void CMonster::Set_PlayerSkill02Command(_bool bAttack, _float fAttack)
{
	m_bPlayerSkill02Command = bAttack;
	m_fPlayerAttack = fAttack;
}

void CMonster::Set_PlayerSkiil04Command(_bool bAttack, _float fAttack)
{
	m_bPlayerSkill04Command = bAttack;
	m_fPlayerAttack = fAttack;
}

void CMonster::AdjustSetDamage()
{
	if (!m_bImpossibleDamaged)
	{
		_float fRealDamage = m_fPlayerAttack - m_fDefence;

		if (fRealDamage < 0.f)
			fRealDamage = 0.f;

		m_strDamage = to_wstring((int)fRealDamage);
		
		m_fHp -= fRealDamage;

		if (m_fHp < 0.f)
			m_fHp = 0.f;
		// 0보다 아래면 죽음

		m_bImpossibleDamaged = true;
	}
}

void CMonster::AdjustSetDamageToSkill()
{
	if (!m_bImpossibleSkillDamaged)
	{
		_float fRealDamage = m_fPlayerAttack - m_fDefence;

		if (fRealDamage < 0.f)
			fRealDamage = 0.f;

		m_strDamage = to_wstring((int)fRealDamage);
		
		m_fHp -= fRealDamage;

		if (m_fHp < 0.f)
			m_fHp = 0.f;
		// 0보다 아래면 죽음

		m_bImpossibleSkillDamaged = true;
	}
}

void CMonster::Imgui_RenderProperty()
{
	ImGui::DragFloat("NamePosX", &m_vMonsterNamePos.x, 0.1f, -1000.f, 1000.f);
	ImGui::DragFloat("NamePosY", &m_vMonsterNamePos.y, 0.1f, -1000.f, 1000.f);
	ImGui::DragFloat("NameScaleX", &m_vMonsterNameScale.x, 0.1f, -1000.f, 1000.f);
	ImGui::DragFloat("NameScaleY", &m_vMonsterNameScale.y, 0.1f, -1000.f, 1000.f);

	m_MonsterUI[TARGET_AIM]->Imgui_RenderProperty();
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
			
				if (fabsf(fDistance) < 10.f &&	pTargetCollider != this->m_pColliderCom[COLLTYPE_AABB])
				{
					m_Monsters.push_back(static_cast<CMonster*>(pMonster));
				}

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

				if (fabsf(fDistance) < 10.f &&	pTargetCollider != this->m_pColliderCom[COLLTYPE_AABB])
				{
					m_Monsters.push_back(static_cast<CMonster*>(pMonster));
				}

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

				if(fabsf(fDistance) < 10.f &&	pTargetCollider != this->m_pColliderCom[COLLTYPE_AABB])
				{
					m_Monsters.push_back(static_cast<CMonster*>(pMonster));
				}

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

	for (auto& pUI : m_MonsterUI)
		Safe_Release(pUI);

	m_MonsterUI.clear();
	m_Monsters.clear();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pFSM);
	Safe_Release(m_pPlayer);
}
