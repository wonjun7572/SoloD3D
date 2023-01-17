#include "stdafx.h"
#include "..\Public\BalianBollwerk.h"
#include "GameInstance.h"
#include "Animation.h"
#include "FSMComponent.h"
#include "Player.h"
#include "ConversationUI.h"

CBalianBollwerk::CBalianBollwerk(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CAlly(pDevice, pContext)
{
}

CBalianBollwerk::CBalianBollwerk(const CBalianBollwerk & rhs)
	: CAlly(rhs)
{
}

HRESULT CBalianBollwerk::Init_Prototype()
{
	if (FAILED(__super::Init_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBalianBollwerk::Init(void * pArg)
{
	CGameObject::GAMEOBJECTDESC			GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof GameObjectDesc);

	GameObjectDesc.TransformDesc.fSpeedPerSec = 5.0f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Init(&GameObjectDesc)))
		return E_FAIL;

	if(g_LEVEL == LEVEL_CHAP1)
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, _float4(30.f, 0.f, 33.f, 1.f));

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pNavigationCom->Set_CurreuntIndex(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

	m_strObjName = L"BalianBollwerk";

	SetUp_UI();

	if (g_LEVEL == LEVEL_CHAP1)
	{
		m_CheckPoints.push_back(_float4(105.f, 7.f, 76.f , 1.f));
		m_CheckPoints.push_back(_float4(105.f, 0.f, 11.f, 1.f));
		m_CheckPoints.push_back(_float4(41.f, 0.f, 14.f, 1.f));
		m_CheckPoints.push_back(_float4(36.f, 0.f, 18.f, 1.f));
		m_CheckPoints.push_back(_float4(33.f, 0.f, 24.f, 1.f));
		m_CheckPoints.push_back(_float4(33.f, 0.f, 33.f, 1.f));
	}

	return S_OK;
}

void CBalianBollwerk::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	// 만약에 플레이어와 상호 작용한 이후다? 그러면 ㄱ
	
	if(g_LEVEL == LEVEL_CHAP1)
		Level_Chap1Tick(TimeDelta);

	m_pModelCom->Play_Animation(TimeDelta);

	_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_float3 vPlayerPos = m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION);

	if (_float3::Distance(vPos, vPlayerPos) > 5.f)
		m_bChase = true;
	else
		m_bChase = false;

	AdditiveAnim(TimeDelta);
}

void CBalianBollwerk::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (m_bConversation == true)
	{
		for (auto pUI : m_UI)
			pUI->Late_Tick(TimeDelta);
	}
}

HRESULT CBalianBollwerk::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		/* 이 모델을 그리기위한 셰이더에 머테리얼 텍스쳐를 전달하낟. */
		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");

		bool HasSpecular = false;
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_SPECULAR, "g_SpecularTexture")))
			HasSpecular = false;
		else
			HasSpecular = true;

		m_pShaderCom->Set_RawValue("g_HasSpecular", &HasSpecular, sizeof(bool));
		m_pShaderCom->Set_RawValue("g_vRimColor", &_float4(0.1f, 0.1f, 1.f, 1.f), sizeof(_float4));

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4));
		RELEASE_INSTANCE(CGameInstance);
		m_pModelCom->Render(m_pShaderCom, i, 0, "g_BoneMatrices");
	}

	return S_OK;
}

void CBalianBollwerk::Level_Chap1Tick(_double TimeDelta)
{
	if (m_bConversation && _float3::Distance(m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION), m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION)) < 10.f)
		Conversation(TimeDelta);

	if (m_pPlayer->Get_Conversation() == true)
		m_bConversation = true;
	// UI 뜨고
	//m_pFSM->Tick(TimeDelta);

	if (!m_bConversation)
	{
		_float4 vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	
		if (m_CheckPoints.size() <= 1)
		{
			if (DistancePointCheck(vPos, m_CheckPoints[0]))
			{
				m_pModelCom->Set_AnimationIndex(BALIANBOLLWERK_Idle_C);
				m_pTransformCom->LookAt(m_CheckPoints[0]);
			}
			else
			{
				m_pTransformCom->ChaseAndLookAt(m_CheckPoints[0], TimeDelta, 0.1f, m_pNavigationCom);
				m_pModelCom->Set_AnimationIndex(BALIANBOLLWERK_Run_F);
			}
		}
		else if (DistancePointCheck(vPos, m_CheckPoints.back()))
			m_CheckPoints.pop_back();
		else
		{
			m_pModelCom->Set_AnimationIndex(BALIANBOLLWERK_Run_F);
			m_pTransformCom->ChaseAndLookAt(m_CheckPoints.back(), TimeDelta, 0.1f, m_pNavigationCom);
		}
	}	
	else
		m_pModelCom->Set_AnimationIndex(BALIANBOLLWERK_Idle_P_01);
}

