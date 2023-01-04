#include "stdafx.h"
#include "..\Public\Trail.h"
#include "GameInstance.h"

CTrail::CTrail(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CGameObject(pDevice, pContext)
{
}

CTrail::CTrail(const CTrail & rhs)
	:CGameObject(rhs)
{
}

HRESULT CTrail::Init_Prototype()
{
	if (FAILED(__super::Init_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTrail::Init(void * pArg)
{
	if (FAILED(__super::Init(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	return S_OK;
}

void CTrail::Tick(_double TimeDelta)
{
}

void CTrail::Late_Tick(_double TimeDelta)
{
}

HRESULT CTrail::Render()
{
	return S_OK;
}

HRESULT CTrail::Trail1_Render(_float fMove, _float4x4 mat)
{
	if (FAILED(Trail1_SetUp_ShaderResources(fMove, mat)))
		return E_FAIL;

	_uint iNumMeshes = m_pTrail1Com->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		/* 이 모델을 그리기위한 셰이더에 머테리얼 텍스쳐를 전달하낟. */
		m_pTrail1Com->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_Texture");

		m_pTrail1Com->Render(m_pShaderCom, i);
	}

	return S_OK;
}

HRESULT CTrail::Trail2_Render(_float fMove, _float4x4 mat)
{
	if (FAILED(Trail2_SetUp_ShaderResources(fMove, mat)))
		return E_FAIL;

	_uint iNumMeshes = m_pTrail2Com->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		/* 이 모델을 그리기위한 셰이더에 머테리얼 텍스쳐를 전달하낟. */
		m_pTrail2Com->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_Texture");

		m_pTrail2Com->Render(m_pShaderCom, i);
	}

	return S_OK;
}

HRESULT CTrail::Trail3_Render(_float fMove, _float4x4 mat)
{
	if (FAILED(Trail3_SetUp_ShaderResources(fMove, mat)))
		return E_FAIL;

	_uint iNumMeshes = m_pTrail3Com->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		/* 이 모델을 그리기위한 셰이더에 머테리얼 텍스쳐를 전달하낟. */
		m_pTrail3Com->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_Texture");

		m_pTrail3Com->Render(m_pShaderCom, i);
	}

	return S_OK;
}

HRESULT CTrail::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_Trail"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("NormalAtk1_Trail"), TEXT("Com_Model"),
		(CComponent**)&m_pTrail1Com)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("NormalAtk2_Trail"), TEXT("Com_Model_2"),
		(CComponent**)&m_pTrail2Com)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("NormalAtk3_Trail"), TEXT("Com_Model_3"),
		(CComponent**)&m_pTrail3Com)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTrail::Trail1_SetUp_ShaderResources(_float fMove, _float4x4 mat)
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if(FAILED(m_pShaderCom->Set_RawValue("g_fMove", &fMove, sizeof(_float))))
		return E_FAIL;

	//if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
	//	return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Matrix("g_WorldMatrix", &mat)))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CTrail::Trail2_SetUp_ShaderResources(_float fMove, _float4x4 mat)
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_fMove", &fMove, sizeof(_float))))
		return E_FAIL;

	//if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
	//	return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Matrix("g_WorldMatrix", &mat)))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CTrail::Trail3_SetUp_ShaderResources(_float fMove, _float4x4 mat)
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_fMove", &fMove, sizeof(_float))))
		return E_FAIL;

	//if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
	//	return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Matrix("g_WorldMatrix", &mat)))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CTrail * CTrail::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTrail*		pInstance = new CTrail(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Created : CTrail");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CTrail::Clone(void * pArg)
{
	CTrail*		pInstance = new CTrail(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTrail");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTrail::Free()
{
	__super::Free();

	Safe_Release(m_pTrail1Com);
	Safe_Release(m_pTrail2Com);
	Safe_Release(m_pTrail3Com);

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
