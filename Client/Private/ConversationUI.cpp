#include "stdafx.h"
#include "..\Public\ConversationUI.h"
#include "GameInstance.h"

CConversationUI::CConversationUI(ID3D11Device * pDeviec, ID3D11DeviceContext * pContext)
	:CUI(pDeviec, pContext)
{
}

CConversationUI::CConversationUI(const CConversationUI & rhs)
	:CUI(rhs)
{
}

HRESULT CConversationUI::Init_Prototype()
{
	if (FAILED(__super::Init_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CConversationUI::Init(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 5.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(CGameObject::Init(&GameObjectDesc)))
		return E_FAIL;

	if (pArg != nullptr)
		memcpy(&m_tagConversationFont, pArg, sizeof(CONVERSATIONFONT));

	m_fX = m_tagConversationFont.fX;
	m_fY = m_tagConversationFont.fY;
	m_fSizeX = m_tagConversationFont.fSizeX;
	m_fSizeY = m_tagConversationFont.fSizeY;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	return S_OK;
}

void CConversationUI::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
}

void CConversationUI::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (!m_bDead && nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CConversationUI::Render()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	pGameInstance->Render_Font(TEXT("Font_Comic"), m_tagConversationFont.szConversation, _float2(m_fX, m_fY), 0.f, _float2(m_fSizeX, m_fSizeY), m_tagConversationFont.vColor);

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

void CConversationUI::SetTransform(const CONVERSATIONFONT & tagFont)
{
	lstrcpy(m_tagConversationFont.szConversation, tagFont.szConversation);
	lstrcpy(m_tagConversationFont.szFontName, tagFont.szFontName);
	m_tagConversationFont.vColor = tagFont.vColor;
	m_fX = tagFont.fX;
	m_fY = tagFont.fY;
	m_fSizeX = tagFont.fSizeX;
	m_fSizeY = tagFont.fSizeY;
}

void CConversationUI::SetConversation(const wstring& strConversation)
{
	lstrcpy(m_tagConversationFont.szConversation, strConversation.c_str());
}

void CConversationUI::Imgui_RenderProperty()
{
	ImGui::DragFloat("ConverSationX", &m_fX, 0.1f, -1000.f, 1000.f);
	ImGui::DragFloat("ConverSationY", &m_fY, 0.1f, -1000.f, 1000.f);
	ImGui::DragFloat("ConverSationSizeX", &m_fSizeX, 0.1f, -1000.f, 1000.f);
	ImGui::DragFloat("ConverSationSizeY", &m_fSizeY, 0.1f, -1000.f, 1000.f);

	m_pTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION,
		XMVectorSet(m_fX, m_fY, 0.f, 1.f));
}

HRESULT CConversationUI::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	return S_OK;
}

CConversationUI * CConversationUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CConversationUI*		pInstance = new CConversationUI(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Created : CConversationUI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CConversationUI::Clone(void * pArg)
{
	CConversationUI*		pInstance = new CConversationUI(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Cloned : CConversationUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CConversationUI::Free()
{
	__super::Free();
}

