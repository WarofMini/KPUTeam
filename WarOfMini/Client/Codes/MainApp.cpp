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
, m_pPVDConnection(NULL)
{
	srand(GetTickCount());
	ZeroMemory(&m_szFPS, sizeof(_tchar) * 128);
	m_bDebugRender = true;
	//�ܼ�â===================================
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
	cout << "�ܼ�â�׽�Ʈ" << endl;
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

	//Physx SDK Engine Init=========================
	InitializePhysxEngine();


	//�ؽ��� �غ�, tga, dds ��� ��밡��
	Ready_TextureFromFile(pGraphicDev, pContext);

	// Buffer
	CResourcesMgr::GetInstance()->Ready_Buffer(pGraphicDev, pContext, RESOURCE_STAGE, CResourcesMgr::BUFFER_RCTEX, L"Buffer_RcTex");
	CResourcesMgr::GetInstance()->Ready_Buffer(pGraphicDev, pContext, RESOURCE_STAGE, CResourcesMgr::BUFFER_CUBE, L"Buffer_CubeTex");
	CResourcesMgr::GetInstance()->Ready_Buffer(pGraphicDev, pContext, RESOURCE_STAGE, CResourcesMgr::BUFFER_FLAGTEX, L"Buffer_FlagTexOne", 10, 10, 40, 20);
	CResourcesMgr::GetInstance()->Ready_Buffer(pGraphicDev, pContext, RESOURCE_STAGE, CResourcesMgr::BUFFER_FLAGTEX, L"Buffer_FlagTexTwo", 10, 10, 40, 20);
	CResourcesMgr::GetInstance()->Ready_Buffer(pGraphicDev, pContext, RESOURCE_STAGE, CResourcesMgr::BUFFER_FLAGTEX, L"Buffer_FlagTexThree", 10, 10, 40, 20);
	// Management
	if (FAILED(CManagement::GetInstance()->Ready_Management(pGraphicDev, pContext, m_pPxPhysicsSDK, m_pPxScene, m_pPxControllerManager, m_pCooking)))
	{
		MSG_BOX(L"Ready_Management Failed");
		return E_FAIL;
	}

	// Scene
	if (FAILED(CManagement::GetInstance()->Change_Scene(CSceneSelector(SCENE_TITLE))))
	{
		MSG_BOX(L"Change_Scene(SCENE_TITLE) Failed");
		return E_FAIL;
	}

	// Input
	if (FAILED(CInput::GetInstance()->Ready_InputDevice(g_hInst, g_hWnd)))
	{
		MSG_BOX(L"Ready_InputDev Failed");
		return FALSE;
	}


	//Ready Font
	CFontMgr::GetInstance()->Ready_Font(pGraphicDev, pContext, L"���");

	return S_OK;
}

INT CMainApp::Update(const _float& fTimeDelta)
{
	m_fTime += fTimeDelta;

	if (m_pPxScene)
	{
		m_pPxScene->simulate(fTimeDelta);
		m_pPxScene->fetchResults(true);
	}

	CInput::GetInstance()->SetInputState();
	// ��ǲ ��ġ �ҽ� ��� �ִ� �Լ�.
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

	//����׿� ��Ʈ ���� ����
	if (GetAsyncKeyState('L') & 1)
	{
		if (m_bDebugRender)
			m_bDebugRender = false;
		else
			m_bDebugRender = true;
	}

	if (m_bDebugRender)
	{
		Render_FPS();
		Render_CurrentScene();
	}

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

	//Physx SDK Release=======================
	ReleasePhysxEngine();
	//========================================

	CFrameMgr::GetInstance()->DestroyInstance();
	CTimeMgr::GetInstance()->DestroyInstance();
	CFontMgr::GetInstance()->DestroyInstance();
	CInput::GetInstance()->DestroyInstance();
	CGraphicDev::GetInstance()->DestroyInstance();

	delete this;
}



void CMainApp::Ready_TextureFromFile(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext)
{
	//�ؽ����̸�-Ȯ����-����(0 �ܵ� �ؽ���, n ������ �ؽ��� �ѹ��� �ε� ��)
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
	//Render Font
	CFontMgr::GetInstance()->Render_Font(L"���", L"<Debug Information>", 15.f, 10.f, 0.f, D3DXCOLOR(0.0f, 0.f, 1.f, 1.f));
	CFontMgr::GetInstance()->Render_Font(L"���", m_szFPS, 15.f, 10.f, 30.f, D3DXCOLOR(0.0f, 1.f, 0.f, 1.f));
}

