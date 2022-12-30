#include "stdafx.h"
#include "..\public\Effect_Rect.h"
#include "GameInstance.h"

CEffect_Rect::CEffect_Rect(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CEffect_Rect::CEffect_Rect(const CEffect_Rect & rhs)
	: CGameObject(rhs)
{
}

HRESULT CEffect_Rect::Init_Prototype()
{
	if (FAILED(__super::Init_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Rect::Init(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 5.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(CGameObject::Init(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scaled(_float3(7.f,7.f,0.f));
	m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 1.f),XMConvertToRadians(90.f));
	m_strObjName = L"Effect_Rect";

	return S_OK;
}

void CEffect_Rect::Tick(_double TimeDelta)
{
	if (m_bTick)
	{
		__super::Tick(TimeDelta);
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), -TimeDelta * 3.f);
		m_pVIBufferCom->Tick(TimeDelta);
	}
}

void CEffect_Rect::Late_Tick(_double TimeDelta)
{
	if (m_bTick)
	{
		__super::Late_Tick(TimeDelta);

		if (nullptr != m_pRendererCom)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}
}

HRESULT CEffect_Rect::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Render();

	return S_OK;
}

void CEffect_Rect::LinkPlayer(_double TimeDelta, _fvector targetpos)
{
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, targetpos);
}

HRESULT CEffect_Rect::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxRectInstancing"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;
	
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_VIBuffer_Rect_Instancing"), TEXT("Com_VIBuffer"),
		(CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Texture_Weapon_Skill1_Effect"), TEXT("Com_Texture"),
		(CComponent**)&m_pTextureCom)))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CEffect_Rect::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	// m_pShaderCom->Set_Matrix("g_WorldMatrix", &m_WorldMatrix);
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
		return E_FAIL;

	return S_OK;
}

CEffect_Rect * CEffect_Rect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CEffect_Rect*		pInstance = new CEffect_Rect(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Created : CEffect_Rect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_Rect::Clone(void * pArg)
{
	CEffect_Rect*		pInstance = new CEffect_Rect(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEffect_Rect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_Rect::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
