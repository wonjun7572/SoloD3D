#include "stdafx.h"
#include "..\Public\DamageFontUI.h"
#include "GameInstance.h"

CDamageFontUI::CDamageFontUI(ID3D11Device * pDeviec, ID3D11DeviceContext * pContext)
	:CUI(pDeviec , pContext)
{
}

CDamageFontUI::CDamageFontUI(const CDamageFontUI & rhs)
	:CUI(rhs)
{
}

HRESULT CDamageFontUI::Init_Prototype()
{
	if (FAILED(__super::Init_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDamageFontUI::Init(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 5.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(CGameObject::Init(&GameObjectDesc)))
		return E_FAIL;

	if (pArg != nullptr)
		memcpy(&m_tagDamageFont, pArg, sizeof(DAMAGEFONT));

	m_fX = m_tagDamageFont.fX;
	m_fY = m_tagDamageFont.fY;
	m_fSizeX = m_tagDamageFont.fSizeX;
	m_fSizeY = m_tagDamageFont.fSizeY;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	return S_OK;
}

void CDamageFontUI::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	m_TimeDelta += TimeDelta;

	if (m_fSizeX >= 2.f && m_fSizeY >= 2.f)
	{
		m_fSizeX -= 75.f *  static_cast<_float>(TimeDelta);
		m_fSizeY -= 75.f *  static_cast<_float>(TimeDelta);
	}
	else
	{
		m_fY -= static_cast<_float>(TimeDelta) * 10.f;
	}

	if (m_TimeDelta > 3.0)
		m_bDead = true;
}

void CDamageFontUI::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (!m_bDead && nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CDamageFontUI::Render()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	
	pGameInstance->Render_Font(TEXT("Font_Comic"), m_tagDamageFont.szDamage, _float2(m_fX, m_fY), 0.f, _float2(m_fSizeX, m_fSizeY), m_tagDamageFont.vColor);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CDamageFontUI::SetUp_Components()
{	
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	return S_OK;
}

CDamageFontUI * CDamageFontUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CDamageFontUI*		pInstance = new CDamageFontUI(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Created : CDamageFontUI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CDamageFontUI::Clone(void * pArg)
{
	CDamageFontUI*		pInstance = new CDamageFontUI(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDamageFontUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDamageFontUI::Free()
{
	__super::Free();
}
