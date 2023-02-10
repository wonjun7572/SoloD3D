#include "stdafx.h"
#include "..\Public\Flogas.h"
#include "GameInstance.h"
#include "FSMComponent.h"
#include "Animation.h"
#include "ConversationUI.h"
#include "Meteor.h"
#include "PlayerCamera.h"
#include "RockNorm04.h"
#include "RockMada.h"
#include "ArcaneE.h"

CFlogas::CFlogas(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CMonster(pDevice, pContext)
{
}

CFlogas::CFlogas(const CFlogas & rhs)
	:CMonster(rhs)
{
}

HRESULT CFlogas::Init_Prototype()
{
	if (FAILED(__super::Init_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CFlogas::Init(void * pArg)
{
	CGameObject::GAMEOBJECTDESC			GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof GameObjectDesc);

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.0f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Init(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_strObjName = L"FLOGAS";

	m_fHp = 500.f;
	m_fMaxHp = 500.f;
	m_fAttack = 30;
	m_fDefence = 5;

	if (g_LEVEL == LEVEL_CHAP1)
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, _float4(35.f, 0.f, 60.f, 1.f));

	if (g_LEVEL == LEVEL_CHAP3)
	{
		m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, _float4(65.f, 25.f, 90.f, 1.f));
	}

	m_pNavigationCom->Set_CurreuntIndex(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	m_vMonsterNamePos = _float2(720.f, 40.f);
	m_vMonsterNameScale = _float2(1.f, 1.f);

	SetUp_UI();

	if (FAILED(__super::SetUP_UI()))
		return E_FAIL;

	m_vRimColor = _float4(0.3f, 0.1f, 0.1f, 1.f);

	return S_OK;
}

void CFlogas::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	AdditiveAnim(TimeDelta);
	Play_Skill(TimeDelta);
	
	if (!m_bDeadAnim)
	{
		m_pTransformCom->LookAt(m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION));
	}
		
	if (m_bConversation)
	{
		Conversation(TimeDelta);
		m_UI[UI_CONVERSATION]->Late_Tick(TimeDelta);
	}
}

void CFlogas::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance)

	if (nullptr != m_pRendererCom	)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOW, this);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}

	RELEASE_INSTANCE(CGameInstance)

	if (m_bDeadAnim)
		return;

	Adjust_Collision(TimeDelta);
}

HRESULT CFlogas::Render()
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

		if (FAILED(m_pDissolveTexCom->Bind_ShaderResource(m_pShaderCom, "g_DissolveTexture")))
			return E_FAIL;

		m_pModelCom->Render(m_pShaderCom, i, 0, "g_BoneMatrices");
	}

	return S_OK;
}

