// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
// Windows 헤더 파일:
#include <windows.h>
#include <functional>
// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <cassert>
#include <cstdio>
#include <shellapi.h>
// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.

// Directx
#include <d3d11_1.h>
#include <d3dx11.h>
#include <d3dx9math.h>
#include <D3DX10Math.h>
#include <d3dcompiler.h>
#include <xnamath.h>
#include <fbxsdk.h>
#include <DXGIFormat.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <vector>
#include <list>
#include <map>
#include <string>
#include <ctime>
#include <algorithm>
#include <process.h>
#include <fstream>
#include "xnacollision.h"

using namespace XNA;

//FMOD====================================================//
#include <io.h>
#include "fmod.h"
#include "fmod.hpp"
#include "fmod_dsp.h"
#include "fmod_errors.h"
//======================================================//

//피직스================================================//
#include <PxPhysicsAPI.h>
using namespace physx;
//=====================================================//
//==서버===============================================//
#pragma comment(lib, "ws2_32")

#include <WinSock2.h>

#include <iostream>
using namespace std;
#include "Protocol.h"
// client 통신 class
#include<fstream>
#include"clientClass.h"

#define WM_SOCKET (WM_USER + 1)
extern AsynchronousClientClass* g_Client;

extern int		g_myid;
extern XMFLOAT3 g_vPos;

///////////////////////////////////////////

#pragma warning(disable : 4005)

#ifdef _DEBUG
//콘솔창
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")

#endif