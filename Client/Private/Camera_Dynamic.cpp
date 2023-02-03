#include "stdafx.h"
#include "..\Public\Camera_Dynamic.h"
#include "GameInstance.h"
#include "PlayerCamera.h"
#include "MathUtils.h"

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
		CameraDesc.vEye = _float4(0.f, 7.f, -5.f, 1.f);
		CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
		CameraDesc.vUp = _float4(0.f, 1.f, 0.f, 0.f);

		CameraDesc.fFovy = XMConvertToRadians(60.f);
		
		CameraDesc.fAspect = static_cast<_float>( g_iWinSizeX / static_cast<_float>(g_iWinSizeY));

		CameraDesc.fNear = 0.02f;
		CameraDesc.fFar = 500.f;

		CameraDesc.TransformDesc.fSpeedPerSec = 10.f;
		CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	}

	if (FAILED(__super::Init(&CameraDesc)))
		return E_FAIL;

	m_strObjName = TEXT("DynamicCamera");

	if (g_LEVEL == LEVEL_CHAP1)
	{
		Load_CheckPoints();
		m_fMultipleTime = 0.7f;
		m_bDynamicCam = !m_bDynamicCam;
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance)
		m_pPlayerCam = static_cast<CPlayerCamera*>(pGameInstance->Find_GameObject(LEVEL_CHAP1, L"Layer_Camera", L"PlayerCamera"));
		RELEASE_INSTANCE(CGameInstance)
		static_cast<CPlayerCamera*>(m_pPlayerCam)->Set_CinematicCam(false);
		m_bPaused = false;
		m_iIndex = 1;
	}
	else if (g_LEVEL == LEVEL_CHAP2)
	{
		Load_CheckPoints();
		m_fMultipleTime = 0.7f;
		m_bDynamicCam = !m_bDynamicCam;
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance)
		m_pPlayerCam = static_cast<CPlayerCamera*>(pGameInstance->Find_GameObject(LEVEL_CHAP2, L"Layer_Camera", L"PlayerCamera"));
		RELEASE_INSTANCE(CGameInstance)
		static_cast<CPlayerCamera*>(m_pPlayerCam)->Set_CinematicCam(false);
		m_bPaused = false;
		m_iIndex = 1;
	}
	

	return S_OK;
}

void CCamera_Dynamic::Tick(_double TimeDelta)
{
	if (m_bDynamicCam)
	{
		__super::Tick(TimeDelta);
		Input_KeyBoard(TimeDelta);
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	
		if (pGameInstance->Key_Down(DIK_F1))
			m_bFix = !m_bFix;
	
		RELEASE_INSTANCE(CGameInstance);
	
		if (m_bFix)
			Mouse_Fix();
	
		return;
	}

	if (!m_bPaused)
	{
		__super::Tick(TimeDelta);
		
		if(g_LEVEL == LEVEL_CHAP1)
			Level_Chap1Tick(TimeDelta);
		else if(g_LEVEL == LEVEL_CHAP2)
			Level_Chap2Tick(TimeDelta);
		else if(g_LEVEL == LEVEL_CHAP3)
			Level_Chap3Tick(TimeDelta);
	}
}

void CCamera_Dynamic::Late_Tick(_double TimeDelta)
{
	if (m_bPaused)
	{
		__super::Late_Tick(TimeDelta);
		if (g_LEVEL == LEVEL_CHAP1)
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			m_pPlayerCam = static_cast<CPlayerCamera*>(pGameInstance->Find_GameObject(LEVEL_CHAP1, L"Layer_Camera", L"PlayerCamera"));
			static_cast<CPlayerCamera*>(m_pPlayerCam)->Set_CinematicCam(true);
			RELEASE_INSTANCE(CGameInstance);
		}
		else if (g_LEVEL == LEVEL_CHAP2)
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			m_pPlayerCam = static_cast<CPlayerCamera*>(pGameInstance->Find_GameObject(LEVEL_CHAP2, L"Layer_Camera", L"PlayerCamera"));
			static_cast<CPlayerCamera*>(m_pPlayerCam)->Set_CinematicCam(true);
			RELEASE_INSTANCE(CGameInstance);
		}
		else if (g_LEVEL == LEVEL_CHAP3)
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			m_pPlayerCam = static_cast<CPlayerCamera*>(pGameInstance->Find_GameObject(LEVEL_CHAP3, L"Layer_Camera", L"PlayerCamera"));
			static_cast<CPlayerCamera*>(m_pPlayerCam)->Set_CinematicCam(true);
			RELEASE_INSTANCE(CGameInstance);
		}
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
	if (!m_bPaused)
	{
		m_TimeDelta += TimeDelta * static_cast<_double>(m_fMultipleTime);
	
		if (m_TimeDelta >= 1.0)
		{
			if (m_iIndex < m_CheckPoints.size() - 3)
			{
				m_iIndex++;
				m_TimeDelta -= 1.0;
			}
			else
			{
				m_bPaused = true;
			}
		}
	}

	_float4 vPos = _float4::CatmullRom(m_CheckPoints[m_iIndex - 1], m_CheckPoints[m_iIndex], m_CheckPoints[m_iIndex + 1], m_CheckPoints[m_iIndex + 2], static_cast<_float>(m_TimeDelta));
	m_pTransformCom->LookAt(vPos, true);
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vPos);
}

