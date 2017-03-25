#ifndef Input_h__
#define Input_h__

#include "Include.h"

class CInput
{
public:
	DECLARE_SINGLETON(CInput)

private:
	explicit	CInput(void);
	virtual		~CInput(void);

public:
	enum MOUSEKEYSTATE { DIM_LB, DIM_RB, DIM_HB, DIM_END };
	enum MOUSEMOVESTATE { DIMS_X, DIMS_Y, DIMS_Z, DIMS_END };


public:
	_byte 						Get_DIKeyState(_ubyte byKeyID);
	_byte 						Get_DIMouseState(MOUSEKEYSTATE byMouseID);
	_long 						Get_DIMouseMove(MOUSEMOVESTATE byMouseMoveState);

	BYTE	GetDIKeyStateOnce(BYTE KeyFlag);
	BYTE	GetDIKeyStateLeave(BYTE KeyFlag);
	BYTE	GetDIMouseStateOnce(BYTE KeyFlag);
	BYTE	GetDIMouseStateLeave(BYTE KeyFlag);

public:
	void						Set_Acquire(void);
public:
	HRESULT						Ready_InputDevice(HINSTANCE hInst, HWND hWnd);
	void						SetInputState(void);
	void						Reset_InputState(void);
	void						ResetOnce(void);
	void						SetAcquire(void);
	void						SetInputOnce(void);

private:
	LPDIRECTINPUT8				m_pInputSDK;
	LPDIRECTINPUTDEVICE8		m_pKeyBoard;
	LPDIRECTINPUTDEVICE8		m_pMouse;
private:
	_byte						m_byKeyState[256];
	DIMOUSESTATE				m_MouseState;

	BYTE						m_byCheckOnce[260];
	BYTE						m_byIsOnce[260];
	BYTE						m_byIsLeave[260];

public:
	void						Release(void);
};

#endif