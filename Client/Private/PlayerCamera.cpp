#include "stdafx.h"
#include "..\Public\PlayerCamera.h"
#include "GameInstance.h"
#include "MathUtils.h"

CPlayerCamera::CPlayerCamera(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCamera(pDevice, pContext)
{
}

CPlayerCamera::CPlayerCamera(const CPlayerCamera & rhs)
	: CCamera(rhs)
{
}

HRESULT CPlayerCamera::Init_Prototype()
{
	if (FAILED(__super::Init_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayerCamera::Init(void * pArg)
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

		CameraDesc.fAspect = static_cast<_float>(g_iWinSizeX / static_cast<_float>(g_iWinSizeY));

		CameraDesc.fNear = 0.02f;
		CameraDesc.fFar = 500.f;

		CameraDesc.TransformDesc.fSpeedPerSec = 2.f;
		CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	}

	if (FAILED(__super::Init(&CameraDesc)))
		return E_FAIL;

	m_strObjName = TEXT("PlayerCamera");
	
	return S_OK;
}

void CPlayerCamera::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	CGameInstance*			pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	//Mouse_Fix();

	m_pTarget = pGameInstance->Find_GameObject(LEVEL_CHAP1, L"Layer_Player", L"Player");
	
	if (m_vLookAt.w == -1.f)
		XMStoreFloat4(&m_vLookAt, m_pTarget->Get_TransformCom()->Get_State(CTransform::STATE_LOOK));
	
	_long			MouseMove_X = 0 , MouseMove_Y = 0;

	if (MouseMove_X = pGameInstance->Get_DIMouseMove(DIMS_X))
	{
		XMStoreFloat4(&m_vLookAt,XMVector4Transform(XMLoadFloat4(&m_vLookAt), XMMatrixRotationY(TimeDelta * MouseMove_X * XMConvertToRadians(90.f) * 0.1f)));
		XMStoreFloat4(&m_vLookAt, XMVector3Normalize(XMLoadFloat4(&m_vLookAt)));
	}

	if (MouseMove_Y = pGameInstance->Get_DIMouseMove(DIMS_Y))
	{
	}
	
	XMStoreFloat4(&m_vPlayerPos, m_pTarget->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION));
	_float4 vLook = CMathUtils::MulNum_Float4(-5.f, m_vLookAt);
	_vector vCamPos = XMVectorSet(m_vPlayerPos.x , m_vPlayerPos.y , m_vPlayerPos.z , 1.f) +	(XMLoadFloat4(&vLook)  +XMVectorSet(0.f, 5.f, 0.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vCamPos);
	m_pTransformCom->LookAt(XMVectorSet(m_vPlayerPos.x, m_vPlayerPos.y ,m_vPlayerPos.z , 1.f) + XMVectorSet(0.f, 3.f, 0.f, 0.f));

	Safe_Release(pGameInstance);
}

void CPlayerCamera::Imgui_RenderProperty()
{
	ImGui::InputFloat("X", &m_X);
	ImGui::InputFloat("Y", &m_Y);
	ImGui::InputFloat("Y1", &m_Y1);
	ImGui::InputFloat("Z", &m_Z);
	ImGui::InputFloat("RX", &m_RX);
	ImGui::InputFloat("RY", &m_RY);
	ImGui::InputFloat("RZ", &m_RZ);
}

void CPlayerCamera::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CPlayerCamera::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CPlayerCamera::Mouse_Fix()
{
	POINT pt{ static_cast<LONG>(g_iWinSizeX >> 1), static_cast<LONG>(g_iWinSizeY >> 1) };

	ClientToScreen(g_hWnd, &pt);
	SetCursorPos(pt.x, pt.y);
}

CPlayerCamera * CPlayerCamera::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayerCamera*	pInstance = new CPlayerCamera(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Created : CPlayerCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPlayerCamera::Clone(void * pArg)
{
	CPlayerCamera*	pInstance = new CPlayerCamera(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPlayerCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerCamera::Free()
{
	__super::Free();
}
