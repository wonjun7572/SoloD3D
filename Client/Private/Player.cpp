#include "stdafx.h"
#include "..\public\Player.h"
#include "GameInstance.h"
#include "Weapon.h"
#include "Shield.h"
#include "Bone.h"
#include "PlayerFSM.h"
#include "Navigation.h"

CPlayer::CPlayer(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CPlayer::CPlayer(const CPlayer & rhs)
	: CGameObject(rhs)
{

}

HRESULT CPlayer::Init_Prototype()
{
	if (FAILED(__super::Init_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Init(void * pArg)
{
	CGameObject::GAMEOBJECTDESC			GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof GameObjectDesc);

	GameObjectDesc.TransformDesc.fSpeedPerSec = 7.0f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Init(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	if (FAILED(SetUp_Parts()))
		return E_FAIL;

	m_strObjName = L"Player";

	m_iCurrentAnimIndex = 23;
	m_PartSize =	static_cast<_uint>(m_PlayerParts.size());
	return S_OK;
}

void CPlayer::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	
	Movement(TimeDelta);

	m_pFSM->Tick(TimeDelta);

	m_pModelCom->Play_Animation(TimeDelta, m_bAnimation);

	for (_uint i = 0; i <m_PartSize; ++i)
	{
		m_PlayerParts[i]->Tick(TimeDelta);
	}

	for (_uint i = 0; i < COLLTYPE_END; ++i)
		m_pColliderCom[i]->Update(m_pTransformCom->Get_WorldMatrix());
}

void CPlayer::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	for (_uint i = 0; i < m_PartSize; ++i)
	{
		m_PlayerParts[i]->Late_Tick(TimeDelta);
	}

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CPlayer::Render()
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

#ifdef _DEBUG
	for (_uint i = 0; i < COLLTYPE_END; ++i)
	{
		if (nullptr != m_pColliderCom[i])
			m_pColliderCom[i]->Render();
	}

	m_pNavigationCom->Render();
#endif

	return S_OK;
}

void CPlayer::Movement(_double TimeDelta)
{
	m_eState = IDLE;
	m_bWalk = false;
	m_bRunning = false;
	m_bCamTurn = true;
	
	CGameInstance*			pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Get_DIKeyState(DIK_W))
	{
		m_bWalk = true;
		m_bCamTurn = false;
		if (pGameInstance->Get_DIKeyState(DIK_LSHIFT))
		{
			m_bWalk = false;
			m_bRunning = true;
		}
		m_eState = CPlayer::FORWARD;
	}

	if (pGameInstance->Get_DIKeyState(DIK_S))
	{
		m_bWalk = true;
		m_bCamTurn = false;
		m_eState = CPlayer::BACK;
	}

	if (pGameInstance->Get_DIKeyState(DIK_A))
	{
		m_bWalk = true;
		m_bCamTurn = false;
		m_eState = CPlayer::LEFT;
	}

	if (pGameInstance->Get_DIKeyState(DIK_D))
	{
		m_bWalk = true;
		m_bCamTurn = false;
		m_eState = CPlayer::RIGHT;
	}

	if (pGameInstance->Get_DIKeyState(DIK_W) && pGameInstance->Get_DIKeyState(DIK_A))
	{
		m_bWalk = true;
		m_bCamTurn = false;
		m_eState = CPlayer::LF;
	}

	if (pGameInstance->Get_DIKeyState(DIK_W) && pGameInstance->Get_DIKeyState(DIK_D))
	{
		m_bWalk = true;
		m_bCamTurn = false;
		m_eState = CPlayer::RF;
	}

	if (pGameInstance->Get_DIKeyState(DIK_S) && pGameInstance->Get_DIKeyState(DIK_A))
	{
		m_bWalk = true;
		m_bCamTurn = false;
		m_eState = CPlayer::LB;
	}

	if (pGameInstance->Get_DIKeyState(DIK_S) && pGameInstance->Get_DIKeyState(DIK_D))
	{
		m_bWalk = true;
		m_bCamTurn = false;
		m_eState = CPlayer::RB;
	}

	if (pGameInstance->Get_DIKeyState(DIK_SPACE))
	{
		m_pTransformCom->Jump(TimeDelta, 0.5f, 0.f);
		m_bJumping = true;
	}
	else
	{
		m_bJumping = false;
		if(m_pTransformCom->Get_World4x4()._42 >= 0.f)
			m_pTransformCom->Go_Down(TimeDelta);
	}
	
	if (pGameInstance->Mouse_Down(DIM_LB))
	{
		m_bAttackClick = true;
	}

	if (m_bAttackClick && m_bAttack_1 == true)
	{
		m_bAttack_2 = true;
		m_bAttackClick = false;
	}
	else if (m_bAttackClick && m_bAttack_0 == true)
	{
		m_bAttack_1 = true;
		m_bAttackClick = false;
	}																																											
	else if (m_bAttackClick && m_bAttack_0 == false)
	{
		m_bAttack_0 = true;
		m_bAttackClick = false;
	}
	else
	{
		if (m_pModelCom->Get_AnimFinished() == true)
		{
			m_bAttack_0 = false;
			m_bAttack_1 = false;
			m_bAttack_2 = false;
		}
	}
	
	if (m_bCamTurn == false && m_bAttack_0 == false)
	{
		_long MouseMove_X = 0;

		if (MouseMove_X = pGameInstance->Get_DIMouseMove(DIMS_X))
		{
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), MouseMove_X * m_MouseSensity * TimeDelta);
		}
	}
		
	RELEASE_INSTANCE(CGameInstance);
}

