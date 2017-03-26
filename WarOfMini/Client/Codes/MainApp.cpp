#include "stdafx.h"
#include "MainApp.h"
#include "SceneSelector.h"
#include "GraphicDev.h"
#include "ResourcesMgr.h"
#include "Management.h"
#include "ShaderMgr.h"
#include "MeshMgr.h"
#include "RenderTargetMgr.h"
#include "CameraMgr.h"
#include "FrameMgr.h"
#include "Timer_Manager.h"
#include "FontMgr.h"
#include "Input.h"

CMainApp::CMainApp(void)
: m_dwRenderCnt(0)
, m_fTime(0.0f)
{
	ZeroMemory(&m_szFPS, sizeof(_tchar) * 128);
	//콘솔창===================================
	/*AllocConsole();
	freopen("CONOUT$", "wt", stdout);
	SetConsoleTitleA("Debug");*/
	//=========================================
}

CMainApp::~CMainApp(void)
{
	//FreeConsole();
}

HRESULT CMainApp::Initialize(void)
{
#ifdef _DEBUG
	cout << "콘솔창테스트" << endl;
#endif

	// Graphic Device
	ID3D11Device* pGraphicDev = NULL;
	ID3D11DeviceContext* pContext = NULL;

	if (FAILED(CGraphicDev::GetInstance()->Ready_GraphicDev(g_hWnd, CGraphicDev::MODE_WIN, WINCX, WINCY, pGraphicDev, pContext)))
	{
		MSG_BOX(L"Ready_GraphicDev Failed");
		return E_FAIL;
	}

	// Resource
	CResourcesMgr::GetInstance()->Reserve_ContainerSize(RESOURCE_END);

	//텍스쳐 준비, tga, dds 모두 사용가능
	Ready_TextureFromFile(pGraphicDev, pContext);

	// Buffer
	CResourcesMgr::GetInstance()->Ready_Buffer(pGraphicDev, pContext, RESOURCE_STAGE, CResourcesMgr::BUFFER_RCTEX, L"Buffer_RcTex");
	CResourcesMgr::GetInstance()->Ready_Buffer(pGraphicDev, pContext, RESOURCE_STAGE, CResourcesMgr::BUFFER_CUBE, L"Buffer_CubeTex");

	// Management
	if (FAILED(CManagement::GetInstance()->Ready_Management(pGraphicDev, pContext)))
	{
		MSG_BOX(L"Ready_Management Failed");
		return E_FAIL;
	}

	// Scene
	if (FAILED(CManagement::GetInstance()->Change_Scene(CSceneSelector(SCENE_LOGO))))
	{
		MSG_BOX(L"Change_Scene(SCENE_STAGE) Failed");
		return E_FAIL;
	}

	// Input
	if (FAILED(CInput::GetInstance()->Ready_InputDevice(g_hInst, g_hWnd)))
	{
		MSG_BOX(L"Ready_InputDev Failed");
		return FALSE;
	}


	//Ready Font
	CFontMgr::GetInstance()->Ready_Font(pGraphicDev, pContext, L"고딕");


	return S_OK;
}

INT CMainApp::Update(const _float& fTimeDelta)
{
	m_fTime += fTimeDelta;


	CInput::GetInstance()->SetInputState();
		// 인풋 장치 소실 잡아 주는 함수.
	Set_Focus();
	Debug_KeyCheck();

	return CManagement::GetInstance()->Update(fTimeDelta);

	return 0;
}

void CMainApp::Render(void)
{
	++m_dwRenderCnt;

	if (m_fTime >= 1.f)
	{
		wsprintf(m_szFPS, L"FPS : %d", m_dwRenderCnt);
		//SetWindowText(g_hWnd, m_szFPS);
		m_fTime = 0.f;
		m_dwRenderCnt = 0;

	}

	CManagement::GetInstance()->Render();

	Render_FPS();

	Render_CurrentScene();

	CManagement::GetInstance()->GetRenderer()->SwapChain_Clear_RenderGroup();
}

CMainApp* CMainApp::Create(void)
{
	CMainApp* pMainApp = new CMainApp;

	if (FAILED(pMainApp->Initialize()))
	{
		MSG_BOX(L"CMainApp Create Failed");
		Safe_Release(pMainApp);
	}

	return pMainApp;
}

