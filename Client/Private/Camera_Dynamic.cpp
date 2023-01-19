#include "stdafx.h"
#include "..\Public\Camera_Dynamic.h"
#include "GameInstance.h"
#include "PlayerCamera.h"

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
		CameraDesc.fFar = 300.f;

		CameraDesc.TransformDesc.fSpeedPerSec = 10.f;
		CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	}

	if (FAILED(__super::Init(&CameraDesc)))
		return E_FAIL;

	m_strObjName = TEXT("DynamicCamera");

	if (g_LEVEL == LEVEL_CHAP1)
	{
		m_CheckPoints.push_back(_float4(37.f, 2.5f, 67.f, 1.f));
		m_CheckPoints.push_back(_float4(31.f, 1.7f, 25.f, 1.f));
		m_CheckPoints.push_back(_float4(10.f, 2.f, 8.f, 1.f));
		m_CheckPoints.push_back(_float4(0.2f, 5.f, -1.5f, 1.f));
	}
	else if (g_LEVEL == LEVEL_CHAP2)
	{
		m_CheckPoints.push_back(_float4(278.f, 39.f, 300.f, 1.f));
		m_CheckPoints.push_back(_float4(314.f, 6.3f, 400.f, 1.f));
		m_CheckPoints.push_back(_float4(310.5f, 9.f, 390.f, 1.f));

		m_CheckPoints.push_back(_float4(150.f, 30.f, 320.f, 1.f));
		m_CheckPoints.push_back(_float4(110.f, 6.f, 367.f, 1.f));
		m_CheckPoints.push_back(_float4(116.f, 12.f, 355.f, 1.f));
		m_CheckPoints.push_back(_float4(128.f, 2.7f, 270.f, 1.f));

		m_CheckPoints.push_back(_float4(56.f, 6.f, 193.f, 1.f));
		m_CheckPoints.push_back(_float4(65.f, 11.f, 200.f, 1.f));
		m_CheckPoints.push_back(_float4(265.f, 50.f, 250.f, 1.f));
		m_CheckPoints.push_back(_float4(316.f, 2.f, 25.f, 1.f));
	}

	return S_OK;
}

void CCamera_Dynamic::Tick(_double TimeDelta)
{
	if (!m_bSwitchCam)
	{
		__super::Tick(TimeDelta);
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		m_pPlayerCam = static_cast<CPlayerCamera*>(pGameInstance->Find_GameObject(pGameInstance->GetCurLevelIdx(), L"Layer_Camera", L"PlayerCamera"));
		RELEASE_INSTANCE(CGameInstance);
		if(g_LEVEL == LEVEL_CHAP1)
			Level_Chap1Tick(TimeDelta);
		else if(g_LEVEL == LEVEL_CHAP2)
			Level_Chap2Tick(TimeDelta);
		else if (g_LEVEL == LEVEL_CHAP3)
		{
			m_bSwitchCam = true;
			static_cast<CPlayerCamera*>(m_pPlayerCam)->Set_CinematicCam(true);
		}
	}
}

void CCamera_Dynamic::Late_Tick(_double TimeDelta)
{
	if (!m_bSwitchCam)
		__super::Late_Tick(TimeDelta);
}

HRESULT CCamera_Dynamic::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;
	
	return S_OK;
}

