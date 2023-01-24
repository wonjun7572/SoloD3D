#include "stdafx.h"
#include "..\Public\Delilah.h"
#include "GameInstance.h"
#include "Animation.h"
#include "FSMComponent.h"
#include "Player.h"
#include "ConversationUI.h"


CDelilah::CDelilah(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CAlly(pDevice, pContext)
{
}

CDelilah::CDelilah(const CDelilah & rhs)
	: CAlly(rhs)
{
}

HRESULT CDelilah::Init_Prototype()
{
	if (FAILED(__super::Init_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDelilah::Init(void * pArg)
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

	m_strObjName = L"Delilah";

	SetUp_UI();
	m_vRimColor = _float4(0.1f, 0.1f, 1.f, 1.f);

	if (g_LEVEL == LEVEL_CHAP2)
	{
		m_CheckPoints.push_back(_float4(274.f, 0.f, 230.f, 1.f) + _float4(CMathUtils::GetRandomFloat(0.f, 20.f), 0.f, CMathUtils::GetRandomFloat(0.f, 20.f), 0.f));
		m_CheckPoints.push_back(_float4(299.f, 0.f, 213.f, 1.f));
		m_CheckPoints.push_back(_float4(295.f, 0.f, 200.f, 1.f));
		m_CheckPoints.push_back(_float4(300.f, 0.f, 163.f, 1.f));
		m_CheckPoints.push_back(_float4(299.f, 0.f, 142.f, 1.f));
		m_CheckPoints.push_back(_float4(307.f, 0.f, 95.f, 1.f));

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		m_pSkeleton = pGameInstance->Find_GameObject(LEVEL_CHAP2, L"Layer_Monster", L"SkeletonWarrior_3");
		RELEASE_INSTANCE(CGameInstance);
	}

	return S_OK;
}

void CDelilah::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	if (g_LEVEL == LEVEL_CHAP2)
		Level_Chap2Tick(TimeDelta);

	m_pModelCom->Play_Animation(TimeDelta);

	AdditiveAnim(TimeDelta);
}

void CDelilah::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (m_bConversation == true)
	{
		for (auto pUI : m_UI)
			pUI->Late_Tick(TimeDelta);
	}
}

HRESULT CDelilah::Render()
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

void CDelilah::Imgui_RenderProperty()
{
}

void CDelilah::Level_Chap2Tick(_double TimeDelta)
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
				m_pModelCom->Set_AnimationIndex(DELILAH_Run_F);
			}
		}
		else if (DistancePointCheck(vPos, m_CheckPoints.back()))
			m_CheckPoints.pop_back();
		else
		{
			m_pModelCom->Set_AnimationIndex(DELILAH_Run_F);
			m_pTransformCom->ChaseAndLookAt(m_CheckPoints.back(), TimeDelta, 0.1f, m_pNavigationCom);
		}
	}
	else
		m_pModelCom->Set_AnimationIndex(DELILAH_Idle_P_01);
}

