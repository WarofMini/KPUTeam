#ifndef Input_h__
#define Input_h__

#include "Include.h"

class CInput
{
public:
	DECLARE_SINGLETON(CInput)

private:
	explicit CInput(void);
			~CInput(void);

public:
	enum MOUSEKEYSTATE { DIM_LB, DIM_RB, DIM_HB, DIM_END };
	enum MOUSEMOVESTATE { DIMS_X, DIMS_Y, DIMS_Z, DIMS_END };

private:
	LPDIRECTINPUT8			m_pInput;
	LPDIRECTINPUTDEVICE8	m_pKeyBoard;
	LPDIRECTINPUTDEVICE8	m_pMouse;

private:
	DIMOUSESTATE				m_MouseState;
	_ubyte						m_byKeyState[256];
	_ubyte						m_byCheckOnce[260];
	_ubyte						m_byIsOnce[260];
	_ubyte						m_byIsLeave[260];


public:
	_ubyte GetDIKeyState(_ubyte byKeyID);
	_ubyte GetDIMouseState(_ubyte byMouseID);
	_long  GetDIMouseMove(MOUSEMOVESTATE byMouseMoveState);

	void	SetAcquire();

public:
	_ubyte	GetDIKeyStateOnce(_ubyte KeyFlag);
	_ubyte	GetDIKeyStateLeave(_ubyte KeyFlag);
	_ubyte	GetDIMouseStateOnce(_ubyte KeyFlag);
	_ubyte	GetDIMouseStateLeave(_ubyte KeyFlag);

public:
	void	SetInputState(void);
	void	ResetInputState(void);
	void	ResetOnce(void);
	HRESULT	InitInputDevice(HINSTANCE hInst, HWND hWnd);


private:
	HRESULT	InitKeyBoard(HWND hWnd);
	HRESULT	InitMouse(HWND hWnd);
	void	SetInputOnce(void);

private:
	void	Release(void);
};

#endif