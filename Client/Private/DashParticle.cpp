#include "stdafx.h"
#include "..\Public\DashParticle.h"
#include "GameInstance.h"

CDashParticle::CDashParticle(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:Effect_Point(pDevice, pContext)
{
}

CDashParticle::CDashParticle(const CDashParticle & rhs)
	:Effect_Point(rhs)
{
}

HRESULT CDashParticle::Init_Prototype()
{
	if (FAILED(__super::Init_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDashParticle::Init(void * pArg)
{
	if (FAILED(__super::Init(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	CTransform::TRANSFORMDESC transformDesc;

	transformDesc.fRotationPerSec = XMConvertToRadians(270.0f);
	transformDesc.fSpeedPerSec = 1.0f;
	m_pTransformCom->Set_TransformDesc(transformDesc);


	return S_OK;
}

void CDashParticle::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	m_fFrame += 4.f * static_cast<_float>(TimeDelta);

	if (m_fFrame > 4.f)
		m_fFrame = 0.f;

	if (m_ParticleDesc.pTargetTransform != nullptr && !m_bLinking)
		m_pTransformCom->SetWorldMatrix(XMLoadFloat4x4(&m_ParticleDesc.PivotMatrix) * m_ParticleDesc.pTargetTransform->Get_WorldMatrix());

	m_pVIBufferCom->Go_Dir(m_pTransformCom->Get_State(CTransform::STATE_LOOK), TimeDelta);
	m_pTransformCom->Set_Scaled(_float3(0.2f,0.2f,0.2f));
}

void CDashParticle::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CDashParticle::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CDashParticle::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxPointInstancing"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_VIBuffer_Point_Instancing"), TEXT("Com_VIBuffer"),
		(CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Texture_Spark"), TEXT("Com_Texture"),
		(CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CDashParticle::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
		return E_FAIL;

	return S_OK;
}

CDashParticle * CDashParticle::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CDashParticle*		pInstance = new CDashParticle(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Created : CDashParticle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CDashParticle::Clone(void * pArg)
{
	CDashParticle*		pInstance = new CDashParticle(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDashParticle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDashParticle::Free()
{
	__super::Free();
}