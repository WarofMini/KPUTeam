#include "stdafx.h"
#include "ToolStage.h"
#include "Texture.h"
#include "Shader.h"
#include "ShaderMgr.h"
#include "SceneMgr.h"
#include "Device.h"
#include "Loading.h"
#include "ResourcesMgr.h"
#include "TimeMgr.h"
#include "RenderMgr.h"
#include "Obj.h"
#include "ObjMgr.h"
#include "AnimationMgr.h"
#include "StaticObject.h"
#include "Info.h"
#include "ObjCol.h"

CToolStage::CToolStage()
{
}


CToolStage::~CToolStage()
{
	Release();
}

HRESULT CToolStage::Initialize(void)
{
	//Mesh Loading
	InitMesh();

	FAILED_CHECK_RETURN_MSG(Add_Environment_Layer(), E_FAIL, L"Environment_Layer Add false");
	FAILED_CHECK_RETURN_MSG(Add_GameLogic_Layer(), E_FAIL, L"GameLogic_Layer Add false");
	//FAILED_CHECK_RETURN_MSG(Add_Light(), E_FAIL, L"Light Add false");

	CObjCol::GetInstance()->Initialize();
	
	return S_OK;
}

int CToolStage::Update(void)
{
	CObjMgr::GetInstance()->Update();

	CObjCol::GetInstance()->Update();

	return 0;
}

void CToolStage::Render(void)
{
	float fTime = CTimeMgr::GetInstance()->GetTime();
	CRenderMgr::GetInstance()->Render(fTime);
}

void CToolStage::Release(void)
{
}

CToolStage * CToolStage::Create(void)
{
	CToolStage* pToolStage = new CToolStage;

	if (FAILED(pToolStage->Initialize()))
	{
		::Safe_Delete(pToolStage);
	}

	return pToolStage;
}

HRESULT CToolStage::Add_Environment_Layer(void)
{
	return S_OK;
}

HRESULT CToolStage::Add_GameLogic_Layer(void)
{
	InitFloor();
	InitToiletFloor();

	return S_OK;
}

HRESULT CToolStage::Add_Dynamic_Buffer(void)
{
	return S_OK;
}

void CToolStage::InitMesh(void)
{
	HRESULT hr = NULL;

	//Texture
	hr = CResourcesMgr::GetInstance()->AddTexture(
		RESOURCE_STAGE
		, L"Texture_Couch"
		, L"../Resource/Office.png");

	FAILED_CHECK_RETURN(hr, );


	//BookMesh Loading
	InitBookMesh();
	//FloorMesh Loading
	InitFloorMesh();
	//DeskMesh Loading
	InitDeskMesh();
	//Chair Loading
	InitChairMesh();
	//Cabinet Loading
	InitCabinetMesh();
	//Shelf Loading
	InitShelfMesh();

	InitPhotoCopier();
	//Plant Loading
	InitPlant();
	//RubbishBin Loading
	InitRubbishBin();

	InitWaterCooler();
	//Couch Loading
	InitCouch();
	//Coffeetable Loading
	InitCoffeetable();
	//BookBox Loading
	InitBookBox();
	//Wall Loading
	InitWall();
	//Painting Loading
	InitPainting();
	//Door Loading
	InitDoor();

	InitToiletStall();
	//Etc Loading
	InitEtc();

}


void CToolStage::InitBookMesh(void)
{
	HRESULT hr = NULL;

	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Book1"
		, "../Resource/Mesh/Book/"
		, "Book1.FBX");

	FAILED_CHECK_RETURN(hr, );

	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Book2"
		, "../Resource/Mesh/Book/"
		, "Book2.FBX");

	FAILED_CHECK_RETURN(hr, );

	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Book3"
		, "../Resource/Mesh/Book/"
		, "Book3.FBX");

	FAILED_CHECK_RETURN(hr, );

}


