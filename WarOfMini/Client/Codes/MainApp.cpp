#include "stdafx.h"
#include "MainApp.h"
#include "Device.h"
#include "TimeMgr.h"
#include "Input.h"
#include "SceneMgr.h"
#include "ResourcesMgr.h"
#include "ShaderMgr.h"
#include "Logo.h"
#include "ObjMgr.h"
#include "AnimationMgr.h"

CMainApp::CMainApp()
{

}

CMainApp::~CMainApp()
{
	Release();
}

HRESULT CMainApp::Initialize(void)
{
#ifdef _DEBUG
	cout << "ㅋㄴ솔창테스트" << endl;
#endif

	HRESULT hr = S_OK;

	hr = CDevice::GetInstance()->CreateDevice();
	m_pGrapicDevcie = CDevice::GetInstance();

	if(FAILED(hr))
	{
		MessageBox(NULL, L"GraphicDev Create Failed", L"Fail", MB_OK);
		return E_FAIL;
	}

	hr = CInput::GetInstance()->InitInputDevice(g_hInst, g_hWnd);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"DInput Create Failed", L"Fail", MB_OK);
		return E_FAIL;
	}

	CTimeMgr::GetInstance()->InitTime();


	if (FAILED(CResourcesMgr::GetInstance()->ReserveContainerSize(RESOURCE_END)))
	{
		MessageBox(NULL, L"System Message", L"Resource Container Reserve Failed", MB_OK);
		return E_FAIL;
	}

	if (FAILED(Add_ShaderFile()))
		return E_FAIL;

	hr = CResourcesMgr::GetInstance()->AddBuffer(RESOURCE_STATIC, BUFFER_RCTEX, L"Buffer_RcTex");
	FAILED_CHECK_RETURN_MSG(hr, E_FAIL, L"Buffer_RcTex 생성 실패");

	CScene* pScene = NULL;
	pScene = CLogo::Create();
	CSceneMgr::GetInstance()->AddScene(SCENE_LOGO, pScene);
	CSceneMgr::GetInstance()->ChangeScene(SCENE_LOGO);

	return S_OK;
}

int CMainApp::Update(void)
{
	CTimeMgr::GetInstance()->SetTime();

	if (GetFocus() == g_hWnd)
		CInput::GetInstance()->SetInputState();
	else
		CInput::GetInstance()->ResetInputState();

	CSceneMgr::GetInstance()->Update();

	return 0;
}

void CMainApp::Render(void)
{
	m_pGrapicDevcie->BeginDevice();
	CSceneMgr::GetInstance()->Render();
	m_pGrapicDevcie->EndDevice();
}

HRESULT CMainApp::Add_ShaderFile(void)
{
	//일반
	HRESULT hr = E_FAIL;
	hr = CShaderMgr::GetInstance()->AddShaderFiles(L"VS", L"../Bin/ShaderCode/Shader.fx", "VS", "vs_5_0", SHADER_VS);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"System Message", L"Vertex Shader(Default) Create Failed", MB_OK);
		return hr;
	}

	hr = CShaderMgr::GetInstance()->AddShaderFiles(L"PS", L"../Bin/ShaderCode/Shader.fx", "PS", "ps_5_0", SHADER_PS);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"System Message", L"PIXEL Shader(Default) Create Failed", MB_OK);
		return hr;
	}
	//////////////////////////////////////////////////////////////////////////

	//로고용
	hr = CShaderMgr::GetInstance()->AddShaderFiles(L"VS_Logo", L"../Bin/ShaderCode/Shader.fx", "VS_Logo", "vs_5_0", SHADER_VS);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"System Message", L"Vertex Shader(LOGO) Create Failed", MB_OK);
		return hr;
	}
	/////////////////////////////////////////////////////////////////////////

	//다이나믹 매쉬용
	hr = CShaderMgr::GetInstance()->AddShaderFiles(L"VS_ANI", L"../Bin/ShaderCode/Shader.fx", "VS_ANI", "vs_5_0", SHADER_ANI);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"System Message", L"Vertex Shader(Ani) Create Failed", MB_OK);
		return hr;
	}
	/////////////////////////////////////////////////////////////////////////

	return S_OK;
}

void CMainApp::Release(void)
{
	CObjMgr::GetInstance()->DestroyInstance();
	CTimeMgr::GetInstance()->DestroyInstance();
	CShaderMgr::GetInstance()->DestroyInstance();
	CInput::GetInstance()->DestroyInstance();
	CSceneMgr::GetInstance()->DestroyInstance();
	CDevice::GetInstance()->DestroyInstance();
}

CMainApp* CMainApp::Create(void)
{
	CMainApp*		pMainApp = new CMainApp;
	if (FAILED(pMainApp->Initialize()))
	{
		delete pMainApp;
		pMainApp = NULL;
	}
	return pMainApp;
}
