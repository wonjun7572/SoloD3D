#include "stdafx.h"
#include "..\Public\PrincePlayer.h"
#include "PlayerCamera.h"
#include "GameInstance.h"
#include "FSMComponent.h"
#include "Horse.h"
#include "Animation.h"

CPrincePlayer::CPrincePlayer(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CGameObject(pDevice, pContext)
{
}

CPrincePlayer::CPrincePlayer(const CPrincePlayer & rhs)
	:CGameObject(rhs)
{
}

HRESULT CPrincePlayer::Init_Prototype()
{
	if (FAILED(__super::Init_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPrincePlayer::Init(void * pArg)
{
	CGameObject::GAMEOBJECTDESC			GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof GameObjectDesc);

	GameObjectDesc.TransformDesc.fSpeedPerSec = 7.0f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Init(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_strObjName = L"Prince";

	if(g_LEVEL == LEVEL_CHAP1)
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(34.383572f, 0.f, 80.610695f, 1.f));
	else if(g_LEVEL == LEVEL_CHAP2)
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(320.f, 0.f, 30.f, 1.f));

	m_pNavigationCom->Set_CurreuntIndex(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

	SetUp_FSM();
	SetUp_HorseFSM();

	return S_OK;
}

void CPrincePlayer::Tick(_double TimeDelta)
{
	if (m_bSetMove)
	{
		__super::Tick(TimeDelta);

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		if (g_LEVEL == LEVEL_CHAP1 && m_pHorse == nullptr)
			m_pHorse = static_cast<CHorse*>(pGameInstance->Find_GameObject(LEVEL_CHAP1, L"Layer_Horse", L"Horse"));
		else if (g_LEVEL == LEVEL_CHAP2 && m_pHorse == nullptr)
			m_pHorse = static_cast<CHorse*>(pGameInstance->Find_GameObject(LEVEL_CHAP2, L"Layer_Horse", L"Horse"));
		RELEASE_INSTANCE(CGameInstance);

		if (!m_bCamChange)
			Movement(TimeDelta);

		if (!m_bRideHorse)
		{
			m_pFSM->Tick(TimeDelta);
			m_pTransformCom->Set_SpeedPerSec(7.f);
		}
		else
		{
			m_pHorseFSM->Tick(TimeDelta);
			m_pTransformCom->Set_SpeedPerSec(14.f);
			m_pHorse->LinkPrince(TimeDelta);
		}

		m_pModelCom->Play_Animation(TimeDelta);

		AdditiveAnim(TimeDelta);

		LinkObject(TimeDelta);
	}
	else
	{
		m_bRideHorse = false;
	}
}

void CPrincePlayer::Late_Tick(_double TimeDelta)
{
	if (m_bSetMove)
	{
		__super::Late_Tick(TimeDelta);

		for (_uint i = 0; i < COLLTYPE_END; ++i)
			m_pColliderCom[i]->Update(m_pTransformCom->Get_WorldMatrix());

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		if (nullptr != m_pRendererCom &&
			true == pGameInstance->isInFrustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION), 2.f))
		{
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOW, this);
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
		}
		RELEASE_INSTANCE(CGameInstance);
	}
}

HRESULT CPrincePlayer::Render()
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

HRESULT CPrincePlayer::RenderShadow()
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

void CPrincePlayer::SetUp_FSM()
{
	m_pFSM = CFSMComponentBuilder()
		.InitState("Idle")
		.AddState("Idle")
		.Tick(this, &CPrincePlayer::Idle_Tick)
		.AddTransition("IDLE to Move", "Move")
		.Predicator([this]()
	{
		return m_bMove;
	})
	
		.AddState("Move")
		.AddTransition("Move to Walk", "Walk")
		.Predicator([this]()
	{
		return m_bWalking && m_bMove;
	})
		.AddTransition("Move to Run", "Run")
		.Predicator([this]()
	{
		return m_bRunning && m_bMove;
	})
		.AddTransition("Move to Idle", "Idle")
		.Predicator([this]()
	{
		return !m_bMove;
	})
	
		/* For. walk */
		.AddState("Walk")
		.Tick(this, &CPrincePlayer::Walk_Tick)
		.AddTransition("Walk to Move", "Move")
		.Predicator([this]()
	{
		return !m_bMove;
	})
		.AddTransition("Walk to Run", "Run")
		.Predicator([this]()
	{
		return m_bRunning && m_bMove;
	})

		/* For. run */
		.AddState("Run")
		.Tick(this, &CPrincePlayer::Run_Tick)
		.AddTransition("Run to Move", "Move")
		.Predicator([this]()
	{
		return !m_bMove;
	})
		.AddTransition("Run to Walk", "Walk")
		.Predicator([this]()
	{
		return m_bWalking && m_bMove;
	})

		.Build();
}

