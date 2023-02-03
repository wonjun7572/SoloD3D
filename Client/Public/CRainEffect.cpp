#include "stdafx.h"
#include "CRainEffect.h"

#include "Camera_Dynamic.h"
#include "GameInstance.h"
#include "PlayerCamera.h"

CRainEffect::CRainEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :Effect_Point(pDevice, pContext)
{
}

CRainEffect::CRainEffect(const Effect_Point& rhs)
    :Effect_Point(rhs)
{
}

HRESULT CRainEffect::Init_Prototype()
{
    if (FAILED(__super::Init_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRainEffect::Init(void* pArg)
{
    if (FAILED(__super::Init(pArg)))
        return E_FAIL;

    if (FAILED(SetUp_Components()))
        return E_FAIL;

    CTransform::TRANSFORMDESC transformDesc;

    transformDesc.fRotationPerSec = XMConvertToRadians(270.0f);
    transformDesc.fSpeedPerSec = 1.0f;
    m_pTransformCom->Set_TransformDesc(transformDesc);

    CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance)

    if(g_LEVEL == LEVEL_CHAP2)
    {
        m_pPlayerCam = static_cast<CPlayerCamera*>(pGameInstance->Find_GameObject(LEVEL_CHAP2, L"Layer_Camera", L"PlayerCamera"));
        m_pDynamicCam = static_cast<CPlayerCamera*>(pGameInstance->Find_GameObject(LEVEL_CHAP2, L"Layer_Camera", L"DynamicCamera"));
    }

    RELEASE_INSTANCE(CGameInstance)

    return S_OK;
}

void CRainEffect::Tick(_double TimeDelta)
{
    Effect_Point::Tick(TimeDelta);

    if(m_pPlayerCam != nullptr && m_pDynamicCam != nullptr && g_LEVEL == LEVEL_CHAP2)
    {
        if(static_cast<CCamera_Dynamic*>(m_pDynamicCam)->Get_Paused() == true)
            m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION,m_pPlayerCam->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION));
        else
            m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION,m_pDynamicCam->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION));
    }
        
    
    m_pVIBufferCom->Go_Dir(_float4(0.f, -1.f,0.f,0.f),TimeDelta);
}

void CRainEffect::Late_Tick(_double TimeDelta)
{
    Effect_Point::Late_Tick(TimeDelta);

    if (nullptr != m_pRendererCom)
        m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CRainEffect::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    if (FAILED(SetUp_ShaderResources()))
        return E_FAIL;

    m_pShaderCom->Begin(0);

    m_pVIBufferCom->Render();

    return S_OK;
}

HRESULT CRainEffect::SetUp_Components()
{
    /* For.Com_Renderer */
    if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
        (CComponent**)&m_pRendererCom)))
            return E_FAIL;

    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxPointInstancing"), TEXT("Com_Shader"),
        (CComponent**)&m_pShaderCom)))
            return E_FAIL;

    /* For.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_VIBuffer_Point_Instancing"), TEXT("Com_VIBuffer"),
        (CComponent**)&m_pVIBufferCom)))
            return E_FAIL;

    /* For.Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Texture_Rain"), TEXT("Com_Texture"),
        (CComponent**)&m_pTextureCom)))
            return E_FAIL;

    return S_OK;
}

HRESULT CRainEffect::SetUp_ShaderResources()
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

    if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4))))
        return E_FAIL;

    RELEASE_INSTANCE(CGameInstance);

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
        return E_FAIL;

    return S_OK;
}

CRainEffect* CRainEffect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CRainEffect*		pInstance = new CRainEffect(pDevice, pContext);

    if (FAILED(pInstance->Init_Prototype()))
    {
        MSG_BOX("Failed to Created : CRainEffect");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject * CRainEffect::Clone(void * pArg)
{
    CRainEffect*		pInstance = new CRainEffect(*this);

    if (FAILED(pInstance->Init(pArg)))
    {
        MSG_BOX("Failed to Cloned : CRainEffect");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CRainEffect::Free()
{
    __super::Free();
}