void CMainApp::Set_Focus(void)
{
	if (g_bFocus)
	{
		if (g_bSetAquire)
		{
			CInput::GetInstance()->Set_Acquire();
			g_bSetAquire = false;
		}
	}
	else
		CInput::GetInstance()->Reset_InputState();

}

void CMainApp::Release(void)
{
	CMeshMgr::GetInstance()->DestroyInstance();
	CResourcesMgr::GetInstance()->DestroyInstance();

	CCameraMgr::GetInstance()->DestroyInstance();
	CShaderMgr::GetInstance()->DestroyInstance();
	CRenderTargetMgr::GetInstance()->DestroyInstance();
	CManagement::GetInstance()->DestroyInstance();

	CFrameMgr::GetInstance()->DestroyInstance();
	CTimeMgr::GetInstance()->DestroyInstance();
	CFontMgr::GetInstance()->DestroyInstance();
	CInput::GetInstance()->DestroyInstance();
	CGraphicDev::GetInstance()->DestroyInstance();

	delete this;
}



void CMainApp::Ready_TextureFromFile(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext)
{
	//텍스쳐이름-확장자-개수(0 단독 텍스쳐, n 여러장 텍스쳐 한번에 로드 시)
	wstring wstrPath = L"../Bin/Data/FirstTextureInfomation.txt";

	wifstream inFile;
	inFile.open(wstrPath.c_str(), ios::in);

	_tchar pFileName[MAX_NAME];
	_tchar pFileType[4];
	_tchar pFileNum[4];

	while (!inFile.eof())
	{
		inFile.getline(pFileName, MAX_NAME, '-');
		inFile.getline(pFileType, 4, '-');
		inFile.getline(pFileNum, 4);

		_ushort wTextureCnt = _wtoi(pFileNum);

		wstring wstrTextureKey = L"Texture_";
		wstrTextureKey += pFileName;

		wstring wstrTexturePath = L"../Bin/Resources/Texture/";
		wstrTexturePath += pFileName;
		if (wTextureCnt) wstrTexturePath += L"%d.";
		else wstrTexturePath += L".";
		wstrTexturePath += pFileType;

		CTextures::TEXTURETYPE eTextureType = CTextures::TYPE_NORMAL;

		if (!lstrcmp(pFileType, L"tga"))
			eTextureType = CTextures::TYPE_TGA;

		else if (!lstrcmp(pFileType, L"dds"))
			eTextureType = CTextures::TYPE_DDSCUBE;

		CResourcesMgr::GetInstance()->Ready_Texture(pGraphicDev, pContext, RESOURCE_STAGE, wstrTextureKey.c_str(), eTextureType, wstrTexturePath.c_str(), wTextureCnt);
	}

	inFile.close();
}


void CMainApp::Render_FPS(void)
{
	////Render Font
	CFontMgr::GetInstance()->Render_Font(L"고딕", L"<Debug Information>", 15.f, 10.f, 0.f, D3DXCOLOR(0.0f, 0.f, 1.f, 1.f));
	CFontMgr::GetInstance()->Render_Font(L"고딕", m_szFPS, 15.f, 10.f, 30.f, D3DXCOLOR(0.0f, 1.f, 0.f, 1.f));
}

void CMainApp::Render_CurrentScene(void)
{
	switch (m_eSceneID)
	{
	case SCENE_LOGO:
		CFontMgr::GetInstance()->Render_Font(L"고딕", L"Current Scene : Logo", 15.f, 10.f, 50.f, D3DXCOLOR(0.0f, 1.f, 0.f, 1.f));

		if (m_bLogoLoading == TRUE)
		{
			CFontMgr::GetInstance()->Render_Font(L"고딕", L"LoadingState : END", 15.f, 10.f, 75.f, D3DXCOLOR(1.0f, 1.f, 0.f, 1.f));
			CFontMgr::GetInstance()->Render_Font(L"고딕", L"Press Enter Key", 15.f, 10.f, 100.f, D3DXCOLOR(1.0f, 1.f, 0.f, 1.f));
		}
		else
			CFontMgr::GetInstance()->Render_Font(L"고딕", L"LoadingState : ING", 15.f, 10.f, 75.f, D3DXCOLOR(0.0f, 0.f, 1.f, 1.f));

		break;


	case SCENE_STAGE:
		Stage_DebugInfo();
		break;
	}
}