void CMainApp::Render_CurrentScene(void)
{
	switch (m_eSceneID)
	{
	case SCENE_LOGO:
		CFontMgr::GetInstance()->Render_Font(L"���", L"Current Scene : Logo", 15.f, 10.f, 50.f, D3DXCOLOR(0.0f, 1.f, 0.f, 1.f));

		if (m_bLogoLoading == TRUE)
		{
			CFontMgr::GetInstance()->Render_Font(L"���", L"LoadingState : END", 15.f, 10.f, 75.f, D3DXCOLOR(1.0f, 1.f, 0.f, 1.f));
			CFontMgr::GetInstance()->Render_Font(L"���", L"Press Enter Key", 15.f, 10.f, 100.f, D3DXCOLOR(1.0f, 1.f, 0.f, 1.f));
		}
		else
			CFontMgr::GetInstance()->Render_Font(L"���", L"LoadingState : ING", 15.f, 10.f, 75.f, D3DXCOLOR(0.0f, 0.f, 1.f, 1.f));

		break;


	case SCENE_STAGE:
		Stage_DebugInfo();
		break;
	}
}

void CMainApp::Stage_DebugInfo(void)
{
	CFontMgr::GetInstance()->Render_Font(L"���", L"Current Scene : Stage", 15.f, 10.f, 50.f, D3DXCOLOR(0.0f, 1.f, 0.f, 1.f));
	CFontMgr::GetInstance()->Render_Font(L"���", L"����Ű ����", 15.f, 10.f, 80.f, D3DXCOLOR(0.1f, 1.f, 0.f, 1.f));
	CFontMgr::GetInstance()->Render_Font(L"���", L"Q : Mouse Fix(ON/OFF)", 15.f, 10.f, 100.f, D3DXCOLOR(0.1f, 1.f, 0.f, 1.f));

	CFontMgr::GetInstance()->Render_Font(L"���", L"Fix Mouse : ", 15.f, 15.f, 120.f, D3DXCOLOR(1.0f, 0.f, 0.f, 1.f));

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
	CFontMgr::GetInstance()->Render_Font(L"���", strFixMouseState.c_str(), 15.f, 100.f, 120.f, FixColor);


	_float fSpeed = CCameraMgr::GetInstance()->Get_CurrentCameraSpeed();

	//�������� ���ڿ��� ��ȯ
	wstring strspeed = to_wstring((_int)fSpeed);

	CFontMgr::GetInstance()->Render_Font(L"���", L"Camera Speed : ", 15.f, 10.f, 140.f, D3DXCOLOR(0.0f, 0.0f, 1.f, 1.f));

	CFontMgr::GetInstance()->Render_Font(L"���", L"Speed Up(Key : O), Speed Down(Key : P)", 15.f, 10.f, 160.f, D3DXCOLOR(0.0f, 0.0f, 1.f, 1.f));

	CFontMgr::GetInstance()->Render_Font(L"���", strspeed.c_str(), 15.f, 125.f, 140.f, D3DXCOLOR(0.0f, 0.0f, 1.f, 1.f));


	CFontMgr::GetInstance()->Render_Font(L"���", L"I : ChangingCamera(Dynamic/Static)",  15.f, 10.f, 180.f, D3DXCOLOR(0.0f, 1.0f, 0.f, 1.f));
	
	
	wstring strCamera = L"CameraState : ";

	//���� ī�޶� 
	if (CCameraMgr::GetInstance()->Get_CurCamera() == CCameraMgr::CAMERA_DYNAMIC)
		strCamera += L" DYNAMIC";
	else if (CCameraMgr::GetInstance()->Get_CurCamera() == CCameraMgr::CAMERA_STATIC)
	{
		strCamera += L" STATIC";
	}

	CFontMgr::GetInstance()->Render_Font(L"���", strCamera.c_str(), 15.f, 10.f, 200.f, D3DXCOLOR(0.0f, 0.0f, 1.f, 1.f));


	CFontMgr::GetInstance()->Render_Font(L"���", L"L : CollisionDraw(On/Off)", 15.f, 10.f, 230.f, D3DXCOLOR(0.0f, 1.0f, 0.f, 1.f));

	CFontMgr::GetInstance()->Render_Font(L"���", L"DebugCollision : ", 15.f, 10.f, 250.f, D3DXCOLOR(0.0f, 1.0f, 0.f, 1.f));


	wstring strOnOff = L"Off";
	D3DXCOLOR OnOffColor = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
	if (g_bCollisionDraw == TRUE)
	{
		OnOffColor = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
		strOnOff = L"ON";
	}

	CFontMgr::GetInstance()->Render_Font(L"���", strOnOff.c_str(), 15.f, 130.f, 252.f, OnOffColor);

	CFontMgr::GetInstance()->Render_Font(L"���", L"F4 : ȭ��ũ�� ����", 15.f, 10.f, 275.f, D3DXCOLOR(0.0f, 1.0f, 0.f, 1.f));

	CFontMgr::GetInstance()->Render_Font(L"���", L"K : ����Ÿ�� On/Off", 15.f, 10.f, 295.f, D3DXCOLOR(0.0f, 1.0f, 0.f, 1.f));

}

