#include "stdafx.h"
#include "..\Public\MonsterNameUI.h"
#include "GameInstance.h"

CMonsterNameUI::CMonsterNameUI(ID3D11Device * pDeviec, ID3D11DeviceContext * pContext)
	:CUI(pDeviec, pContext)
{
}

CMonsterNameUI::CMonsterNameUI(const CMonsterNameUI & rhs)
	:CUI(rhs)
{
}

HRESULT CMonsterNameUI::Init_Prototype()
{
	if (FAILED(__super::Init_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonsterNameUI::Init(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 5.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(CGameObject::Init(&GameObjectDesc)))
		return E_FAIL;

	if (pArg != nullptr)
		memcpy(&m_tagNameFont, pArg, sizeof(NAMEFONT));

	m_fX = m_tagNameFont.fX;
	m_fY = m_tagNameFont.fY;
	m_fSizeX = m_tagNameFont.fSizeX;
	m_fSizeY = m_tagNameFont.fSizeY;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	return S_OK;
}

void CMonsterNameUI::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
}

void CMonsterNameUI::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (!m_bDead && nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CMonsterNameUI::Render()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	pGameInstance->Render_Font(TEXT("Font_Comic"), m_tagNameFont.szName, _float2(m_fX, m_fY), 0.f, _float2(m_fSizeX, m_fSizeY), m_tagNameFont.vColor);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CMonsterNameUI::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	return S_OK;
}

CMonsterNameUI * CMonsterNameUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMonsterNameUI*		pInstance = new CMonsterNameUI(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Created : CMonsterNameUI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMonsterNameUI::Clone(void * pArg)
{
	CMonsterNameUI*		pInstance = new CMonsterNameUI(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMonsterNameUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMonsterNameUI::Free()
{
	__super::Free();
}