void CToolStage::InitFloorMesh(void)
{
	HRESULT hr = NULL;
	//Floor1
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Floor1"
		, "../Resource/Mesh/Floor/"
		, "Floor1.FBX");

	FAILED_CHECK_RETURN(hr, );

	//Floor2
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Floor2"
		, "../Resource/Mesh/Floor/"
		, "Floor2.FBX");

	//Floor3
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Floor3"
		, "../Resource/Mesh/Floor/"
		, "Floor3.FBX");

	FAILED_CHECK_RETURN(hr, );
}


void CToolStage::InitDeskMesh(void)
{
	HRESULT hr = NULL;
	//Desk1
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Desk1"
		, "../Resource/Mesh/Desk/"
		, "Desk1.FBX");
	FAILED_CHECK_RETURN(hr, );

	//Desk2
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Desk2"
		, "../Resource/Mesh/Desk/"
		, "Desk2.FBX");
	FAILED_CHECK_RETURN(hr, );

	//Desk3
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Desk3"
		, "../Resource/Mesh/Desk/"
		, "Desk3.FBX");
	FAILED_CHECK_RETURN(hr, );

	//Desk4
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Desk4"
		, "../Resource/Mesh/Desk/"
		, "Desk4.FBX");
	FAILED_CHECK_RETURN(hr, );

	//Desk5
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Desk5"
		, "../Resource/Mesh/Desk/"
		, "Desk5.FBX");
	FAILED_CHECK_RETURN(hr, );

	//Desk6
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Desk6"
		, "../Resource/Mesh/Desk/"
		, "Desk6.FBX");
	FAILED_CHECK_RETURN(hr, );

	//Desk7
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Desk7"
		, "../Resource/Mesh/Desk/"
		, "Desk7.FBX");
	FAILED_CHECK_RETURN(hr, );


	//Desk8
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Desk8"
		, "../Resource/Mesh/Desk/"
		, "Desk8.FBX");
	FAILED_CHECK_RETURN(hr, );


	//Desk9
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Desk9"
		, "../Resource/Mesh/Desk/"
		, "Desk9.FBX");
	FAILED_CHECK_RETURN(hr, );

	//Desk10
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Desk10"
		, "../Resource/Mesh/Desk/"
		, "Desk10.FBX");
	FAILED_CHECK_RETURN(hr, );

	//Desk11
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Desk11"
		, "../Resource/Mesh/Desk/"
		, "Desk11.FBX");
	FAILED_CHECK_RETURN(hr, );

	//Desk12
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Desk12"
		, "../Resource/Mesh/Desk/"
		, "Desk12.FBX");
	FAILED_CHECK_RETURN(hr, );
}


void CToolStage::InitChairMesh(void)
{
	HRESULT hr = NULL;

	//Chair1
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Chair1"
		, "../Resource/Mesh/Chair/"
		, "Chair1.FBX");
	FAILED_CHECK_RETURN(hr, );

	//Chair2
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Chair2"
		, "../Resource/Mesh/Chair/"
		, "Chair2.FBX");
	FAILED_CHECK_RETURN(hr, );

	//Chair3
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Chair3"
		, "../Resource/Mesh/Chair/"
		, "Chair3.FBX");
	FAILED_CHECK_RETURN(hr, );

	//Chair4
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Chair4"
		, "../Resource/Mesh/Chair/"
		, "Chair4.FBX");
	FAILED_CHECK_RETURN(hr, );
}


void CToolStage::InitCabinetMesh(void)
{
	HRESULT hr = NULL;
	//Cabinet1
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Cabinet1"
		, "../Resource/Mesh/Cabinet/"
		, "Cabinet1.FBX");
	FAILED_CHECK_RETURN(hr, );

	//Cabinet2
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Cabinet2"
		, "../Resource/Mesh/Cabinet/"
		, "Cabinet2.FBX");
	FAILED_CHECK_RETURN(hr, );
}


void CToolStage::InitShelfMesh(void)
{
	HRESULT hr = NULL;

	//Shelf1
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Shelf1"
		, "../Resource/Mesh/Shelf/"
		, "Shelf1.FBX");
	FAILED_CHECK_RETURN(hr, );

	//Shelf2
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Shelf2"
		, "../Resource/Mesh/Shelf/"
		, "Shelf2.FBX");
	FAILED_CHECK_RETURN(hr, );

	//Shelf3
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Shelf3"
		, "../Resource/Mesh/Shelf/"
		, "Shelf3.FBX");
	FAILED_CHECK_RETURN(hr, );


}

