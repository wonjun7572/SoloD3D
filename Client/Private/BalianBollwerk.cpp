#include "stdafx.h"
#include "..\Public\BalianBollwerk.h"
#include "GameInstance.h"
#include "Animation.h"
#include "FSMComponent.h"
#include "Player.h"
#include "ConversationUI.h"
#include "GameUtils.h"

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

	ALLYDESC allyDesc;
	ZeroMemory(&allyDesc, sizeof allyDesc);


	if(g_LEVEL == LEVEL_CHAP1)
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, _float4(30.f, 0.f, 33.f, 1.f));
	else if (pArg != nullptr && g_LEVEL == LEVEL_CHAP2)
	{
		memcpy(&allyDesc, pArg, sizeof allyDesc);
		m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(allyDesc.fRadian));
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, allyDesc.vPos);
	}

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pNavigationCom->Set_CurreuntIndex(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

	m_strObjName = L"BalianBollwerk";

	SetUp_UI();
	m_vRimColor = _float4(0.1f, 0.1f, 1.f, 1.f);
	
	if (g_LEVEL == LEVEL_CHAP1)
	{
		m_CheckPoints.push_back(_float4(105.f, 7.f, 76.f , 1.f));
		m_CheckPoints.push_back(_float4(105.f, 0.f, 11.f, 1.f));
		m_CheckPoints.push_back(_float4(41.f, 0.f, 14.f, 1.f));
		m_CheckPoints.push_back(_float4(36.f, 0.f, 18.f, 1.f));
		m_CheckPoints.push_back(_float4(33.f, 0.f, 24.f, 1.f));
		m_CheckPoints.push_back(_float4(33.f, 0.f, 33.f, 1.f));
	}
	else if (g_LEVEL == LEVEL_CHAP2)
	{
		m_CheckPoints.push_back(_float4(279.f, 0.f, 235.f, 1.f));
		m_CheckPoints.push_back(_float4(299.f, 0.f, 213.f, 1.f));
		m_CheckPoints.push_back(_float4(295.f, 0.f, 200.f, 1.f));
		m_CheckPoints.push_back(_float4(300.f, 0.f, 163.f, 1.f));
		m_CheckPoints.push_back(_float4(299.f, 0.f, 142.f, 1.f));
		m_CheckPoints.push_back(_float4(307.f, 0.f, 95.f, 1.f));	
	
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		m_pSkeleton = pGameInstance->Find_GameObject(LEVEL_CHAP2, L"Layer_Monster", L"SkeletonWarrior_0");
		RELEASE_INSTANCE(CGameInstance);
	}

	TimeConversation = 0.0;
	return S_OK;
}

void CBalianBollwerk::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	// 만약에 플레이어와 상호 작용한 이후다? 그러면 ㄱ
	
	if (g_LEVEL == LEVEL_CHAP1)
		Level_Chap1Tick(TimeDelta);
	else if (g_LEVEL == LEVEL_CHAP2)
		Level_Chap2Tick(TimeDelta);

	m_pModelCom->Play_Animation(TimeDelta);
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

void CBalianBollwerk::Level_Chap1Tick(_double TimeDelta)
{
	if (m_bConversation && _float3::Distance(m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION), m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION)) < 10.f)
		Conversation(TimeDelta);

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

void CBalianBollwerk::Level_Chap2Tick(_double TimeDelta)
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
				m_bMonsterChase = true;
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
	if (g_LEVEL == LEVEL_CHAP1)
	{
		m_UI[UI_CONVERSATION]->Tick(TimeDelta);

		TimeConversation += TimeDelta;

		if (TimeConversation > 2.0f)
			m_strConversation = L"안녕하세요";
		else if (TimeConversation > 1.8f)
			m_strConversation = L"안녕하세";
		else if (TimeConversation > 1.6f)
			m_strConversation = L"안녕하";
		else if (TimeConversation > 1.4f)
			m_strConversation = L"안녕";
		else if (TimeConversation > 1.2f)
			m_strConversation = L"안";

		static_cast<CConversationUI*>(m_UI[UI_CONVERSATION])->SetConversation(m_strConversation);

		if (TimeConversation > 5.f)
		{
			static_cast<CPlayer*>(m_pPlayer)->Set_PlayerUI(true);
			m_bConversation = false;
		}
		else
			static_cast<CPlayer*>(m_pPlayer)->Set_PlayerUI(false);
	}
	else if (g_LEVEL == LEVEL_CHAP2)
	{
		m_UI[UI_CONVERSATION]->Tick(TimeDelta);

		TimeConversation += TimeDelta;

		if (TimeConversation > 2.0f)
			m_strConversation = L"알아서 도망쳐라이~";
		else if (TimeConversation > 1.8f)
			m_strConversation = L"앞에 좀비";
		else if (TimeConversation > 1.6f)
			m_strConversation = L"앞에 좀";
		else if (TimeConversation > 1.4f)
			m_strConversation = L"앞에";
		else if (TimeConversation > 1.2f)
			m_strConversation = L"앞";

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
		.AddTransition("Idel to Monster_Chase", "Monster_Chase")
		.Predicator([this]()
	{
		return m_bMonsterChase;
	})

		.AddState("Monster_Chase")
		.Tick([this](_double TimeDelta)
	{
		if (m_pSkeleton != nullptr)
		{
			if (_float3::Distance(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION), m_pSkeleton->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION)) > 2.f)
			{
				m_pTransformCom->ChaseAndLookAt(m_pSkeleton->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION), TimeDelta, 0.1f, m_pNavigationCom);
				m_pModelCom->Set_AnimationIndex(BALIANBOLLWERK_Run_F);
			}
			else
			{
				m_pTransformCom->LookAt(m_pSkeleton->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION));
				m_pModelCom->Set_AnimationIndex(BALIANBOLLWERK_Idle_C);
				m_AttackDelayTime += TimeDelta;
			}
		}
		else
		{
			m_bMonsterChase = false;
		}
	})
		.AddTransition("Monster_Chase to Idle", "Idle")
		.Predicator([this]()
	{
		return !m_bMonsterChase;
	})
		.AddTransition("Monster_Chase to Attack", "Attack")
		.Predicator([this]()
	{
		return m_AttackDelayTime > 0.2;
	})

		.AddState("Attack")
		.OnStart([this]()
	{
		m_iRandAttack = rand() % 2;

		if (m_iRandAttack == 0)
		{
			m_pModelCom->Reset_AnimPlayTime(BALIANBOLLWERK_ATK_01);
			m_pModelCom->Set_AnimationIndex(BALIANBOLLWERK_ATK_01);
		}
		else if (m_iRandAttack == 1)
		{
			m_pModelCom->Reset_AnimPlayTime(BALIANBOLLWERK_ATK_02);
			m_pModelCom->Set_AnimationIndex(BALIANBOLLWERK_ATK_02);
		}
	})
		.OnExit([this]()
	{
		m_AttackDelayTime = 0.0;
	})
		.AddTransition("Attack to Idle", "Idle")
		.Predicator([this]()
	{
		return 	AnimFinishChecker(BALIANBOLLWERK_ATK_01) ||
			AnimFinishChecker(BALIANBOLLWERK_ATK_02);
	})

		.Build();
}

void CBalianBollwerk::SetUp_UI()
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
	
	if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	if (Get_CamDistance() > 30.f)
		m_vRimColor = _float4(0.f, 0.f, 0.f, 0.f);
	else
		m_vRimColor = _float4(0.1f, 0.1f, 1.0f, 1.f);

	if (FAILED(m_pShaderCom->Set_RawValue("g_vRimColor", &m_vRimColor, sizeof(_float4))))
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