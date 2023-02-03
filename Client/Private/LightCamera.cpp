#include "stdafx.h"
#include "..\Public\LightCamera.h"
#include "GameInstance.h"
#include "PipeLine.h"

CLightCamera::CLightCamera(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CCamera(pDevice, pContext)
{
}

CLightCamera::CLightCamera(const CLightCamera & rhs)
	:CCamera(rhs)
{
}

HRESULT CLightCamera::Init_Prototype()
{
	if (FAILED(__super::Init_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLightCamera::Init(void * pArg)
{
	CCamera::CAMERADESC			CameraDesc;
	ZeroMemory(&CameraDesc, sizeof CameraDesc);

	if (nullptr != pArg)
		memcpy(&CameraDesc, pArg, sizeof(CAMERADESC));
	else if(g_LEVEL == LEVEL_CHAP1)
	{
		CameraDesc.vEye = _float4(-5.f, 50.f, -5.f, 1.f);
		CameraDesc.vAt = _float4(60.f, 0.f, 60.f, 1.f);
		CameraDesc.vUp = _float4(0.f, 1.f, 0.f, 0.f);

		CameraDesc.fFovy = XMConvertToRadians(60.f);

		CameraDesc.fAspect = static_cast<_float>(g_iWinSizeX / static_cast<_float>(g_iWinSizeY));

		CameraDesc.fNear = 0.2f;
		CameraDesc.fFar = 500.f;

		CameraDesc.TransformDesc.fSpeedPerSec = 10.f;
		CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	}
	else if(g_LEVEL == LEVEL_CHAP2)
	{
		CameraDesc.vEye = _float4(448.828f, 111.951f, 168.571f, 1.f);
		CameraDesc.vAt = _float4(261.089f, -10.f, 226.346f, 1.f);
		CameraDesc.vUp = _float4(0.f, 1.f, 0.f, 0.f);

		CameraDesc.fFovy = XMConvertToRadians(60.f);

		CameraDesc.fAspect = static_cast<_float>(g_iWinSizeX / static_cast<_float>(g_iWinSizeY));

		CameraDesc.fNear = 0.2f;
		CameraDesc.fFar = 500.f;

		CameraDesc.TransformDesc.fSpeedPerSec = 10.f;
		CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	}

	if (FAILED(__super::Init(&CameraDesc)))
		return E_FAIL;

	return S_OK;
}

void CLightCamera::Tick(_double TimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance)
	pGameInstance->Set_Transform(CPipeLine::D3DTS_LIGHTVIEW, m_pTransformCom->Get_WorldMatrixInverse());
	//원래의 투영행렬을 던져주면 됨
	RELEASE_INSTANCE(CGameInstance)
}

void CLightCamera::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CLightCamera::Render()
{
	return S_OK;
}

CLightCamera * CLightCamera::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLightCamera*	pInstance = new CLightCamera(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Created : CLightCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CLightCamera::Clone(void* pArg)
{
	CLightCamera*	pInstance = new CLightCamera(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Cloned : CLightCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLightCamera::Free()
{
	__super::Free();
}
