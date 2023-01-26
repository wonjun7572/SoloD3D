#include "stdafx.h"
#include "..\Public\Effect.h"
#include "GameInstance.h"

CEffect::CEffect(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CGameObject(pDevice,pContext)
{
}

CEffect::CEffect(const CEffect & rhs)
	:CGameObject(rhs)
{
}

HRESULT CEffect::Init_Prototype()
{
	if (FAILED(__super::Init_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect::Init(void * pArg)
{
	if (FAILED(__super::Init(pArg)))
		return E_FAIL;

	return S_OK;
}

void CEffect::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
}

void CEffect::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CEffect::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CEffect::LinkObject(_double TimeDelta, _float4 targetpos)
{
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, targetpos);
}

void CEffect::Free()
{
	__super::Free();
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