HRESULT CFlogas::RenderShadow()
{
	if (FAILED(__super::RenderShadow()))
		return E_FAIL;

	if (FAILED(SetUP_ShadowShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
		m_pModelCom->Render(m_pShaderCom, i, 1, "g_BoneMatrices");

	return S_OK;
}

void CFlogas::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();

	if (ImGui::Button("Navi~"))
	{
		m_pNavigationCom->Set_CurreuntIndex(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	}

	ImGui::DragFloat("Dissolve", &m_fDissolveAmount, 0.01f, -10.f, 10.f);
	ImGui::DragFloat("Finge", &m_fFringeAmount, 0.01f, -10.f, 10.f);

	if (ImGui::Button("Skill1"))
		m_bSkill_1ToPlayer = true;

	if (ImGui::Button("Skill2"))
		m_bSkill_2ToPlayer = true;

	if (ImGui::Button("Skill5"))
		m_bSkill_5ToPlayer = true;
}

void CFlogas::Conversation(_double TimeDelta)
{
	m_UI[UI_CONVERSATION]->Tick(TimeDelta);

	if (TimeConversation < 3.f)
	{
		m_strConversation = L"너가 내 동생을 죽였느냐..";
		TimeConversation += TimeDelta;
	}
	else if (TimeConversation >= 3.f && TimeConversation < 6.f)
	{
		m_strConversation = L"한번 해보자는거구나";
		TimeConversation += TimeDelta;
	}
	else if (TimeConversation >= 6.f && TimeConversation < 9.f)
	{
		m_strConversation = L"드간다!!";
		TimeConversation += TimeDelta;
	}

	static_cast<CConversationUI*>(m_UI[UI_CONVERSATION])->SetConversation(m_strConversation);

	if (TimeConversation > 9.f)
	{
		m_bConversation = false;
		m_bSpawnToIdle = true;
		static_cast<CPlayer*>(m_pPlayer)->Set_PlayerUI(true);
	}
	else
		static_cast<CPlayer*>(m_pPlayer)->Set_PlayerUI(false);
}

void CFlogas::SetUp_FSM()
{
	m_pFSM = CFSMComponentBuilder()
		.InitState("Spawn")
		.AddState("Spawn")
		.Tick([this](_double TimeDelta)
	{
		if(!m_bConversation)
			m_bConversation = true;

		m_pModelCom->Set_AnimationIndex(FLOGAS_Idle_C);
	})
		.OnExit([this]()
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance)
			pGameInstance->Play_Sound(L"Hanuman_Breath_Voice.wav", 1.f);
		RELEASE_INSTANCE(CGameInstance)
	})
		.AddTransition("Spawn to Idle" , "Idle")
		.Predicator([this]() 
	{
		return !m_bConversation && m_bSpawnToIdle;
	})
		.AddState("Idle")
	
		.Tick([this](_double TimeDelta) 
	{
		m_pModelCom->Set_AnimationIndex(FLOGAS_Idle_C);
	})
		.AddTransition("Idle to Dead", "Dead")
		.Predicator([this]()
	{
		return m_bDeadAnim;
	})
		.AddTransition("Idle to Skill_1", "Skill_1")
		.Predicator([this]()
	{
		return m_bSkill_1ToPlayer;
	})
		.AddTransition("Idle to Skill_2", "Skill_2")
		.Predicator([this]()
	{
		return m_bSkill_2ToPlayer;
	})
		.AddTransition("Idle to Skill_5", "Skill_5")
		.Predicator([this]()
	{
		return m_bSkill_5ToPlayer;
	})
	
		.AddState("Skill_1")
		.OnStart([this]()
	{
		m_pModelCom->Reset_AnimPlayTime(FLOGAS_SK_Firing_01);
		m_pModelCom->Set_AnimationIndex(FLOGAS_SK_Firing_01);

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance)
			pGameInstance->Play_Sound(L"Flogas_Skill_01.mp3", 1.f);

		for (int i = 0; i < 9; i++)
		{
				CRockMada::DESC rockmadaDesc;
				ZeroMemory(&rockmadaDesc, sizeof(CRockMada::DESC));

				_float4 vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
				vLook = XMVector3TransformNormal(vLook, XMMatrixRotationY(XMConvertToRadians(-80.f + (i * 20.f))));
				_float4 vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
				// 내 앞으로
				rockmadaDesc.vDestination = vPos + vLook * 5.f;
				rockmadaDesc.vDestination.y += 0.6f;
				pGameInstance->Clone_GameObject(g_LEVEL, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_RockMada"), &rockmadaDesc);
		}

		for (int i = 0; i < 9; i++)
		{
			CRockMada::DESC rockmadaDesc;
			ZeroMemory(&rockmadaDesc, sizeof(CRockMada::DESC));

			_float4 vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
			vLook = XMVector3TransformNormal(vLook, XMMatrixRotationY(XMConvertToRadians(-80.f + (i * 20.f))));
			_float4 vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
			// 내 앞으로
			rockmadaDesc.vDestination = vPos + vLook * 10.f;
			rockmadaDesc.vDestination.y += 0.6f;
			pGameInstance->Clone_GameObject(g_LEVEL, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_RockMada"), &rockmadaDesc);
		}

		for (int i = 0; i < 9; i++)
		{
			CRockMada::DESC rockmadaDesc;
			ZeroMemory(&rockmadaDesc, sizeof(CRockMada::DESC));

			_float4 vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
			vLook = XMVector3TransformNormal(vLook, XMMatrixRotationY(XMConvertToRadians(-80.f + (i * 20.f))));
			_float4 vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
			// 내 앞으로
			rockmadaDesc.vDestination = vPos + vLook * 15.f;
			rockmadaDesc.vDestination.y += 0.6f;
			pGameInstance->Clone_GameObject(g_LEVEL, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_RockMada"), &rockmadaDesc);
		}
		RELEASE_INSTANCE(CGameInstance)
	})
		.Tick([this](_double TimeDelta)
	{
		_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	})
		.OnExit([this]()
	{
		m_bSkill_1ToPlayer = false;
		m_SkillDelayTime = 0.0;
	})
		.AddTransition("Skill_1 to Idle", "Idle")
		.Predicator([this]()
	{
		return AnimFinishChecker(FLOGAS_SK_Firing_01, 0.85);
	})

		.AddState("Skill_2")
		.OnStart([this]()
	{
		m_pModelCom->Reset_AnimPlayTime(FLOGAS_SK_Firing_02);
		m_pModelCom->Set_AnimationIndex(FLOGAS_SK_Firing_02);

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance)
			pGameInstance->Play_Sound(L"Flogas_Skill_02.mp3", 1.f);

			for (int i = 0; i < 18; ++i)
			{
				CRockNorm04::DESC rockmadaDesc;
				ZeroMemory(&rockmadaDesc, sizeof(CRockNorm04::DESC));
				_float4 vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
				vLook = XMVector3TransformNormal(vLook, XMMatrixRotationY(XMConvertToRadians(-90.f + (i * 10))));
				_float4 vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
				// 내 앞으로
				rockmadaDesc.vDestination = vPos + vLook* 7.f;
				rockmadaDesc.vDestination.y += 7.f;
				rockmadaDesc.vDir = vLook;
				rockmadaDesc.iProperty = CRockNorm04::FLOGAS;
				pGameInstance->Clone_GameObject(g_LEVEL, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_RockNorm04"), &rockmadaDesc);
			}
	
		RELEASE_INSTANCE(CGameInstance)
	})
		.Tick([this](_double TimeDelta)
	{
		_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	})
		.OnExit([this]()
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance)
			pGameInstance->Play_Sound(L"Flogas_Skill_02_2.mp3", 1.f);
		RELEASE_INSTANCE(CGameInstance)

		m_bSkill_2ToPlayer = false;
		m_SkillDelayTime = 0.0;
	})
		.AddTransition("Skill_2 to Idle", "Idle")
		.Predicator([this]()
	{
		return AnimFinishChecker(FLOGAS_SK_Firing_02, 0.85);
	})

		.AddState("Skill_5")
		.OnStart([this]()
	{
		m_pModelCom->Reset_AnimPlayTime(FLOGAS_SK_Firing_05);
		m_pModelCom->Set_AnimationIndex(FLOGAS_SK_Firing_05);

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance)
			pGameInstance->Play_Sound(L"Flogas_Skill_05_1.mp3", 1.f);
		RELEASE_INSTANCE(CGameInstance)
	})
		.Tick([this](_double TimeDelta)
	{
		_float4 vPlayerPos = m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION);
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		CMeteor::DESC meteorDesc;

		pGameInstance->Play_Sound(L"Flogas_Skill_05_2.mp3", 1.f,false,SOUND_BOSS);

		if (AnimIntervalChecker(FLOGAS_SK_Firing_05, 0.2, 0.3))
		{
				ZeroMemory(&meteorDesc, sizeof(CMeteor::DESC));
				meteorDesc.vDestination = _float4(vPlayerPos.x + CMathUtils::GetRandomFloat(-10.f, 10.f),
					vPlayerPos.y + 0.6f,
					vPlayerPos.z + CMathUtils::GetRandomFloat(-10.f, 10.f), 1.f);

				pGameInstance->Clone_GameObject(g_LEVEL, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Stone"), &meteorDesc);
		}

		if (AnimIntervalChecker(FLOGAS_SK_Firing_05, 0.4, 0.5))
		{
				ZeroMemory(&meteorDesc, sizeof(CMeteor::DESC));
				meteorDesc.vDestination = _float4(vPlayerPos.x + CMathUtils::GetRandomFloat(-10.f, 10.f),
					vPlayerPos.y + 0.6f,
					vPlayerPos.z + CMathUtils::GetRandomFloat(-10.f, 10.f), 1.f);

				pGameInstance->Clone_GameObject(g_LEVEL, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Stone"), &meteorDesc);
		}

		if (AnimIntervalChecker(FLOGAS_SK_Firing_05, 0.6, 0.7))
		{
				ZeroMemory(&meteorDesc, sizeof(CMeteor::DESC));
				meteorDesc.vDestination = _float4(vPlayerPos.x + CMathUtils::GetRandomFloat(-10.f, 10.f),
					vPlayerPos.y + 0.6f,
					vPlayerPos.z + CMathUtils::GetRandomFloat(-10.f, 10.f), 1.f);

				pGameInstance->Clone_GameObject(g_LEVEL, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Stone"), &meteorDesc);
		}

		RELEASE_INSTANCE(CGameInstance);
	})
		.OnExit([this]()
	{
		m_bSkill_5ToPlayer = false;
		m_SkillDelayTime = 0.0;

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance)
			pGameInstance->Play_Sound(L"Flogas_Skill_05_3.mp3", 1.f, false, SOUND_BOSS);
		RELEASE_INSTANCE(CGameInstance)
	})
		.AddTransition("Skill_5 to Idle", "Idle")
		.Predicator([this]()
	{
		return AnimFinishChecker(FLOGAS_SK_Firing_05, 0.85);
	})

	
		// Dead
		.AddState("Dead")
		.OnStart([this]()
	{
		m_pModelCom->Reset_AnimPlayTime(FLOGAS_Die);
		m_pModelCom->Set_AnimationIndex(FLOGAS_Die);

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance)
			pGameInstance->Play_Sound(L"Flogas_Die_1.mp3", 1.f);
		RELEASE_INSTANCE(CGameInstance)
	})
		.AddTransition("Dead to DeadBody", "DeadBody")
			.Predicator([this]()
		{
			return AnimFinishChecker(FLOGAS_Die, 0.9);
		})
		.AddState("DeadBody")
			.OnStart([this]()
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance)
			pGameInstance->Play_Sound(L"Flogas_Die_3.mp3", 1.f);
		pGameInstance->Stop_Sound(SOUND_BGM);
		pGameInstance->Play_Sound(L"Logo_BGM.ogg", 1.f, true, SOUND_BGM);
		RELEASE_INSTANCE(CGameInstance)
	})
		.Tick([this](_double TimeDelta)
	{
		m_pModelCom->Set_AnimationIndex(FLOGAS_DeadBody);
		m_dDeadTime += TimeDelta;
		m_fDissolveAmount += static_cast<_float>(TimeDelta);

		if (m_dDeadTime > 3.0)
		{
			m_bDead = true;
			if (!m_bArcane)
			{
				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance)
					CArcaneE::DESC desc;
				desc.vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
				pGameInstance->Clone_GameObject(g_LEVEL, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_ArcaneE"), &desc);
				RELEASE_INSTANCE(CGameInstance)
					m_bArcane = true;
			}
		}
	})

			.Build();
}