void CMainApp::Stage_DebugInfo(void)
{
	CFontMgr::GetInstance()->Render_Font(L"고딕", L"Current Scene : Stage", 15.f, 10.f, 50.f, D3DXCOLOR(0.0f, 1.f, 0.f, 1.f));
	CFontMgr::GetInstance()->Render_Font(L"고딕", L"단축키 모음", 15.f, 10.f, 80.f, D3DXCOLOR(0.1f, 1.f, 0.f, 1.f));
	CFontMgr::GetInstance()->Render_Font(L"고딕", L"Q : Mouse Fix(ON/OFF)", 15.f, 10.f, 100.f, D3DXCOLOR(0.1f, 1.f, 0.f, 1.f));

	CFontMgr::GetInstance()->Render_Font(L"고딕", L"Fix Mouse : ", 15.f, 15.f, 120.f, D3DXCOLOR(1.0f, 0.f, 0.f, 1.f));

	wstring strFixMouseState;
	D3DXCOLOR FixColor;

	if ((*CCameraMgr::GetInstance()->Get_CurCameraFixMouseCheck()) == true) //FixMouse On
	{
		strFixMouseState = L"On";
		FixColor = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	}
	else
	{
		strFixMouseState = L"Off";
		FixColor = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
	}
	CFontMgr::GetInstance()->Render_Font(L"고딕", strFixMouseState.c_str(), 15.f, 100.f, 120.f, FixColor);


	_float fSpeed = CCameraMgr::GetInstance()->Get_CurrentCameraSpeed();

	//정수형을 문자열로 변환
	wstring strspeed = to_wstring((_int)fSpeed);

	CFontMgr::GetInstance()->Render_Font(L"고딕", L"Camera Speed : ", 15.f, 10.f, 140.f, D3DXCOLOR(0.0f, 0.0f, 1.f, 1.f));

	CFontMgr::GetInstance()->Render_Font(L"고딕", L"Speed Up(Key : O), Speed Down(Key : P)", 15.f, 10.f, 160.f, D3DXCOLOR(0.0f, 0.0f, 1.f, 1.f));

	CFontMgr::GetInstance()->Render_Font(L"고딕", strspeed.c_str(), 15.f, 125.f, 140.f, D3DXCOLOR(0.0f, 0.0f, 1.f, 1.f));


	CFontMgr::GetInstance()->Render_Font(L"고딕", L"I : ChangingCamera(Dynamic/Static)",  15.f, 10.f, 180.f, D3DXCOLOR(0.0f, 1.0f, 0.f, 1.f));
	
	
	wstring strCamera = L"CameraState : ";

	//현재 카메라 
	if (CCameraMgr::GetInstance()->Get_CurCamera() == CCameraMgr::CAMERA_DYNAMIC)
		strCamera += L" DYNAMIC";
	else if (CCameraMgr::GetInstance()->Get_CurCamera() == CCameraMgr::CAMERA_STATIC)
	{
		strCamera += L" STATIC";
	}

	CFontMgr::GetInstance()->Render_Font(L"고딕", strCamera.c_str(), 15.f, 10.f, 200.f, D3DXCOLOR(0.0f, 0.0f, 1.f, 1.f));


	CFontMgr::GetInstance()->Render_Font(L"고딕", L"L : CollisionDraw(On/Off)", 15.f, 10.f, 230.f, D3DXCOLOR(0.0f, 1.0f, 0.f, 1.f));

	CFontMgr::GetInstance()->Render_Font(L"고딕", L"DebugCollision : ", 15.f, 10.f, 250.f, D3DXCOLOR(0.0f, 1.0f, 0.f, 1.f));


	wstring strOnOff = L"Off";
	D3DXCOLOR OnOffColor = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
	if (g_bCollisionDraw == TRUE)
	{
		OnOffColor = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
		strOnOff = L"ON";
	}

	CFontMgr::GetInstance()->Render_Font(L"고딕", strOnOff.c_str(), 15.f, 130.f, 252.f, OnOffColor);

}

//디버그 On/Off 체크
void CMainApp::Debug_KeyCheck(void)
{
	if (GetAsyncKeyState('L') & 1)
	{
		if (g_bCollisionDraw == TRUE)
			g_bCollisionDraw = FALSE;
		else
			g_bCollisionDraw = TRUE;
	}
}
