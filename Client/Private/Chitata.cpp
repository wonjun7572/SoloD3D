#include "stdafx.h"
#include "..\Public\Chitata.h"
#include "GameInstance.h"
#include "Animation.h"
#include "FSMComponent.h"
#include "Player.h"
#include "ConversationUI.h"

CChitata::CChitata(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CAlly(pDevice, pContext)
{
}

CChitata::CChitata(const CChitata & rhs)
	: CAlly(rhs)
{
}

HRESULT CChitata::Init_Prototype()
{
	if (FAILED(__super::Init_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CChitata::Init(void * pArg)
{
	CGameObject::GAMEOBJECTDESC			GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof GameObjectDesc);

	GameObjectDesc.TransformDesc.fSpeedPerSec = 6.0f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Init(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	ALLYDESC allyDesc;
	ZeroMemory(&allyDesc, sizeof allyDesc);

	if (pArg != nullptr && g_LEVEL == LEVEL_CHAP2)
	{
		memcpy(&allyDesc, pArg, sizeof allyDesc);
		m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(allyDesc.fRadian));
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, allyDesc.vPos);
	}

	m_pNavigationCom->Set_CurreuntIndex(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

	m_strObjName = L"Chitata";

	SetUp_UI();
	m_vRimColor = _float4(0.1f, 0.1f, 1.f, 1.f);

	if (g_LEVEL == LEVEL_CHAP2)
	{
		m_CheckPoints.push_back(_float4(269.f, 0.f, 225.f, 1.f) + _float4(CMathUtils::GetRandomFloat(0.f, 20.f), 0.f, CMathUtils::GetRandomFloat(0.f, 20.f), 0.f));
		m_CheckPoints.push_back(_float4(299.f, 0.f, 213.f, 1.f));
		m_CheckPoints.push_back(_float4(295.f, 0.f, 200.f, 1.f));
		m_CheckPoints.push_back(_float4(300.f, 0.f, 163.f, 1.f));
		m_CheckPoints.push_back(_float4(299.f, 0.f, 142.f, 1.f));
		m_CheckPoints.push_back(_float4(307.f, 0.f, 95.f, 1.f));
	}

	return S_OK;
}

void CChitata::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	if (g_LEVEL == LEVEL_CHAP2)
		Level_Chap2Tick(TimeDelta);

	m_pModelCom->Play_Animation(TimeDelta);

	AdditiveAnim(TimeDelta);
}

void CChitata::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
	if (m_bConversation == true)
	{
		for (auto pUI : m_UI)
			pUI->Late_Tick(TimeDelta);
	}
}

HRESULT CChitata::Render()
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

		bool HasNormal = false;
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_NORMALS, "g_NormalTexture")))
			HasNormal = false;
		else
			HasNormal = true;

		m_pShaderCom->Set_RawValue("g_HasNormal", &HasNormal, sizeof(bool));

		bool HasSpecular = false;
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_SPECULAR, "g_SpecularTexture")))
			HasSpecular = false;
		else
			HasSpecular = true;

		m_pShaderCom->Set_RawValue("g_HasSpecular", &HasSpecular, sizeof(bool));

		m_pModelCom->Render(m_pShaderCom, i, 0, "g_BoneMatrices");
	}

	return S_OK;
}

void CChitata::Imgui_RenderProperty()
{
}

void CChitata::Level_Chap2Tick(_double TimeDelta)
{
	if (m_bSecondStageCheck && m_pFSM != nullptr)
	{
		m_pFSM->Tick(TimeDelta);
		return;
	}

	if (m_bConversation && _float3::Distance(m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION), m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION)) < 2.f)
		Conversation(TimeDelta);

	if (!m_bConversation)
	{
		_float4 vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

		if (m_CheckPoints.size() <= 1)
		{
			if (DistancePointCheck(vPos, m_CheckPoints[0]))
			{
				m_bSecondStageCheck = true;
			}
			else
			{
				m_pTransformCom->ChaseAndLookAt(m_CheckPoints[0], TimeDelta, 0.1f, m_pNavigationCom);
				m_pModelCom->Set_AnimationIndex(CHITATA_Run_F);
			}
		}
		else if (DistancePointCheck(vPos, m_CheckPoints.back()))
			m_CheckPoints.pop_back();
		else
		{
			m_pModelCom->Set_AnimationIndex(CHITATA_Run_F);
			m_pTransformCom->ChaseAndLookAt(m_CheckPoints.back(), TimeDelta, 0.1f, m_pNavigationCom);
		}
	}
	else
		m_pModelCom->Set_AnimationIndex(CHITATA_Idle_P_01);
}

