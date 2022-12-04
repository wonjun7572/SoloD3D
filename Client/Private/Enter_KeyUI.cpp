#include "stdafx.h"
#include "..\Public\Enter_KeyUI.h"
#include "GameInstance.h"

CEnter_KeyUI::CEnter_KeyUI(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CEnter_KeyUI::CEnter_KeyUI(const CEnter_KeyUI & rhs)
	:CGameObject(rhs)
{
}

HRESULT CEnter_KeyUI::Init_Prototype()
{
	if (FAILED(__super::Init_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEnter_KeyUI::Init(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GAMEOBJECTDESC));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 5.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Init(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_fSizeX = 256.f;
	m_fSizeY = 128.f;

	m_fX = m_fSizeX;
	m_fY = m_fSizeY;

	m_pTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(static_cast<float>(g_iWinSizeX), static_cast<float>(g_iWinSizeY), 0.f, 1.f));

	return S_OK;
}

void CEnter_KeyUI::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	//pGameInstance->Render_Font(TEXT("Font_Nexon"), L"을(를) 눌러 시작하기", XMVectorSet(m_fX + m_fSizeX * 0.5f, m_fY, 0.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f));

	RELEASE_INSTANCE(CGameInstance);

	//KeyInput(TimeDelta);
}

void CEnter_KeyUI::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	// UI로 사용할 것이면 RenderGroup을 UI로 바꿔줘야함
	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CEnter_KeyUI::Render()
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

void CEnter_KeyUI::KeyInput(_double TimeDelta)
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

HRESULT CEnter_KeyUI::SetUp_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_LOGO, TEXT("Prototype_Component_Texture_EnterKey"), TEXT("Com_Texture"),
		(CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEnter_KeyUI::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
		return E_FAIL;

	return S_OK;
}

CEnter_KeyUI * CEnter_KeyUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CEnter_KeyUI*		pInstance = new CEnter_KeyUI(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Created : CEnter_KeyUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEnter_KeyUI::Clone(void * pArg)
{
	CEnter_KeyUI*		pInstance = new CEnter_KeyUI(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBackGround");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEnter_KeyUI::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
