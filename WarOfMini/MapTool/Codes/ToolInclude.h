#ifndef ToolInclude_h__
#define ToolInclude_h__

#include "ToolEnum.h"
#include "ToolValue.h"
#include "ToolMacro.h"
#include "ToolStruct.h"

//전역변수
extern HWND			g_hWnd;
extern HINSTANCE	g_hInst;
extern DWORD		g_dwLightIndex;
extern D3DXVECTOR3	g_vLightDir;

//MFC 전역 마우스
extern D3DXVECTOR3	g_vMouse;
extern D3DXVECTOR3  g_vViewMouse;
extern bool			g_bRBtnCheck;

#endif
