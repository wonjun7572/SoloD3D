#include "stdafx.h"
#include "BoomWave_0.h"
#include "GameInstance.h"

CBoomWave_0::CBoomWave_0(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CEffect_Mesh(pDevice, pContext)
{
}

CBoomWave_0::CBoomWave_0(const CBoomWave_0 & rhs)
	: CEffect_Mesh(rhs)
{
}

HRESULT CBoomWave_0::Init_Prototype()
{
	if (FAILED(__super::Init_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoomWave_0::Init(void * pArg)
{
	if (FAILED(__super::Init(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;


	CTransform::TRANSFORMDESC transformDesc;

	transformDesc.fRotationPerSec = XMConvertToRadians(270.0f);
	transformDesc.fSpeedPerSec = 1.0f;
	m_pTransformCom->Set_TransformDesc(transformDesc);

	m_UVMoveFactor = _float2(0.f, -1.f);

	return S_OK;
}

void CBoomWave_0::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	
	m_UVMoveFactor.x += static_cast<_float>(TimeDelta) * m_fUVSpeed;
	if (m_UVMoveFactor.x >= 1.f)
		m_UVMoveFactor.x = -1.f;

	m_UVMoveFactor.y += static_cast<_float>(TimeDelta) * m_fUVSpeed;
	if (m_UVMoveFactor.y >= 1.f)
		m_UVMoveFactor.y = -1.f;
	
	if (m_MEffectDesc.pTargetTransform != nullptr && !m_bLinking)
		m_pTransformCom->SetWorldMatrix(XMLoadFloat4x4(&m_MEffectDesc.PivotMatrix) * m_MEffectDesc.pTargetTransform->Get_WorldMatrix());
}

void CBoomWave_0::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CBoomWave_0::Render()
{
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pDiffuseTexCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", m_MEffectDesc.iDiffuseTex);
		m_pMaskTexCom->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture", m_MEffectDesc.iMaskTex); // 28
		m_pModelCom->Render(m_pShaderCom, i, m_MEffectDesc.iPassIndex); // 2
	}

	return S_OK;
}

void CBoomWave_0::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();
}

HRESULT CBoomWave_0::SetUp_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("BoomWave_0"), TEXT("Com_Model"),
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

HRESULT CBoomWave_0::SetUp_ShaderResources()
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

CBoomWave_0 * CBoomWave_0::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBoomWave_0*		pInstance = new CBoomWave_0(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Created : CBoomWave_0");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CBoomWave_0::Clone(void * pArg)
{
	CBoomWave_0*		pInstance = new CBoomWave_0(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBoomWave_0");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBoomWave_0::Free()
{
	__super::Free();
}
