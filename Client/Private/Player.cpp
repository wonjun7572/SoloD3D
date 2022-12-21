#include "stdafx.h"
#include "..\public\Player.h"
#include "GameInstance.h"
#include "Weapon.h"
#include "Bone.h"
#include "PlayerFSM.h"
#include "Navigation.h"
#include "Animation.h"

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

	m_PartSize = static_cast<_uint>(m_PlayerParts.size());

	return S_OK;
}

void CPlayer::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	Movement(TimeDelta);
	Set_AnimIndex(m_eState);
	m_pModelCom->Play_Animation(TimeDelta);
	MoveToAnim(TimeDelta);

	//m_pFSM->Tick(TimeDelta);

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

void CPlayer::Set_AnimIndex(PLAYER_STATE eType)
{
	switch (eType)
	{
	case PLAYER_FOR:
		m_pModelCom->Set_AnimationIndex(113);
		break;
	case PLAYER_ATTACK3:
		m_pModelCom->Set_AnimationIndex(6);
		break;
	case PLAYER_ATTACK2:
		m_pModelCom->Set_AnimationIndex(4);
		break;
	case PLAYER_ATTACK1:
		m_pModelCom->Set_AnimationIndex(2);
		break;
	case PLAYER_BR:
		m_pModelCom->Set_AnimationIndex(32);
		break;
	case PLAYER_BL:
		m_pModelCom->Set_AnimationIndex(31);
		break;
	case PLAYER_FR:
		m_pModelCom->Set_AnimationIndex(38);
		break;
	case PLAYER_FL:
		m_pModelCom->Set_AnimationIndex(37);
		break;
	case PLAYER_LM:
		m_pModelCom->Set_AnimationIndex(45);
		break;
	case PLAYER_RM:
		m_pModelCom->Set_AnimationIndex(46);
		break;
	case PLAYER_BM:
		m_pModelCom->Set_AnimationIndex(30);
		break;
	case PLAYER_FM:
		m_pModelCom->Set_AnimationIndex(34);
		break;
	case PLAYER_READY:
		m_pModelCom->Set_AnimationIndex(24);
		break;
	case PLAYER_WALKF:
		m_pModelCom->Set_AnimationIndex(115);
		break;
	case PLAYER_WALKB:
		m_pModelCom->Set_AnimationIndex(114);
		break;
	case PLAYER_SK01:
		m_pModelCom->Set_AnimationIndex(78);
		break;
	case PLAYER_SK02:
		m_pModelCom->Set_AnimationIndex(58);
		break;
	case PLAYER_SK03:
		m_pModelCom->Set_AnimationIndex(99);
		break;
	}
}

void CPlayer::MoveToAnim(_double TimeDelta)
{
	//if (m_eState == CPlayer::PLAYER_FOR)
	{
		_vector   vMovePos;
		ZeroMemory(&vMovePos, sizeof(_vector));

		vMovePos = XMLoadFloat4(&m_pModelCom->Get_MovePos());

		_float4 vIsZero;
		XMStoreFloat4(&vIsZero, vMovePos);

		if (0 == vIsZero.x &&
			0 == vIsZero.z)
			return;

		// Pivot만큼 줄여줌
		XMVector3TransformCoord(vMovePos, XMMatrixRotationY(XMConvertToRadians(270.0f)));
		vMovePos = XMVectorSetW(vMovePos, 1.f);

		// 이전 이동값과 차이를 구함
		_vector   vDifferent =	XMVectorSubtract(vMovePos, XMLoadFloat4(&m_vAnimationMove));
		vDifferent = XMVectorSetW(vDifferent, 1.f);

		// 이전 이동값과 현재 이동값 동기화
		XMStoreFloat4(&m_vAnimationMove, vMovePos);

		_float4   vMoving;
		XMStoreFloat4(&vMoving, vDifferent);

		m_pTransformCom->Go_Straight(vMoving.z * 5.f);
		m_pTransformCom->Go_Right(vMoving.x* 5.f);
	}
}

