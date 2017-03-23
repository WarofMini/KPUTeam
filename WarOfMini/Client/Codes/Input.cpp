#include "stdafx.h"
#include "Input.h"

IMPLEMENT_SINGLETON(CInput)


CInput::CInput(void)
{

}

CInput::~CInput(void)
{
}


_byte CInput::Get_DIKeyState(_ubyte byKeyID)
{
	return m_byKeyState[byKeyID];
}

_byte CInput::Get_DIMouseState(MOUSEKEYSTATE byMouseID)
{
	return m_MouseState.rgbButtons[byMouseID];
}

_long CInput::Get_DIMouseMove(MOUSEMOVESTATE byMouseMoveState)
{
	return *(((_long*)(&m_MouseState)) + byMouseMoveState);
}

void CInput::Set_Acquire(void)
{
	m_pKeyBoard->Acquire();
	m_pMouse->Acquire();
}

HRESULT CInput::Ready_InputDevice(HINSTANCE hInst, HWND hWnd)
{
	if (FAILED(DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pInputSDK, NULL)))
		return E_FAIL;

	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysKeyboard, &m_pKeyBoard, NULL)))
		return E_FAIL;

	m_pKeyBoard->SetDataFormat(&c_dfDIKeyboard);
	m_pKeyBoard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
	m_pKeyBoard->Acquire();

	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysMouse, &m_pMouse, NULL)))
		return E_FAIL;

	m_pMouse->SetDataFormat(&c_dfDIMouse);
	m_pMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
	m_pMouse->Acquire();

	return S_OK;
}

void CInput::SetUp_InputState(void)
{
	m_pKeyBoard->GetDeviceState(256, m_byKeyState);
	m_pMouse->GetDeviceState(sizeof(m_MouseState), &m_MouseState);
}

void CInput::Reset_InputState(void)
{
	ZeroMemory(m_byKeyState, sizeof(_ubyte) * 256);
	ZeroMemory(&m_MouseState, sizeof(MOUSEKEYSTATE));
}


void CInput::Release(void)
{
	Safe_Release(m_pMouse);
	Safe_Release(m_pKeyBoard);
	Safe_Release(m_pInputSDK);

	delete this;
}