void CCamera_Dynamic::Level_Chap2Tick(_double TimeDelta)
{
	if (!m_bPaused)
	{
		m_TimeDelta += TimeDelta * static_cast<_double>(m_fMultipleTime);

		if (m_TimeDelta >= 1.0)
		{
			if (m_iIndex < m_CheckPoints.size() - 3)
			{
				m_iIndex++;
				m_TimeDelta -= 1.0;
			}
			else
			{
				m_bPaused = true;
			}
		}
	}

	_float4 vPos = _float4::CatmullRom(m_CheckPoints[m_iIndex - 1], m_CheckPoints[m_iIndex], m_CheckPoints[m_iIndex + 1], m_CheckPoints[m_iIndex + 2], static_cast<_float>(m_TimeDelta));
	m_pTransformCom->LookAt(vPos, true);
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vPos);
}

void CCamera_Dynamic::Level_Chap3Tick(_double TimeDelta)
{
	if (!m_bPaused)
	{
		m_TimeDelta += TimeDelta * static_cast<_double>(m_fMultipleTime);

		if (m_TimeDelta >= 1.0)
		{
			if (m_iIndex < m_CheckPoints.size() - 3)
			{
				m_iIndex++;
				m_TimeDelta -= 1.0;
			}
			else
			{
				m_bPaused = true;
			}
		}
	}

	_float4 vPos = _float4::CatmullRom(m_CheckPoints[m_iIndex - 1], m_CheckPoints[m_iIndex], m_CheckPoints[m_iIndex + 1], m_CheckPoints[m_iIndex + 2], static_cast<_float>(m_TimeDelta));
	m_pTransformCom->LookAt(vPos, true);
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vPos);
}

void CCamera_Dynamic::Input_KeyBoard(_double TimeDelta)
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

