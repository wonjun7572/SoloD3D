#include "stdafx.h"
#include "..\Private\Izuna.h"
#include "GameInstance.h"
#include "IzunaFSM.h"
#include "Timer.h"

CIzuna::CIzuna(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CGameObject(pDevice,pContext)
{
}

CIzuna::CIzuna(const CIzuna & rhs)
	:CGameObject(rhs)
{
}

HRESULT CIzuna::Init_Prototype()
{
	if (FAILED(__super::Init_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CIzuna::Init(void * pArg)
{
	CGameObject::GAMEOBJECTDESC			GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof GameObjectDesc);

	GameObjectDesc.TransformDesc.fSpeedPerSec = 1.0f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Init(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(25.f, 0.f, 0.f, 1.f));

	m_pFSM = CIzunaFSM::Create(this);
	m_Components.insert({ L"FSM", m_pFSM });
	Safe_AddRef(m_pFSM);

	return S_OK;
}

void CIzuna::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	Collision_ToObstacle(TimeDelta);
	Collision_ToEnemy(TimeDelta);
	
	m_pFSM->Tick(TimeDelta);

	m_pModelCom->Play_Animation(TimeDelta, m_bAnimationFinished);

	for (_uint i = 0; i < COLLTYPE_END; ++i)
		m_pColliderCom[i]->Update(m_pTransformCom->Get_WorldMatrix());
}

void CIzuna::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CIzuna::Render()
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
#endif

	return S_OK;
}

void CIzuna::Collision_ToObstacle(_double TimeDelta)
{
	CGameInstance*			pGameInstance = GET_INSTANCE(CGameInstance);

	CCollider*		pTargetCollider = (CCollider*)pGameInstance->Get_ComponentPtr(LEVEL_CHAP1, TEXT("Layer_Obstacle"), TEXT("Com_OBB"));
	
	if (nullptr == pTargetCollider)
		return;
	
	// 혹시 pTarget이 서서쏘는 엄폐물일 경우와 그렇지 않는 경우를 나눈다.

	// 현재는 앉아쏴 엄폐물만 존재
	if (m_pColliderCom[COLLTYPE_OBB]->Collision(pTargetCollider) == true)
	{
		m_eState = STATE_KNEEL;
		m_isJumping = true;
	}
	else
	{
		m_eState = STATE_NORMAL;
		m_isJumping = false;
	}

	// 구 충돌로 적이 있으면 총 쏘는애니메이션 나오고 몬스터에게 발사체 발사
	// 충돌이 아니라...... 조졌다.
	RELEASE_INSTANCE(CGameInstance);
}

void CIzuna::Collision_ToEnemy(_double TimeDelta)
{
	CGameInstance*			pGameInstance = GET_INSTANCE(CGameInstance);

	CCollider*		pTargetCollider = (CCollider*)pGameInstance->Get_ComponentPtr(LEVEL_CHAP1, TEXT("Layer_Monster"), TEXT("Com_SPHERE") , 1);

	if (nullptr == pTargetCollider)
		return;

	// 구 충돌로 적이 있으면 총 쏘는애니메이션 나오고 몬스터에게 발사체 발사
	if (m_pColliderCom[COLLTYPE_SPHERE]->Collision(pTargetCollider) == true)
	{
		m_isRun = false;
		m_isAttack = true;
		m_pTransformCom->LookAt(XMLoadFloat3(&pTargetCollider->Get_CollisionCenter()));
	}
	else
	{
		m_isAttack = false;
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVectorSet(0.f, 0.f, 1.f, 0.f));
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CIzuna::Imgui_RenderProperty()
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

	// 상태 제어 후 어떤 애니메이션으로 갈것인가를 판단하자
	static int e = 0;
	ImGui::RadioButton("Normal", &e, 0); ImGui::SameLine();
	ImGui::RadioButton("Stand", &e, 1); ImGui::SameLine();
	ImGui::RadioButton("Kneel", &e, 2);

	// 상태 만약 근처에 적이 있고 엄폐물이 있는것이 확인 되었으면 
	// 근처에 적이 있고 엄폐물이 없다면 그 자리에 멈춰서서 normal 상태로 총 쏘기
	// obb 박스까지 간다음에 장애물에 충돌이 된다면 그 때는 Kneel 상태로 바꾼다.
	// 엄폐물 중 긴 엄폐물이라면 stand 아니라면 kneel
	if (e == 0)
		m_eState = CIzuna::STATE_NORMAL;
	if(e == 1)
		m_eState = CIzuna::STATE_STAND;
	if(e==2)
		m_eState = CIzuna::STATE_KNEEL;

	// 적이 없을 때 달린다
	if (ImGui::Button("Run"))
	{
		m_isRun = !m_isRun;
	}
	ImGui::SameLine();
	// 적이 있다면 어택을 한다
	if (ImGui::Button("Attack"))
	{
		m_isAttack = !m_isAttack;
	}
	ImGui::SameLine();
	// 달리다 장애물과 부딪히면 점프를 한다.
	if (ImGui::Button("Jump"))
	{
		m_isJumping = !m_isJumping;
	}
	// 스킬 버튼을 누르면 컷신과 함께 스킬을 사용한다.
	if (ImGui::Button("Skill"))
	{
		m_isExSkill = !m_isExSkill;
	}
}

HRESULT CIzuna::SetUp_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Model_Izuna"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC			ColliderDesc;

	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(0.7f, 1.5f, 0.7f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_AABB"),
		(CComponent**)&m_pColliderCom[COLLTYPE_AABB], &ColliderDesc)))
		return E_FAIL;

	/* For.Com_OBB */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(0.7f, 0.7f, 0.7f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_OBB"),
		(CComponent**)&m_pColliderCom[COLLTYPE_OBB], &ColliderDesc)))
		return E_FAIL;

	/* For.Com_SPHERE */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(5.f, 5.f, 5.f);
	ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Collider_SPHERE"), TEXT("Com_SPHERE"),
		(CComponent**)&m_pColliderCom[COLLTYPE_SPHERE], &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CIzuna::SetUp_ShaderResources()
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

CIzuna * CIzuna::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CIzuna*		pInstance = new CIzuna(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Created : CIzuna");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CIzuna::Clone(void * pArg)
{
	CIzuna*		pInstance = new CIzuna(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Cloned : CIzuna");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CIzuna::Free()
{
	__super::Free();

	for (_uint i = 0; i < COLLTYPE_END; ++i)
		Safe_Release(m_pColliderCom[i]);

	Safe_Release(m_pFSM);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
