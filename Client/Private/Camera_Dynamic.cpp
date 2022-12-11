#include "stdafx.h"
#include "..\Public\Camera_Dynamic.h"
#include "GameInstance.h"
#include "Bone.h"
#include "Sheila.h"

CCamera_Dynamic::CCamera_Dynamic(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera(pDevice, pContext)
{
}

CCamera_Dynamic::CCamera_Dynamic(const CCamera_Dynamic & rhs)
	: CCamera(rhs)
{
}

HRESULT CCamera_Dynamic::Init_Prototype()
{
	if (FAILED(__super::Init_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCamera_Dynamic::Init(void* pArg)
{
	CCamera::CAMERADESC			CameraDesc;
	ZeroMemory(&CameraDesc, sizeof CameraDesc);

	if (nullptr != pArg)
		memcpy(&CameraDesc, pArg, sizeof(CAMERADESC));
	else
	{
		CameraDesc.vEye = _float4(0.f, 10.f, -10.f, 1.f);
		CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
		CameraDesc.vUp = _float4(0.f, 1.f, 0.f, 0.f);

		CameraDesc.fFovy = XMConvertToRadians(60.f);
		
		CameraDesc.fAspect = static_cast<_float>( g_iWinSizeX / static_cast<_float>(g_iWinSizeY));

		CameraDesc.fNear = 0.2f;
		CameraDesc.fFar = 500.f;

		CameraDesc.TransformDesc.fSpeedPerSec = 5.f;
		CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	}

	if (FAILED(__super::Init(&CameraDesc)))
		return E_FAIL;

	m_fSensitivity = 0.1f;

	return S_OK;
}

void CCamera_Dynamic::Tick(_double TimeDelta)
{
	SwitchCam(TimeDelta);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Key_Down(DIK_F1))
		m_bStatic = !m_bStatic;

	if (m_bStatic == true)
		m_eType = CCamera_Dynamic::TYPE_STATIC;

	else if (!m_bStatic)
		m_eType = CCamera_Dynamic::TYPE_DYNAMIC;

	RELEASE_INSTANCE(CGameInstance);

	__super::Tick(TimeDelta);
}

void CCamera_Dynamic::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CCamera_Dynamic::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;
	
	return S_OK;
}

void CCamera_Dynamic::SwitchCam(_double TimeDelta)
{
	CGameInstance*			pGameInstance =GET_INSTANCE(CGameInstance);
	_matrix TransformMat;
	m_pPlayer = pGameInstance->Find_GameObject(LEVEL_CHAP1, TEXT("Layer_Player"), TEXT("Sheila"));
	switch (m_eType)
	{
	case Client::CCamera_Dynamic::TYPE_DYNAMIC:
		if (pGameInstance->Get_DIKeyState(DIK_W))
		{
			m_pTransformCom->Go_Straight(TimeDelta);
		}

		if (pGameInstance->Get_DIKeyState(DIK_S))
		{
			m_pTransformCom->Go_Backward(TimeDelta);
		}

		if (pGameInstance->Get_DIKeyState(DIK_A))
		{
			m_pTransformCom->Go_Left(TimeDelta);
		}

		if (pGameInstance->Get_DIKeyState(DIK_D))
		{
			m_pTransformCom->Go_Right(TimeDelta);
		}

		if (pGameInstance->Key_Down(DIK_T))
		{
			m_bFix = !m_bFix;
		}
		if (!m_bFix)
		{
			Mouse_Fix();

			_long			MouseMove = 0;

			if (MouseMove = pGameInstance->Get_DIMouseMove(DIMS_X))
			{
				m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), TimeDelta * MouseMove * m_fSensitivity);
			}

			if (MouseMove = pGameInstance->Get_DIMouseMove(DIMS_Y))
			{
				m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), TimeDelta * MouseMove * m_fSensitivity);
			}
		}
		break;
	case Client::CCamera_Dynamic::TYPE_STATIC:
		// 뼈에 붙일 때
		//m_pPlayer = pGameInstance->Find_GameObject(LEVEL_CHAP1, TEXT("Layer_Player"), TEXT("Sheila"));
		//if(m_pPlayer != nullptr)
		//	m_pPlayerBone = static_cast<CModel*>(m_pPlayer->Find_Component(TEXT("Com_Model")))->Get_BonePtr("Bip001");
		//{
		//	// 라이플 일때
		//	_float4x4 OffsetMat = CMathUtils::Mul_Matrix(m_pPlayerBone->Get_CombindMatrix(),
		//		static_cast<CModel*>(m_pPlayer->Find_Component(TEXT("Com_Model")))->Get_PivotMatrix());
		//	
		//	_matrix TransformMat = XMMatrixAffineTransformation(XMVectorSet(0.5f, 0.5f, 0.5f, 0.f), XMVectorSet(0.f, 0.f, 0.f, 1.f),
		//		XMQuaternionRotationRollPitchYaw(0.f, 1.75f, -1.575f), XMVectorSet(-0.05f, 0.f, -0.28f, 1.f)) *
		//		XMLoadFloat4x4(&OffsetMat) *
		//		m_pPlayer->Get_TransformCom()->Get_WorldMatrix();
		//	
		//	m_pTransformCom->SetWorldMatrix(TransformMat);
		//}
		switch (static_cast<CSheila*>(m_pPlayer)->Get_WeaponType())
		{
		case CSheila::TYPE_RIFLE:
			TransformMat = XMMatrixAffineTransformation(XMVectorSet(0.5f, 0.5f, 0.5f, 0.f), XMVectorSet(0.f, 0.f, 0.f, 1.f),
				XMQuaternionRotationRollPitchYaw(m_RX, m_RY, m_RZ), XMVectorSet(m_X, 0.65f, m_Z, 1.f)) *
				m_pPlayer->Get_TransformCom()->Get_WorldMatrix();
			m_pTransformCom->SetWorldMatrix(TransformMat);
			break;
		case CSheila::TYPE_SHOTGUN:
			TransformMat = XMMatrixAffineTransformation(XMVectorSet(0.5f, 0.5f, 0.5f, 0.f), XMVectorSet(0.f, 0.f, 0.f, 1.f),
				XMQuaternionRotationRollPitchYaw(m_RX, m_RY, m_RZ), XMVectorSet(m_X, 0.7f, m_Z, 1.f)) *
				m_pPlayer->Get_TransformCom()->Get_WorldMatrix();
			m_pTransformCom->SetWorldMatrix(TransformMat);
			break;
		case CSheila::TYPE_PISTOL:
			TransformMat = XMMatrixAffineTransformation(XMVectorSet(0.5f, 0.5f, 0.5f, 0.f), XMVectorSet(0.f, 0.f, 0.f, 1.f),
				XMQuaternionRotationRollPitchYaw(m_RX, m_RY, m_RZ), XMVectorSet(m_X, 0.8f, m_Z, 1.f)) *
				m_pPlayer->Get_TransformCom()->Get_WorldMatrix();
			m_pTransformCom->SetWorldMatrix(TransformMat);
			break;
		case CSheila::TYPE_SNIPER:
			TransformMat = XMMatrixAffineTransformation(XMVectorSet(0.5f, 0.5f, 0.5f, 0.f), XMVectorSet(0.f, 0.f, 0.f, 1.f),
				XMQuaternionRotationRollPitchYaw(m_RX, m_RY, m_RZ), XMVectorSet(m_X, 0.55f, m_Z, 1.f)) *
				m_pPlayer->Get_TransformCom()->Get_WorldMatrix();
			m_pTransformCom->SetWorldMatrix(TransformMat);
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CCamera_Dynamic::Imgui_RenderProperty()
{
	ImGui::DragFloat("X", &m_X, 0.01f, -10.f, 10.f);
	ImGui::DragFloat("Y", &m_Y, 0.01f, -10.f, 10.f);
	ImGui::DragFloat("Z", &m_Z, 0.01f, -10.f, 10.f);
	ImGui::DragFloat("RX", &m_RX, 0.01f, -180.f, 180.f);
	ImGui::DragFloat("RY", &m_RY, 0.01f, -180.f, 180.f);
	ImGui::DragFloat("RZ", &m_RZ, 0.01f, -180.f, 180.f);
	ImGui::DragFloat("SX", &m_SX, 0.01f, 0.f, 180.f);
	ImGui::DragFloat("SY", &m_SY, 0.01f, 0.f, 180.f);
	ImGui::DragFloat("SZ", &m_SZ, 0.01f, 0.f, 180.f);
}

void CCamera_Dynamic::Mouse_Fix()
{
	POINT pt{ static_cast<LONG>(g_iWinSizeX >> 1), static_cast<LONG>(g_iWinSizeY >> 1) };

	ClientToScreen(g_hWnd, &pt);
	SetCursorPos(pt.x, pt.y);
}

CCamera_Dynamic * CCamera_Dynamic::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamera_Dynamic*	pInstance = new CCamera_Dynamic(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Created : CCamera_Dynamic");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCamera_Dynamic::Clone(void* pArg)
{
	CCamera_Dynamic*	pInstance = new CCamera_Dynamic(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCamera_Dynamic");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Dynamic::Free()
{
	__super::Free();

	Safe_Release(m_pPlayer);
	Safe_Release(m_pPlayerBone);
}
