#include "stdafx.h"
#include "..\Public\TestCylinder.h"
#include "GameInstance.h"

CTestCylinder::CTestCylinder(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CGameObject(pDevice, pContext)
{
}

CTestCylinder::CTestCylinder(const CTestCylinder & rhs)
	:CGameObject(rhs)
{
}

HRESULT CTestCylinder::Init_Prototype()
{
	if (FAILED(__super::Init_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTestCylinder::Init(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GAMEOBJECTDESC));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 5.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Init(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Go_Right(2);

	return S_OK;
}

void CTestCylinder::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
}

void CTestCylinder::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
}

HRESULT CTestCylinder::Render()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CTestCylinder::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxNorTex"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_VIBuffer_Cylinder"), TEXT("Com_VIBuffer"),
		(CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Texture_Terrain"), TEXT("Com_Texture"),
		(CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTestCylinder::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Matrix("g_WorldMatrix", &m_pTransformCom->Get_World4x4())))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof _float4)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))
		return E_FAIL;


	return S_OK;
}

CTestCylinder * CTestCylinder::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTestCylinder*		pInstance = new CTestCylinder(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Created : CTestCylinder");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CTestCylinder::Clone(void * pArg)
{
	CTestCylinder*		pInstance = new CTestCylinder(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTestCylinder");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTestCylinder::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
