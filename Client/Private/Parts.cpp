#include "stdafx.h"
#include "..\Public\Parts.h"
#include "GameInstance.h"
#include "Bone.h"

CParts::CParts(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CGameObject(pDevice, pContext)
{
}

CParts::CParts(const CParts & rhs)
	:CGameObject(rhs)
{
}

HRESULT CParts::Init_Prototype()
{
	if (FAILED(__super::Init_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CParts::Init(void * pArg)
{
	if (FAILED(__super::Init(pArg)))
		return E_FAIL;

	return S_OK;
}

void CParts::Tick(_double TimeDelta)
{
}

void CParts::Late_Tick(_double TimeDelta)
{
}

HRESULT CParts::Render()
{
	return S_OK;
}

void CParts::Play_AdditiveAnim(_double TimeDelta, float fRatio)
{
	m_pModelCom->Set_AdditiveAnimIndex(m_iAdditiveAnimIndex);
	m_pModelCom->Play_AddtivieAnim(TimeDelta, fRatio);
}

void CParts::LinkPlayer(CTransform * pTarget)
{
	m_pTransformCom->SetWorldMatrix(pTarget->Get_WorldMatrix());
}

void CParts::Imgui_RenderProperty()
{

}

void CParts::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
