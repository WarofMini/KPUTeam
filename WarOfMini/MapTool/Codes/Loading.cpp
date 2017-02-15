#include "stdafx.h"
#include "Loading.h"
#include "ResourcesMgr.h"
#include "Scene.h"
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

	////Texture
	//hr = CResourcesMgr::GetInstance()->AddTexture(
	//	RESOURCE_STAGE
	//	, L"Texture_Couch"
	//	, L"../Resource/Office.png");

	//FAILED_CHECK_RETURN(hr, );


	////Buffer
	//hr = CResourcesMgr::GetInstance()->AddMesh(
	//	RESOURCE_STAGE,
	//	MESH_STATIC
	//	, L"Mesh_Couch"
	//	, "../Resource/Mesh/"
	//	, "Couch.FBX");

	//FAILED_CHECK_RETURN(hr, );


	m_bComplete = true;

}

CLoading* CLoading::Create(LOADINGID eLoadID)
{
	CLoading*	pLoading = new CLoading(eLoadID);
	if (FAILED(pLoading->InitLoading()))
		::Safe_Delete(pLoading);

	return pLoading;
}

UINT WINAPI CLoading::LoadingFunction(void* pArg)
{
	CLoading*		pLoading = (CLoading*)pArg;

	switch (pLoading->m_eLoadID)
	{
	case CLoading::LOADING_TOOLSTAGE:
		pLoading->StageLoading();
		break;

	case CLoading::LOADING_STAGEEND:
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