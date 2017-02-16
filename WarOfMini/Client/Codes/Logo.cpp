#include "stdafx.h"
#include "Logo.h"
#include "Include.h"
#include "SceneMgr.h"
#include "Stage.h"
#include "ResourcesMgr.h"
#include "TimeMgr.h"
#include "RenderMgr.h"
#include "ObjMgr.h"
#include "LogoBack.h"
#include "Loading.h"
#include "AnimationMgr.h"

CLogo::CLogo()
	: m_pLoading(NULL)
	, m_bDynamicLoading(false)
{
}


CLogo::~CLogo()
{
}

HRESULT CLogo::Initialize(void)
{
#ifdef _DEBUG
	cout << "Logo테스트" << endl;
#endif

	HRESULT hr = NULL;

	hr = CResourcesMgr::GetInstance()->AddTexture(RESOURCE_LOGO,
		L"Texture_Logo",
		L"../Bin/Resources/Logo.jpg");
	if (FAILED(hr))		return E_FAIL;

	FAILED_CHECK_RETURN_MSG(Add_GameLogic_Layer(), E_FAIL, L"GameLogic_Layer Add false");

	return S_OK;
}

int CLogo::Update(void)
{
	if (m_pLoading == NULL)
	{
		m_pLoading = CLoading::Create(CLoading::LOADING_STAGE);
	}
	
	if (m_bDynamicLoading == false)
		Add_Dynamic_Buffer();

	if (m_bDynamicLoading && m_pLoading->GetComplete() == true && GetAsyncKeyState(VK_RETURN))
	{
		CScene* pScene = NULL;
		pScene = CStage::Create();
		CSceneMgr::GetInstance()->AddScene(SCENE_STAGE, pScene);
		CSceneMgr::GetInstance()->ChangeScene(SCENE_STAGE);
	}

	return 0;
}

void CLogo::Render(void)
{
	float fTime = CTimeMgr::GetInstance()->GetTime();
	CRenderMgr::GetInstance()->Render(fTime);
}

HRESULT CLogo::Add_GameLogic_Layer(void)
{
	CObject*	pInstance = NULL;

	pInstance = CLogoBack::Create();
	NULL_CHECK_RETURN_MSG(pInstance, E_FAIL, L"CLogoBack 생성 실패");
	CObjMgr::GetInstance()->AddObject(L"LogoBack", pInstance);

	return S_OK;
}

HRESULT CLogo::Add_Dynamic_Buffer(void)
{
	if (m_bDynamicLoading)
		return S_OK;
	HRESULT hr = NULL;

	vector<string> vecAni;

	/////
	vecAni.push_back("Soldier_Idle_NG");
	vecAni.push_back("Soldier_Jump_NG");

	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_DYNAMIC,
		L"Mesh_Soldier",
		"../Bin/Resources/Soldier/Animation/",
		NULL, vecAni);
	FAILED_CHECK(hr);

	CAnimationMgr::GetInstance()->AddAnimation(L"Soldier_Animation", &vecAni);

	vecAni.clear();
	
	m_bDynamicLoading = true;

	return S_OK;
}

void CLogo::Release(void)
{
	::Safe_Release(m_pLoading);

	list<CObject*>::iterator iter = CObjMgr::GetInstance()->Get_ObjList(L"LogoBack")->begin();
	list<CObject*>::iterator iter_end = CObjMgr::GetInstance()->Get_ObjList(L"LogoBack")->end();

	for (iter; iter != iter_end;)
	{
		CRenderMgr::GetInstance()->DelRenderGroup(TYPE_NONEALPHA, *iter);
		::Safe_Release(*iter);
		CObjMgr::GetInstance()->Get_ObjList(L"LogoBack")->erase(iter++);
	}
}

CLogo * CLogo::Create(void)
{
	CLogo* pLogo = new CLogo;
	if (FAILED(pLogo->Initialize()))
	{
		::Safe_Delete(pLogo);

	}
	return pLogo;
}