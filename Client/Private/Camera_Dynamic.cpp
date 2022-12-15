#include "stdafx.h"
#include "..\Public\Camera_Dynamic.h"
#include "GameInstance.h"
#include "Bone.h"

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

		CameraDesc.fNear = 0.02f;
		CameraDesc.fFar = 500.f;

		CameraDesc.TransformDesc.fSpeedPerSec = 2.f;
		CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	}

	if (FAILED(__super::Init(&CameraDesc)))
		return E_FAIL;

	m_fSensitivity = 0.1f;

	m_strObjName = TEXT("Camera");

	return S_OK;
}

void CCamera_Dynamic::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	CGameInstance*			pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	m_pCharacterTransform[CHARACTER_MOMOI] = pGameInstance->Find_GameObject(LEVEL_CHAP1, L"Layer_Character", L"Momoi")->Get_TransformCom();
	m_pCharacterTransform[CHARACTER_IZUNA] = pGameInstance->Find_GameObject(LEVEL_CHAP1, L"Layer_Character", L"Izuna")->Get_TransformCom();
	m_pCharacterTransform[CHARACTER_AZUSA] = pGameInstance->Find_GameObject(LEVEL_CHAP1, L"Layer_Character", L"Azusa")->Get_TransformCom();
	m_pCharacterTransform[CHARACTER_MIDORI] = pGameInstance->Find_GameObject(LEVEL_CHAP1, L"Layer_Character", L"Midori")->Get_TransformCom();

	switch (m_eState)
	{
	case STATE_DYNAMIC:
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
	case STATE_TPS:
		if (m_pCharacterTransform[CHARACTER_MOMOI] != nullptr && m_eCharacter == CCamera_Dynamic::CHARACTER_MOMOI)
		{
			_matrix 	TransformMat = XMMatrixAffineTransformation(XMVectorSet(0.5f, 0.5f, 0.5f, 0.f), XMVectorSet(0.f, 0.f, 0.f, 1.f),
				XMQuaternionRotationRollPitchYaw(0.1f, 0.f, 0.f), XMVectorSet(0.f, 1.5f, -2.5f, 1.f)) *
				m_pCharacterTransform[CHARACTER_MOMOI]->Get_WorldMatrix();

			m_pTransformCom->SetWorldMatrix(TransformMat);
		}

		if (m_pCharacterTransform[CHARACTER_IZUNA] != nullptr && m_eCharacter == CCamera_Dynamic::CHARACTER_IZUNA)
		{
			_matrix 	TransformMat = XMMatrixAffineTransformation(XMVectorSet(0.5f, 0.5f, 0.5f, 0.f), XMVectorSet(0.f, 0.f, 0.f, 1.f),
				XMQuaternionRotationRollPitchYaw(0.1f, 0.f, 0.f), XMVectorSet(0.f, 1.5f, -2.5f, 1.f)) *
				m_pCharacterTransform[CHARACTER_IZUNA]->Get_WorldMatrix();

			m_pTransformCom->SetWorldMatrix(TransformMat);
		}

		if (m_pCharacterTransform[CHARACTER_AZUSA] != nullptr && m_eCharacter == CCamera_Dynamic::CHARACTER_AZUSA)
		{
			_matrix 	TransformMat = XMMatrixAffineTransformation(XMVectorSet(0.5f, 0.5f, 0.5f, 0.f), XMVectorSet(0.f, 0.f, 0.f, 1.f),
				XMQuaternionRotationRollPitchYaw(0.1f, 0.f, 0.f), XMVectorSet(0.f, 1.5f, -2.5f, 1.f)) *
				m_pCharacterTransform[CHARACTER_AZUSA]->Get_WorldMatrix();

			m_pTransformCom->SetWorldMatrix(TransformMat);
		}

		if (m_pCharacterTransform[CHARACTER_MIDORI] != nullptr && m_eCharacter == CCamera_Dynamic::CHARACTER_MIDORI)
		{
			_matrix 	TransformMat = XMMatrixAffineTransformation(XMVectorSet(0.5f, 0.5f, 0.5f, 0.f), XMVectorSet(0.f, 0.f, 0.f, 1.f),
				XMQuaternionRotationRollPitchYaw(0.1f, 0.f, 0.f), XMVectorSet(0.f, 1.5f, -2.5f, 1.f)) *
				m_pCharacterTransform[CHARACTER_MIDORI]->Get_WorldMatrix();

			m_pTransformCom->SetWorldMatrix(TransformMat);
		}

		break;
	case STATE_ORIGINAL:
		{
			m_pTransformCom->LookAt(m_pCharacterTransform[CHARACTER_IZUNA]->Get_State(CTransform::STATE_TRANSLATION));
			_float3 vPos;
			XMStoreFloat3(&vPos,m_pCharacterTransform[CHARACTER_IZUNA]->Get_State(CTransform::STATE_TRANSLATION));
			_float3 vCamPos;
			XMStoreFloat3(&vCamPos,	m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
			
			if(CMathUtils::Distance(_float3(vPos.x + 8.f, vPos.y + 8.f, vPos.z - 5.f), vCamPos) > 5.f)
				m_pTransformCom->Chase(XMLoadFloat4(&_float4(vPos.x + 5.f, vPos.y + 8.f, vPos.z - 2.f, 1.f)), TimeDelta);
			else
				m_pTransformCom->Chase(XMLoadFloat4(&_float4(vPos.x + 5.f, vPos.y + 8.f, vPos.z - 2.f, 1.f)), TimeDelta * 0.5f);
		}
		break;
	default:
		break;
	}

	Safe_Release(pGameInstance);
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

void CCamera_Dynamic::Imgui_RenderProperty()
{
	ImGui::InputFloat("X", &m_X);
	ImGui::InputFloat("Y", &m_Y);
	ImGui::InputFloat("Z", &m_Z);
	ImGui::InputFloat("RX", &m_RX);
	ImGui::InputFloat("RY", &m_RY);
	ImGui::InputFloat("RZ", &m_RZ);

	static int e = 0;
	ImGui::RadioButton("Original", &e, 0); ImGui::SameLine();
	ImGui::RadioButton("TPS", &e, 1); ImGui::SameLine();
	ImGui::RadioButton("Dynamic", &e, 2);

	if (e == 0)
		m_eState = STATE_ORIGINAL;
	if (e == 1)
		m_eState = STATE_TPS;
	if (e == 2)
		m_eState = STATE_DYNAMIC;

	static int a = 0;
	ImGui::RadioButton("Momoi", &a, 0); ImGui::SameLine();
	ImGui::RadioButton("Izuna", &a, 1); ImGui::SameLine();
	ImGui::RadioButton("Azusa", &a, 2); ImGui::SameLine();
	ImGui::RadioButton("Midori", &a, 3);

	if (a == 0)
		m_eCharacter = CHARACTER_MOMOI;
	if (a == 1)
		m_eCharacter = CHARACTER_IZUNA;
	if (a == 2)
		m_eCharacter = CHARACTER_AZUSA;
	if (a == 3)
		m_eCharacter = CHARACTER_MIDORI;
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
}
