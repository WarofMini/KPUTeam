#include "stdafx.h"
#include "Loading.h"

#include "Include.h"
#include <process.h>
#include "ResourcesMgr.h"
#include "Scene.h"
#include "Stage.h"
#include "SceneMgr.h"
#include "AnimationMgr.h"

CLoading::CLoading(LOADINGID eLoadID)
	: m_eLoadID(eLoadID)
	, m_hThread(NULL)
	, m_bComplete(false)
{
	ZeroMemory(m_szLoadMessage, sizeof(TCHAR) * 128);
	ZeroMemory(&m_CSKey, sizeof(CRITICAL_SECTION));
}

CLoading::~CLoading(void)
{

}

bool CLoading::GetComplete(void)
{
	return m_bComplete;
}

const TCHAR* CLoading::GetLoadMessage(void)
{
	return m_szLoadMessage;
}

HRESULT CLoading::InitLoading(void)
{
	InitializeCriticalSection(&m_CSKey);

	m_hThread = (HANDLE)_beginthreadex(NULL, 0, LoadingFunction, this, 0, NULL);
	if (m_hThread == NULL)
		return E_FAIL;

	return S_OK;
}

void CLoading::StageLoading(void)
{
	HRESULT hr = NULL;

	cout << "TextureLoading" << endl;//Texture

	hr = CResourcesMgr::GetInstance()->AddTexture(
		RESOURCE_STAGE
		, L"Texture_Soldier"
		, L"../Bin/Resources/Soldier/Texture/Soldier_1_albedo.png");

	hr = CResourcesMgr::GetInstance()->AddTexture(
		RESOURCE_STAGE
		, L"Texture_MachineGun"
		, L"../Bin/Resources/Soldier/Texture/MachineGun_diffuse.png");

// 	if(FAILED(Load_Soldier()))
// 		FAILED_CHECK_RETURN(hr, );
		
	//cout << "StaticBufferLoading" << endl;	//Buffer
	
	cout << "Thread Loading End" << endl;
	m_bComplete = true;
}

HRESULT CLoading::Load_Soldier(void)
{
	cout << "DynamicBufferLoading" << endl;
	HRESULT hr = NULL;

	vector<string> vecAni;

	vecAni.push_back("Soldier_Idle");
	vecAni.push_back("Soldier_Jump");

	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_DYNAMIC,
		L"Mesh_Soldier",
		"../Bin/Resources/Soldier/Animation/",
		NULL, vecAni);
	FAILED_CHECK(hr);

	CAnimationMgr::GetInstance()->AddAnimation(L"Soldier_Animation", &vecAni);

	vecAni.clear();

	return S_OK;
}

CLoading* CLoading::Create(LOADINGID eLoadID)
{
	CLoading*	pLoading = new CLoading(eLoadID);
	if (FAILED(pLoading->InitLoading()))
		::Safe_Release(pLoading);

	return pLoading;
}

UINT WINAPI CLoading::LoadingFunction(void* pArg)
{
	CLoading*		pLoading = (CLoading*)pArg;

	switch (pLoading->m_eLoadID)
	{
	case CLoading::LOADING_STAGE:
		pLoading->StageLoading();
		break;
	}
	return 0;
}

void CLoading::Release(void)
{
	DeleteCriticalSection(&m_CSKey);
	WaitForSingleObject(m_hThread, INFINITE);
	CloseHandle(m_hThread);
}