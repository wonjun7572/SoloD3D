#include "stdafx.h"
#include "..\Public\Midori.h"
#include "Camera_Dynamic.h"
#include "GameInstance.h"
#include "MidoriFSM.h"
#include "Timer.h"
#include "Layer.h"

// 얘는 STAND 없음

CMidori::CMidori(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CCharacter(pDevice, pContext)
{
}

CMidori::CMidori(const CMidori & rhs)
	:CCharacter(rhs)
{
}

HRESULT CMidori::Init_Prototype()
{
	if (FAILED(__super::Init_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMidori::Init(void * pArg)
{
	CGameObject::GAMEOBJECTDESC			GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof GameObjectDesc);

	GameObjectDesc.TransformDesc.fSpeedPerSec = 1.0f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Init(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(25.f, 0.f, -6.f, 1.f));

	m_strObjName = TEXT("Midori");
	return S_OK;
}

void CMidori::Tick(_double TimeDelta)
{
	m_isRun = true;

	if (m_pCam->Get_CAMSTATE() == CCamera_Dynamic::STATE_TPS
		&& m_pCam->Get_CHARACTERCAM() == CCamera_Dynamic::CHARACTER_MIDORI)
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

void CMidori::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CMidori::Render()
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

void CMidori::Movement(_double TimeDelta)
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

void CMidori::Collision_ToObstacle(_double TimeDelta)
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

void CMidori::Collision_ToEnemy(_double TimeDelta)
{
	CGameInstance*			pGameInstance = GET_INSTANCE(CGameInstance);

	vector<pair<CCollider*, CTransform*>> vecTarget;
	vector<float> vecDistance;

	for (_uint i = 0; i < 4; ++i)
	{
		CCollider*	pTargetCollider = (CCollider*)pGameInstance->Get_ComponentPtr(LEVEL_CHAP1, TEXT("Layer_Enemy"), TEXT("Com_SPHERE"), i);
		CTransform*	pTargetPos = (CTransform*)pGameInstance->Get_ComponentPtr(LEVEL_CHAP1, TEXT("Layer_Enemy"), TEXT("Com_Transform"), i);

		if (pTargetCollider != nullptr)
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

void CMidori::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();
}

HRESULT CMidori::SetUp_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Model_Midori"), TEXT("Com_Model"),
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
	ColliderDesc.vSize = _float3(12.f,12.f, 12.f);
	ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Collider_SPHERE"), TEXT("Com_SPHERE"),
		(CComponent**)&m_pColliderCom[COLLTYPE_SPHERE], &ColliderDesc)))
		return E_FAIL;

	m_pFSM = CMidoriFSM::Create(this);
	m_Components.insert({ L"FSM", m_pFSM });
	Safe_AddRef(m_pFSM);

	return S_OK;
}

HRESULT CMidori::SetUp_ShaderResources()
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

CMidori * CMidori::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMidori*		pInstance = new CMidori(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Created : CMidori");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CMidori::Clone(void * pArg)
{
	CMidori*		pInstance = new CMidori(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMidori");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMidori::Free()
{
	__super::Free();

	Safe_Release(m_pFSM);
}