void CFlogas::SetUp_UI()
{
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
	CGameObject* pUI = nullptr;
	pUI = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_ConversationUI"), &conversationDesc);

	if (nullptr != pUI)
		m_UI.push_back(pUI);

	RELEASE_INSTANCE(CGameInstance);
}

void CFlogas::Adjust_Collision(_double TimeDelta)
{
	CollisionToPlayer(TimeDelta);
	CollisionToMonster(TimeDelta);

	if (m_bPlayerAttackCommand)
		CollisionToWeapon(TimeDelta);

	if (m_bPlayerSkill02Command)
		CollisionToWeaponSkill02(TimeDelta);

	if (m_bPlayerSkill04Command)
		CollisionToWeaponSkill04(TimeDelta);

	if (!m_bPlayerSkill02Command && !m_bPlayerSkill04Command)
		m_bImpossibleSkillDamaged = false;
}

void CFlogas::CollisionToPlayer(_double TimeDelta)
{
	_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_float3 vPlayerPos = m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION);
	_float fDistance = CMathUtils::Distance(vPos, vPlayerPos);

	if (fabsf(fDistance) < 20.f)
		m_bPlayerChase = true;
	else
		m_bPlayerChase = false;
}

void CFlogas::Play_Skill(_double TimeDelta)
{
	m_SkillDelayTime += TimeDelta;

	if (m_SkillDelayTime > 7.0 &&
		!m_bSkill_1ToPlayer &&
		!m_bSkill_2ToPlayer &&
		!m_bSkill_5ToPlayer &&
		!m_bSkillOrderFinish)
	{
		if (m_iSkillOrder == 0)
			m_bSkill_1ToPlayer = true;
		if (m_iSkillOrder == 1)
			m_bSkill_5ToPlayer = true;
		if (m_iSkillOrder == 2)
		{
			m_bSkill_2ToPlayer = true;
			m_bSkillOrderFinish = true;
		}
		m_iSkillOrder++;
	}
	else if (m_SkillDelayTime > 7.0 &&
		!m_bSkill_1ToPlayer &&
		!m_bSkill_2ToPlayer &&
		!m_bSkill_5ToPlayer)
	{
		_uint iSkill = rand() % 3;

		if (iSkill == 0)
			m_bSkill_2ToPlayer = true;
		if (iSkill == 1)
			m_bSkill_5ToPlayer = true;
		if (iSkill == 2)
			m_bSkill_1ToPlayer = true;
	}
}

