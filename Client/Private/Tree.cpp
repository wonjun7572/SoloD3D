#include "stdafx.h"
#include "..\Public\Tree.h"
#include "GameInstance.h"
#include "ProgressBarUI.h"
#include "ConversationUI.h"

CTree::CTree(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CGameObject(pDevice,pContext)
{
}

CTree::CTree(const CTree & rhs)
	:CGameObject(rhs)
{
}

HRESULT CTree::Init_Prototype()
{
	if (FAILED(__super::Init_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTree::Init(void * pArg)
{
	if (FAILED(__super::Init(pArg)))
		return E_FAIL;

	CProgressBarUI::PROGRESSBARDESC progressBarDesc;
	ZeroMemory(&progressBarDesc, sizeof(CProgressBarUI::PROGRESSBARDESC));

	progressBarDesc.fAmount = 1.f;
	progressBarDesc.fSizeX = 200.f;
	progressBarDesc.fSizeY = 40.f;
	progressBarDesc.fX = -670.f;
	progressBarDesc.fY = 30.f;
	progressBarDesc.iAlbedoTexNum = 3;
	progressBarDesc.iFillTexNum = 3;
	progressBarDesc.iBackTexNum = 4;
	progressBarDesc.iPassIndex = 1;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	m_pUI = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_ProgressBarUI"), &progressBarDesc);

	CConversationUI::CONVERSATIONFONT conversationDesc;

	wcscpy_s(conversationDesc.szConversation, MAX_PATH, L"");
	wcscpy_s(conversationDesc.szFontName, MAX_PATH, L"");
	conversationDesc.vColor = _float4(1.f, 1.f, 1.f, 1.f);

	conversationDesc.fX = 48.f;
	conversationDesc.fY = 371.f;
	conversationDesc.fSizeX = 0.5f;
	conversationDesc.fSizeY = 0.5f;

	m_pCountDown = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_ConversationUI"), &conversationDesc);

	RELEASE_INSTANCE(CGameInstance);

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, _float4(262.5f, 0.f ,252.f , 1.f));

	m_strObjName = L"Tree";

	return S_OK;
}

void CTree::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	m_fHp -=	static_cast<_float>(TimeDelta);
	UI_Tick(TimeDelta);
	m_pUI->Tick(TimeDelta);
	m_pCountDown->Tick(TimeDelta);
}

void CTree::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
	m_pUI->Late_Tick(TimeDelta);
	m_pCountDown->Late_Tick(TimeDelta);
}

HRESULT CTree::Render()
{
	return S_OK;
}

void CTree::UI_Tick(_double TimeDelta)
{
	m_strCountDown = L"남은 체력: " + to_wstring(static_cast<int>(m_fHp));
	static_cast<CConversationUI*>(m_pCountDown)->SetConversation(m_strCountDown);
	static_cast<CProgressBarUI*>(m_pUI)->Set_Amount((1.f + m_fHp / m_fMaxHp) - 1.f);
}

void CTree::Imgui_RenderProperty()
{
	m_pUI->Imgui_RenderProperty();
	m_pCountDown->Imgui_RenderProperty();
}

CTree * CTree::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTree*		pInstance = new CTree(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Created : CTree");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CTree::Clone(void * pArg)
{
	CTree*		pInstance = new CTree(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTree");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTree::Free()
{
	__super::Free();
	Safe_Release(m_pUI);
	Safe_Release(m_pCountDown);
}