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
	vecAni.push_back("Soldier_CrawlingBack");
	vecAni.push_back("Soldier_CrawlingFront");
	vecAni.push_back("Soldier_CrawlingLeft");
	vecAni.push_back("Soldier_CrawlingRight");
	vecAni.push_back("Soldier_Damage");
	vecAni.push_back("Soldier_Death");
	vecAni.push_back("Soldier_Death2");
	vecAni.push_back("Soldier_GetCover");

	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_DYNAMIC,
		L"Mesh_Soldier",
		"../Bin/Resources/Soldier/Animation/",
		NULL, vecAni);
	FAILED_CHECK(hr);

	CAnimationMgr::GetInstance()->AddAnimation(L"Soldier_Animation", &vecAni);

	vecAni.clear();
	

	//Init Static Mesh
	InitStaticMesh();

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

void CLogo::InitStaticMesh(void)
{
	//Static Mesh Loading----------------------------------
	HRESULT hr = NULL;

	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Book1"
		, "../Bin/Resources/StaticMesh/Mesh/Book/"
		, "Book1.FBX");


	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Book2"
		, "../Bin/Resources/StaticMesh/Mesh/Book/"
		, "Book2.FBX");

	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Book3"
		, "../Bin/Resources/StaticMesh/Mesh/Book/"
		, "Book3.FBX");

	//Floor1
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Floor1"
		, "../Bin/Resources/StaticMesh/Mesh/Floor/"
		, "Floor1.FBX");

	//Floor2
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Floor2"
		, "../Bin/Resources/StaticMesh/Mesh/Floor/"
		, "Floor2.FBX");

	//Floor3
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Floor3"
		, "../Bin/Resources/StaticMesh/Mesh/Floor/"
		, "Floor3.FBX");


	//Desk1
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Desk1"
		, "../Bin/Resources/StaticMesh/Mesh/Desk/"
		, "Desk1.FBX");


	//Desk2
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Desk2"
		, "../Bin/Resources/StaticMesh/Mesh/Desk/"
		, "Desk2.FBX");


	//Desk3
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Desk3"
		, "../Bin/Resources/StaticMesh/Mesh/Desk/"
		, "Desk3.FBX");


	//Desk4
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Desk4"
		, "../Bin/Resources/StaticMesh/Mesh/Desk/"
		, "Desk4.FBX");


	//Desk5
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Desk5"
		, "../Bin/Resources/StaticMesh/Mesh/Desk/"
		, "Desk5.FBX");


	//Desk6
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Desk6"
		, "../Bin/Resources/StaticMesh/Mesh/Desk/"
		, "Desk6.FBX");

	//Desk7
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Desk7"
		, "../Bin/Resources/StaticMesh/Mesh/Desk/"
		, "Desk7.FBX");



	//Desk8
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Desk8"
		, "../Bin/Resources/StaticMesh/Mesh/Desk/"
		, "Desk8.FBX");



	//Desk9
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Desk9"
		, "../Bin/Resources/StaticMesh/Mesh/Desk/"
		, "Desk9.FBX");

	//Desk10
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Desk10"
		, "../Bin/Resources/StaticMesh/Mesh/Desk/"
		, "Desk10.FBX");

	//Desk11
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Desk11"
		, "../Bin/Resources/StaticMesh/Mesh/Desk/"
		, "Desk11.FBX");

	//Desk12
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Desk12"
		, "../Bin/Resources/StaticMesh/Mesh/Desk/"
		, "Desk12.FBX");


	//Chair1
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Chair1"
		, "../Bin/Resources/StaticMesh/Mesh/Chair/"
		, "Chair1.FBX");

	//Chair2
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Chair2"
		, "../Bin/Resources/StaticMesh/Mesh/Chair/"
		, "Chair2.FBX");

	//Chair3
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Chair3"
		, "../Bin/Resources/StaticMesh/Mesh/Chair/"
		, "Chair3.FBX");


	//Chair4
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Chair4"
		, "../Bin/Resources/StaticMesh/Mesh/Chair/"
		, "Chair4.FBX");



	//Cabinet1
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Cabinet1"
		, "../Bin/Resources/StaticMesh/Mesh/Cabinet/"
		, "Cabinet1.FBX");

	//Cabinet2
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Cabinet2"
		, "../Bin/Resources/StaticMesh/Mesh/Cabinet/"
		, "Cabinet2.FBX");



	//Shelf1
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Shelf1"
		, "../Bin/Resources/StaticMesh/Mesh/Shelf/"
		, "Shelf1.FBX");

	//Shelf2
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Shelf2"
		, "../Bin/Resources/StaticMesh/Mesh/Shelf/"
		, "Shelf2.FBX");

	//Shelf3
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Shelf3"
		, "../Bin/Resources/StaticMesh/Mesh/Shelf/"
		, "Shelf3.FBX");


	//Chair1
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Photocopier"
		, "../Bin/Resources/StaticMesh/Mesh/"
		, "Photocopier.FBX");


	//Plant1
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Plant1"
		, "../Bin/Resources/StaticMesh/Mesh/Plant/"
		, "Plant1.FBX");

	//Plant2
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Plant2"
		, "../Bin/Resources/StaticMesh/Mesh/Plant/"
		, "Plant2.FBX");


	//Plant3
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Plant3"
		, "../Bin/Resources/StaticMesh/Mesh/Plant/"
		, "Plant3.FBX");


	//Plant4
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Plant4"
		, "../Bin/Resources/StaticMesh/Mesh/Plant/"
		, "Plant4.FBX");


	//Plant5
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Plant5"
		, "../Bin/Resources/StaticMesh/Mesh/Plant/"
		, "Plant5.FBX");


	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_RubbishBin1"
		, "../Bin/Resources/StaticMesh/Mesh/RubbishBin/"
		, "RubbishBin1.FBX");

	//RubbishBin2
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_RubbishBin2"
		, "../Bin/Resources/StaticMesh/Mesh/RubbishBin/"
		, "RubbishBin2.FBX");



	//WaterCooler
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_WaterCooler"
		, "../Bin/Resources/StaticMesh/Mesh/"
		, "WaterCooler.FBX");


	//Couch1
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Couch1"
		, "../Bin/Resources/StaticMesh/Mesh/Couch/"
		, "Couch1.FBX");

	//Couch2
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Couch2"
		, "../Bin/Resources/StaticMesh/Mesh/Couch/"
		, "Couch2.FBX");

	//Couch3
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Couch3"
		, "../Bin/Resources/StaticMesh/Mesh/Couch/"
		, "Couch3.FBX");


	//Couch4
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Couch4"
		, "../Bin/Resources/StaticMesh/Mesh/Couch/"
		, "Couch4.FBX");


	//Coffeetable1
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Coffeetable1"
		, "../Bin/Resources/StaticMesh/Mesh/Coffeetable/"
		, "Coffeetable1.FBX");

	//Coffeetable2
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Coffeetable2"
		, "../Bin/Resources/StaticMesh/Mesh/Coffeetable/"
		, "Coffeetable2.FBX");


	//Bookbox1
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_BookBox1"
		, "../Bin/Resources/StaticMesh/Mesh/BookBox/"
		, "BookBox1.FBX");

	//Bookbox2
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_BookBox2"
		, "../Bin/Resources/StaticMesh/Mesh/BookBox/"
		, "BookBox2.FBX");


	//Wall1
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Wall1"
		, "../Bin/Resources/StaticMesh/Mesh/Wall/"
		, "Wall1.FBX");

	//Wall2
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Wall2"
		, "../Bin/Resources/StaticMesh/Mesh/Wall/"
		, "Wall2.FBX");

	//Wall3
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Wall3"
		, "../Bin/Resources/StaticMesh/Mesh/Wall/"
		, "Wall3.FBX");


	//Wall4
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Wall4"
		, "../Bin/Resources/StaticMesh/Mesh/Wall/"
		, "Wall4.FBX");


	//Wall5
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Wall5"
		, "../Bin/Resources/StaticMesh/Mesh/Wall/"
		, "Wall5.FBX");

	//Wall6
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Wall6"
		, "../Bin/Resources/StaticMesh/Mesh/Wall/"
		, "Wall6.FBX");


	//Wall7
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Wall7"
		, "../Bin/Resources/StaticMesh/Mesh/Wall/"
		, "Wall7.FBX");

	//Wall8
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Wall8"
		, "../Bin/Resources/StaticMesh/Mesh/Wall/"
		, "Wall8.FBX");

	//Wall9
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Wall9"
		, "../Bin/Resources/StaticMesh/Mesh/Wall/"
		, "Wall9.FBX");


	//Wall10
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Wall10"
		, "../Bin/Resources/StaticMesh/Mesh/Wall/"
		, "Wall10.FBX");


	//Painting1
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Painting1"
		, "../Bin/Resources/StaticMesh/Mesh/Painting/"
		, "Painting1.FBX");

	//Painting2
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Painting2"
		, "../Bin/Resources/StaticMesh/Mesh/Painting/"
		, "Painting2.FBX");



	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Clock"
		, "../Bin/Resources/StaticMesh/Mesh/"
		, "Clock.FBX");

	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_HandDryer"
		, "../Bin/Resources/StaticMesh/Mesh/"
		, "HandDryer.FBX");

	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Mirror"
		, "../Bin/Resources/StaticMesh/Mesh/"
		, "Mirror.FBX");

	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_PaperTowelDispenser"
		, "../Bin/Resources/StaticMesh/Mesh/"
		, "PaperTowelDispenser.FBX");


	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Sinks"
		, "../Bin/Resources/StaticMesh/Mesh/"
		, "Sinks.FBX");

	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_SoapDispenser"
		, "../Bin/Resources/StaticMesh/Mesh/"
		, "SoapDispenser.FBX");

	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Urinal"
		, "../Bin/Resources/StaticMesh/Mesh/"
		, "Urinal.FBX");

	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Door"
		, "../Bin/Resources/StaticMesh/Mesh/Door/"
		, "Door1.FBX");

	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Door1"
		, "../Bin/Resources/StaticMesh/Mesh/Door/"
		, "Door2.FBX");


	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_ToiletStall1"
		, "../Bin/Resources/StaticMesh/Mesh/ToiletStall/"
		, "ToiletStall1.FBX");
	FAILED_CHECK_RETURN(hr, );

	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_ToiletStall2"
		, "../Bin/Resources/StaticMesh/Mesh/ToiletStall/"
		, "ToiletStall2.FBX");
	FAILED_CHECK_RETURN(hr, );


	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Pinboard"
		, "../Bin/Resources/StaticMesh/Mesh/"
		, "Pinboard.FBX");


	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_PowerOutlet"
		, "../Bin/Resources/StaticMesh/Mesh/"
		, "PowerOutlet.FBX");
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