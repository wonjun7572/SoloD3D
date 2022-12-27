#include "Input_Device.h"

IMPLEMENT_SINGLETON(CInput_Device)

CInput_Device::CInput_Device()
	: m_pInputSDK(nullptr), m_pKeyBoard(nullptr), m_pMouse(nullptr)
{
	ZeroMemory(m_byKeyState, sizeof(m_byKeyState));
	ZeroMemory(&m_MouseState, sizeof(m_MouseState));

	ZeroMemory(m_bKeyUp, sizeof(_bool) * 256);
	ZeroMemory(m_bKeyDown, sizeof(_bool) * 256);

	ZeroMemory(m_bMouseUp, sizeof(_bool) * DIM_END);
	ZeroMemory(m_bMouseDown, sizeof(_bool) * DIM_END);
}

_bool CInput_Device::Mouse_Down(MOUSEKEYSTATE byKeyID, _bool* pData)
{
	_bool* pCompareData = NULL;

	if (pData == NULL)
	{
		pCompareData = &m_bMouseDown[byKeyID];
	}
	else
	{
		pCompareData = pData;
	}

	if (m_MouseState.rgbButtons[byKeyID] & 0x80)
	{
		//  ������ ���� true
		if (*pCompareData == false)
		{
			*pCompareData = true;
			return TRUE;
		}
	}
	else
	{
		//  ���� ���� false
		if (*pCompareData == true)
		{
			*pCompareData = false;
		}
	}

	return FALSE;
}

_bool CInput_Device::Mouse_Up(MOUSEKEYSTATE byKeyID, _bool* pData)
{
	_bool* pCompareData = NULL;
	if (pData == NULL)
	{
		pCompareData = &m_bMouseUp[byKeyID];
	}
	else
	{
		pCompareData = pData;
	}

	if (m_MouseState.rgbButtons[byKeyID] & 0x80)
	{
		//  ������ ���� true
		if (*pCompareData == false)
		{
			*pCompareData = true;
		}
	}
	else
	{
		if (*pCompareData == true)//  ���� ���� false
		{
			*pCompareData = false;
			return TRUE;
		}
	}

	return FALSE;
}

_bool CInput_Device::Key_Pressing(_ubyte byKeyID)
{
	if (Get_DIKeyState(byKeyID) & 0x80)
		return true;

	return false;
}

_bool CInput_Device::Key_Down(_ubyte byKeyID, _bool* pData)
{
	_bool* pCompareData = NULL;
	if (pData == NULL)
	{
		pCompareData = &m_bKeyDown[byKeyID];
	}
	else
	{
		pCompareData = pData;
	}

	if (m_byKeyState[byKeyID] & 0x80)
	{
		//  ������ ���� true
		if (*pCompareData == false)
		{
			*pCompareData = true;
			return TRUE;
		}
	}
	else
	{
		//  ���� ���� false
		if (*pCompareData == true)
		{
			*pCompareData = false;
		}
	}

	return FALSE;
}

_bool CInput_Device::Key_Up(_ubyte byKeyID, _bool* pData)
{
	_bool* pCompareData = NULL;

	if (pData == NULL)
	{
		pCompareData = &m_bKeyUp[byKeyID];
	}
	else
	{
		pCompareData = pData;
	}

	if (m_byKeyState[byKeyID] & 0x80)
	{
		//  ������ ���� true
		if (*pCompareData == false)
		{
			*pCompareData = true;
		}
	}
	else
	{
		if (*pCompareData == true)//  ���� ���� false
		{
			*pCompareData = false;
			return TRUE;
		}
	}

	return FALSE;
}

HRESULT CInput_Device::Ready_Input_Device(HINSTANCE hInst, HWND hWnd)
{
	// DInput �İ�ü�� �����ϴ� �Լ�
	if (FAILED(DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pInputSDK, nullptr)))
		return E_FAIL;


	// Ű���� ��ü ����
	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysKeyboard, &m_pKeyBoard, nullptr)))
		return E_FAIL;

	// ������ Ű���� ��ü�� ���� ������ �� ��ü���� �����ϴ� �Լ�
	m_pKeyBoard->SetDataFormat(&c_dfDIKeyboard);

	// ��ġ�� ���� �������� �������ִ� �Լ�, (Ŭ���̾�Ʈ�� ���ִ� ���¿��� Ű �Է��� ������ ������ �����ϴ� �Լ�)
	m_pKeyBoard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// ��ġ�� ���� access ������ �޾ƿ��� �Լ�
	m_pKeyBoard->Acquire();


	// ���콺 ��ü ����
	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr)))
		return E_FAIL;

	// ������ ���콺 ��ü�� ���� ������ �� ��ü���� �����ϴ� �Լ�
	m_pMouse->SetDataFormat(&c_dfDIMouse);

	// ��ġ�� ���� �������� �������ִ� �Լ�, Ŭ���̾�Ʈ�� ���ִ� ���¿��� Ű �Է��� ������ ������ �����ϴ� �Լ�
	m_pMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// ��ġ�� ���� access ������ �޾ƿ��� �Լ�
	m_pMouse->Acquire();

	return S_OK;
}

void CInput_Device::Invalidate_Input_Device(void)
{
	m_pKeyBoard->GetDeviceState(256, m_byKeyState);
	m_pMouse->GetDeviceState(sizeof(m_MouseState), &m_MouseState);
}

void Engine::CInput_Device::Free(void)
{
	Safe_Release(m_pKeyBoard);
	Safe_Release(m_pMouse);
	Safe_Release(m_pInputSDK);
}
