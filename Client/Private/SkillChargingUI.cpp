#include "stdafx.h"
#include "SkillChargingUI.h"
#include "GameInstance.h"

CSkillChargingUI::CSkillChargingUI(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CUI(pDevice, pContext)
{
}

CSkillChargingUI::CSkillChargingUI(const CSkillChargingUI & rhs)
	: CUI(rhs)
{
}

HRESULT CSkillChargingUI::Init_Prototype()
{
	if (FAILED(__super::Init_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkillChargingUI::Init(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 5.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(CGameObject::Init(&GameObjectDesc)))
		return E_FAIL;

	ZeroMemory(&m_SkillIconDesc, sizeof(m_SkillIconDesc));

	if (pArg != nullptr)
		memcpy(&m_SkillIconDesc, pArg, sizeof(m_SkillIconDesc));
	else
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_fSizeX = m_SkillIconDesc.fSizeX;
	m_fSizeY = m_SkillIconDesc.fSizeY;

	m_fX = m_SkillIconDesc.fX; // -300
	m_fY = m_SkillIconDesc.fY;

	m_pTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION,
		XMVectorSet(m_fX, m_fY, 0.f, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));

	return S_OK;
}

void CSkillChargingUI::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
}

void CSkillChargingUI::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CSkillChargingUI::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (m_SkillIconDesc.fAmount >= 0.f)
	{
		if (FAILED(SetUp_ShaderBaseResources()))
			return E_FAIL;

		m_pShaderCom->Begin(1);
		m_pVIBufferCom->Render();
	}

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(m_SkillIconDesc.iPassIndex);
	m_pVIBufferCom->Render();

	return S_OK;
}

void CSkillChargingUI::Imgui_RenderProperty()
{
	ImGui::DragFloat("Alpha", &m_SkillIconDesc.fAlpha, 0.1f, -10.f, 10.f);
	ImGui::DragFloat("Amount", &m_SkillIconDesc.fAmount, 0.1f, -10.f, 10.f);

	ImGui::DragFloat("X", &m_fX, 0.1f, -1000.f, 1000.f);
	ImGui::DragFloat("Y", &m_fY, 0.1f, -1000.f, 1000.f);
	ImGui::DragFloat("SizeX", &m_fSizeX, 0.1f, -1000.f, 1000.f);
	ImGui::DragFloat("SizeY", &m_fSizeY, 0.1f, -1000.f, 1000.f);

	m_pTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION,
		XMVectorSet(m_fX, m_fY, 0.f, 1.f));
}

void CSkillChargingUI::Set_Amount(_float fAmount)
{
	m_SkillIconDesc.fAmount = fAmount;
}

HRESULT CSkillChargingUI::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_SkillProgressUI"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"),
		(CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Texture_SkillIcon"), TEXT("Com_Texture"),
		(CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_BaseTexture */
	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Texture_SkillIconBase"), TEXT("Com_BaseTexture"),
		(CComponent**)&m_pBaseTexture)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkillChargingUI::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture",m_SkillIconDesc.iTexNum)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_fAlpha", &m_SkillIconDesc.fAlpha, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_fAmount", &m_SkillIconDesc.fAmount, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkillChargingUI::SetUp_ShaderBaseResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	_int iClick = 0;
	if (m_bClicked)
		iClick = 1;
	else
		iClick = 0;

	if (FAILED(m_pBaseTexture->Bind_ShaderResource(m_pShaderCom, "g_Texture", iClick)))
		return E_FAIL;

	m_bClicked = false;

	if (FAILED(m_pShaderCom->Set_RawValue("g_fAlpha", &m_SkillIconDesc.fAlpha, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

CSkillChargingUI * CSkillChargingUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSkillChargingUI*		pInstance = new CSkillChargingUI(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Created : CSkillChargingUI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSkillChargingUI::Clone(void * pArg)
{
	CSkillChargingUI*		pInstance = new CSkillChargingUI(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSkillChargingUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSkillChargingUI::Free()
{
	__super::Free();
	Safe_Release(m_pBaseTexture);
}
