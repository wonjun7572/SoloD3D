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

	GameObjectDesc.TransformDesc.fSpeedPerSec = 1.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(CGameObject::Init(&GameObjectDesc)))
		return E_FAIL;

	if (pArg != nullptr)
		memcpy(&m_tagDamageFont, pArg, sizeof(DAMAGEFONT));

	if (FAILED(SetUp_Components()))
		return E_FAIL;
	
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, m_tagDamageFont.vPos);

	m_fSize = 1.5f;
	m_pTransformCom->Set_Scaled(_float3(m_fSize, m_fSize, m_fSize));

	return S_OK;
}

void CDamageFontUI::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	m_TimeDelta += TimeDelta;
	
	if(m_fSize >= 1.f)
		m_fSize -= static_cast<_float>(TimeDelta);
	
	m_pTransformCom->Set_Scaled(_float3(m_fSize, m_fSize, m_fSize));

	if (m_TimeDelta > 2.0)
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

	if (m_tagDamageFont.iVersion == 0)
	{
		pGameInstance->DrawTextInWorld(TEXT("Font_Comic"), m_tagDamageFont.szDamage, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION), m_pTransformCom->Get_Scaled(), m_tagDamageFont.vColor);
	}
	else if(m_tagDamageFont.iVersion == 1)
	{
		wstring strText, strDamage;
		strText = L"Ä¡¸íÅ¸ ";
		strDamage = m_tagDamageFont.szDamage;
		strText += strDamage;
		pGameInstance->DrawTextInWorld(TEXT("Font_Comic"), strText.c_str(), m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION), m_pTransformCom->Get_Scaled(), m_tagDamageFont.vColor);
	}
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
