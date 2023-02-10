#include "stdafx.h"
#include "..\Public\ArcaneE.h"
#include "GameInstance.h"
#include "ConversationUI.h"
#include "Player.h"

CArcaneE::CArcaneE(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CGameObject(pDevice, pContext)
{
}

CArcaneE::CArcaneE(const CArcaneE & rhs)
	: CGameObject(rhs)
{
}

HRESULT CArcaneE::Init_Prototype()
{
	if (FAILED(__super::Init_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CArcaneE::Init(void * pArg)
{
	CGameObject::GAMEOBJECTDESC GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	GameObjectDesc.TransformDesc.fSpeedPerSec = 3.0f;

	if (FAILED(__super::Init(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_strObjName = L"Effect_Rect_Arcane";

	DESC desc;

	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));
	if (pArg != nullptr)
	{
		memcpy(&desc, pArg, sizeof(DESC));
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, desc.vPos);
	}

	CConversationUI::CONVERSATIONFONT conversationDesc;

	wcscpy_s(conversationDesc.szConversation, MAX_PATH, L"");
	wcscpy_s(conversationDesc.szFontName, MAX_PATH, L"");
	conversationDesc.vColor = _float4(0.1f, 0.1f, 0.1f, 1.f);

	conversationDesc.fX = 375.f;
	conversationDesc.fY = 800.f;
	conversationDesc.fSizeX = 1.f;
	conversationDesc.fSizeY = 1.f;
	conversationDesc.bTextureOn = true;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	m_pConversationUI = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_ConversationUI"), &conversationDesc);
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CArcaneE::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	m_pTransformCom->Go_Up(TimeDelta);

	if (m_bConversation)
	{
		Conversation(TimeDelta);
		m_pConversationUI->Late_Tick(TimeDelta);
	}

	m_fFrame += 42.0f * static_cast<_float>(TimeDelta);

	if (m_fFrame >= 42.0f)
		m_fFrame = 0.f;
}

void CArcaneE::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	__super::Compute_CamDistance();

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CArcaneE::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(1);

	m_pVIBufferCom->Render();

	return S_OK;
}

void CArcaneE::Conversation(_double TimeDelta)
{
	m_pConversationUI->Tick(TimeDelta);

	if (TimeConversation < 2.f)
	{
		m_strConversation = L"저를 살려줘서 고마워요";
		TimeConversation += TimeDelta;
		if (!m_bFinished)
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
				pGameInstance->Play_Sound(L"FINISH.mp3", 1.f);
			RELEASE_INSTANCE(CGameInstance);
			m_bFinished = true;
		}
	}
	else if (TimeConversation >= 2.f && TimeConversation < 4.f)
	{
		m_strConversation = L"정말 수고 많으셨어요";
		TimeConversation += TimeDelta;
	}
	else if (TimeConversation >= 4.f && TimeConversation < 6.f)
	{
		m_strConversation = L"앞으로 모든 일이 잘되길 기도할께요";
		TimeConversation += TimeDelta;
	}

	static_cast<CConversationUI*>(m_pConversationUI)->SetConversation(m_strConversation);
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CPlayer* pPlayer =	static_cast<CPlayer*>(pGameInstance->Find_GameObject(g_LEVEL, L"Layer_Player", L"Player"));
	RELEASE_INSTANCE(CGameInstance);
	static_cast<CPlayer*>(pPlayer)->Set_PlayerUI(false);
}

HRESULT CArcaneE::SetUp_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Texture_Arcane"), TEXT("Com_Texture"),
		(CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CArcaneE::SetUp_ShaderResources()
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

	RELEASE_INSTANCE(CGameInstance);

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", (_uint)m_fFrame)))
		return E_FAIL;

	return S_OK;
}

CArcaneE * CArcaneE::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CArcaneE*		pInstance = new CArcaneE(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Created : CArcaneE");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CArcaneE::Clone(void * pArg)
{
	CArcaneE*		pInstance = new CArcaneE(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Cloned : CArcaneE");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CArcaneE::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pConversationUI);
}