void CChitata::Conversation(_double TimeDelta)
{
	if (g_LEVEL == LEVEL_CHAP2)
	{
		m_UI[UI_CONVERSATION]->Tick(TimeDelta);

		TimeConversation += TimeDelta;

		if (TimeConversation > 2.0f)
			m_strConversation = L"냥냥냥냥";
		else if (TimeConversation > 1.8f)
			m_strConversation = L"앞에 좀비냥냥";
		else if (TimeConversation > 1.6f)
			m_strConversation = L"앞에 좀냥냥";
		else if (TimeConversation > 1.4f)
			m_strConversation = L"앞에냥냥";
		else if (TimeConversation > 1.2f)
			m_strConversation = L"냥냥스";

		static_cast<CConversationUI*>(m_UI[UI_CONVERSATION])->SetConversation(m_strConversation);

		if (TimeConversation > 5.f)
		{
			static_cast<CPlayer*>(m_pPlayer)->Set_PlayerUI(true);
			m_bConversation = false;
		}
		else
			static_cast<CPlayer*>(m_pPlayer)->Set_PlayerUI(false);
	}
}

_bool CChitata::DistancePointCheck(_float4 vTargetPos, _float4 vPos)
{
	if (_float4::Distance(vPos, vTargetPos) < 2.f)
		return true;

	return false;
}

void CChitata::SetUp_FSM()
{
	m_pFSM = CFSMComponentBuilder()
		.InitState("Idle")
		.AddState("Idle")
		.Tick([this](_double TimeDelta)
	{
		m_pModelCom->Set_AnimationIndex(CHITATA_Idle_C);
	})
		.AddTransition("Idle to Chase", "Chase")
		.Predicator([this]()
	{
		return m_bChase;
	})

		.AddState("Chase")
		.Tick([this](_double TimeDelta)
	{
		m_pModelCom->Set_AnimationIndex(CHITATA_Run_F);
		m_pTransformCom->ChaseAndLookAt(m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION), TimeDelta, 5.f, m_pNavigationCom);
	})
		.AddTransition("Chase to Idle", "Idle")
		.Predicator([this]()
	{
		return !m_bChase;
	})

		.Build();
}

void CChitata::SetUp_UI()
{
	CConversationUI::CONVERSATIONFONT conversationDesc;

	wcscpy_s(conversationDesc.szConversation, MAX_PATH, L"");
	wcscpy_s(conversationDesc.szFontName, MAX_PATH, L"");
	conversationDesc.vColor = _float4(1.f, 1.f, 1.f, 1.f);

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

void CChitata::AdditiveAnim(_double TimeDelta)
{
	if (m_bFrontDamaged)
	{
		AdjustSetDamage();
		m_pModelCom->Set_AdditiveAnimIndex(CHITATA_ADD_DMG_F);
		m_pModelCom->Play_AddtivieAnim(TimeDelta, 1.f);
	}

	if (AnimFinishChecker(CHITATA_ADD_DMG_F))
	{
		m_bFrontDamaged = false;
		m_bImpossibleDamaged = false;
		m_pModelCom->Reset_AnimPlayTime(CHITATA_ADD_DMG_F);
	}

	///////////////////////////////////////

	if (m_bBackDamaged)
	{
		AdjustSetDamage();
		m_pModelCom->Set_AdditiveAnimIndex(CHITATA_ADD_DMG_B);
		m_pModelCom->Play_AddtivieAnim(TimeDelta, 1.f);
	}

	if (AnimFinishChecker(CHITATA_ADD_DMG_B))
	{
		m_bBackDamaged = false;
		m_bImpossibleDamaged = false;
		m_pModelCom->Reset_AnimPlayTime(CHITATA_ADD_DMG_B);
	}
}

HRESULT CChitata::SetUp_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_CHAP2, TEXT("Prototype_Component_Model_Chitata"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC			ColliderDesc;

	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vCenter = _float3(0.f, 0.7f, 0.f);
	ColliderDesc.vSize = _float3(0.5f, 0.7f, 0.5f);

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

HRESULT CChitata::SetUp_ShaderResources()
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
	if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;
	_float3 vCamPos = _float3(pGameInstance->Get_CamPosition().x, pGameInstance->Get_CamPosition().y, pGameInstance->Get_CamPosition().z);
	if (_float3::Distance(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION), vCamPos) > 30.f)
		m_vRimColor = _float4(0.f, 0.f, 0.f, 0.f);
	else
		m_vRimColor = _float4(0.1f, 0.1f, 1.f, 1.f);
	if (FAILED(m_pShaderCom->Set_RawValue("g_vRimColor", &m_vRimColor, sizeof(_float4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

_bool CChitata::AnimFinishChecker(ANIMATION eAnim, _double FinishRate)
{
	return m_pModelCom->Get_IndexAnim(eAnim)->Get_PlayRate() >= FinishRate;
}

_bool CChitata::AnimIntervalChecker(ANIMATION eAnim, _double StartRate, _double FinishRate)
{
	if (m_pModelCom->Get_IndexAnim(eAnim)->Get_PlayRate() > StartRate &&
		m_pModelCom->Get_IndexAnim(eAnim)->Get_PlayRate() <= FinishRate)
	{
		return true;
	}

	return false;
}

CChitata * CChitata::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CChitata*		pInstance = new CChitata(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Created : CChitata");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CChitata::Clone(void * pArg)
{
	CChitata*		pInstance = new CChitata(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Cloned : CChitata");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CChitata::Free()
{
	__super::Free();

	for (auto pUI : m_UI)
		Safe_Release(pUI);

	m_UI.clear();
}