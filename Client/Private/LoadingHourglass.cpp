#include "stdafx.h"
#include "..\Public\LoadingHourglass.h"
#include "GameInstance.h"

CLoadingHourglass::CLoadingHourglass(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CGameObject(pDevice,pContext)
{
}

CLoadingHourglass::CLoadingHourglass(const CLoadingHourglass & rhs)
	:CGameObject(rhs)
{
}

HRESULT CLoadingHourglass::Init_Prototype()
{
	if (FAILED(__super::Init_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoadingHourglass::Init(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 5.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Init(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_fSizeX = 256.f;
	m_fSizeY = 256.f;

	m_fX = m_fSizeX;
	m_fY = m_fSizeY;

	m_pTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(static_cast<float>(g_iWinSizeX), static_cast<float>(g_iWinSizeY), 0.f, 1.f));

	return S_OK;
}

void CLoadingHourglass::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	
	m_iDeffuseTexNum ++;
	if (m_iDeffuseTexNum > 56)
		m_iDeffuseTexNum = 0;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	//pGameInstance->Render_Font(TEXT("Font_Nexon"), L"을(를) 눌러 시작하기", XMVectorSet(m_fX + m_fSizeX * 0.5f, m_fY, 0.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f));

	RELEASE_INSTANCE(CGameInstance);

	KeyInput(TimeDelta);
}

void CLoadingHourglass::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	// UI로 사용할 것이면 RenderGroup을 UI로 바꿔줘야함
	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CLoadingHourglass::Render()
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

void CLoadingHourglass::KeyInput(_double TimeDelta)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Get_DIKeyState(DIK_LALT) && pGameInstance->Get_DIKeyState(DIK_UPARROW))
		m_fSizeY += 1.f;
	else if (pGameInstance->Get_DIKeyState(DIK_LALT) && pGameInstance->Get_DIKeyState(DIK_DOWNARROW))
		m_fSizeY -= 1.f;
	else if (pGameInstance->Get_DIKeyState(DIK_LALT) && pGameInstance->Get_DIKeyState(DIK_RIGHTARROW))
		m_fSizeX -= 1.f;
	else if (pGameInstance->Get_DIKeyState(DIK_LALT) && pGameInstance->Get_DIKeyState(DIK_LEFTARROW))
		m_fSizeX += 1.f;
	else if (pGameInstance->Get_DIKeyState(DIK_UPARROW))
		m_fY -= 1.f;
	else if (pGameInstance->Get_DIKeyState(DIK_DOWNARROW))
		m_fY += 1.f;
	else if (pGameInstance->Get_DIKeyState(DIK_LEFTARROW))
		m_fX -= 1.f;
	else if (pGameInstance->Get_DIKeyState(DIK_RIGHTARROW))
		m_fX += 1.f;

	m_pTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(static_cast<float>(g_iWinSizeX), static_cast<float>(g_iWinSizeY), 0.f, 1.f));


	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CLoadingHourglass::SetUp_Components()
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
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_LoadingGlass"), TEXT("Com_Texture"),
		(CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoadingHourglass::SetUp_ShaderResources()
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

CLoadingHourglass * CLoadingHourglass::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLoadingHourglass*		pInstance = new CLoadingHourglass(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Created : CLoadingHourglass");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CLoadingHourglass::Clone(void * pArg)
{
	CLoadingHourglass*		pInstance = new CLoadingHourglass(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Cloned : CLoadingHourglass");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLoadingHourglass::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
