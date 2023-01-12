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

	m_pTransformCom->Set_Scaled(_float3(5.f, 5.f, 1.f));
	m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	m_strObjName = L"Effect_Rect";

	m_fAlpha = 5.f;
	m_UVMoveFactor = _float2(0.f, 0.f);

	return S_OK;
}

void CEffect_Rect::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	
	m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), TimeDelta * -1.0);
}

void CEffect_Rect::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CEffect_Rect::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(1);

	m_pVIBufferCom->Render();

	return S_OK;
}

void CEffect_Rect::Imgui_RenderProperty()
{
	ImGui::DragFloat("Alpha", &m_fAlpha, 0.1f, -10.f, 10.f);
	ImGui::DragFloat("MoveX", &m_UVMoveFactor.x, 0.1f, -10.f, 10.f);
	ImGui::DragFloat("MoveY", &m_UVMoveFactor.y, 0.1f, -10.f, 10.f);

	ImGui::Begin("Effect");
	m_pTransformCom->Imgui_RenderProperty();
	ImGui::End();
}

void CEffect_Rect::LinkObject(_double TimeDelta, _fvector targetpos)
{
	_float3 vPos = targetpos;

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(vPos.x, vPos.y + 0.6f, vPos.z, 1.f));
}

HRESULT CEffect_Rect::SetUp_Components()
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

HRESULT CEffect_Rect::SetUp_ShaderResources()
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
