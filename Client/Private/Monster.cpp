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
		m_bDead = true;

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
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
