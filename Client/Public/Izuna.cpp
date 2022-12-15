#include "stdafx.h"
#include "..\Private\Izuna.h"
#include "GameInstance.h"
#include "IzunaFSM.h"
#include "Timer.h"
#include "MathUtils.h"
#include "Camera_Dynamic.h"
#include "Layer.h"

CIzuna::CIzuna(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CCharacter(pDevice,pContext)
{
}

CIzuna::CIzuna(const CIzuna & rhs)
	: CCharacter(rhs)
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

	m_strObjName = TEXT("Izuna");
	return S_OK;
}

void CIzuna::Tick(_double TimeDelta)
{
	m_isRun = true;

	if (m_pCam->Get_CAMSTATE() == CCamera_Dynamic::STATE_TPS 
		 && m_pCam->Get_CHARACTERCAM() == CCamera_Dynamic::CHARACTER_IZUNA)
	{
		m_pTransformCom->Set_SpeedPerSec(2.f);
		Movement(TimeDelta);
	}
	else
	{
		m_pTransformCom->Set_SpeedPerSec(1.f);
		Collision_ToObstacle(TimeDelta);
		Collision_ToEnemy(TimeDelta);

		if (m_isRun == true || m_isJumping == true)
		{
			m_pTransformCom->Go_Straight(TimeDelta);
		}
	}

	m_pFSM->Tick(TimeDelta);
	m_pModelCom->Play_Animation(TimeDelta, m_bAnimationFinished);

	__super::Tick(TimeDelta);
}

void CIzuna::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
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

	return S_OK;
}

void CIzuna::Movement(_double TimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	m_eState = STATE_NORMAL;

	m_isRun = false;
	m_isJumping = false;
	m_isAttack = false;

	if (pGameInstance->Get_DIKeyState(DIK_W))
	{
		m_pTransformCom->Go_Straight(TimeDelta);
		m_isRun = true;
	}

	if (pGameInstance->Get_DIKeyState(DIK_A))
	{
		m_pTransformCom->Go_Left(TimeDelta);
		m_isRun = true;
	}

	if (pGameInstance->Get_DIKeyState(DIK_D))
	{
		m_pTransformCom->Go_Right(TimeDelta);
		m_isRun = true;
	}

	if (pGameInstance->Get_DIKeyState(DIK_S))
	{
		m_pTransformCom->Go_Backward(TimeDelta);
		m_isRun = true;
	}

	if (pGameInstance->Get_DIMouseState(DIM_LB))
	{
		m_isAttack = true;
	}

	if (pGameInstance->Get_DIKeyState(DIK_SPACE))
	{
		m_isRun = true;
		m_isJumping = true;
	}

	if (pGameInstance->Get_DIKeyState(DIK_C))
	{
		m_eState = STATE_KNEEL;
	}

	_long			MouseMove = 0;

	if (MouseMove = pGameInstance->Get_DIMouseMove(DIMS_X))
	{
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), TimeDelta * MouseMove * 0.1f);
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CIzuna::Collision_ToObstacle(_double TimeDelta)
{
	CGameInstance*			pGameInstance = GET_INSTANCE(CGameInstance);

	vector<CCollider*> vecTarget;

	_uint iSize = pGameInstance->Find_Layer(LEVEL_CHAP1, TEXT("Layer_Obstacle"))->GetGameObjects().size();

	for (_uint i = 0; i < iSize; ++i)
	{
		CCollider*		pTargetCollider = (CCollider*)pGameInstance->Get_ComponentPtr(LEVEL_CHAP1, TEXT("Layer_Obstacle"), TEXT("Com_AABB"), i);
		vecTarget.push_back(pTargetCollider);
	}

	if (vecTarget.size() == 0)
		return;

	_uint i = 0;

	for (auto& pTarget : vecTarget)
	{
		if (m_pColliderCom[COLLTYPE_AABB]->Collision(pTarget) == true)
		{
			m_eState = STATE_KNEEL;
			m_isJumping = true;
		}
		else
		{
			i++;
		}
	}

	if (i == iSize)
	{
		m_eState = STATE_NORMAL;
		m_isJumping = false;
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CIzuna::Collision_ToEnemy(_double TimeDelta)
{
	CGameInstance*			pGameInstance = GET_INSTANCE(CGameInstance);

	vector<pair<CCollider*, CTransform*>> vecTarget;
	vector<float> vecDistance;

	for (_uint i = 0; i < 4; ++i) 
	{
		CCollider*	pTargetCollider = (CCollider*)pGameInstance->Get_ComponentPtr(LEVEL_CHAP1, TEXT("Layer_Enemy"), TEXT("Com_SPHERE") , i);
		CTransform*	pTargetPos = (CTransform*)pGameInstance->Get_ComponentPtr(LEVEL_CHAP1, TEXT("Layer_Enemy"), TEXT("Com_Transform"), i);
		
		if(pTargetCollider != nullptr)
			vecTarget.push_back({ pTargetCollider, pTargetPos });
	}

	if (vecTarget.size() == 0)
	{
		m_isRun = true;
		m_isAttack = false;
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVectorSet(0.f, 0.f, 1.f, 0.f));
		vecTarget.clear();
		vecDistance.clear();
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	for (auto& pTarget : vecTarget)
	{
		_float3 fSour, fDest;
		XMStoreFloat3(&fSour, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
		XMStoreFloat3(&fDest, pTarget.second->Get_State(CTransform::STATE_TRANSLATION));
		vecDistance.push_back(CMathUtils::Distance(fSour, fDest));
	}

	sort(vecDistance.begin(), vecDistance.end(), [](_float first, _float second) -> bool { return first < second; });

	for (auto& pTarget : vecTarget)
	{
		_float3 fSour, fDest;
		XMStoreFloat3(&fSour, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
		XMStoreFloat3(&fDest, pTarget.second->Get_State(CTransform::STATE_TRANSLATION));

		if (fabs(vecDistance[0] - CMathUtils::Distance(fSour, fDest)) <= 0.01f)
		{
			// 구 충돌로 적이 있으면 총 쏘는애니메이션 나오고 몬스터에게 발사체 발사
			if (m_pColliderCom[COLLTYPE_SPHERE]->Collision(pTarget.first))
			{
				m_isRun = false;
				m_isAttack = true;
				m_pTransformCom->LookAt(pTarget.second->Get_State(CTransform::STATE_TRANSLATION));
			}
			else
			{
				m_isRun = true;
				m_isAttack = false;
				m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVectorSet(0.f, 0.f, 1.f, 0.f));
			}
		}
	}

	vecTarget.clear();
	vecDistance.clear();
	RELEASE_INSTANCE(CGameInstance);
}

void CIzuna::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();

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
	ColliderDesc.vSize = _float3(0.7f, 0.7f, 0.7f);
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
	ColliderDesc.vSize = _float3(6.f, 6.f, 6.f);
	ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Collider_SPHERE"), TEXT("Com_SPHERE"),
		(CComponent**)&m_pColliderCom[COLLTYPE_SPHERE], &ColliderDesc)))
		return E_FAIL;

	m_pFSM = CIzunaFSM::Create(this);
	m_Components.insert({ L"FSM", m_pFSM });
	Safe_AddRef(m_pFSM);

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

	Safe_Release(m_pFSM);
}
