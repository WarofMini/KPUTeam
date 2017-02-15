#include "stdafx.h"
#include "Stage.h"
#include "Include.h"
#include "Object.h"
#include "Soldier.h"
#include "ObjMgr.h"
#include "TimeMgr.h"
#include "RenderMgr.h"
#include "Camera.h"

CStage::CStage()
{
}


CStage::~CStage()
{
}

HRESULT CStage::Initialize(void)
{
#ifdef _DEBUG
	cout << "StageÅ×½ºÆ®" << endl;
#endif

	if (FAILED(CCamera::GetInstance()->Initialize()))
	{
		MessageBox(NULL, L"System Message", L"Camera Initialize Failed", MB_OK);
	}

	if (FAILED(CreateSoldier()))
		return E_FAIL;

	return S_OK;
}

int CStage::Update(void)
{
	CCamera::GetInstance()->Update();
	CObjMgr::GetInstance()->Update();

	return 0;
}

void CStage::Render(void)
{
	float fTime = CTimeMgr::GetInstance()->GetTime();
	CRenderMgr::GetInstance()->Render(fTime);
}

HRESULT CStage::CreateSoldier(void)
{
	CObject* pObject = NULL;
	pObject = CSoldier::Create();
	if (pObject == NULL)
		return E_FAIL;

	CObjMgr::GetInstance()->AddObject(L"Soldier", pObject);

	return S_OK;
}

void CStage::Release(void)
{
	CCamera::GetInstance()->DestroyInstance();
}

CStage * CStage::Create(void)
{
	CStage* pStage = new CStage;
	if (FAILED(pStage->Initialize()))
	{
		::Safe_Delete(pStage);

	}
	return pStage;
}