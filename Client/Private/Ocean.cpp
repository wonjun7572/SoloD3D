#include "stdafx.h"
#include "..\Public\Ocean.h"
#include "GameInstance.h"

COcean::COcean(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CGameObject(pDevice,pContext)
{
}

COcean::COcean(const COcean & rhs)
	:CGameObject(rhs)
{
}

HRESULT COcean::Init_Prototype()
{
	if (FAILED(__super::Init_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT COcean::Init(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 5.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Init(&GameObjectDesc)))
		return E_FAIL;
	
	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Go_Left(30);
	m_pTransformCom->Go_Down(2);

	
	return S_OK;
}

void COcean::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	m_fTimeDelta += static_cast<float>(TimeDelta * 0.03f);
}

void COcean::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_PRIORITY, this);
}

HRESULT COcean::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(1);

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT COcean::SetUp_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_VIBuffer_Ocean"), TEXT("Com_VIBuffer"),
		(CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Texture_Water"), TEXT("Com_Texture"),
		(CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT COcean::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	float			gWaveHeight = 5.f;
	float			gSpeed = 2.f;
	float			gWaveFrequency = 15.f;
	float			gUVSpeed = 0.15f;

	if (FAILED(m_pShaderCom->Set_RawValue("gTime", &m_fTimeDelta, sizeof _float)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("gWaveHeight", &gWaveHeight, sizeof _float)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("gSpeed", &gSpeed, sizeof _float)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("gWaveFrequency", &gWaveFrequency, sizeof _float)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("gUVSpeed", &gUVSpeed, sizeof _float)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Matrix("g_WorldMatrix", &m_pTransformCom->Get_World4x4())))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof _float4)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

COcean * COcean::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	COcean*		pInstance = new COcean(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Created : COcean");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * COcean::Clone(void * pArg)
{
	COcean*		pInstance = new COcean(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Cloned : COcean");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void COcean::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
