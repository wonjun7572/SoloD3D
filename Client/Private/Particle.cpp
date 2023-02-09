#include "stdafx.h"
#include "..\Public\Particle.h"
#include "GameInstance.h"

CParticle::CParticle(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CGameObject(pDevice, pContext)
{
}

CParticle::CParticle(const CParticle & rhs)
	: CGameObject(rhs)
{
}

HRESULT CParticle::Init_Prototype()
{
	if (FAILED(__super::Init_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CParticle::Init(void * pArg)
{
	ZeroMemory(&m_Desc, sizeof(DESC));

	if (pArg == nullptr)
		return E_FAIL;

	memcpy(&m_Desc, pArg, sizeof(DESC));

	CGameObject::GAMEOBJECTDESC GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(270.0f);
	GameObjectDesc.TransformDesc.fSpeedPerSec = 1.0f;

	if (FAILED(__super::Init(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	return S_OK;
}

void CParticle::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	
	m_Desc.fShowTime -= static_cast<_float>(TimeDelta);

	if (m_Desc.iProperty == COCHLEA && m_Desc.fShowTime <= 0.f)
		m_bDead = true;

	if (m_Desc.iProperty == COCHLEA)
		m_pVIBufferCom->Tick(TimeDelta);
	else if(m_Desc.iProperty == GODIRECTION)
		m_pVIBufferCom->Go_Dir(m_vDir, TimeDelta);
}

void CParticle::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
}

HRESULT CParticle::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CParticle::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxPtInst_Cochlea"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// 여기서 버퍼 사본 생성할때 애초에 다르게 파티클 생성해주는것 

	CVIBuffer_Point_Cochlea::DESC CochleaDesc;
	ZeroMemory(&CochleaDesc, sizeof(CVIBuffer_Point_Cochlea::DESC));
	memcpy(&CochleaDesc, &m_Desc.ViBufferDesc, sizeof(CVIBuffer_Point_Cochlea::DESC));

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_VIBuffer_Point_Cochlea"), TEXT("Com_VIBuffer"),
		(CComponent**)&m_pVIBufferCom, &CochleaDesc)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_CHAP1, m_Desc.pTextureTag , TEXT("Com_Texture"),
		(CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CParticle::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom) return E_FAIL;
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"))) return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)))) return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)))) return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4)))) return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture"))) return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_fSizeX_UV", &m_Desc.fSizeUV_X, sizeof(_float)))) return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fSizeY_UV", &m_Desc.fSizeUV_Y, sizeof(_float)))) return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_iSpriteCountX", &m_Desc.iSpriteCount_X, sizeof(_uint)))) return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_iSpriteCountY", &m_Desc.iSpriteCount_Y, sizeof(_uint)))) return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CParticle * CParticle::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CParticle*		pInstance = new CParticle(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Created : CParticle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CParticle::Clone(void * pArg)
{
	CParticle*		pInstance = new CParticle(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Cloned : CParticle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CParticle::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}