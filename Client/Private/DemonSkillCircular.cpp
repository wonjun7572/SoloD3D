#include "stdafx.h"
#include "..\Public\DemonSkillCircular.h"
#include "GameInstance.h"

CDemonSkillCircular::CDemonSkillCircular(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect_Rect(pDevice, pContext)
{
}

CDemonSkillCircular::CDemonSkillCircular(const CEffect_Rect & rhs)
	: CEffect_Rect(rhs)
{
}

HRESULT CDemonSkillCircular::Init_Prototype()
{
	if (FAILED(__super::Init_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDemonSkillCircular::Init(void * pArg)
{
	if (FAILED(__super::Init(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_strObjName = L"Effect_SkillCircular";

	m_fAlpha = 1.f;
	m_UVMoveFactor = _float2(0.f, 0.f);

	return S_OK;
}

void CDemonSkillCircular::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
}

void CDemonSkillCircular::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (m_RectEffectDesc.pTargetTransform != nullptr && m_bLinking)
		m_pTransformCom->SetWorldMatrix(XMLoadFloat4x4(&m_RectEffectDesc.PivotMatrix) * m_RectEffectDesc.pTargetTransform->Get_WorldMatrix());
}

HRESULT CDemonSkillCircular::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(2);

	m_pVIBufferCom->Render();

	return S_OK;
}

void CDemonSkillCircular::Imgui_RenderProperty()
{
	ImGui::DragFloat("Amount", &m_fAmount, 0.01f, 0.f, 1.f);
}

void CDemonSkillCircular::Set_Amount(_float fAmount)
{
	m_fAmount = fAmount;
}

HRESULT CDemonSkillCircular::SetUp_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Texture_DemonSkillFloorD"), TEXT("Com_Texture"),
		(CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_MaskTex */
	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Texture_DemonSkillFloork"), TEXT("Com_MaskTex"),
		(CComponent**)&m_pMaskTexCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CDemonSkillCircular::SetUp_ShaderResources()
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

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
		return E_FAIL;

	if (FAILED(m_pMaskTexCom->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_fAmount", &m_fAmount, sizeof(_float))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CDemonSkillCircular * CDemonSkillCircular::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CDemonSkillCircular*		pInstance = new CDemonSkillCircular(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Created : CDemonSkillCircular");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CDemonSkillCircular::Clone(void * pArg)
{
	CDemonSkillCircular*		pInstance = new CDemonSkillCircular(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDemonSkillCircular");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDemonSkillCircular::Free()
{
	__super::Free();
	Safe_Release(m_pMaskTexCom);
}
