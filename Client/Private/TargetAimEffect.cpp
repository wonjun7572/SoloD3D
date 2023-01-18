#include "stdafx.h"
#include "..\Public\TargetAimEffect.h"
#include "GameInstance.h"

CTargetAimEffect::CTargetAimEffect(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CEffect_Rect(pDevice, pContext)
{
}

CTargetAimEffect::CTargetAimEffect(const CEffect_Rect & rhs)
	:CEffect_Rect(rhs)
{
}

HRESULT CTargetAimEffect::Init_Prototype()
{
	if (FAILED(__super::Init_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTargetAimEffect::Init(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 5.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(CGameObject::Init(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scaled(_float3(5.f, 5.f, 1.f));
	m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	m_strObjName = L"Effect_Rect";

	m_fAlpha = 5.f;
	m_UVMoveFactor = _float2(0.f, 0.f);

	return S_OK;
}

void CTargetAimEffect::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), TimeDelta * -1.0);
}

void CTargetAimEffect::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CTargetAimEffect::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(1);

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CTargetAimEffect::SetUp_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Texture_MonsterAimUI"), TEXT("Com_Texture"),
		(CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTargetAimEffect::SetUp_ShaderResources()
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

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_AlbedoTexture", 1)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_UVMoveFactor", &m_UVMoveFactor, sizeof(_float2))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CTargetAimEffect * CTargetAimEffect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTargetAimEffect*		pInstance = new CTargetAimEffect(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Created : CTargetAimEffect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CTargetAimEffect::Clone(void * pArg)
{
	CTargetAimEffect*		pInstance = new CTargetAimEffect(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTargetAimEffect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTargetAimEffect::Free()
{
	__super::Free();
}
