#include "stdafx.h"
#include "..\Public\FlameE.h"
#include "GameInstance.h"

CFlameE::CFlameE(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CEffect_Rect(pDevice, pContext)
{
}

CFlameE::CFlameE(const CEffect_Rect & rhs)
	: CEffect_Rect(rhs)
{
}

HRESULT CFlameE::Init_Prototype()
{
	if (FAILED(__super::Init_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CFlameE::Init(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 5.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(CGameObject::Init(&GameObjectDesc)))
		return E_FAIL;

	XMStoreFloat4x4(&m_RectEffectDesc.PivotMatrix, XMMatrixIdentity());
	m_RectEffectDesc.pTargetTransform = nullptr;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	_float4 vPos;

	if (pArg != nullptr)
	{
		memcpy(&vPos, pArg, sizeof(_float4));
	}

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vPos);
	m_strObjName = L"Effect_Rect_Explosion";

	LIGHTDESC			LightDesc;
	ZeroMemory(&LightDesc, sizeof LightDesc);
	LightDesc.eType = LIGHTDESC::TYPE_POINT;
	LightDesc.isEnable = true;
	XMStoreFloat4(&LightDesc.vPosition, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	LightDesc.fRange = 5.0f;
	LightDesc.vDiffuse = _float4(0.6f, 0.4f, 0.f, 1.f);
	LightDesc.vAmbient = _float4(0.4f, 0.2f, 0.2f, 0.2f);
	LightDesc.vSpecular = LightDesc.vDiffuse;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	m_fAlpha = 1.f;
	m_UVMoveFactor = _float2(0.f, 0.f);

	return S_OK;
}

void CFlameE::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	Compute_BillBoard();
	m_pTransformCom->Set_Scaled(_float3(4.f, 4.f, m_CSize[2]));

	m_fFrame += 32.0f * static_cast<_float>(TimeDelta);

	if (m_fFrame >= 32.0f)
		m_fFrame = 0.f;
}

void CFlameE::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	__super::Compute_CamDistance();

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CFlameE::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Render();

	return S_OK;
}

void CFlameE::Imgui_RenderProperty()
{
	ImGui::DragFloat3("AfterScale", m_CSize, 0.01f, 0.f, 100.f);
}

HRESULT CFlameE::SetUp_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Texture_Flame"), TEXT("Com_Texture"),
		(CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CFlameE::SetUp_ShaderResources()
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

CFlameE * CFlameE::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CFlameE*		pInstance = new CFlameE(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Created : CFlameE");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CFlameE::Clone(void * pArg)
{
	CFlameE*		pInstance = new CFlameE(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Cloned : CFlameE");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CFlameE::Free()
{
	__super::Free();
}
