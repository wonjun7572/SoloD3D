#include "stdafx.h"
#include "..\Public\Belt.h"
#include "GameInstance.h"
#include "Bone.h"

CBelt::CBelt(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CParts(pDevice, pContext)
{
}

CBelt::CBelt(const CBelt & rhs)
	: CParts(rhs)
{
}

HRESULT CBelt::Init_Prototype()
{
	if (FAILED(__super::Init_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBelt::Init(void * pArg)
{
	if (FAILED(__super::Init(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	return S_OK;
}

void CBelt::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	m_pModelCom[m_eModelState]->Set_AnimationIndex(m_iAnimIndex);
	m_pModelCom[m_eModelState]->Play_Animation(TimeDelta);
}

void CBelt::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CBelt::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBelt::SetUp_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Model_Belt_B"), TEXT("Com_Model_B"),
		(CComponent**)&m_pModelCom[MODEL_NOMAL])))
		return E_FAIL;
	
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Model_Belt_A"), TEXT("Com_Model_A"),
		(CComponent**)&m_pModelCom[MODEL_A])))
		return E_FAIL;

	return S_OK;
}

CBelt * CBelt::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBelt*		pInstance = new CBelt(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Created : CBelt");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CBelt::Clone(void * pArg)
{
	CBelt*		pInstance = new CBelt(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBelt");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBelt::Free()
{
	__super::Free();
}
