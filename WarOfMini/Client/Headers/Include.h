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

extern XMFLOAT3		g_vPlayerPos;


extern _bool		g_bFocus;

extern _bool		g_bSetAquire;

extern _bool		g_bCollisionDraw;


extern _uint		WINCX;
extern _uint		WINCY;

extern DIRECTIONALIGHT_CB g_tDirectionalLight;

extern BYTE		g_CurrentScene;

extern _uint	g_iCurrentCount;

extern _bool	g_bCountCheck;

extern wstring  g_strFullPath;

//Sound Value
extern _float g_fBGMValue;
extern _float g_fEffectSoundValue;

extern _bool  g_bCursorShow;

extern _int	  g_GameState;

extern _float g_fGunReaction;

extern _float g_fLightPower;

extern _bool  g_bBlackOut;

#endif