void CPrincePlayer::SetUp_HorseFSM()
{
	m_pHorseFSM = CFSMComponentBuilder()
		.InitState("Idle")
		.AddState("Idle")
		.Tick([this](_double TimeDelta)
	{
		m_pModelCom->Set_AnimationIndex(PRINCE_mount_idle_horse);
		m_pHorse->Set_Anim(CHorse::HORSE_IDLE);
	})
		.AddTransition("Idle to Move", "Move")
		.Predicator([this]()
	{
		return m_bMove;
	})
		
		.AddState("Move")
		.AddTransition("Move to Run", "Run")
		.Predicator([this]()
	{
		return m_bRunning && m_bMove;
	})
		.AddTransition("Move to Idle", "Idle")
		.Predicator([this]()
	{
		return !m_bMove;
	})

		.AddState("Run")
		.Tick([this](_double TimeDelta)
	{
		if (m_eState == CPrincePlayer::PLAYER_FM)
		{
			m_pModelCom->Set_AnimationIndex(PRINCE_Mount_Run_F);
			m_pHorse->Set_Anim(CHorse::HORSE_RUN_F);
		}
	})
		.AddTransition("Run to Move", "Move")
		.Predicator([this]()
	{
		return !m_bMove;
	})
		
		.Build();
}

void CPrincePlayer::Movement(_double TimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance)

		if (m_pNavigationCom != nullptr)
		{
			if (!m_bRideHorse)
			{
				_float yPos = 0.f;
				m_pNavigationCom->isHeighit_OnNavigation(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION), &yPos);

				if (m_bJump)
				{
					static _float fGravity = 8.81f;
					_float4 vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
					_float	fJumpPower = yPos + m_fJumpPower;
					_float fVelocity = sqrtf(fabsf(fJumpPower * 2.f * fGravity));

					if (vPos.y >= fVelocity)
						fGravity *= -1.f;

					_float y = max(vPos.y + fGravity * static_cast<_float>(TimeDelta), yPos);

					if (y - yPos <= 0.00001f)
					{
						fGravity *= -1.f;
						m_bJump = false;
					}

					m_pTransformCom->Set_Height(y);
				}
				else
					m_pTransformCom->Set_Height(yPos);
			}
			else
			{
				_float yPos = 0.f;
				m_pNavigationCom->isHeighit_OnNavigation(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION), &yPos);

				if (m_bJump)
				{
					static _float fGravity = 8.81f;
					_float4 vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
					_float	fJumpPower = yPos + m_fJumpPower;
					_float fVelocity = sqrtf(fabsf(fJumpPower * 2.f * fGravity));

					if (vPos.y >= fVelocity)
						fGravity *= -1.f;

					_float y = max(vPos.y + fGravity * static_cast<_float>(TimeDelta), yPos);

					if (y - yPos <= 0.00001f)
					{
						fGravity *= -1.f;
						m_bJump = false;
					}

					m_pTransformCom->Set_Height(y + 1.2f);
				}
				else
					m_pTransformCom->Set_Height(yPos + 1.2f);
			}
		}

	if (pGameInstance->Key_Down(DIK_G))
		m_bRideHorse = !m_bRideHorse;

	if (!pGameInstance->Key_Pressing(DIK_W) &&
		!pGameInstance->Key_Pressing(DIK_A) &&
		!pGameInstance->Key_Pressing(DIK_S) &&
		!pGameInstance->Key_Pressing(DIK_D))
		m_bMove = false;
	else
		m_bMove = true;

	if (m_bMove)
	{
		if (pGameInstance->Get_DIKeyState(DIK_W) & 0x80)
		{
			m_bRunning = true;
			m_eState = PLAYER_FM;
			m_pTransformCom->Go_Straight(TimeDelta * m_fVelocity, m_pNavigationCom);
		}

		if (m_eState == PLAYER_FM)
		{
			if (pGameInstance->Key_Up(DIK_W))
			{
				m_bRunning = false;
			}
		}

		if (pGameInstance->Get_DIKeyState(DIK_S) & 0x80)
		{
			m_bRunning = true;
			m_eState = PLAYER_BM;
			m_pTransformCom->Go_Backward(TimeDelta * m_fVelocity, m_pNavigationCom);
		}

		if (m_eState == PLAYER_BM)
		{
			if (pGameInstance->Key_Up(DIK_S))
			{
				m_bRunning = false;
			}
		}

		if (pGameInstance->Get_DIKeyState(DIK_D) & 0x80)
		{
			m_bRunning = true;
			m_eState = PLAYER_RM;
			m_pTransformCom->Go_Right(TimeDelta, m_pNavigationCom);
		}

		if (m_eState == PLAYER_RM)
		{
			if (pGameInstance->Key_Up(DIK_D))
			{
				m_bRunning = false;
			}
		}

		if (pGameInstance->Get_DIKeyState(DIK_A) & 0x80)
		{
			m_bRunning = true;
			m_eState = PLAYER_LM;
			m_pTransformCom->Go_Left(TimeDelta, m_pNavigationCom);
		}

		if (m_eState == PLAYER_LM)
		{
			if (pGameInstance->Key_Up(DIK_A))
			{
				m_bRunning = false;
			}
		}

		if (pGameInstance->Get_DIKeyState(DIK_W) & 0x80 && pGameInstance->Get_DIKeyState(DIK_D) & 0x80)
		{
			m_bRunning = true;
			m_eState = PLAYER_FR;
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), TimeDelta);
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
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), -1.0 * TimeDelta);
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
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), -1.0 * TimeDelta);
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
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), TimeDelta);
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
			m_bWalking = true;
			m_eState = PLAYER_FM;
			m_fVelocity = 0.7f;
		}

		if (m_eState == PLAYER_FM)
		{
			if (pGameInstance->Key_Up(DIK_C))
			{
				m_bRunning = true;
				m_bWalking = false;
				m_eState = PLAYER_FM;
				m_fVelocity = 1.f;
			}
		}

		if (pGameInstance->Get_DIKeyState(DIK_S) & 0x80 && pGameInstance->Get_DIKeyState(DIK_C) & 0x80)
		{
			m_bRunning = false;
			m_bWalking = true;
			m_eState = PLAYER_BM;
			m_fVelocity = 0.7f;
		}

		if (m_eState == PLAYER_BM)
		{
			if (pGameInstance->Key_Up(DIK_C))
			{
				m_bRunning = true;
				m_bWalking = false;
				m_eState = PLAYER_BM;
				m_fVelocity = 1.f;
			}
		}		
	}

	if (!m_bCamTurn && m_bPlayerTurn)
	{
		_long MouseMove_X = 0;

		if (MouseMove_X = pGameInstance->Get_DIMouseMove(DIMS_X))
		{
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), MouseMove_X * m_MouseSensity * TimeDelta);
		}
	}

	RELEASE_INSTANCE(CGameInstance)
}