//¹Ù´Ú »ý¼º
void CToolStage::InitFloor(void)
{
	wstring strName = L"Mesh_Floor1";

	float m_iSize = 195.f;
	float m_fSize = 1.01f;

	m_iSize *= m_fSize;

	for (int i = 0; i < 8; ++i)
	{
		for (int j = 0; j < 6; ++j)
		{
			CObj* pObject = NULL;

			pObject = CToolStaticObject::Create(strName);

			pObject->GetInfo()->m_fAngle[ANGLE_X] += (float)(D3DXToRadian(90.f));
			pObject->GetInfo()->m_vScale = D3DXVECTOR3(m_fSize, m_fSize, m_fSize);
			((CToolStaticObject*)pObject)->SetMode(MODE_FIX);

			pObject->GetInfo()->m_vPos = D3DXVECTOR3((j % 6) * m_iSize + 100.f, 0.f, i * m_iSize - 100.f);

			CObjMgr::GetInstance()->AddObject(L"StaticObject", pObject);
		}
	}


	strName = L"Mesh_Ceiling";


	m_iSize = 195.f;
	m_fSize = 1.01f;

	m_iSize *= m_fSize;


	for (int i = 0; i < 8; ++i)
	{
		for (int j = 0; j < 6; ++j)
		{
			CObj* pObject = NULL;

			pObject = CToolStaticObject::Create(strName);

			pObject->GetInfo()->m_fAngle[ANGLE_X] += (float)(D3DXToRadian(90.f));
			pObject->GetInfo()->m_vScale = D3DXVECTOR3(m_fSize, m_fSize, m_fSize);
			((CToolStaticObject*)pObject)->SetMode(MODE_FIX);

			pObject->GetInfo()->m_vPos = D3DXVECTOR3((j % 6) * m_iSize + 100.f, 366.f, i * m_iSize - 100.f);

			CObjMgr::GetInstance()->AddObject(L"StaticObject", pObject);
		}
	}


}

void CToolStage::InitToiletFloor(void)
{
	wstring strName = L"Mesh_Floor2";

	float m_iSize = 195.f;
	float m_fSize = 1.01f;

	m_iSize *= m_fSize;

	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			CObj* pObject = NULL;

			pObject = CToolStaticObject::Create(strName);

			pObject->GetInfo()->m_fAngle[ANGLE_X] += (float)(D3DXToRadian(90.f));
			pObject->GetInfo()->m_vScale = D3DXVECTOR3(m_fSize, m_fSize, m_fSize);
			((CToolStaticObject*)pObject)->SetMode(MODE_FIX);

			pObject->GetInfo()->m_vPos = D3DXVECTOR3((j % 4) * m_iSize + 1282.f, 0.1f, i * m_iSize + 300.f);

			CObjMgr::GetInstance()->AddObject(L"StaticObject", pObject);
		}
	}





	strName = L"Mesh_Ceiling";

	m_iSize = 195.f;
	m_fSize = 1.01f;

	m_iSize *= m_fSize;

	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			CObj* pObject = NULL;

			pObject = CToolStaticObject::Create(strName);

			pObject->GetInfo()->m_fAngle[ANGLE_X] += (float)(D3DXToRadian(90.f));
			pObject->GetInfo()->m_vScale = D3DXVECTOR3(m_fSize, m_fSize, m_fSize);
			((CToolStaticObject*)pObject)->SetMode(MODE_FIX);

			pObject->GetInfo()->m_vPos = D3DXVECTOR3((j % 4) * m_iSize + 1282.f, 366.f, i * m_iSize + 300.f);

			CObjMgr::GetInstance()->AddObject(L"StaticObject", pObject);
		}
	}

}

void CToolStage::InitPhotoCopier(void)
{
	HRESULT hr = NULL;
	//Chair1
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Photocopier"
		, "../Resource/Mesh/"
		, "Photocopier.FBX");
	FAILED_CHECK_RETURN(hr, );
}

