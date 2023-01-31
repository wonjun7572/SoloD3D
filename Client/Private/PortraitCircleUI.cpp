#include "stdafx.h"
#include "..\Public\PortraitCircleUI.h"
#include "GameInstance.h"

CPortraitCircleUI::CPortraitCircleUI(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CUI(pDevice,pContext)
{
}

CPortraitCircleUI::CPortraitCircleUI(const CPortraitCircleUI & rhs)
	:CUI(rhs)
{
}

HRESULT CPortraitCircleUI::Init_Prototype()
{
	if (FAILED(__super::Init_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPortraitCircleUI::Init(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 5.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(CGameObject::Init(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_fSizeX = 128.f;
	m_fSizeY = 128.f;
	m_fX = 0.f;
	m_fY = -325.f;

	m_pTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION,
		XMVectorSet(m_fX, m_fY, 0.f, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));

	return S_OK;
}

void CPortraitCircleUI::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	m_fFrame += 15.f * static_cast<_float>(TimeDelta);

	if (m_fFrame >= 27.0f)
		m_fFrame = 1.f;
}

void CPortraitCircleUI::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CPortraitCircleUI::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;
	m_pShaderCom->Begin(3);
	m_pVIBufferCom->Render();

	if (FAILED(SetUp_ShaderResourcesIcon()))
		return E_FAIL;
	m_pShaderCom->Begin(3);
	m_pVIBufferCom->Render();

	if (FAILED(SetUp_ShaderResourcesAni()))
		return E_FAIL;
	m_pShaderCom->Begin(3);
	m_pVIBufferCom->Render();

	return S_OK;
}

void CPortraitCircleUI::Imgui_RenderProperty()
{
	ImGui::DragFloat("X", &m_fX, 0.1f, -1000.f, 1000.f);
	ImGui::DragFloat("Y", &m_fY, 0.1f, -1000.f, 1000.f);
	ImGui::DragFloat("SizeX", &m_fSizeX, 0.1f, -1000.f, 1000.f);
	ImGui::DragFloat("SizeY", &m_fSizeY, 0.1f, -1000.f, 1000.f);

	m_pTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION,
		XMVectorSet(m_fX, m_fY, 0.f, 1.f));
}

HRESULT CPortraitCircleUI::SetUp_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Texture_PortraitCircle"), TEXT("Com_Texture"),
		(CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_PortraitCom */
	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Texture_Portrait"), TEXT("Com_PortraitCom"),
		(CComponent**)&m_pPortraitCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPortraitCircleUI::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_AlbedoTexture")))
		return E_FAIL;

	m_fAlpha = 1.f;

	if (FAILED(m_pShaderCom->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

HRESULT CPortraitCircleUI::SetUp_ShaderResourcesIcon()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pPortraitCom->Bind_ShaderResource(m_pShaderCom, "g_AlbedoTexture")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

HRESULT CPortraitCircleUI::SetUp_ShaderResourcesAni()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_AlbedoTexture", (_int)m_fFrame)))
		return E_FAIL;

	m_fAlpha *= 2.f;

	if (FAILED(m_pShaderCom->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

CPortraitCircleUI * CPortraitCircleUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPortraitCircleUI*		pInstance = new CPortraitCircleUI(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Created : CPortraitCircleUI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPortraitCircleUI::Clone(void * pArg)
{
	CPortraitCircleUI*		pInstance = new CPortraitCircleUI(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPortraitCircleUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPortraitCircleUI::Free()
{
	__super::Free();
	Safe_Release(m_pPortraitCom);
}