void CBalianBollwerk::Imgui_RenderProperty()
{
	if (ImGui::Button("Navi~"))
	{
		m_pNavigationCom->Set_CurreuntIndex(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	}
}

void CBalianBollwerk::Conversation(_double TimeDelta)
{
	m_UI[UI_CONVERSATION]->Tick(TimeDelta);

	TimeConversation += TimeDelta;
	if(TimeConversation > 6.8f)
		static_cast<CConversationUI*>(m_UI[UI_CONVERSATION])->SetConversation(L"Hello! My Name is BallanBollwerk");
	else if(TimeConversation > 6.4f)
		static_cast<CConversationUI*>(m_UI[UI_CONVERSATION])->SetConversation(L"Hello! My Name is BallanBollwer");
	else if (TimeConversation > 6.2f)
		static_cast<CConversationUI*>(m_UI[UI_CONVERSATION])->SetConversation(L"Hello! My Name is BallanBollwe");
	else if (TimeConversation > 6.0f)
		static_cast<CConversationUI*>(m_UI[UI_CONVERSATION])->SetConversation(L"Hello! My Name is BallanBollw");
	else if (TimeConversation > 5.8f)
		static_cast<CConversationUI*>(m_UI[UI_CONVERSATION])->SetConversation(L"Hello! My Name is BallanBoll");
	else if (TimeConversation > 5.6f)
		static_cast<CConversationUI*>(m_UI[UI_CONVERSATION])->SetConversation(L"Hello! My Name is BallanBol");
	else if (TimeConversation > 5.4f)
		static_cast<CConversationUI*>(m_UI[UI_CONVERSATION])->SetConversation(L"Hello! My Name is BallanBo");
	else if (TimeConversation > 5.2f)
		static_cast<CConversationUI*>(m_UI[UI_CONVERSATION])->SetConversation(L"Hello! My Name is BallanB");
	else if (TimeConversation > 5.0f)
		static_cast<CConversationUI*>(m_UI[UI_CONVERSATION])->SetConversation(L"Hello! My Name is Ballan");
	else if (TimeConversation > 4.8f)
		static_cast<CConversationUI*>(m_UI[UI_CONVERSATION])->SetConversation(L"Hello! My Name is Balla");
	else if (TimeConversation > 4.6f)
		static_cast<CConversationUI*>(m_UI[UI_CONVERSATION])->SetConversation(L"Hello! My Name is Ball");
	else if (TimeConversation > 4.4f)
		static_cast<CConversationUI*>(m_UI[UI_CONVERSATION])->SetConversation(L"Hello! My Name is Bal");
	else if (TimeConversation > 4.2f)
		static_cast<CConversationUI*>(m_UI[UI_CONVERSATION])->SetConversation(L"Hello! My Name is Ba");
	else if (TimeConversation > 4.0f)
		static_cast<CConversationUI*>(m_UI[UI_CONVERSATION])->SetConversation(L"Hello! My Name is B");
	else if (TimeConversation > 3.8f)
		static_cast<CConversationUI*>(m_UI[UI_CONVERSATION])->SetConversation(L"Hello! My Name is ");
	else if (TimeConversation > 3.6f)
		static_cast<CConversationUI*>(m_UI[UI_CONVERSATION])->SetConversation(L"Hello! My Name i");
	else if (TimeConversation > 3.4f)
		static_cast<CConversationUI*>(m_UI[UI_CONVERSATION])->SetConversation(L"Hello! My Name");
	else if (TimeConversation > 3.2f)
		static_cast<CConversationUI*>(m_UI[UI_CONVERSATION])->SetConversation(L"Hello! My Nam");
	else if (TimeConversation > 3.0f)
		static_cast<CConversationUI*>(m_UI[UI_CONVERSATION])->SetConversation(L"Hello! My Na");
	else if (TimeConversation > 2.8f)
		static_cast<CConversationUI*>(m_UI[UI_CONVERSATION])->SetConversation(L"Hello! My N");
	else if (TimeConversation > 2.6f)
		static_cast<CConversationUI*>(m_UI[UI_CONVERSATION])->SetConversation(L"Hello! My");
	else if (TimeConversation > 2.4f)
		static_cast<CConversationUI*>(m_UI[UI_CONVERSATION])->SetConversation(L"Hello! M");
	else if (TimeConversation > 2.2f)
		static_cast<CConversationUI*>(m_UI[UI_CONVERSATION])->SetConversation(L"Hello!");
	else if (TimeConversation > 2.0f)
		static_cast<CConversationUI*>(m_UI[UI_CONVERSATION])->SetConversation(L"Hello");
	else if (TimeConversation > 1.8f)
		static_cast<CConversationUI*>(m_UI[UI_CONVERSATION])->SetConversation(L"Hell");
	else if (TimeConversation > 1.6f)
		static_cast<CConversationUI*>(m_UI[UI_CONVERSATION])->SetConversation(L"Hel");
	else if (TimeConversation > 1.4f)
		static_cast<CConversationUI*>(m_UI[UI_CONVERSATION])->SetConversation(L"He");
	else if (TimeConversation > 1.2f)
		static_cast<CConversationUI*>(m_UI[UI_CONVERSATION])->SetConversation(L"H");

	if (TimeConversation > 8.f)
		m_bConversation = false;
}

_bool CBalianBollwerk::DistancePointCheck(_float4 vTargetPos, _float4 vPos)
{
	if (_float4::Distance(vPos, vTargetPos) < 2.f)
		return true;

	return false;
}

void CBalianBollwerk::SetUp_FSM()
{
	m_pFSM = CFSMComponentBuilder()
		.InitState("Idle")
		.AddState("Idle")
		.Tick([this](_double TimeDelta)
	{
		m_pModelCom->Set_AnimationIndex(BALIANBOLLWERK_Idle_C);
	})
		.AddTransition("Idle to Chase", "Chase")
		.Predicator([this]()
	{
		return m_bChase;
	})

		.AddState("Chase")
		.Tick([this](_double TimeDelta)
	{
		m_pModelCom->Set_AnimationIndex(BALIANBOLLWERK_Run_F);
		m_pTransformCom->ChaseAndLookAt(m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION), TimeDelta, 5.f, m_pNavigationCom);
	})
		.AddTransition("Chase to Idle", "Idle")
		.Predicator([this]()
	{
		return !m_bChase;
	})

		.Build();
}