void CCamera_Dynamic::Imgui_RenderProperty()
{
	if (ImGui::Button("Start"))
	{
		m_bDynamicCam = !m_bDynamicCam;
		
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		
		if(g_LEVEL == LEVEL_CHAP1)
			m_pPlayerCam = static_cast<CPlayerCamera*>(pGameInstance->Find_GameObject(LEVEL_CHAP1, L"Layer_Camera", L"PlayerCamera"));
		else if (g_LEVEL == LEVEL_CHAP2)
			m_pPlayerCam = static_cast<CPlayerCamera*>(pGameInstance->Find_GameObject(LEVEL_CHAP2, L"Layer_Camera", L"PlayerCamera"));
		else if (g_LEVEL == LEVEL_CHAP3)
			m_pPlayerCam = static_cast<CPlayerCamera*>(pGameInstance->Find_GameObject(LEVEL_CHAP3, L"Layer_Camera", L"PlayerCamera"));
		
		RELEASE_INSTANCE(CGameInstance);
		
		static_cast<CPlayerCamera*>(m_pPlayerCam)->Set_CinematicCam(false);
		m_bPaused = false;
		m_iIndex = 1;
	}

	if (ImGui::Button("Pause"))
	{
		m_bDynamicCam = false;
		m_bPaused = true;
	}

	ImGui::DragFloat("##MultipleTime", &m_fMultipleTime, 0.01f, 0.01f, 10.f);

	static _int  iSelectPointTag = -1;
	size_t CheckPointsSize = m_CheckPoints.size();
	char** pCheckPointsTag = new char*[CheckPointsSize];

	if (ImGui::BeginListBox("##CHECKPOINTLIST"))
	{
		for (size_t i = 0; i < CheckPointsSize; ++i)
		{
			pCheckPointsTag[i] = new char[MAX_PATH];
			sprintf_s(pCheckPointsTag[i], MAX_PATH, "X : %f , Y: %f, Z: %f", m_CheckPoints[i].x, m_CheckPoints[i].y, m_CheckPoints[i].z);
		}

		for (_int i = 0; i < CheckPointsSize; ++i)
		{
			const bool is_selected = (iSelectPointTag == i);
			if (ImGui::Selectable(pCheckPointsTag[i], is_selected))
				iSelectPointTag = i;

			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndListBox();
	}

	for (_int i = 0; i < CheckPointsSize; ++i)
		Safe_Delete_Array(pCheckPointsTag[i]);

	Safe_Delete_Array(pCheckPointsTag);

	if (ImGui::Button("DELETE ALL"))
		m_CheckPoints.clear();

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	ImGui::Text("If you want to Push Point Press Key J");
	
	if (pGameInstance->Key_Down(DIK_J))
		m_CheckPoints.push_back(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

	RELEASE_INSTANCE(CGameInstance);

	if (CheckPointsSize > 0)
	{
		char* szButton = new char[MAX_PATH];
		sprintf_s(szButton, MAX_PATH, "Delete_Index : %d ", iSelectPointTag);

		if (ImGui::Button(szButton))
			m_CheckPoints.erase(m_CheckPoints.begin() + iSelectPointTag);

		Safe_Delete_Array(szButton);
	}

	if (ImGui::Button("SAVE"))
		Save_CheckPoints();

	if (ImGui::Button("LOAD"))
	{
		m_CheckPoints.clear();
		Load_CheckPoints();
	}
}

HRESULT CCamera_Dynamic::Save_CheckPoints()
{
	_tchar* pDataName = new _tchar[MAX_PATH];

	switch (g_LEVEL)
	{
	case LEVEL_CHAP1:
		lstrcpy(pDataName, TEXT("../Bin/Data/CinematicCam_CHAP1.dat"));
		break;
	case LEVEL_CHAP2:
		lstrcpy(pDataName, TEXT("../Bin/Data/CinematicCam_CHAP2.dat"));
		break;
	case LEVEL_CHAP3:
		lstrcpy(pDataName, TEXT("../Bin/Data/CinematicCam_CHAP3.dat"));
		break;
	}

	DWORD	dwByte = 0;

	HANDLE      hFile = CreateFile(pDataName
		, GENERIC_WRITE
		, 0
		, nullptr
		, CREATE_ALWAYS
		, FILE_ATTRIBUTE_NORMAL
		, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	size_t CheckPointsSize = m_CheckPoints.size();
	WriteFile(hFile, &CheckPointsSize, sizeof(size_t), &dwByte, nullptr);

	for (auto& pPoint : m_CheckPoints)
	{
		WriteFile(hFile, &pPoint, sizeof(_float4), &dwByte, nullptr);
	}

	MSG_BOX("Save_Complete!!");

	CloseHandle(hFile);
	Safe_Delete_Array(pDataName);
	return S_OK;
}

HRESULT CCamera_Dynamic::Load_CheckPoints()
{
	_tchar* pDataName = new _tchar[MAX_PATH];

	switch (g_LEVEL)
	{
	case LEVEL_CHAP1:
		lstrcpy(pDataName, TEXT("../Bin/Data/CinematicCam_CHAP1.dat"));
		break;
	case LEVEL_CHAP2:
		lstrcpy(pDataName, TEXT("../Bin/Data/CinematicCam_CHAP2.dat"));
		break;
	case LEVEL_CHAP3:
		lstrcpy(pDataName, TEXT("../Bin/Data/CinematicCam_CHAP3.dat"));
		break;
	}

	DWORD	dwByte = 0;

	HANDLE      hFile = CreateFile(pDataName
		, GENERIC_READ
		, FILE_SHARE_READ
		, 0
		, OPEN_EXISTING
		, FILE_ATTRIBUTE_NORMAL
		, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	size_t CheckPointsSize;
	ReadFile(hFile, &CheckPointsSize, sizeof(size_t), &dwByte, nullptr);

	for (size_t i = 0; i < CheckPointsSize; ++i)
	{
		_float4 vPoint;
		ReadFile(hFile, &vPoint, sizeof(_float4), &dwByte, nullptr);
		m_CheckPoints.push_back(vPoint);
	}

	CloseHandle(hFile);

	Safe_Delete_Array(pDataName);

	return S_OK;
}

void CCamera_Dynamic::Mouse_Fix()
{
	POINT pt{ static_cast<LONG>(g_iWinSizeX >> 1), static_cast<LONG>(g_iWinSizeY >> 1) };

	ClientToScreen(g_hWnd, &pt);
	SetCursorPos(pt.x, pt.y);
	//ShowCursor(false);
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
