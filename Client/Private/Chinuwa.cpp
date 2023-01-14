#include "stdafx.h"
#include "..\Public\Chinuwa.h"
#include "GameInstance.h"
#include "Animation.h"
#include "FSMComponent.h"
#include "Player.h"

CChinuwa::CChinuwa(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CAlly(pDevice, pContext)
{
}

CChinuwa::CChinuwa(const CChinuwa & rhs)
	: CAlly(rhs)
{
}

HRESULT CChinuwa::Init_Prototype()
{
	if (FAILED(__super::Init_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CChinuwa::Init(void * pArg)
{
	CGameObject::GAMEOBJECTDESC			GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof GameObjectDesc);

	GameObjectDesc.TransformDesc.fSpeedPerSec = 5.0f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Init(&GameObjectDesc)))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(rand() % 10 + 32.f, 0.f, rand() % 3 + 70.f, 1.f));

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pNavigationCom->Set_CurreuntIndex(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

	m_strObjName = L"Chinuwa";

	return S_OK;
}

void CChinuwa::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	AdditiveAnim(TimeDelta);
}

void CChinuwa::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CChinuwa::Render()
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

		m_pModelCom->Render(m_pShaderCom, i, 0, "g_BoneMatrices");
	}

	return S_OK;
}

void CChinuwa::Imgui_RenderProperty()
{
	if (ImGui::Button("Navi~"))
	{
		m_pNavigationCom->Set_CurreuntIndex(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	}

	if (ImGui::Button("FrontDamage"))
		m_bFrontDamaged = true;

	if (ImGui::Button("BackDamage"))
		m_bBackDamaged = true;

	m_pColliderCom[COLLTYPE_AABB]->FixedSizeForImgui(COLLTYPE_AABB);
	m_pColliderCom[COLLTYPE_SPHERE]->FixedSizeForImgui(COLLTYPE_SPHERE);
}

void CChinuwa::SetUp_FSM()
{
	m_pFSM = CFSMComponentBuilder()
		.InitState("Idle")
		.AddState("Idle")
		.Tick([this](_double TimeDelta)
	{
		m_pModelCom->Set_AnimationIndex(CHINUWA_Idle_C);
	})
		.AddTransition("Idle to Chase", "Chase")
		.Predicator([this]()
	{
		return m_bChase;
	})

		.AddState("Chase")
		.Tick([this](_double TimeDelta)
	{
		m_pModelCom->Set_AnimationIndex(CHINUWA_Run_F);
		m_pTransformCom->ChaseAndLookAt(m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION), TimeDelta, 5.f, m_pNavigationCom);
	})
		.AddTransition("Chase to Idle", "Idle")
		.Predicator([this]()
	{
		return !m_bChase;
	})

		.Build();
}

void CChinuwa::AdditiveAnim(_double TimeDelta)
{
	if (m_bFrontDamaged)
	{
		AdjustSetDamage();
		m_pModelCom->Set_AdditiveAnimIndex(CHINUWA_ADD_DMG_F);
		m_pModelCom->Play_AddtivieAnim(TimeDelta, 1.f);
	}

	if (AnimFinishChecker(CHINUWA_ADD_DMG_F))
	{
		m_bFrontDamaged = false;
		m_bImpossibleDamaged = false;
		m_pModelCom->Reset_AnimPlayTime(CHINUWA_ADD_DMG_F);
	}

	///////////////////////////////////////

	if (m_bBackDamaged)
	{
		AdjustSetDamage();
		m_pModelCom->Set_AdditiveAnimIndex(CHINUWA_ADD_DMG_B);
		m_pModelCom->Play_AddtivieAnim(TimeDelta, 1.f);
	}

	if (AnimFinishChecker(CHINUWA_ADD_DMG_B))
	{
		m_bBackDamaged = false;
		m_bImpossibleDamaged = false;
		m_pModelCom->Reset_AnimPlayTime(CHINUWA_ADD_DMG_B);
	}
}

HRESULT CChinuwa::SetUp_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Model_Chinuwa"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC			ColliderDesc;

	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vCenter = _float3(0.f, 1.5f, 0.f);
	ColliderDesc.vSize = _float3(0.7f, 1.5f, 0.7f);

	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_AABB"),
		(CComponent**)&m_pColliderCom[COLLTYPE_AABB], &ColliderDesc)))
		return E_FAIL;

	/* For.Com_SPHERE */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vSize = _float3(0.7f, 0.7f, 0.7f);

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

HRESULT CChinuwa::SetUp_ShaderResources()
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

_bool CChinuwa::AnimFinishChecker(ANIMATION eAnim, _double FinishRate)
{
	return m_pModelCom->Get_IndexAnim(eAnim)->Get_PlayRate() >= FinishRate;
}

_bool CChinuwa::AnimIntervalChecker(ANIMATION eAnim, _double StartRate, _double FinishRate)
{
	if (m_pModelCom->Get_IndexAnim(eAnim)->Get_PlayRate() > StartRate &&
		m_pModelCom->Get_IndexAnim(eAnim)->Get_PlayRate() <= FinishRate)
	{
		return true;
	}

	return false;
}

CChinuwa * CChinuwa::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CChinuwa*		pInstance = new CChinuwa(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Created : CChinuwa");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CChinuwa::Clone(void * pArg)
{
	CChinuwa*		pInstance = new CChinuwa(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Cloned : CChinuwa");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CChinuwa::Free()
{
	__super::Free();
}