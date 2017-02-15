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

CToolStage::CToolStage()
{
}


CToolStage::~CToolStage()
{

}

HRESULT CToolStage::Initialize(void)
{
	//Mesh Loading
	InitMesh();

	FAILED_CHECK_RETURN_MSG(Add_Environment_Layer(), E_FAIL, L"Environment_Layer Add false");
	FAILED_CHECK_RETURN_MSG(Add_GameLogic_Layer(), E_FAIL, L"GameLogic_Layer Add false");
	//FAILED_CHECK_RETURN_MSG(Add_Light(), E_FAIL, L"Light Add false");
	
	return S_OK;
}

int CToolStage::Update(void)
{
	CObjMgr::GetInstance()->Update();

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

	//Buffer

	//BookMesh Loading
	InitBookMesh();
}

void CToolStage::InitBookMesh(void)
{
	HRESULT hr = NULL;

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
		, L"Mesh_Couch"
		, "../Resource/Mesh/"
		, "Couch.FBX");

	FAILED_CHECK_RETURN(hr, );


	//hr = CResourcesMgr::GetInstance()->AddMesh(
	//	RESOURCE_STAGE,
	//	MESH_STATIC
	//	, L"Mesh_Book2"
	//	, "../Resource/Mesh/"
	//	, "Book2.FBX");

	//FAILED_CHECK_RETURN(hr, );

	//hr = CResourcesMgr::GetInstance()->AddMesh(
	//	RESOURCE_STAGE,
	//	MESH_STATIC
	//	, L"Mesh_Book3"
	//	, "../Resource/Mesh/"
	//	, "Book3.FBX");

	//FAILED_CHECK_RETURN(hr, );
}