void CPlayer::Movement(_double TimeDelta)
{
	if (!m_bMove)
		return;

	CGameInstance*			pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (m_bMove && !m_bAction)
	{
		if (pGameInstance->Get_DIKeyState(DIK_W) & 0x80)
		{
			m_bRunning = true;
			m_eState = PLAYER_FM;
			m_pTransformCom->Go_Straight(TimeDelta * m_fVelocity);
		}

		if (m_eState == PLAYER_FM)
		{
			if (pGameInstance->Key_Up(DIK_W))
			{
				m_bRunning = false;
				m_eState = PLAYER_READY;
			}
		}

		if (pGameInstance->Get_DIKeyState(DIK_S) & 0x80)
		{
			m_bRunning = true;
			m_eState = PLAYER_BM;
			m_pTransformCom->Go_Backward(TimeDelta * m_fVelocity);
		}

		if (m_eState == PLAYER_BM)
		{
			if (pGameInstance->Key_Up(DIK_S))
			{
				m_bRunning = false;
				m_eState = PLAYER_READY;
			}
		}

		if (pGameInstance->Get_DIKeyState(DIK_D) & 0x80)
		{
			m_bRunning = true;
			m_eState = PLAYER_RM;
			m_pTransformCom->Go_Right(TimeDelta);
		}

		if (m_eState == PLAYER_RM)
		{
			if (pGameInstance->Key_Up(DIK_D))
			{
				m_bRunning = false;
				m_eState = PLAYER_READY;
			}
		}

		if (pGameInstance->Get_DIKeyState(DIK_A) & 0x80)
		{
			m_bRunning = true;
			m_eState = PLAYER_LM;
			m_pTransformCom->Go_Left(TimeDelta);
		}

		if (m_eState == PLAYER_LM)
		{
			if (pGameInstance->Key_Up(DIK_A))
			{
				m_bRunning = false;
				m_eState = PLAYER_READY;
			}
		}

		if (pGameInstance->Get_DIKeyState(DIK_W) & 0x80 && pGameInstance->Get_DIKeyState(DIK_D) & 0x80)
		{
			m_bRunning = true;
			m_eState = PLAYER_FR;
		}

		if (m_eState == PLAYER_FR)
		{
			if (pGameInstance->Key_Up(DIK_W))
				m_eState = PLAYER_RM;

			if (pGameInstance->Key_Up(DIK_D))
				m_eState = PLAYER_FM;
		}

		if (pGameInstance->Get_DIKeyState(DIK_W) & 0x80 && pGameInstance->Get_DIKeyState(DIK_A) & 0x80)
		{
			m_bRunning = true;
			m_eState = PLAYER_FL;
		}

		if (m_eState == PLAYER_FL)
		{
			if (pGameInstance->Key_Up(DIK_W))
				m_eState = PLAYER_LM;

			if (pGameInstance->Key_Up(DIK_A))
				m_eState = PLAYER_FM;
		}

		if (pGameInstance->Get_DIKeyState(DIK_S) & 0x80 && pGameInstance->Get_DIKeyState(DIK_D) & 0x80)
		{
			m_bRunning = true;
			m_eState = PLAYER_BR;
		}

		if (m_eState == PLAYER_BR)
		{
			if (pGameInstance->Key_Up(DIK_S))
				m_eState = PLAYER_RM;

			if (pGameInstance->Key_Up(DIK_D))
				m_eState = PLAYER_BM;
		}

		if (pGameInstance->Get_DIKeyState(DIK_S) & 0x80 && pGameInstance->Get_DIKeyState(DIK_A) & 0x80)
		{
			m_bRunning = true;
			m_eState = PLAYER_BL;
		}

		if (m_eState == PLAYER_BL)
		{
			if (pGameInstance->Key_Up(DIK_W))
				m_eState = PLAYER_LM;

			if (pGameInstance->Key_Up(DIK_D))
				m_eState = PLAYER_BM;
		}

		if (pGameInstance->Get_DIKeyState(DIK_W) & 0x80 && pGameInstance->Get_DIKeyState(DIK_C) & 0x80)
		{
			m_bRunning = false;
			m_bWalk = true;
			m_eState = PLAYER_FM;
			m_fVelocity = 0.7f;
		}

		if (m_eState == PLAYER_WALKF)
		{
			if (pGameInstance->Key_Up(DIK_C))
			{
				m_bRunning = true;
				m_bWalk = false;
				m_eState = PLAYER_FM;
				m_fVelocity = 1.f;
			}
		}

		if (pGameInstance->Get_DIKeyState(DIK_S) & 0x80 && pGameInstance->Get_DIKeyState(DIK_C) & 0x80)
		{
			m_bRunning = false;
			m_bWalk = true;
			m_eState = PLAYER_WALKB;
			m_fVelocity = 0.7f;
		}

		if (m_eState == PLAYER_WALKB)
		{
			if (pGameInstance->Key_Up(DIK_C))
			{
				m_bRunning = true;
				m_bWalk = false;
				m_eState = PLAYER_BM;
				m_fVelocity = 1.f;
			}
		}
	}
	
	if (pGameInstance->Mouse_Down(DIM_LB))
	{
		m_bAction = TRUE;

		switch (m_eState)
		{
		case PLAYER_READY:
			m_eState = PLAYER_ATTACK1;
			break;
		case PLAYER_ATTACK1:
			m_bAttack = TRUE;
			break;
		case PLAYER_ATTACK2:
			m_bAttack = TRUE;
			break;
		case PLAYER_ATTACK3:
			m_bAttack = TRUE;
			break;
		case PLAYER_FR:
			m_eState = PLAYER_ATTACK1;
		}
	}

	if (m_bAction)
	{
		if (m_eState == PLAYER_ATTACK1 && m_pModelCom->Check_AnimationSet(1.f))
		{
			if (m_bAttack)
			{
				m_eState = PLAYER_ATTACK2;
				m_bAttack = false;
				RELEASE_INSTANCE(CGameInstance);
				return;
			}

			m_eState = PLAYER_READY;
			m_bAttack = false;
			m_bAction = false;
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		if (m_eState == PLAYER_ATTACK2 && m_pModelCom->Check_AnimationSet(5.f))
		{
			if (m_bAttack)
			{
				m_eState = PLAYER_ATTACK3;
				m_bAttack = false;
				RELEASE_INSTANCE(CGameInstance);
				return;
			}
			
			m_eState = PLAYER_READY;
			m_bAttack = false;
			m_bAction = false;
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		if (m_eState == PLAYER_ATTACK3 && m_pModelCom->Check_AnimationSet(2.f))
		{
			if (m_bAttack)
			{
				m_eState = PLAYER_ATTACK1;
				m_bAttack = false;
				RELEASE_INSTANCE(CGameInstance);
				return;
			}

			m_eState = PLAYER_READY;
			m_bAttack = false;
			m_bAction = false;
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
	}

	if (pGameInstance->Mouse_Down(DIM_RB))
	{
		m_bAction = true;

		switch (m_eState)
		{
		case Client::CPlayer::PLAYER_READY:
			m_eState = PLAYER_SK03;
			break;
		default:
			break;
		}
	}

	if (m_eState == PLAYER_SK03 && m_pModelCom->Check_AnimationSet(0.4f))
	{
		m_eState = PLAYER_READY;
		m_bAttack = false;
		m_bAction = false;
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	if (pGameInstance->Key_Down(DIK_E))
	{
		m_bAction = true;
		
		switch (m_eState)
		{
		case Client::CPlayer::PLAYER_READY:
			m_eState = PLAYER_SK01;
			break;
		default:
			break;
		}
	}

	if (m_eState == PLAYER_SK01 && m_pModelCom->Check_AnimationSet(0.4f))
	{
		m_eState = PLAYER_READY;
		m_bAttack = false;
		m_bAction = false;
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	if (pGameInstance->Key_Down(DIK_Q))
	{
		m_bAction = true;

		switch (m_eState)
		{
		case Client::CPlayer::PLAYER_READY:
			m_eState = PLAYER_SK02;
			break;
		default:
			break;
		}
	}

	if (m_eState == PLAYER_SK02 && m_pModelCom->Check_AnimationSet(0.4f))
	{
		m_eState = PLAYER_READY;
		m_bAttack = false;
		m_bAction = false;
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	if (pGameInstance->Key_Down(DIK_LSHIFT))
	{
		m_bAction = true;
		m_eState = PLAYER_FOR;
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	if (m_eState == PLAYER_FOR)
	{
		if (m_pModelCom->Check_AnimationSet(0.4f))
		{
			m_eState = PLAYER_READY;
			m_bAttack = false;
			m_bAction = false;
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
	}

	if (m_bCamTurn == false && m_bAttack == false)
	{
		_long MouseMove_X = 0;

		if (MouseMove_X = pGameInstance->Get_DIMouseMove(DIMS_X))
		{
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), MouseMove_X * m_MouseSensity * TimeDelta);
		}
	}
		
	RELEASE_INSTANCE(CGameInstance);
	return;
}

void CPlayer::Imgui_RenderProperty()
{
	
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
