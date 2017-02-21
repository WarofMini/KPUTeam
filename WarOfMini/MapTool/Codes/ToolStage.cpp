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
}

void CToolStage::InitBookMesh(void)
{
	HRESULT hr = NULL;

	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Couch"
		, "../Resource/Mesh/"
		, "Couch.FBX");

	FAILED_CHECK_RETURN(hr, );


	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Book1"
		, "../Resource/Mesh/"
		, "Book1.FBX");

	FAILED_CHECK_RETURN(hr, );

	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Book2"
		, "../Resource/Mesh/"
		, "Book2.FBX");

	FAILED_CHECK_RETURN(hr, );

	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Book3"
		, "../Resource/Mesh/"
		, "Book3.FBX");

	FAILED_CHECK_RETURN(hr, );


	//Floor1
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Floor1"
		, "../Resource/Mesh/"
		, "Floor1.FBX");

	FAILED_CHECK_RETURN(hr, );

	//Floor2
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Floor2"
		, "../Resource/Mesh/"
		, "Floor2.FBX");

	FAILED_CHECK_RETURN(hr, );
}

void CToolStage::InitFloor(void)
{
	wstring strName = L"Mesh_Floor1";

	float m_iSize = 500.f;
	float m_fSize = 0.4f;

	m_iSize *= m_fSize;

	for (int i = 0; i < 5; ++i)
	{
		for (int j = 0; j < 5; ++j)
		{
			CObj* pObject = NULL;

			pObject = CToolStaticObject::Create(strName);

			pObject->GetInfo()->m_fAngle[ANGLE_X] += (float)(D3DXToRadian(90.f));
			pObject->GetInfo()->m_vScale = D3DXVECTOR3(m_fSize, m_fSize, m_fSize);
			((CToolStaticObject*)pObject)->SetMode(MODE_FIX);

			pObject->GetInfo()->m_vPos = D3DXVECTOR3((j % 5) * m_iSize, 0.f, i * m_iSize);

			CObjMgr::GetInstance()->AddObject(L"StaticObject", pObject);
		}
	}
}