void CFlogas::AdditiveAnim(_double TimeDelta)
{
	if (m_bFrontDamaged)
	{
		AdjustSetDamage();
		m_pModelCom->Set_AdditiveAnimIndex(FLOGAS_ADD_DMG_F);
		m_pModelCom->Play_AddtivieAnim(TimeDelta, 1.f);
	}
	else if (!m_bFrontDamaged)
	{
		m_pModelCom->Reset_AnimPlayTime(FLOGAS_ADD_DMG_F);
	}

	if (AnimFinishChecker(FLOGAS_ADD_DMG_F))
	{
		m_bFrontDamaged = false;
		m_bImpossibleDamaged = false;
	}

	///////////////////////////////////////

	if (m_bBackDamaged)
	{
		AdjustSetDamage();
		m_pModelCom->Set_AdditiveAnimIndex(FLOGAS_ADD_DMG_B);
		m_pModelCom->Play_AddtivieAnim(TimeDelta, 1.f);
	}
	else if (!m_bBackDamaged)
	{
		m_pModelCom->Reset_AnimPlayTime(FLOGAS_ADD_DMG_B);
	}

	if (AnimFinishChecker(FLOGAS_ADD_DMG_B))
	{
		m_bBackDamaged = false;
		m_bImpossibleDamaged = false;
	}
}

