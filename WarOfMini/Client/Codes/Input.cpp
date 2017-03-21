#include "stdafx.h"
#include "Input.h"

IMPLEMENT_SINGLETON(CInput)

CInput::CInput(void)
: m_pInput(NULL),
  m_pKeyBoard(NULL),
  m_pMouse(NULL)
{
	ZeroMemory(m_byKeyState, sizeof(_ubyte) * 256);
	ZeroMemory(m_byCheckOnce, sizeof(_ubyte) * 260);
	ZeroMemory(m_byIsOnce, sizeof(_ubyte) * 260);
	ZeroMemory(m_byIsLeave, sizeof(_ubyte) * 260);
	ZeroMemory(&m_MouseState, sizeof(MOUSEKEYSTATE));
}

CInput::~CInput(void)
{
}

_ubyte CInput::GetDIKeyState(_ubyte byKeyID)
{
	return m_byKeyState[byKeyID];
}

_ubyte CInput::GetDIMouseState(_ubyte byMouseID)
{
	return m_MouseState.rgbButtons[byMouseID];
}

_long CInput::GetDIMouseMove(MOUSEMOVESTATE byMouseMoveState)
{
	return *(((_long*)&m_MouseState) + byMouseMoveState);
}

void CInput::SetInputState(void)
{
	ResetOnce();
	SetAcquire();

	m_pKeyBoard->GetDeviceState(256, m_byKeyState);
	m_pMouse->GetDeviceState(sizeof(MOUSEKEYSTATE), &m_MouseState);

	SetInputOnce();
}

void CInput::ResetInputState(void)
{
	ZeroMemory(m_byKeyState, sizeof(_ubyte) * 256);
	ZeroMemory(m_byCheckOnce, sizeof(_ubyte) * 260);
	ZeroMemory(m_byIsOnce, sizeof(_ubyte) * 260);
	ZeroMemory(m_byIsLeave, sizeof(_ubyte) * 260);
	ZeroMemory(&m_MouseState, sizeof(MOUSEKEYSTATE));
}

HRESULT CInput::InitInputDevice(HINSTANCE hInst, HWND hWnd)
{
	HRESULT	hr = NULL;


	hr = DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pInput, NULL);
	FAILED_CHECK_MSG(hr, L"Input Created Failed");

	hr = InitKeyBoard(hWnd);
	FAILED_CHECK_MSG(hr, L"KeyBoard Created Failed");

	hr = InitMouse(hWnd);
	FAILED_CHECK_MSG(hr, L"Mouse Created Failed");

	return S_OK;
}

void CInput::SetAcquire()
{
	if (m_pKeyBoard)
		m_pKeyBoard->Acquire();

	if (m_pMouse)
		m_pMouse->Acquire();
}

HRESULT CInput::InitKeyBoard(HWND hWnd)
{
	HRESULT		hr = NULL;
	hr = m_pInput->CreateDevice(GUID_SysKeyboard, &m_pKeyBoard, NULL);
	FAILED_CHECK(hr);

	m_pKeyBoard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
	FAILED_CHECK(hr);

	m_pKeyBoard->SetDataFormat(&c_dfDIKeyboard);
	FAILED_CHECK(hr);

	m_pKeyBoard->Acquire();

	return S_OK;
}

HRESULT CInput::InitMouse(HWND hWnd)
{
	HRESULT		hr = NULL;
	hr = m_pInput->CreateDevice(GUID_SysMouse, &m_pMouse, NULL);
	FAILED_CHECK(hr);

	m_pMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
	FAILED_CHECK(hr);

	m_pMouse->SetDataFormat(&c_dfDIMouse);
	FAILED_CHECK(hr);

	m_pMouse->Acquire();

	return S_OK;
}

void CInput::SetInputOnce(void)
{
	for (int i = 0; i < 256; ++i)
	{
		if (m_byKeyState[i] && !m_byCheckOnce[i])
		{
			m_byIsOnce[i] = true;
			m_byCheckOnce[i] = true;
		}
		else if (m_byKeyState[i] && m_byCheckOnce[i])
			m_byIsOnce[i] = false;
	}

	for (int i = 0; i < 4; ++i)
	{
		if (m_MouseState.rgbButtons[i] && !m_byCheckOnce[i + 256])
		{
			m_byIsOnce[i + 256] = true;
			m_byCheckOnce[i + 256] = true;
		}
		else if (m_MouseState.rgbButtons[i] && m_byCheckOnce[i + 256])
			m_byIsOnce[i + 256] = false;
	}
}

void CInput::ResetOnce(void)
{
	for (int i = 0; i < 256; ++i)
	{
		m_byIsOnce[i] = false;
		m_byIsLeave[i] = false;

		if (!m_byKeyState[i])
		{
			if (m_byCheckOnce[i])
			{
				m_byIsLeave[i] = true;
				m_byCheckOnce[i] = false;
			}
		}
	}

	for (int i = 0; i < 4; ++i)
	{
		m_byIsOnce[i + 256] = false;
		m_byIsLeave[i + 256] = false;
		if (!m_MouseState.rgbButtons[i])
		{
			if (m_byCheckOnce[i + 256])
			{
				m_byIsLeave[i + 256] = true;
				m_byCheckOnce[i + 256] = false;
			}
		}
	}
}

_ubyte CInput::GetDIKeyStateOnce(_ubyte KeyFlag)
{
	return m_byIsOnce[KeyFlag];
}

_ubyte CInput::GetDIKeyStateLeave(_ubyte KeyFlag)
{
	return m_byIsLeave[KeyFlag];
}

_ubyte CInput::GetDIMouseStateOnce(_ubyte KeyFlag)
{
	return m_byIsOnce[KeyFlag + 256];
}

_ubyte CInput::GetDIMouseStateLeave(_ubyte KeyFlag)
{
	return m_byIsLeave[KeyFlag + 256];
}

void CInput::Release(void)
{
	Safe_Release(m_pMouse);
	Safe_Release(m_pKeyBoard);
	Safe_Release(m_pInput);

	delete this;
}

