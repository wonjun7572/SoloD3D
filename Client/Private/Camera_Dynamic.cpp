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
		CameraDesc.vEye = _float4(0.f, 30.f, 0.f, 1.f);
		CameraDesc.vAt = _float4(320.f, 0.f, 30.f, 1.f);
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
		m_CheckPoints.push_back(_float4(64.f, 3.5f, 200.f, 1.f)); 
		m_CheckPoints.push_back(_float4(115.f, 3.5f, 359.f, 1.f));
		m_CheckPoints.push_back(_float4(311.f, 3.5f, 393.f, 1.f));
		m_CheckPoints.push_back(_float4(262.5f, 50.f, 252.f, 1.f));
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
	}
}

void CCamera_Dynamic::Late_Tick(_double TimeDelta)
{
	if (!m_bSwitchCam)
	{
		if (g_LEVEL == LEVEL_CHAP1)
		{
			Level_Chap1Tick(TimeDelta);
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			m_pPlayer = static_cast<CPlayerCamera*>(pGameInstance->Find_GameObject(LEVEL_CHAP1, L"Layer_Player", L"PlayerCamera"));
			RELEASE_INSTANCE(CGameInstance);
		}
		else if (g_LEVEL == LEVEL_CHAP2)
		{
			Level_Chap2Tick(TimeDelta);
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			m_pPlayer = static_cast<CPlayerCamera*>(pGameInstance->Find_GameObject(LEVEL_CHAP2, L"Layer_Player", L"PlayerCamera"));
			RELEASE_INSTANCE(CGameInstance);
		}
		else if (g_LEVEL == LEVEL_CHAP3)
		{
			m_bSwitchCam = true;
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			m_pPlayer = static_cast<CPlayerCamera*>(pGameInstance->Find_GameObject(LEVEL_CHAP3, L"Layer_Player", L"PlayerCamera"));
			RELEASE_INSTANCE(CGameInstance);
			static_cast<CPlayerCamera*>(m_pPlayerCam)->Set_CinematicCam(true);
		}
		__super::Late_Tick(TimeDelta);
	}
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
	m_TimeDelta += TimeDelta * 0.5f;

	_float4 vPosition = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

	if (DistancePointCheck(vPosition, m_pPlayerCam->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION)) 
		&& m_TimeDelta > 30.f)
	{
		m_bSwitchCam = true;
		static_cast<CPlayerCamera*>(m_pPlayerCam)->Set_CinematicCam(true);
	}

	if (m_TimeDelta < 5.f)
	{
		_float3 vPlayerCamPos = m_pPlayerCam->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION);
		_float3 vPos = _float3::Lerp(_float3(vPosition.x, vPosition.y, vPosition.z), _float3(vPlayerCamPos.x, vPlayerCamPos.y, vPlayerCamPos.z), static_cast<float>(TimeDelta) * 0.8f);
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, _float4(vPos.x, vPos.y, vPos.z, 1.f));
		m_pTransformCom->LookAt(_float4(vPlayerCamPos.x, vPlayerCamPos.y, vPlayerCamPos.z, 1.f));
	}
	if (m_TimeDelta > 5.f && m_TimeDelta <= 10.f)
	{
		_float3 vPos = _float3::Lerp(_float3(vPosition.x, vPosition.y, vPosition.z), _float3(m_CheckPoints[0].x, m_CheckPoints[0].y, m_CheckPoints[0].z), static_cast<float>(TimeDelta) * 0.8f);
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, _float4(vPos.x, vPos.y, vPos.z, 1.f));
		m_pTransformCom->LookAt(_float4(45.5f, 0.f, 183.375f, 1.f));
	}
	else if (m_TimeDelta > 10.f&& m_TimeDelta <= 15.f)
	{
		_float3 vPos = _float3::Lerp(_float3(vPosition.x, vPosition.y, vPosition.z), _float3(m_CheckPoints[1].x, m_CheckPoints[1].y, m_CheckPoints[1].z), static_cast<float>(TimeDelta) * 0.8f);
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, _float4(vPos.x, vPos.y, vPos.z, 1.f));
		m_pTransformCom->LookAt(_float4(103.4f, 0.f, 379.f, 1.f));
	}
	else if (m_TimeDelta > 15.f && m_TimeDelta <= 20.f)
	{
		_float3 vPos = _float3::Lerp(_float3(vPosition.x, vPosition.y, vPosition.z), _float3(m_CheckPoints[2].x, m_CheckPoints[2].y, m_CheckPoints[2].z), static_cast<float>(TimeDelta) * 0.8f);
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, _float4(vPos.x, vPos.y, vPos.z, 1.f));
		m_pTransformCom->LookAt(_float4(317.f, 0.f, 410.f, 1.f));
	}
	else if (m_TimeDelta > 20.f && m_TimeDelta <= 25.f)
	{
		_float3 vPos = _float3::Lerp(_float3(vPosition.x, vPosition.y, vPosition.z), _float3(m_CheckPoints[3].x, m_CheckPoints[3].y, m_CheckPoints[3].z), static_cast<float>(TimeDelta) * 0.8f);
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, _float4(vPos.x, vPos.y, vPos.z, 1.f));
		m_pTransformCom->LookAt(_float4(262.5f, 0.f, 252.f, 1.f));
	}
	else if (m_TimeDelta > 25.f)
	{
		_float3 vPlayerCamPos = m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION);
		
		_float3 vPos = _float3::Lerp(_float3(vPosition.x, vPosition.y, vPosition.z), _float3(vPlayerCamPos.x,vPlayerCamPos.y, vPlayerCamPos.z), static_cast<float>(TimeDelta) * 0.8f);
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, _float4(vPos.x, vPos.y, vPos.z, 1.f));
		m_pTransformCom->LookAt(_float4(vPlayerCamPos.x, vPlayerCamPos.y, vPlayerCamPos.z, 1.f));
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
