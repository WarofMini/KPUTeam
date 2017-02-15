#include "stdafx.h"
#include "Input.h"

IMPLEMENT_SINGLETON(CInput)

CInput::CInput(void)
	: m_pInput(NULL),
	m_pKeyBoard(NULL),
	m_pMouse(NULL)
{
	ZeroMemory(m_byKeyState, sizeof(BYTE) * 256);
	ZeroMemory(m_byCheckOnce, sizeof(BYTE) * 260);
	ZeroMemory(m_byIsOnce, sizeof(BYTE) * 260);
	ZeroMemory(m_byIsLeave, sizeof(BYTE) * 260);
	ZeroMemory(&m_MouseState, sizeof(DIMOUSESTATE));
}

CInput::~CInput(void)
{
	Release();
}

BYTE CInput::GetDIKeyState(BYTE KeyFlag)
{
	return m_byKeyState[KeyFlag];
}

BYTE CInput::GetDIMouseState(BYTE KeyFlag)
{
	return m_MouseState.rgbButtons[KeyFlag];
}

long CInput::GetDIMouseMove(MOUSEMOVE KeyFlag)
{
	return *(((long*)&m_MouseState) + KeyFlag);
}

void CInput::SetInputState(void)
{
	ResetOnce();
	SetAcquire();

	m_pKeyBoard->GetDeviceState(256, m_byKeyState);
	m_pMouse->GetDeviceState(sizeof(DIMOUSESTATE), &m_MouseState);

	SetInputOnce();
}

void CInput::ResetInputState(void)
{
	ZeroMemory(m_byKeyState, sizeof(BYTE) * 256);
	ZeroMemory(m_byCheckOnce, sizeof(BYTE) * 260);
	ZeroMemory(m_byIsOnce, sizeof(BYTE) * 260);
	ZeroMemory(m_byIsLeave, sizeof(BYTE) * 260);
	ZeroMemory(&m_MouseState, sizeof(DIMOUSESTATE));
}

HRESULT CInput::InitInputDevice(HINSTANCE hInst, HWND hWnd)
{
	HRESULT	hr = NULL;


	hr = DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pInput, NULL);
	FAILED_CHECK_MSG(hr, L"입력장치 생성 실패");

	hr = InitKeyBoard(hWnd);
	FAILED_CHECK_MSG(hr, L"키보드 생성 실패");

	hr = InitMouse(hWnd);
	FAILED_CHECK_MSG(hr, L"마우스 생성 실패");
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
	for (int b = 0; b < 256; ++b)
	{
		if (m_byKeyState[b] && !m_byCheckOnce[b])
		{
			m_byIsOnce[b] = true;
			m_byCheckOnce[b] = true;
		}
		else if (m_byKeyState[b] && m_byCheckOnce[b])
			m_byIsOnce[b] = false;
	}

	for (int b = 0; b < 4; ++b)
	{
		if (m_MouseState.rgbButtons[b] && !m_byCheckOnce[b + 256])
		{
			m_byIsOnce[b + 256] = true;
			m_byCheckOnce[b + 256] = true;
		}
		else if (m_MouseState.rgbButtons[b] && m_byCheckOnce[b + 256])
			m_byIsOnce[b + 256] = false;
	}
}

void CInput::ResetOnce(void)
{
	for (int b = 0; b < 256; ++b)
	{
		m_byIsOnce[b] = false;
		m_byIsLeave[b] = false;
		if (!m_byKeyState[b])
		{
			if (m_byCheckOnce[b])
				m_byIsLeave[b] = true;
			m_byCheckOnce[b] = false;
		}
	}

	for (int b = 0; b < 4; ++b)
	{
		m_byIsOnce[b + 256] = false;
		m_byIsLeave[b + 256] = false;
		if (!m_MouseState.rgbButtons[b])
		{
			if (m_byCheckOnce[b + 256])
				m_byIsLeave[b + 256] = true;
			m_byCheckOnce[b + 256] = false;
		}
	}
}

BYTE CInput::GetDIKeyStateOnce(BYTE KeyFlag)
{
	return m_byIsOnce[KeyFlag];
}

BYTE CInput::GetDIKeyStateLeave(BYTE KeyFlag)
{
	return m_byIsLeave[KeyFlag];
}

BYTE CInput::GetDIMouseStateOnce(BYTE KeyFlag)
{
	return m_byIsOnce[KeyFlag + 256];
}

BYTE CInput::GetDIMouseStateLeave(BYTE KeyFlag)
{
	return m_byIsLeave[KeyFlag + 256];
}

void CInput::Release(void)
{
	Safe_Release(m_pMouse);
	Safe_Release(m_pKeyBoard);
	Safe_Release(m_pInput);
}

