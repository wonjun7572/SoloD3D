#include "stdafx.h"
#include "..\Public\Upper.h"
#include "GameInstance.h"
#include "Bone.h"

CUpper::CUpper(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CParts(pDevice, pContext)
{
}

CUpper::CUpper(const CUpper & rhs)
	: CParts(rhs)
{
}

HRESULT CUpper::Init_Prototype()
{
	if (FAILED(__super::Init_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUpper::Init(void * pArg)
{
	if (FAILED(__super::Init(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	return S_OK;
}

void CUpper::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	m_pModelCom[m_eModelState]->Set_AnimationIndex(m_iAnimIndex);
	m_pModelCom[m_eModelState]->Play_Animation(TimeDelta);
}

void CUpper::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CUpper::Render()
{
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUpper::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxAnimModel"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Model_Upper_B"), TEXT("Com_Model_B"),
		(CComponent**)&m_pModelCom[MODEL_NOMAL])))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Model_Upper_A"), TEXT("Com_Model_A"),
		(CComponent**)&m_pModelCom[MODEL_A])))
		return E_FAIL;
	
	return S_OK;
}

CUpper * CUpper::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUpper*		pInstance = new CUpper(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Created : CUpper");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUpper::Clone(void * pArg)
{
	CUpper*		pInstance = new CUpper(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUpper");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUpper::Free()
{
	__super::Free();
}
