// stdafx.h : ���� ��������� ���� ��������� �ʴ�
// ǥ�� �ý��� ���� ���� �Ǵ� ������Ʈ ���� ���� ������
// ��� �ִ� ���� �����Դϴ�.
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
// Windows ��� ����:
#include <windows.h>

// C ��Ÿ�� ��� �����Դϴ�.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


// TODO: ���α׷��� �ʿ��� �߰� ����� ���⿡�� �����մϴ�.

// Directx
#include <d3d11_1.h>
#include <d3dx11.h>
#include <D3DX10Math.h>
#include <d3dcompiler.h>
#include <xnamath.h>
#include <fbxsdk.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include <vector>
#include <list>
#include <map>
#include <string>
#include <ctime>

#include <process.h>
///////////////////////////////////////////
#pragma comment(lib, "ws2_32")

#include <WinSock2.h>

#include <iostream>
using namespace std;

#include "Protocol.h"
// client ��� class
#include<fstream>
#include"clientClass.h"

#define WM_SOCKET (WM_USER + 1)

static AsynchronousClientClass g_Client;
///////////////////////////////////////////

#pragma warning(disable : 4005)

#ifdef _DEBUG
//�ܼ�â
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")

#endif