// WarOfMini.cpp : ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"
#include "WarOfMini.h"
#include "Include.h"
#include "MainApp.h"
#include "Timer_Manager.h"
#include "FrameMgr.h"
#include "FontMgr.h"
#include "GraphicDev.h"

//��üũ
//#include "vld.h"

#define MAX_LOADSTRING 100

// ���� ����:
HINSTANCE g_hInst;                               // ���� �ν��Ͻ��Դϴ�.
WCHAR szTitle[MAX_LOADSTRING];                  // ���� ǥ���� �ؽ�Ʈ�Դϴ�.
WCHAR szWindowClass[MAX_LOADSTRING];            // �⺻ â Ŭ���� �̸��Դϴ�.

HWND g_hWnd;
Scene_Type	m_eSceneID;
BOOL m_bLogoLoading;
_bool g_bFocus;
_bool g_bSetAquire;
_bool g_bCollisionDraw;
_uint WINCX;
_uint WINCY;
_uint g_iCurrentCount;
_bool g_bCountCheck;
DIRECTIONALIGHT_CB g_tDirectionalLight;
AsynchronousClientClass* g_Client = NULL;
wstring g_strFullPath;

//Sound Value
_float g_fBGMValue = 0.5f;
_float g_fEffectSoundValue = 0.5f;

//Mouse Cursor
_bool  g_bCursorShow = false;

_bool  g_bBlackOut = false;


// �� �ڵ� ��⿡ ��� �ִ� �Լ��� ������ �����Դϴ�.
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);


int APIENTRY wWinMain( HINSTANCE hInstance,
                      HINSTANCE hPrevInstance,
                      LPWSTR    lpCmdLine,
                      int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: ���⿡ �ڵ带 �Է��մϴ�.

    // ���� ���ڿ��� �ʱ�ȭ�մϴ�.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WAROFMINI, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

	WINCX = 1600, WINCY = 900;
	g_iCurrentCount = 0;
	g_bCountCheck = false;
	g_strFullPath = L"";

    // ���� ���α׷� �ʱ�ȭ�� �����մϴ�.
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WAROFMINI));

	m_bLogoLoading = FALSE;
	g_bCollisionDraw = FALSE;
	static bool	m_bServerConnected = false;

	if (m_bServerConnected == false)
	{
		g_Client = new AsynchronousClientClass;
		g_Client->InitSock(g_hWnd);
		m_bServerConnected = true;
	}

    MSG msg;
	msg.message = WM_NULL;

	CMainApp*	pMainApp = NULL;
	pMainApp = CMainApp::Create();

	if (NULL == pMainApp)
	{
		Safe_Release(pMainApp);
		return 0;
	}

	if (FAILED(CTimeMgr::GetInstance()->Ready_Timer(L"Timer_Immediate")))
		return FALSE;

	if (FAILED(CTimeMgr::GetInstance()->Ready_Timer(L"Timer_FPS:60")))
		return FALSE;

	//������ �Ŵ���
	if (FAILED(CFrameMgr::GetInstance()->Ready_Frame(L"Frame_60", 60.0f)))
		return FALSE;



    // �⺻ �޽��� �����Դϴ�.
	while (TRUE)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (WM_QUIT == msg.message)
				break;

			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			CTimeMgr::GetInstance()->SetUp_TimeDelta(L"Timer_Immediate");

			_float	fTimeDelta_Immediate = CTimeMgr::GetInstance()->Get_TimeDelta(L"Timer_Immediate");

			//1�ʿ� 60���� true�� ����, �������� false
			if (CFrameMgr::GetInstance()->isPermit_Call(L"Frame_60", fTimeDelta_Immediate))
			{
				CTimeMgr::GetInstance()->SetUp_TimeDelta(L"Timer_FPS:60");

				_float fTimeDelta_60 = CTimeMgr::GetInstance()->Get_TimeDelta(L"Timer_FPS:60");

				if(pMainApp->Update(fTimeDelta_60))
					break;
				pMainApp->Render();
			}

		}
	}

	Safe_Release(pMainApp);

	delete g_Client;
	g_Client = nullptr;

    return (int) msg.wParam;
}



//
//  �Լ�: MyRegisterClass()
//
//  ����: â Ŭ������ ����մϴ�.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WAROFMINI));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = NULL;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   �Լ�: InitInstance(HINSTANCE, int)
//
//   ����: �ν��Ͻ� �ڵ��� �����ϰ� �� â�� ����ϴ�.
//
//   ����:
//
//        �� �Լ��� ���� �ν��Ͻ� �ڵ��� ���� ������ �����ϰ�
//        �� ���α׷� â�� ���� ���� ǥ���մϴ�.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;
	RECT rc = { 0, 0, WINCX, WINCY };

   g_hInst = hInstance; // �ν��Ͻ� �ڵ��� ���� ������ �����մϴ�.

   AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
	   CW_USEDEFAULT, 0, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance, NULL);

   g_hWnd = hWnd;

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  �Լ�: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ����:  �� â�� �޽����� ó���մϴ�.
//
//  WM_COMMAND  - ���� ���α׷� �޴��� ó���մϴ�.
//  WM_PAINT    - �� â�� �׸��ϴ�.
//  WM_DESTROY  - ���� �޽����� �Խ��ϰ� ��ȯ�մϴ�.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_KILLFOCUS:
		g_bFocus = false;
		g_bSetAquire = true;
		break;

	case WM_SETFOCUS:
		g_bFocus = true;
		break;
	case WM_SIZE:
	{
		WINCX = LOWORD(lParam);
		WINCY = HIWORD(lParam);
		
		//CGraphicDev::GetInstance()->OnResizeBackBuffers();
		break;
	}
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // �޴� ������ ���� �м��մϴ�.
            switch (wmId)
            {
			case IDM_ABOUT:
				DialogBox(g_hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
				break;

            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: ���⿡ hdc�� ����ϴ� �׸��� �ڵ带 �߰��մϴ�.
            EndPaint(hWnd, &ps);
        }
        break;

	/*case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		}
		break;*/
	/*case WM_DESTROY:
		PostQuitMessage(0);
		break;*/
	case WM_SOCKET:
		g_Client->ProcessWinMessage(hWnd, message, wParam, lParam);
		return 0;
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;

		}
		break;
	}
	return (INT_PTR)FALSE;
}
