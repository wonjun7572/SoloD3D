#include "stdafx.h"
#include "..\Public\ProgressBarUI.h"
#include "GameInstance.h"

CProgressBarUI::CProgressBarUI(ID3D11Device * pDeviec, ID3D11DeviceContext * pContext)
	:CUI(pDeviec, pContext)
{
}

CProgressBarUI::CProgressBarUI(const CProgressBarUI & rhs)
	:CUI(rhs)
{
}

HRESULT CProgressBarUI::Init_Prototype()
{
	if (FAILED(__super::Init_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CProgressBarUI::Init(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 5.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(CGameObject::Init(&GameObjectDesc)))
		return E_FAIL;

	ZeroMemory(&m_ProgressBarDesc, sizeof(m_ProgressBarDesc));

	if (pArg != nullptr)
		memcpy(&m_ProgressBarDesc, pArg, sizeof(m_ProgressBarDesc));
	else
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_fSizeX = m_ProgressBarDesc.fSizeX;
	m_fSizeY = m_ProgressBarDesc.fSizeY;
	m_fX = m_ProgressBarDesc.fX; // -300
	m_fY = m_ProgressBarDesc.fY;
	m_pTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION,
		XMVectorSet(m_fX, m_fY, 0.f, 1.f));
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));
	return S_OK;
}

void CProgressBarUI::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
}

void CProgressBarUI::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CProgressBarUI::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(m_ProgressBarDesc.iPassIndex);

	m_pVIBufferCom->Render();

	return S_OK;
}

void CProgressBarUI::Imgui_RenderProperty()
{
	ImGui::DragFloat("Amount", &m_ProgressBarDesc.fAmount, 0.1f, -10.f, 10.f);

	ImGui::DragFloat("X", &m_fX, 0.1f, -1000.f, 1000.f);
	ImGui::DragFloat("Y", &m_fY, 0.1f, -1000.f, 1000.f);
	ImGui::DragFloat("SizeX", &m_fSizeX, 0.1f, -1000.f, 1000.f);
	ImGui::DragFloat("SizeY", &m_fSizeY, 0.1f, -1000.f, 1000.f);

	m_pTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION,
		XMVectorSet(m_fX, m_fY, 0.f, 1.f));
}

void CProgressBarUI::Set_Amount(_float fAmount)
{
	m_ProgressBarDesc.fAmount = fAmount;
}

HRESULT CProgressBarUI::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_ProgressBar"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"),
		(CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Texture_ProgressBar"), TEXT("Com_Texture"),
		(CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CProgressBarUI::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_AlbedoTexture", m_ProgressBarDesc.iAlbedoTexNum)))
		return E_FAIL;
	
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_FillTexture", m_ProgressBarDesc.iFillTexNum)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_BackTexture", m_ProgressBarDesc.iBackTexNum)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_fAmount", &m_ProgressBarDesc.fAmount, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

CProgressBarUI * CProgressBarUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CProgressBarUI*		pInstance = new CProgressBarUI(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Created : CProgressBarUI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CProgressBarUI::Clone(void * pArg)
{
	CProgressBarUI*		pInstance = new CProgressBarUI(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Cloned : CProgressBarUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CProgressBarUI::Free()
{
	__super::Free();
}