void CCamera_Dynamic::Level_Chap1Tick(_double TimeDelta)
{
	_float4 vPosition = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

	if (m_bCheckPointFinish)
	{
		_float3 vPos = _float3::Lerp(_float3(vPosition.x, vPosition.y, vPosition.z), m_pPlayerCam->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION), static_cast<float>(TimeDelta) * 1.2f);
		m_pTransformCom->LookAt(_float4(vPos.x, vPos.y, vPos.z, 1.f));
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, _float4(vPos.x, vPos.y, vPos.z, 1.f));

		if (DistancePointCheck(vPosition, m_pPlayerCam->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION)))
		{
			m_bSwitchCam = true;
			static_cast<CPlayerCamera*>(m_pPlayerCam)->Set_CinematicCam(true);
		}
		return;
	}

	if (m_CheckPoints.size() <= 1)
	{
		if (DistancePointCheck(vPosition, m_CheckPoints[0]))
		{
			m_bCheckPointFinish = true;
		}
		else
		{
			_float3 vPos = _float3::Lerp(_float3(vPosition.x, vPosition.y, vPosition.z), _float3(m_CheckPoints[0].x, m_CheckPoints[0].y, m_CheckPoints[0].z), static_cast<float>(TimeDelta) * 1.2f);
			m_pTransformCom->LookAt(_float4(m_CheckPoints[0].x, m_CheckPoints[0].y, m_CheckPoints[0].z, 1.f));
			m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, _float4(vPos.x, vPos.y, vPos.z, 1.f));
		}
	}
	else if (DistancePointCheck(vPosition, m_CheckPoints.back()))
		m_CheckPoints.pop_back();
	else
	{
		_float3 vPos = _float3::Lerp(_float3(vPosition.x, vPosition.y, vPosition.z), _float3(m_CheckPoints.back().x, m_CheckPoints.back().y, m_CheckPoints.back().z), static_cast<float>(TimeDelta) * 1.2f);
		m_pTransformCom->LookAt(_float4(m_CheckPoints.back().x, m_CheckPoints.back().y, m_CheckPoints.back().z, 1.f));
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, _float4(vPos.x, vPos.y, vPos.z, 1.f));
	}
}

void CCamera_Dynamic::Level_Chap2Tick(_double TimeDelta)
{
	_float4 vPosition = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

	if (m_bCheckPointFinish)
	{
		_float3 vPos = _float3::Lerp(_float3(vPosition.x, vPosition.y, vPosition.z), m_pPlayerCam->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION), static_cast<float>(TimeDelta) * 1.2f);
		m_pTransformCom->LookAt(_float4(vPos.x, vPos.y, vPos.z, 1.f));
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, _float4(vPos.x, vPos.y, vPos.z, 1.f));

		if (DistancePointCheck(vPosition, m_pPlayerCam->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION)))
		{
			m_bSwitchCam = true;
			static_cast<CPlayerCamera*>(m_pPlayerCam)->Set_CinematicCam(true);
		}
		return;
	}

	if (m_CheckPoints.size() <= 1)
	{
		if (DistancePointCheck(vPosition, m_CheckPoints[0]))
		{
			m_bCheckPointFinish = true;
		}
		else
		{
			_float3 vPos = _float3::Lerp(_float3(vPosition.x, vPosition.y, vPosition.z), _float3(m_CheckPoints[0].x, m_CheckPoints[0].y, m_CheckPoints[0].z), static_cast<float>(TimeDelta) * 1.2f);
			m_pTransformCom->LookAt(_float4(m_CheckPoints[0].x, m_CheckPoints[0].y, m_CheckPoints[0].z, 1.f));
			m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, _float4(vPos.x, vPos.y, vPos.z, 1.f));
		}
	}
	else if (DistancePointCheck(vPosition, m_CheckPoints.back()))
		m_CheckPoints.pop_back();
	else
	{
		_float3 vPos = _float3::Lerp(_float3(vPosition.x, vPosition.y, vPosition.z), _float3(m_CheckPoints.back().x, m_CheckPoints.back().y, m_CheckPoints.back().z), static_cast<float>(TimeDelta) * 1.2f);
		m_pTransformCom->LookAt(_float4(m_CheckPoints.back().x, m_CheckPoints.back().y, m_CheckPoints.back().z, 1.f));
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, _float4(vPos.x, vPos.y, vPos.z, 1.f));
	}
}

_bool CCamera_Dynamic::DistancePointCheck(_float4 vTargetPos, _float4 vPos)
{
	if (_float4::Distance(vPos, vTargetPos) < 5.f)
		return true;

	return false;
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