void CToolStage::InitPlant(void)
{
	HRESULT hr = NULL;

	//Plant1
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Plant1"
		, "../Resource/Mesh/Plant/"
		, "Plant1.FBX");
	FAILED_CHECK_RETURN(hr, );

	//Plant2
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Plant2"
		, "../Resource/Mesh/Plant/"
		, "Plant2.FBX");
	FAILED_CHECK_RETURN(hr, );


	//Plant3
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Plant3"
		, "../Resource/Mesh/Plant/"
		, "Plant3.FBX");
	FAILED_CHECK_RETURN(hr, );


	//Plant4
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Plant4"
		, "../Resource/Mesh/Plant/"
		, "Plant4.FBX");
	FAILED_CHECK_RETURN(hr, );


	//Plant5
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Plant5"
		, "../Resource/Mesh/Plant/"
		, "Plant5.FBX");
	FAILED_CHECK_RETURN(hr, );
}

void CToolStage::InitRubbishBin(void)
{

	HRESULT hr = NULL;

	//RubbishBin1
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_RubbishBin1"
		, "../Resource/Mesh/RubbishBin/"
		, "RubbishBin1.FBX");
	FAILED_CHECK_RETURN(hr, );

	//RubbishBin2
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_RubbishBin2"
		, "../Resource/Mesh/RubbishBin/"
		, "RubbishBin2.FBX");
	FAILED_CHECK_RETURN(hr, );
}

void CToolStage::InitWaterCooler(void)
{
	HRESULT hr = NULL;

	//WaterCooler
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_WaterCooler"
		, "../Resource/Mesh/"
		, "WaterCooler.FBX");
	FAILED_CHECK_RETURN(hr, );
}

void CToolStage::InitCouch(void)
{
	HRESULT hr = NULL;

	//Couch1
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Couch1"
		, "../Resource/Mesh/Couch/"
		, "Couch1.FBX");
	FAILED_CHECK_RETURN(hr, );

	//Couch2
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Couch2"
		, "../Resource/Mesh/Couch/"
		, "Couch2.FBX");
	FAILED_CHECK_RETURN(hr, );

	//Couch3
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Couch3"
		, "../Resource/Mesh/Couch/"
		, "Couch3.FBX");
	FAILED_CHECK_RETURN(hr, );


	//Couch4
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Couch4"
		, "../Resource/Mesh/Couch/"
		, "Couch4.FBX");
	FAILED_CHECK_RETURN(hr, );
}

void CToolStage::InitCoffeetable(void)
{

	HRESULT hr = NULL;

	//Coffeetable1
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Coffeetable1"
		, "../Resource/Mesh/Coffeetable/"
		, "Coffeetable1.FBX");
	FAILED_CHECK_RETURN(hr, );

	//Coffeetable2
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Coffeetable2"
		, "../Resource/Mesh/Coffeetable/"
		, "Coffeetable2.FBX");
	FAILED_CHECK_RETURN(hr, );
}

void CToolStage::InitBookBox(void)
{
	HRESULT hr = NULL;

	//Bookbox1
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_BookBox1"
		, "../Resource/Mesh/BookBox/"
		, "BookBox1.FBX");
	FAILED_CHECK_RETURN(hr, );

	//Bookbox2
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_BookBox2"
		, "../Resource/Mesh/BookBox/"
		, "BookBox2.FBX");
	FAILED_CHECK_RETURN(hr, );
}

