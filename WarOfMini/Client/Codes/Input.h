#pragma once
#include "Include.h"

class CInput
{
private:
	CInput(void);
	~CInput(void);

public:
	DECLARE_SINGLETON(CInput)

public:
	enum MOUSECLICK { DIM_LBUTTON, DIM_RBUTTON, DIM_MBUTTON };
	enum MOUSEMOVE { DIM_X, DIM_Y, DIM_Z };


public:
	BYTE GetDIKeyState(BYTE KeyFlag);
	BYTE GetDIMouseState(BYTE KeyFlag);
	long GetDIMouseMove(MOUSEMOVE KeyFlag);

	BYTE	GetDIKeyStateOnce(BYTE KeyFlag);
	BYTE	GetDIKeyStateLeave(BYTE KeyFlag);
	BYTE	GetDIMouseStateOnce(BYTE KeyFlag);
	BYTE	GetDIMouseStateLeave(BYTE KeyFlag);

public:
	void	SetInputState(void);
	void	ResetInputState(void);
	void	ResetOnce(void);
	HRESULT	InitInputDevice(HINSTANCE hInst, HWND hWnd);
	void	SetAcquire();

private:
	HRESULT	InitKeyBoard(HWND hWnd);
	HRESULT	InitMouse(HWND hWnd);
	void	SetInputOnce(void);

private:
	LPDIRECTINPUT8			m_pInput;
	LPDIRECTINPUTDEVICE8	m_pKeyBoard;
	LPDIRECTINPUTDEVICE8	m_pMouse;

private:					
	BYTE					m_byKeyState[256];
	DIMOUSESTATE			m_MouseState;

	BYTE						m_byCheckOnce[260];
	BYTE						m_byIsOnce[260];
	BYTE						m_byIsLeave[260];

private:
	void	Release(void);
};