void CBalianBollwerk::SetUp_UI()
{
	CConversationUI::CONVERSATIONFONT conversationDesc;

	wcscpy_s(conversationDesc.szConversation, MAX_PATH, L"");
	wcscpy_s(conversationDesc.szFontName, MAX_PATH, L"");
	conversationDesc.vColor = _float4(0.3f, 1.f, 0.f, 1.f);

	conversationDesc.fX = 475.f;
	conversationDesc.fY = 610.f;
	conversationDesc.fSizeX = 1.f;
	conversationDesc.fSizeY = 1.f;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CGameObject* pUI = nullptr;
	pUI = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_ConversationUI"), &conversationDesc);

	if (nullptr != pUI)
		m_UI.push_back(pUI);

	RELEASE_INSTANCE(CGameInstance);
}

void CBalianBollwerk::AdditiveAnim(_double TimeDelta)
{
	if (m_bFrontDamaged)
	{
		AdjustSetDamage();
		m_pModelCom->Set_AdditiveAnimIndex(BALIANBOLLWERK_ADD_DMG_F);
		m_pModelCom->Play_AddtivieAnim(TimeDelta, 1.f);
	}

	if (AnimFinishChecker(BALIANBOLLWERK_ADD_DMG_F))
	{
		m_bFrontDamaged = false;
		m_bImpossibleDamaged = false;
		m_pModelCom->Reset_AnimPlayTime(BALIANBOLLWERK_ADD_DMG_F);
	}

	///////////////////////////////////////

	if (m_bBackDamaged)
	{
		AdjustSetDamage();
		m_pModelCom->Set_AdditiveAnimIndex(BALIANBOLLWERK_ADD_DMG_B);
		m_pModelCom->Play_AddtivieAnim(TimeDelta, 1.f);
	}

	if (AnimFinishChecker(BALIANBOLLWERK_ADD_DMG_B))
	{
		m_bBackDamaged = false;
		m_bImpossibleDamaged = false;
		m_pModelCom->Reset_AnimPlayTime(BALIANBOLLWERK_ADD_DMG_B);
	}
}