void CPrincePlayer::AdditiveAnim(_double TimeDelta)
{
}

void CPrincePlayer::LinkObject(_double TimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance)

	m_pCam = static_cast<CPlayerCamera*>(pGameInstance->Find_GameObject(pGameInstance->GetCurLevelIdx(), L"Layer_Camera", L"PlayerCamera"));

	if (pGameInstance->Key_Down(DIK_F2))
		m_bCamChange = !m_bCamChange;

	if (m_pCam != nullptr && !m_bCamChange)
		m_pCam->LinkPlayer(TimeDelta, m_pTransformCom, m_bCamTurn);
	else if (m_pCam != nullptr && m_bCamChange)
		m_pCam->DynamicCamera(TimeDelta);

	RELEASE_INSTANCE(CGameInstance)
}

void CPrincePlayer::Reset_Anim(ANIMATION eAnim)
{
	m_pModelCom->Reset_AnimPlayTime(eAnim);
}

void CPrincePlayer::Set_Anim(ANIMATION eAnim)
{
	m_pModelCom->Set_AnimationIndex(eAnim);
}

void CPrincePlayer::Imgui_RenderProperty()
{
}

void CPrincePlayer::Idle_Tick(_double TimeDelta)
{
	m_pModelCom->Set_AnimationIndex(PRINCE_Idle_WP);
}

