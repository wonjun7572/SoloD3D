#include "stdafx.h"
#include "FSkillTrail.h"
#include "GameInstance.h"

CFSkillTrail::CFSkillTrail(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CEffect_Mesh(pDevice, pContext)
{
}

CFSkillTrail::CFSkillTrail(const CFSkillTrail & rhs)
	: CEffect_Mesh(rhs)
{
}

HRESULT CFSkillTrail::Init_Prototype()
{
	if (FAILED(__super::Init_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CFSkillTrail::Init(void * pArg)
{
	if (FAILED(__super::Init(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	return S_OK;
}

void CFSkillTrail::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	m_UVMoveFactor.x += static_cast<_float>(TimeDelta) * m_fUVSpeed;
	if (m_UVMoveFactor.x >= 1.f)
		m_UVMoveFactor.x = -1.f;

	if (m_MEffectDesc.pTargetTransform != nullptr && !m_bLinking)
		m_pTransformCom->SetWorldMatrix(XMLoadFloat4x4(&m_MEffectDesc.PivotMatrix) * m_MEffectDesc.pTargetTransform->Get_WorldMatrix());
}

void CFSkillTrail::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CFSkillTrail::Render()
{
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pDiffuseTexCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", m_MEffectDesc.iDiffuseTex);
		m_pMaskTexCom->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture", m_MEffectDesc.iMaskTex);
		m_pModelCom->Render(m_pShaderCom, i, m_MEffectDesc.iPassIndex);
	}

	return S_OK;
}

HRESULT CFSkillTrail::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_MeshEffect"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("FSkillTrail"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Texture_Diffuse */
	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Texture_Trail"), TEXT("Com_Texture_Diffuse"),
		(CComponent**)&m_pDiffuseTexCom)))
		return E_FAIL;

	/* For.Com_Texture_Mask */
	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Texture_Trail"), TEXT("Com_Texture_Mask"),
		(CComponent**)&m_pMaskTexCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CFSkillTrail::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_UVMoveFactor", &m_UVMoveFactor, sizeof(_float2))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CFSkillTrail * CFSkillTrail::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CFSkillTrail*		pInstance = new CFSkillTrail(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Created : CFSkillTrail");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CFSkillTrail::Clone(void * pArg)
{
	CFSkillTrail*		pInstance = new CFSkillTrail(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Cloned : CFSkillTrail");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CFSkillTrail::Free()
{
	__super::Free();
}