void CDelilah::Conversation(_double TimeDelta)
{
	if (g_LEVEL == LEVEL_CHAP2)
	{
		m_UI[UI_CONVERSATION]->Tick(TimeDelta);

		TimeConversation += TimeDelta;

		if (TimeConversation > 2.0f)
			m_strConversation = L"그..래 내 도와줄께 \n 알아서 도망쳐라이~ 난 간다잉~";
		else if (TimeConversation > 1.8f)
			m_strConversation = L"그..래 내가";
		else if (TimeConversation > 1.6f)
			m_strConversation = L"그..래 내";
		else if (TimeConversation > 1.4f)
			m_strConversation = L"그..래";
		else if (TimeConversation > 1.2f)
			m_strConversation = L"그..";

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

_bool CDelilah::DistancePointCheck(_float4 vTargetPos, _float4 vPos)
{
	if (_float4::Distance(vPos, vTargetPos) < 2.f)
		return true;

	return false;
}

void CDelilah::SetUp_FSM()
{
	m_pFSM = CFSMComponentBuilder()
		.InitState("Idle")
		.AddState("Idle")
		.Tick([this](_double TimeDelta)
	{
		m_pModelCom->Set_AnimationIndex(DELILAH_Idle_C);
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
				m_pModelCom->Set_AnimationIndex(DELILAH_Run_F);
			}
			else
			{
				m_pTransformCom->LookAt(m_pSkeleton->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION));
				m_pModelCom->Set_AnimationIndex(DELILAH_Idle_C);
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
		m_iRandAttack = rand() % 5;
		
		if (m_iRandAttack == 0)
		{
			m_pModelCom->Reset_AnimPlayTime(DELILAH_ATK_01);
			m_pModelCom->Set_AnimationIndex(DELILAH_ATK_01);
		}
		else if (m_iRandAttack == 1)
		{
			m_pModelCom->Reset_AnimPlayTime(DELILAH_ATK_02);
			m_pModelCom->Set_AnimationIndex(DELILAH_ATK_02);
		}
		else if (m_iRandAttack == 2)
		{
			m_pModelCom->Reset_AnimPlayTime(DELILAH_SK_Firing_01);
			m_pModelCom->Set_AnimationIndex(DELILAH_SK_Firing_01);
		}
		else if (m_iRandAttack == 3)
		{
			m_pModelCom->Reset_AnimPlayTime(DELILAH_SK_Firing_02);
			m_pModelCom->Set_AnimationIndex(DELILAH_SK_Firing_02);
		}
		else if (m_iRandAttack == 4)
		{
			m_pModelCom->Reset_AnimPlayTime(DELILAH_SK_Firing_03);
			m_pModelCom->Set_AnimationIndex(DELILAH_SK_Firing_03);
		}
	})
		.OnExit([this]()
	{
		m_AttackDelayTime = 0.0;
	})
		.AddTransition("Attack to Idle", "Idle")
		.Predicator([this]()
	{
		return AnimFinishChecker(DELILAH_SK_Firing_01) ||
			AnimFinishChecker(DELILAH_SK_Firing_02) || 
			AnimFinishChecker(DELILAH_SK_Firing_03) || 
			AnimFinishChecker(DELILAH_ATK_01) ||
			AnimFinishChecker(DELILAH_ATK_02);
	})

		.Build();
}

void CDelilah::SetUp_UI()
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

void CDelilah::AdditiveAnim(_double TimeDelta)
{
	if (m_bFrontDamaged)
	{
		AdjustSetDamage();
		m_pModelCom->Set_AdditiveAnimIndex(DELILAH_ADD_DMG_F);
		m_pModelCom->Play_AddtivieAnim(TimeDelta, 1.f);
	}

	if (AnimFinishChecker(DELILAH_ADD_DMG_F))
	{
		m_bFrontDamaged = false;
		m_bImpossibleDamaged = false;
		m_pModelCom->Reset_AnimPlayTime(DELILAH_ADD_DMG_F);
	}

	///////////////////////////////////////

	if (m_bBackDamaged)
	{
		AdjustSetDamage();
		m_pModelCom->Set_AdditiveAnimIndex(DELILAH_ADD_DMG_B);
		m_pModelCom->Play_AddtivieAnim(TimeDelta, 1.f);
	}

	if (AnimFinishChecker(DELILAH_ADD_DMG_B))
	{
		m_bBackDamaged = false;
		m_bImpossibleDamaged = false;
		m_pModelCom->Reset_AnimPlayTime(DELILAH_ADD_DMG_B);
	}
}

HRESULT CDelilah::SetUp_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_CHAP2, TEXT("Prototype_Component_Model_Delilah"), TEXT("Com_Model"),
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

HRESULT CDelilah::SetUp_ShaderResources()
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

_bool CDelilah::AnimFinishChecker(ANIMATION eAnim, _double FinishRate)
{
	return m_pModelCom->Get_IndexAnim(eAnim)->Get_PlayRate() >= FinishRate;
}

_bool CDelilah::AnimIntervalChecker(ANIMATION eAnim, _double StartRate, _double FinishRate)
{
	if (m_pModelCom->Get_IndexAnim(eAnim)->Get_PlayRate() > StartRate &&
		m_pModelCom->Get_IndexAnim(eAnim)->Get_PlayRate() <= FinishRate)
	{
		return true;
	}

	return false;
}

CDelilah * CDelilah::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CDelilah*		pInstance = new CDelilah(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Created : CDelilah");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CDelilah::Clone(void * pArg)
{
	CDelilah*		pInstance = new CDelilah(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDelilah");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDelilah::Free()
{
	__super::Free();

	for (auto pUI : m_UI)
		Safe_Release(pUI);

	m_UI.clear();
}