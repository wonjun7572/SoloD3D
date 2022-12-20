#include "stdafx.h"
#include "..\public\Monster.h"
#include "GameInstance.h"

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

	return S_OK;
}

void CMonster::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	for (_uint i = 0; i < COLLTYPE_END; ++i)
		m_pColliderCom[i]->Update(m_pTransformCom->Get_WorldMatrix());
}

void CMonster::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

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

void CMonster::Free()
{
	__super::Free();

	for (_uint i = 0; i < COLLTYPE_END; ++i)
		Safe_Release(m_pColliderCom[i]);

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
