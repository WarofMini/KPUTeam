#ifndef Include_h__
#define Include_h__

#include "../Default/stdafx.h"

using namespace std;

#include "Enum.h"
#include "Value.h"
#include "Macro.h"
#include "Struct.h"
#include "Typedef.h"
#include "Functor.h"
#include "Function.h"

//전역변수
extern HWND			g_hWnd;
extern HINSTANCE	g_hInst;
extern DWORD		g_dwLightIndex;
extern D3DXVECTOR3	g_vLightDir;

extern Scene_Type	m_eSceneID;

extern BOOL			m_bLogoLoading;
#endif