void CPrincePlayer::Walk_Tick(_double TImeDelat)
{
	if (m_ePreState != m_eState)
	{
		switch (m_eState)
		{
		case Client::CPrincePlayer::PLAYER_FM:
			Reset_Anim(PRINCE_Walk_F);
			break;
		case Client::CPrincePlayer::PLAYER_BM:
			Reset_Anim(PRINCE_Walk_B);
			break;
		}

		m_ePreState = m_eState;
	}
}

void CPrincePlayer::Run_Tick(_double TimeDelta)
{
	switch (m_eState)
	{
	case Client::CPrincePlayer::PLAYER_FM:
		m_pModelCom->Set_AnimationIndex(PRINCE_Run_F_WP);
		break;
	case Client::CPrincePlayer::PLAYER_BM:
		m_pModelCom->Set_AnimationIndex(PRINCE_Run_B_WP);
		break;
	case Client::CPrincePlayer::PLAYER_RM:
		m_pModelCom->Set_AnimationIndex(PRINCE_Run_R_WP);
		break;
	case Client::CPrincePlayer::PLAYER_LM:
		m_pModelCom->Set_AnimationIndex(PRINCE_Run_L_WP);
		break;
	case Client::CPrincePlayer::PLAYER_FR:
		m_pModelCom->Set_AnimationIndex(PRINCE_Run_FR_WP);
		break;
	case Client::CPrincePlayer::PLAYER_FL:
		m_pModelCom->Set_AnimationIndex(PRINCE_Run_FL_WP);
		break;
	case Client::CPrincePlayer::PLAYER_BR:
		m_pModelCom->Set_AnimationIndex(PRINCE_Run_BR_WP);
		break;
	case Client::CPrincePlayer::PLAYER_BL:
		m_pModelCom->Set_AnimationIndex(PRINCE_Run_BL_WP);
		break;
	}
}

_bool CPrincePlayer::AnimFinishChecker(ANIMATION eAnim, _double FinishRate)
{
	return m_pModelCom->Get_IndexAnim(eAnim)->Get_PlayRate() >= FinishRate;
}

_bool CPrincePlayer::AnimIntervalChecker(ANIMATION eAnim, _double StartRate, _double FinishRate)
{
	if (m_pModelCom->Get_IndexAnim(eAnim)->Get_PlayRate() > StartRate &&
		m_pModelCom->Get_IndexAnim(eAnim)->Get_PlayRate() <= FinishRate)
	{
		return true;
	}

	return false;
}

void CPrincePlayer::AnimEditPlayTime(ANIMATION eAnim, _double PlayTime)
{
	m_pModelCom->Get_IndexAnim(eAnim)->Set_MulSecond(PlayTime);
}

HRESULT CPrincePlayer::SetUp_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Model_PrinceH"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC			ColliderDesc;

	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(0.3f, 1.1f, 0.3f);
	ColliderDesc.vCenter = _float3(0.f, 1.3f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_AABB"),
		(CComponent**)&m_pColliderCom[COLLTYPE_AABB], &ColliderDesc)))
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

HRESULT CPrincePlayer::SetUp_ShaderResources()
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
	if (FAILED(m_pShaderCom->Set_RawValue("g_vRimColor", &m_vRimColor, sizeof(_float4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CPrincePlayer::SetUP_ShadowShaderResources()
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

CPrincePlayer * CPrincePlayer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPrincePlayer*		pInstance = new CPrincePlayer(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Created : CPrincePlayer");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPrincePlayer::Clone(void * pArg)
{
	CPrincePlayer*		pInstance = new CPrincePlayer(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPrincePlayer");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPrincePlayer::Free()
{
	__super::Free();
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pNavigationCom);

	for (_uint i = 0; i < COLLTYPE_END; ++i)
		Safe_Release(m_pColliderCom[i]);

	Safe_Release(m_pFSM);
	Safe_Release(m_pHorseFSM);
}