void CPlayer::Imgui_RenderProperty()
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

HRESULT CPlayer::SetUp_Parts()
{
	CGameObject*		pPartObject = nullptr;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CWeapon::WEAPONDESC			WeaponDesc;
	ZeroMemory(&WeaponDesc, sizeof(CWeapon::WEAPONDESC));

	WeaponDesc.PivotMatrix = m_pModelCom->Get_PivotMatrix();
	WeaponDesc.pSocket = m_pModelCom->Get_BonePtr("BN_Weapon_R");
	WeaponDesc.pTargetTransform = m_pTransformCom;
	Safe_AddRef(WeaponDesc.pSocket);
	Safe_AddRef(m_pTransformCom);

	pPartObject = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Weapon"), &WeaponDesc);
	
	if (nullptr == pPartObject)
		return E_FAIL;

	m_PlayerParts.push_back(pPartObject);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CPlayer::SetUp_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Model_HumanF"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC			ColliderDesc;

	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(0.5f, 2.f, 0.5f);
	ColliderDesc.vCenter = _float3(0.f, 1.5f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_AABB"),
		(CComponent**)&m_pColliderCom[COLLTYPE_AABB], &ColliderDesc)))
		return E_FAIL;

	/* For.Com_OBB */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(0.5f, 2.f, 0.5f);
	ColliderDesc.vCenter = _float3(0.f, 1.5f, 0.f);

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

	/* For.Com_Navigation */
	CNavigation::NAVIDESC			NaviDesc;
	ZeroMemory(&NaviDesc, sizeof(CNavigation::NAVIDESC));

	NaviDesc.iCurrentIndex = 0;

	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"),
		(CComponent**)&m_pNavigationCom, &NaviDesc)))
		return E_FAIL;

	m_pFSM = CPlayerFSM::Create(this);
	m_Components.insert({ L"FSM", m_pFSM });
	Safe_AddRef(m_pFSM);

	return S_OK;
}

HRESULT CPlayer::SetUp_ShaderResources()
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

CPlayer * CPlayer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayer*		pInstance = new CPlayer(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Created : CPlayer");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPlayer::Clone(void * pArg)
{
	CPlayer*		pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPlayer");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();

	for (_uint i = 0; i < COLLTYPE_END; ++i)
		Safe_Release(m_pColliderCom[i]);

	for (auto& pPart : m_PlayerParts)
		Safe_Release(pPart);

	m_PlayerParts.clear();

	Safe_Release(m_pFSM);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pNavigationCom);
}
