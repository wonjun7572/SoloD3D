#include "stdafx.h"
#include "..\Public\FireE_0.h"
#include "GameInstance.h"

CFireE_0::CFireE_0(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CEffect_Rect(pDevice, pContext)
{
}

CFireE_0::CFireE_0(const CEffect_Rect & rhs)
	: CEffect_Rect(rhs)
{
}

HRESULT CFireE_0::Init_Prototype()
{
	if (FAILED(__super::Init_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CFireE_0::Init(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 5.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(CGameObject::Init(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(rand() % 10, 0.f, rand() % 10, 1.f));
	m_strObjName = L"Effect_Rect_FireE_0";

	m_fAlpha = 1.f;
	m_UVMoveFactor = _float2(0.f, 0.f);

	return S_OK;
}

void CFireE_0::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	Compute_BillBoard();

	m_fFrame += 32.0f * TimeDelta;

	if (m_fFrame >= 32.0f)
		m_fFrame = 0.f;
}

void CFireE_0::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	__super::Compute_CamDistance();

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CFireE_0::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CFireE_0::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_TextureEffect"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"),
		(CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Texture_Fire"), TEXT("Com_Texture"),
		(CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CFireE_0::SetUp_ShaderResources()
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

	if (FAILED(m_pShaderCom->Set_ShaderResourceView("g_DepthTexture", pGameInstance->Get_DepthTargetSRV())))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_AlbedoTexture", (_uint)m_fFrame)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_UVMoveFactor", &m_UVMoveFactor, sizeof(_float2))))
		return E_FAIL;

	return S_OK;
}

CFireE_0 * CFireE_0::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CFireE_0*		pInstance = new CFireE_0(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Created : CFireE_0");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CFireE_0::Clone(void * pArg)
{
	CFireE_0*		pInstance = new CFireE_0(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Cloned : CFireE_0");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CFireE_0::Free()
{
	__super::Free();
}
