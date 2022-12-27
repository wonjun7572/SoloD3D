#include "stdafx.h"
#include "..\Public\LoadingBackGround.h"
#include "GameInstance.h"

CLoadingBackGround::CLoadingBackGround(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CGameObject(pDevice, pContext)
{
}

CLoadingBackGround::CLoadingBackGround(const CLoadingBackGround & rhs)
	: CGameObject(rhs)
{
}

HRESULT CLoadingBackGround::Init_Prototype()
{
	if (FAILED(__super::Init_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoadingBackGround::Init(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;

	GameObjectDesc.TransformDesc.fSpeedPerSec = 5.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Init(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_fSizeX = static_cast<_float>(g_iWinSizeX);
	m_fSizeY = static_cast<_float>(g_iWinSizeY);

	m_fX = m_fSizeX;
	m_fY = m_fSizeY;

	m_pTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fX - g_iWinSizeX * 1.f, -m_fY + g_iWinSizeY * 1.f, 0.f, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(static_cast<float>(g_iWinSizeX), static_cast<float>(g_iWinSizeY), 0.f, 1.f));

	return S_OK;
}

void CLoadingBackGround::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	m_ChangeTime += TimeDelta;

	if (m_ChangeTime > 3)
	{
		m_iDeffuseTexNum++;
		m_ChangeTime = 0.0;
		if (m_iDeffuseTexNum > 6)
			m_iDeffuseTexNum = 0;
	}

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	RELEASE_INSTANCE(CGameInstance);
}

void CLoadingBackGround::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	// UI로 사용할 것이면 RenderGroup을 UI로 바꿔줘야함
	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CLoadingBackGround::Render()
{
	if (m_pShaderCom == nullptr)
		return E_FAIL;

	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CLoadingBackGround::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"),
		(CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_LoadingBackGround"), TEXT("Com_Texture"),
		(CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoadingBackGround::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iDeffuseTexNum)))
		return E_FAIL;

	return S_OK;
}

CLoadingBackGround * CLoadingBackGround::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLoadingBackGround*		pInstance = new CLoadingBackGround(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Created : CLoadingBackGround");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CLoadingBackGround::Clone(void * pArg)
{
	CLoadingBackGround*		pInstance = new CLoadingBackGround(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Cloned : CLoadingBackGround");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLoadingBackGround::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