void CToolStage::InitWall(void)
{
	HRESULT hr = NULL;

	//Wall1
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Wall1"
		, "../Resource/Mesh/Wall/"
		, "Wall1.FBX");
	FAILED_CHECK_RETURN(hr, );

	//Wall2
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Wall2"
		, "../Resource/Mesh/Wall/"
		, "Wall2.FBX");
	FAILED_CHECK_RETURN(hr, );

	//Wall3
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Wall3"
		, "../Resource/Mesh/Wall/"
		, "Wall3.FBX");
	FAILED_CHECK_RETURN(hr, );


	//Wall4
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Wall4"
		, "../Resource/Mesh/Wall/"
		, "Wall4.FBX");
	FAILED_CHECK_RETURN(hr, );


	//Wall5
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Wall5"
		, "../Resource/Mesh/Wall/"
		, "Wall5.FBX");
	FAILED_CHECK_RETURN(hr, );

	//Wall6
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Wall6"
		, "../Resource/Mesh/Wall/"
		, "Wall6.FBX");
	FAILED_CHECK_RETURN(hr, );


	//Wall7
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Wall7"
		, "../Resource/Mesh/Wall/"
		, "Wall7.FBX");
	FAILED_CHECK_RETURN(hr, );

	//Wall8
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Wall8"
		, "../Resource/Mesh/Wall/"
		, "Wall8.FBX");
	FAILED_CHECK_RETURN(hr, );

	//Wall9
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Wall9"
		, "../Resource/Mesh/Wall/"
		, "Wall9.FBX");
	FAILED_CHECK_RETURN(hr, );


	//Wall10
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Wall10"
		, "../Resource/Mesh/Wall/"
		, "Wall10.FBX");
	FAILED_CHECK_RETURN(hr, );
}

void CToolStage::InitPainting(void)
{
	HRESULT hr = NULL;

	//Painting1
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Painting1"
		, "../Resource/Mesh/Painting/"
		, "Painting1.FBX");
	FAILED_CHECK_RETURN(hr, );

	//Painting2
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Painting2"
		, "../Resource/Mesh/Painting/"
		, "Painting2.FBX");
	FAILED_CHECK_RETURN(hr, );
}

void CToolStage::InitDoor(void)
{
	HRESULT hr = NULL;

	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Door1"
		, "../Resource/Mesh/Door/"
		, "Door1.FBX");
	FAILED_CHECK_RETURN(hr, );

	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Door2"
		, "../Resource/Mesh/Door/"
		, "Door2.FBX");
	FAILED_CHECK_RETURN(hr, );
}

void CToolStage::InitToiletStall(void)
{
	HRESULT hr = NULL;

	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_ToiletStall1"
		, "../Resource/Mesh/ToiletStall/"
		, "ToiletStall1.FBX");
	FAILED_CHECK_RETURN(hr, );

	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_ToiletStall2"
		, "../Resource/Mesh/ToiletStall/"
		, "ToiletStall2.FBX");
	FAILED_CHECK_RETURN(hr, );
}

void CToolStage::InitEtc(void)
{
	HRESULT hr = NULL;

	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Clock"
		, "../Resource/Mesh/"
		, "Clock.FBX");
	FAILED_CHECK_RETURN(hr, );


	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Pinboard"
		, "../Resource/Mesh/"
		, "Pinboard.FBX");
	FAILED_CHECK_RETURN(hr, );

	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_PowerOutlet"
		, "../Resource/Mesh/"
		, "PowerOutlet.FBX");
	FAILED_CHECK_RETURN(hr, );

	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_SoapDispenser"
		, "../Resource/Mesh/"
		, "SoapDispenser.FBX");
	FAILED_CHECK_RETURN(hr, );

	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Urinal"
		, "../Resource/Mesh/"
		, "Urinal.FBX");
	FAILED_CHECK_RETURN(hr, );

	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_HandDryer"
		, "../Resource/Mesh/"
		, "HandDryer.FBX");
	FAILED_CHECK_RETURN(hr, );

	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Mirror"
		, "../Resource/Mesh/"
		, "Mirror.FBX");
	FAILED_CHECK_RETURN(hr, );

	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_PaperTowelDispenser"
		, "../Resource/Mesh/"
		, "PaperTowelDispenser.FBX");
	FAILED_CHECK_RETURN(hr, );

	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Sinks"
		, "../Resource/Mesh/"
		, "Sinks.FBX");
	FAILED_CHECK_RETURN(hr, );


	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Ceiling"
		, "../Resource/Mesh/"
		, "Ceiling.FBX");
	FAILED_CHECK_RETURN(hr, );
}