HRESULT CFlogas::SetUp_Components()
{
	if (FAILED(__super::SetUp_Components()))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxAnimModel"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Model_Flogas"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC			ColliderDesc;

	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vCenter = _float3(0.f, 2.f, 0.f);
	ColliderDesc.vSize = _float3(1.f, 6.f, 1.f);

	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_AABB"),
		(CComponent**)&m_pColliderCom[COLLTYPE_AABB], &ColliderDesc)))
		return E_FAIL;

	/* For.Com_SPHERE */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vSize = _float3(1.5f, 1.5f, 1.5f);

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

HRESULT CFlogas::SetUp_ShaderResources()
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

	if (FAILED(m_pShaderCom->Set_RawValue("g_vRimColor", &m_vRimColor, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("fDissolveAmount", &m_fDissolveAmount, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("fFringeAmount", &m_fFringeAmount, sizeof(_float))))
		return E_FAIL;

	_bool bTRUE = true;

	if (FAILED(m_pShaderCom->Set_RawValue("g_bDissolve", &bTRUE, sizeof(_bool))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CFlogas::SetUP_ShadowShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_LIGHTVIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

_bool CFlogas::AnimFinishChecker(ANIMATION eAnim, _double FinishRate)
{
	return m_pModelCom->Get_IndexAnim(eAnim)->Get_PlayRate() >= FinishRate;
}

_bool CFlogas::AnimIntervalChecker(ANIMATION eAnim, _double StartRate, _double FinishRate)
{
	if (m_pModelCom->Get_IndexAnim(eAnim)->Get_PlayRate() > StartRate &&
		m_pModelCom->Get_IndexAnim(eAnim)->Get_PlayRate() <= FinishRate)
	{
		return true;
	}
	return false;
}

CFlogas * CFlogas::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CFlogas*		pInstance = new CFlogas(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Created : CFlogas");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CFlogas::Clone(void * pArg)
{
	CFlogas*		pInstance = new CFlogas(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Cloned : CFlogas");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CFlogas::Free()
{
	__super::Free();

	for (auto pCollider : m_MonsterColliders)
		Safe_Release(pCollider);

	m_MonsterColliders.clear();

	for (auto pUI : m_UI)
		Safe_Release(pUI);

	m_UI.clear();
}

