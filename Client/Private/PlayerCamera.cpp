#include "stdafx.h"
#include "..\Public\PlayerCamera.h"
#include "GameInstance.h"
#include "MathUtils.h"
#include "Player.h"

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

		CameraDesc.TransformDesc.fSpeedPerSec = 10.f;
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

	if (pGameInstance->Key_Down(DIK_F1))
		m_bFix = !m_bFix;

	if(m_bFix)
		Mouse_Fix();

	Safe_Release(pGameInstance);
}

void CPlayerCamera::Imgui_RenderProperty()
{
	ImGui::Text("%f", &m_fDistanceToTarget);

	ImGui::DragFloat("DistanceToTarget", &m_fDistanceToTarget, 0.01f, 0.f, 20.f);
	ImGui::DragFloat("CamY", &m_fCamImguiY, 0.01f, 0.f, 20.f);
	ImGui::DragFloat("CamLookY", &m_fCamImguiLookY, 0.01f, 0.f, 20.f);
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

void CPlayerCamera::ShakeUpdate(_double TimeDelta)
{
	if (m_fShakeDuration > 0)
	{
		_float4 Randompos = _float4(CMathUtils::GetRandomFloat(-1.f, 1.f), CMathUtils::GetRandomFloat(-1.f, 1.f), CMathUtils::GetRandomFloat(-1.f, 1.f), 1.f);
		m_vPlayerPos = m_vPlayerPos + (Randompos * m_fShakeAmount);
		m_fShakeDuration -= TimeDelta * m_fDecreaseFactor;
	}
	else
		m_fShakeDuration = 0;
}

void CPlayerCamera::Shake(_float fShakeDuration, _float fShakeAmount, _float fDecreaseFactor)
{
	m_fShakeDuration = fShakeDuration;
	m_fShakeAmount = fShakeAmount;
	m_fDecreaseFactor = fDecreaseFactor;
}

void CPlayerCamera::LinkPlayer(_double TimeDelta, CTransform* pTarget, _bool bCamTurn)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (m_vLookAt.w == -1.f)
		XMStoreFloat4(&m_vLookAt, pTarget->Get_State(CTransform::STATE_LOOK));

	if (pTarget && bCamTurn == true)
	{
		_long			MouseMove_X = 0, MouseMove_Y = 0;
		_long			MouseZoom = 0;

		if (MouseMove_X = pGameInstance->Get_DIMouseMove(DIMS_X))
		{
			XMStoreFloat4(&m_vLookAt, XMVector4Transform(XMLoadFloat4(&m_vLookAt), XMMatrixRotationY(static_cast<float>(TimeDelta) * MouseMove_X * XMConvertToRadians(90.f) * 0.1f)));
			XMStoreFloat4(&m_vLookAt, XMVector3Normalize(XMLoadFloat4(&m_vLookAt)));
		}

		if (MouseMove_Y = pGameInstance->Get_DIMouseMove(DIMS_Y))
		{
			m_fCamY += MouseMove_Y * static_cast<float>(TimeDelta);

			if (m_fCamY < -5.f)
				m_fCamY = -5.f;

			if (m_fCamY > 5.f)
				m_fCamY = 5.f;
		}

		if (MouseZoom = pGameInstance->Get_DIMouseMove(DIMS_Z))
		{
			m_fDistanceToTarget += -1.f * MouseZoom * static_cast<float>(TimeDelta);

			if (m_fDistanceToTarget < 1.f)
				m_fDistanceToTarget = 1.f;

			if (m_fDistanceToTarget > 7.f)
				m_fDistanceToTarget = 7.f;
		}

		XMStoreFloat4(&m_vPlayerPos, pTarget->Get_State(CTransform::STATE_TRANSLATION));
		_float4 vLook = CMathUtils::MulNum_Float4(-m_fDistanceToTarget, m_vLookAt);
		ShakeUpdate(TimeDelta);
		_vector vCamPos = XMVectorSet(m_vPlayerPos.x, m_vPlayerPos.y, m_vPlayerPos.z, 1.f) + (XMLoadFloat4(&vLook) + XMVectorSet(0.f, 5.f + m_fCamY, 0.f, 0.f));
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vCamPos);
		m_pTransformCom->LookAt(XMVectorSet(m_vPlayerPos.x, m_vPlayerPos.y, m_vPlayerPos.z, 1.f) + XMVectorSet(0.f, 3.f, 0.f, 0.f));
		m_bChange = false;
	}
	else
	{
		if (!m_bChange)
		{
			XMStoreFloat4(&m_vLookAt, pTarget->Get_State(CTransform::STATE_LOOK));
			XMStoreFloat4(&m_vPlayerPos, pTarget->Get_State(CTransform::STATE_TRANSLATION));
			_float4 vLook = CMathUtils::MulNum_Float4(-m_fDistanceToTarget, m_vLookAt);
			_vector vCamPos = XMVectorSet(m_vPlayerPos.x, m_vPlayerPos.y, m_vPlayerPos.z, 1.f) + (XMLoadFloat4(&vLook) + XMVectorSet(0.f, 5.f, 0.f, 0.f));

			_vector V = XMVectorSubtract(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION), vCamPos);
			_vector X = XMVector4Length(V);
			_float fDistance = XMVectorGetX(V);
			
			if (fabs(fDistance) > 0.1f)
				m_pTransformCom->Chase(vCamPos, static_cast<float>(TimeDelta) * 1.2f);
			else
				m_bChange = true;
		}
		else
		{
			XMStoreFloat4(&m_vLookAt, pTarget->Get_State(CTransform::STATE_LOOK));
			XMStoreFloat4(&m_vPlayerPos, pTarget->Get_State(CTransform::STATE_TRANSLATION));
			_float4 vLook = CMathUtils::MulNum_Float4(-m_fDistanceToTarget, m_vLookAt);
			ShakeUpdate(TimeDelta);
			_vector vCamPos = XMVectorSet(m_vPlayerPos.x, m_vPlayerPos.y, m_vPlayerPos.z, 1.f) + (XMLoadFloat4(&vLook) + XMVectorSet(0.f, 5.f, 0.f, 0.f));
			m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vCamPos);
			m_fCamY = 0.f;
		}
		m_pTransformCom->LookAt(XMVectorSet(m_vPlayerPos.x, m_vPlayerPos.y, m_vPlayerPos.z, 1.f) + XMVectorSet(0.f, 3.f, 0.f, 0.f));
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CPlayerCamera::DynamicCamera(_double TimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

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

	_long			MouseMove = 0;

	if (m_bFix)
	{
		if (MouseMove = pGameInstance->Get_DIMouseMove(DIMS_X))
		{
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), TimeDelta * MouseMove * 0.1f);
		}

		if (MouseMove = pGameInstance->Get_DIMouseMove(DIMS_Y))
		{
			m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), TimeDelta * MouseMove * 0.1f);
		}
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CPlayerCamera::Mouse_Fix()
{
	POINT pt{ static_cast<LONG>(g_iWinSizeX >> 1), static_cast<LONG>(g_iWinSizeY >> 1) };

	ClientToScreen(g_hWnd, &pt);
	SetCursorPos(pt.x, pt.y);
	//ShowCursor(false);
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