//����� On/Off üũ
void CMainApp::Debug_KeyCheck(void)
{
	/*
	if (GetAsyncKeyState('L') & 1)
	{
		if (g_bCollisionDraw == TRUE)
			g_bCollisionDraw = FALSE;
		else
			g_bCollisionDraw = TRUE;
	}
	*/
	//âũ�� ���� (��üȭ��, âȭ��)
	/*
	if (GetAsyncKeyState(VK_F4) & 1)
	{
		CGraphicDev::GetInstance()->ChangeScreenMode();
	}
	*/
}

void CMainApp::InitializePhysxEngine()
{

	m_pPxFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_PxDefaultAllocatorCallback, m_PxDefaultErrorCallback);
	PxTolerancesScale PxScale = PxTolerancesScale();
	PxScale.length /= 100;
	PxScale.mass /= 100;
	PxScale.speed /= 100;
	m_pPxPhysicsSDK = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pPxFoundation, PxScale, false);
	

	//Cooking Init
	PxCookingParams params(PxScale);
	params.meshWeldTolerance = 0.001f;
	params.meshPreprocessParams = PxMeshPreprocessingFlags(PxMeshPreprocessingFlag::eWELD_VERTICES | PxMeshPreprocessingFlag::eREMOVE_UNREFERENCED_VERTICES | PxMeshPreprocessingFlag::eREMOVE_DUPLICATED_TRIANGLES);
	m_pCooking = PxCreateCooking(PX_PHYSICS_VERSION, *m_pPxFoundation, params);


	if (FAILED(m_pPxPhysicsSDK == NULL))
	{
		MSG_BOX(L"PhysicsSDK Initialize Failed");
	}

	PxInitExtensions(*m_pPxPhysicsSDK);
	PxSceneDesc sceneDesc(m_pPxPhysicsSDK->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.8f, 0.0f);


	if (!sceneDesc.cpuDispatcher)
	{
		PxDefaultCpuDispatcher* pCpuDispatcher = PxDefaultCpuDispatcherCreate(1);
		sceneDesc.cpuDispatcher = pCpuDispatcher;
	}

	if (!sceneDesc.filterShader)
		sceneDesc.filterShader = PxDefaultSimulationFilterShader;

	m_pPxScene = m_pPxPhysicsSDK->createScene(sceneDesc);
	m_pPxControllerManager = PxCreateControllerManager(*m_pPxScene);


	// Physx Visual Debugger
	if (NULL == m_pPxPhysicsSDK->getPvdConnectionManager())
	{
		cout << "PVD Connect Failed" << endl;
		return;
	}
	PxVisualDebuggerConnectionFlags connectionFlags = PxVisualDebuggerExt::getAllConnectionFlags();
	m_pPxPhysicsSDK->getVisualDebugger()->setVisualDebuggerFlag(PxVisualDebuggerFlag::eTRANSMIT_SCENEQUERIES, true);
	m_pPVDConnection = PxVisualDebuggerExt::createConnection(m_pPxPhysicsSDK->getPvdConnectionManager(), "127.0.0.1", 5425, 1000, connectionFlags);

}

void CMainApp::ReleasePhysxEngine()
{
	if (m_pPVDConnection) m_pPVDConnection->release();
	if (m_pPxControllerManager) m_pPxControllerManager->release();
	if (m_pPxScene) m_pPxScene->release();
	if (m_pPxFoundation) m_pPxFoundation->release();
	if (m_pPxPhysicsSDK) m_pPxPhysicsSDK->release();
	if (m_pCooking) m_pCooking->release();
}
