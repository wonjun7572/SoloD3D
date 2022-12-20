#include "stdafx.h"
#include "..\Public\Demon.h"
#include "GameInstance.h"
#include "DemonFSM.h"
#include "Weapon.h"

CDemon::CDemon(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CMonster(pDevice, pContext)
{
}

CDemon::CDemon(const CDemon & rhs)
	: CMonster(rhs)
{
}

HRESULT CDemon::Init_Prototype()
{
	if (FAILED(__super::Init_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDemon::Init(void * pArg)
{
	CGameObject::GAMEOBJECTDESC			GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof GameObjectDesc);

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.0f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Init(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_strObjName = L"Demon";

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(30.f, 0.f, 30.f, 1.f));

	return S_OK;
}

void CDemon::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	CollisionToPlayer(TimeDelta);
	CollisionToWeapon(TimeDelta);

	m_pFSM->Tick(TimeDelta);

	m_pModelCom->Play_Animation(TimeDelta, m_bAnimation);
}

void CDemon::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CDemon::Render()
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

void CDemon::Imgui_RenderProperty()
{
	if (ImGui::CollapsingHeader("For.Animation"))
	{
		const char* combo_preview_value = m_pModelCom->Get_AnimationName()[m_iCurrentAnimIndex];

		if (ImGui::BeginCombo("ANIM", combo_preview_value))
		{
			for (_uint i = 0; i < m_pModelCom->Get_AnimationsNum(); i++)
			{
				const bool is_selected = (m_iCurrentAnimIndex == i);
				if (ImGui::Selectable(m_pModelCom->Get_AnimationName()[i], is_selected))
					m_iCurrentAnimIndex = i;

				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		ImGui::Text("Current Anim Index"); ImGui::SameLine();
		ImGui::Text(to_string(m_iCurrentAnimIndex).c_str());
	}
}

void CDemon::CollisionToPlayer(_double TimeDelta)
{
	CGameInstance*			pGameInstance = GET_INSTANCE(CGameInstance);

	CPlayer*		pPlayer = static_cast<CPlayer*>(pGameInstance->Find_GameObject(LEVEL_CHAP1, L"Layer_Player", L"Player"));
	_vector			vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_vector			vPlayerPos = pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION);

	_vector			V = XMVectorSubtract(vPos, vPlayerPos);
	_vector			X = XMVector4Length(V);
	_float			fDistanceToTarget = XMVectorGetX(X);

	if (fabs(fDistanceToTarget) < 3)
	{
		m_pTransformCom->LookAt(vPlayerPos);
		m_bAttack = true;
	}
	else
	{
		CCollider*		pTargetCollider = (CCollider*)pGameInstance->Get_ComponentPtr(LEVEL_CHAP1, TEXT("Layer_Player"), TEXT("Com_SPHERE"));
		CCollider*		pTargetCollider_AABB = (CCollider*)pGameInstance->Get_ComponentPtr(LEVEL_CHAP1, TEXT("Layer_Player"), TEXT("Com_AABB"));

		if (nullptr == pTargetCollider)
			return;

		if (m_pColliderCom[COLLTYPE_SPHERE]->Collision(pTargetCollider) == true)
		{
			m_bRun = true;
			m_pTransformCom->LookAt(vPlayerPos);
			m_pTransformCom->Chase(vPlayerPos, TimeDelta);
		}
		else
		{
			m_bRun = false;
		}
		m_bAttack = false;
	}
	RELEASE_INSTANCE(CGameInstance);
}

void CDemon::CollisionToWeapon(_double TimeDelta)
{
	CGameInstance*			pGameInstance = GET_INSTANCE(CGameInstance);

	CPlayer* pPlayer = static_cast<CPlayer*>(pGameInstance->Find_GameObject(LEVEL_CHAP1, L"Layer_Player", L"Player"));

	_vector vTargetToDir = XMVector3Normalize(XMVectorSubtract(pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION), m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION)));
	
	_vector vDot = XMVector3Dot(m_pTransformCom->Get_State(CTransform::STATE_LOOK), vTargetToDir);
	_float fDot = XMVectorGetX(vDot);

	if (pPlayer->Get_State() == CPlayer::ATTACK)
	{
		for (_uint i = 0; i < 5; i++)
		{
			CCollider* pTargetCollider = static_cast<CWeapon*>(pPlayer->Get_PlayerParts()[0])->Get_Collider(i);

			if (nullptr == pTargetCollider)
				return;

			if (m_pColliderCom[COLLTYPE_OBB]->Collision(pTargetCollider) == true)
			{
				if (fDot < 0)
					m_eState = CDemon::PLAYER_BACK;
				else
					m_eState = CDemon::PLAYER_FRONT;

				m_bHit = true;
			}
		}
	}

	if (m_pModelCom->Get_AnimFinished() == true)
	{
		m_bHit = false;
	}

	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CDemon::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Shader_VtxAnimModel"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Model_Demon"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC			ColliderDesc;

	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(1.2f, 3.f, 1.2f);
	ColliderDesc.vCenter = _float3(0.f,3.5, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_AABB"),
		(CComponent**)&m_pColliderCom[COLLTYPE_AABB], &ColliderDesc)))
		return E_FAIL;

	/* For.Com_OBB */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(1.2f, 3.f, 1.2f);
	ColliderDesc.vRotation = _float3(0.f, 0.0f, 0.f);
	ColliderDesc.vCenter = _float3(0.f, 3.5, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_OBB"),
		(CComponent**)&m_pColliderCom[COLLTYPE_OBB], &ColliderDesc)))
		return E_FAIL;

	/* For.Com_SPHERE */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(20.f, 20.f, 20.f);
	ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Collider_SPHERE"), TEXT("Com_SPHERE"),
		(CComponent**)&m_pColliderCom[COLLTYPE_SPHERE], &ColliderDesc)))
		return E_FAIL;

	m_pFSM = CDemonFSM::Create(this);
	m_Components.insert({ L"FSM", m_pFSM });
	Safe_AddRef(m_pFSM);

	return S_OK;
}

HRESULT CDemon::SetUp_ShaderResources()
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

	/* For.Lights */
	const LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(0);
	if (nullptr == pLightDesc)
		return E_FAIL;
	//
	//if (FAILED(m_pShaderCom->Set_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Set_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Set_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Set_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
	//	return E_FAIL;

	//if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4))))
	//	return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CDemon * CDemon::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CDemon*		pInstance = new CDemon(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Created : CDemon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CDemon::Clone(void * pArg)
{
	CDemon*		pInstance = new CDemon(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDemon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDemon::Free()
{
	__super::Free();
	Safe_Release(m_pFSM);
}