HRESULT CBalianBollwerk::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxAnimModel"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Model_BalianBollwerk"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC			ColliderDesc;

	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vCenter = _float3(0.f, 1.2f, 0.f);
	ColliderDesc.vSize = _float3(0.5f, 1.2f, 0.5f);

	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_AABB"),
		(CComponent**)&m_pColliderCom[COLLTYPE_AABB], &ColliderDesc)))
		return E_FAIL;

	/* For.Com_SPHERE */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vSize = _float3(0.5f, 0.5f, 0.5f);

	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Collider_SPHERE"), TEXT("Com_SPHERE"),
		(CComponent**)&m_pColliderCom[COLLTYPE_SPHERE], &ColliderDesc)))
		return E_FAIL;

	if (g_LEVEL == LEVEL_CHAP1)
	{
		/* For.Com_Navigation */
		CNavigation::NAVIDESC			NaviDesc;
		ZeroMemory(&NaviDesc, sizeof(CNavigation::NAVIDESC));

		NaviDesc.iCurrentIndex = 0;

		if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"),
			(CComponent**)&m_pNavigationCom, nullptr)))
			return E_FAIL;
	}
	else if (g_LEVEL == LEVEL_CHAP2)
	{
		/* For.Com_Navigation */
		CNavigation::NAVIDESC			NaviDesc;
		ZeroMemory(&NaviDesc, sizeof(CNavigation::NAVIDESC));

		NaviDesc.iCurrentIndex = 0;

		if (FAILED(__super::Add_Component(LEVEL_CHAP2, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"),
			(CComponent**)&m_pNavigationCom, &NaviDesc)))
			return E_FAIL;
	}
	else if (g_LEVEL == LEVEL_CHAP3)
	{
		/* For.Com_Navigation */
		CNavigation::NAVIDESC			NaviDesc;
		ZeroMemory(&NaviDesc, sizeof(CNavigation::NAVIDESC));

		NaviDesc.iCurrentIndex = 0;

		if (FAILED(__super::Add_Component(LEVEL_CHAP3, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"),
			(CComponent**)&m_pNavigationCom, &NaviDesc)))
			return E_FAIL;
	}
	return S_OK;
}

HRESULT CBalianBollwerk::SetUp_ShaderResources()
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

	return S_OK;
}

_bool CBalianBollwerk::AnimFinishChecker(ANIMATION eAnim, _double FinishRate)
{
	return m_pModelCom->Get_IndexAnim(eAnim)->Get_PlayRate() >= FinishRate;
}

_bool CBalianBollwerk::AnimIntervalChecker(ANIMATION eAnim, _double StartRate, _double FinishRate)
{
	if (m_pModelCom->Get_IndexAnim(eAnim)->Get_PlayRate() > StartRate &&
		m_pModelCom->Get_IndexAnim(eAnim)->Get_PlayRate() <= FinishRate)
	{
		return true;
	}

	return false;
}

CBalianBollwerk * CBalianBollwerk::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBalianBollwerk*		pInstance = new CBalianBollwerk(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Created : CBalianBollwerk");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CBalianBollwerk::Clone(void * pArg)
{
	CBalianBollwerk*		pInstance = new CBalianBollwerk(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBalianBollwerk");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBalianBollwerk::Free()
{
	__super::Free();

	for (auto pUI : m_UI)
		Safe_Release(pUI);

	m_UI.clear();
}