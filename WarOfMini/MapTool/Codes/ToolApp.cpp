#include "stdafx.h"
#include "ToolApp.h"

//½Ì±ÛÅæ °´Ã¼
#include "Device.h"
#include "TimeMgr.h"
#include "ResourcesMgr.h"
#include "ShaderMgr.h"
#include "Input.h"
#include "RenderMgr.h"
#include "ObjMgr.h"
#include "SceneMgr.h"

//ÀÏ¹Ý°´Ã¼
#include "Shader.h"
#include "Obj.h"
#include "Info.h"
#include "Texture.h"
#include "Scene.h"
#include "ToolStage.h"
#include "ToolCamera.h"


CToolApp::CToolApp()
{
	/*AllocConsole();
	freopen("CONOUT$", "wt", stdout);
	SetConsoleTitleA("Debug");*/
}


CToolApp::~CToolApp()
{
	//FreeConsole();
	Release();

}

HRESULT CToolApp::Initialize(void)
{
	srand(UINT(time(NULL)));


	CDevice::GetInstance()->CreateDevice();
	m_pGrapicDevcie = CDevice::GetInstance();

	CTimeMgr::GetInstance()->InitTime();

	HRESULT hr;

	if (FAILED(CResourcesMgr::GetInstance()->ReserveContainerSize(RESOURCE_END)))
	{
		MessageBox(NULL, L"System Message", L"Resource Container Reserve Failed", MB_OK);
		return E_FAIL;
	}
	//»ö±ò Shader=====================================================================================================================
	hr = CShaderMgr::GetInstance()->AddShaderFiles(L"VS_COLOR", L"../ShaderCode/Shader.fx", "VS_COLOR", "vs_5_0", SHADER_VS_COLOR);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"System Message", L"Vertex Shader(Color) Create Failed", MB_OK);
		return hr;
	}

	hr = CShaderMgr::GetInstance()->AddShaderFiles(L"PS_COLOR", L"../ShaderCode/Shader.fx", "PS_COLOR", "ps_5_0", SHADER_PS);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"System Message", L"PIXEL Shader(Color) Create Failed", MB_OK);
		return hr;
	}

	//================================================================================================================================

	//ÀÏ¹Ý Shader
	hr = CShaderMgr::GetInstance()->AddShaderFiles(L"VS", L"../ShaderCode/Shader.fx", "VS", "vs_5_0", SHADER_VS);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"System Message", L"Vertex Shader(Default) Create Failed", MB_OK);
		return hr;
	}

	hr = CShaderMgr::GetInstance()->AddShaderFiles(L"PS", L"../ShaderCode/Shader.fx", "PS", "ps_5_0", SHADER_PS);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"System Message", L"PIXEL Shader(Default) Create Failed", MB_OK);
		return hr;
	}

	//Camera======================================================================
	if (FAILED(CToolCamera::GetInstance()->Initialize()))
	{
		MessageBox(NULL, L"System Message", L"ToolCamera Initialize Failed", MB_OK);
	}

	//Input
	hr = CInput::GetInstance()->InitInputDevice(g_hInst, g_hWnd);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"DInput Create Failed", L"Fail", MB_OK);
		return E_FAIL;
	}

	CScene* pScene = NULL;

	pScene = CToolStage::Create();

	CSceneMgr::GetInstance()->AddScene(SCENE_TOOLSTAGE, pScene);

	CSceneMgr::GetInstance()->ChangeScene(SCENE_TOOLSTAGE);
		
	return S_OK;
}

int CToolApp::Update(void)
{
	CTimeMgr::GetInstance()->SetTime();
	CInput::GetInstance()->SetInputState();

	CToolCamera::GetInstance()->Update();
	CSceneMgr::GetInstance()->Update();

	return 0;
}

void CToolApp::Render(void)
{
	m_pGrapicDevcie->BeginDevice();
	CSceneMgr::GetInstance()->Render();
	m_pGrapicDevcie->EndDevice();
}

CToolApp * CToolApp::Create(void)
{
	CToolApp*		pToolApp = new CToolApp;
	if (FAILED(pToolApp->Initialize()))
	{
		::Safe_Delete(pToolApp);
	}

	return pToolApp;
}

void CToolApp::Release(void)
{
	CDevice::GetInstance()->DestroyInstance();
	CTimeMgr::GetInstance()->DestroyInstance();
	CToolCamera::GetInstance()->DestroyInstance();
	CShaderMgr::GetInstance()->DestroyInstance();
	CInput::GetInstance()->DestroyInstance();
	CRenderMgr::GetInstance()->DestroyInstance();
	CObjMgr::GetInstance()->DestroyInstance();
	CSceneMgr::GetInstance()->DestroyInstance();
	CResourcesMgr::GetInstance()->DestroyInstance();
}
