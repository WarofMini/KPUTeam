#include "stdafx.h"
#include "Stage.h"
#include "Include.h"
#include "Object.h"
#include "Soldier.h"
#include "ObjMgr.h"
#include "TimeMgr.h"
#include "RenderMgr.h"
#include "Camera.h"
#include "StaticObject.h"
#include "Info.h"

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

	if (FAILED(CreateStaticFloor()))
		return E_FAIL;

	if (FAILED(CreateToiletFloor()))
		return E_FAIL;

	if (FAILED(CreateStaticObject()))
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

HRESULT CStage::CreateStaticFloor(void)
{
	wstring strName = L"Mesh_Floor1";

	float m_iSize = 500.f;
	float m_fSize = 0.4f;

	m_iSize *= m_fSize;

	for (int i = 0; i < 8; ++i)
	{
		for (int j = 0; j < 6; ++j)
		{
			CStaticObject* pObject = NULL;

			pObject = CStaticObject::Create(strName);

			pObject->GetInfo()->m_fAngle[ANGLE_X] += (float)(D3DXToRadian(90.f));
			pObject->GetInfo()->m_vScale = D3DXVECTOR3(m_fSize, m_fSize, m_fSize);

			pObject->GetInfo()->m_vPos = D3DXVECTOR3((j % 6) * m_iSize, 0.f, i * m_iSize);

			CObjMgr::GetInstance()->AddObject(L"StaticObject", pObject);
		}
	}

	return S_OK;
}

HRESULT CStage::CreateToiletFloor(void)
{
	wstring strName = L"Mesh_Floor2";

	float m_iSize = 500.f;
	float m_fSize = 0.4f;

	m_iSize *= m_fSize;

	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			CStaticObject* pObject = NULL;

			pObject = CStaticObject::Create(strName);

			pObject->GetInfo()->m_fAngle[ANGLE_X] += (float)(D3DXToRadian(90.f));
			pObject->GetInfo()->m_vScale = D3DXVECTOR3(m_fSize, m_fSize, m_fSize);

			pObject->GetInfo()->m_vPos = D3DXVECTOR3((j % 4) * m_iSize + 1182.f, 0.1f, i * m_iSize + 400.f);

			CObjMgr::GetInstance()->AddObject(L"StaticObject", pObject);
		}
	}

	return S_OK;
}


//Load Static Object Data
HRESULT CStage::CreateStaticObject(void)
{
	TCHAR*	tPath = L"../Bin/Data/MapObj.dat";

	HANDLE	hFile = CreateFile(tPath, GENERIC_READ, 0, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	DWORD	dwByte = 0;

	while (true)
	{
		OBJ_INFO   tInfo;

		ReadFile(hFile, &tInfo, sizeof(OBJ_INFO), &dwByte, NULL);

		if (dwByte == 0)
		{
			CloseHandle(hFile);
			break;
		}

		CObject* pObject = NULL;

		wstring strName = tInfo.m_szName;

		pObject = CStaticObject::Create(strName);
		((CStaticObject*)(pObject))->SetObjInfo(&tInfo);
		((CStaticObject*)(pObject))->InfoSetting();
		CObjMgr::GetInstance()->AddObject(L"StaticObject", pObject);

	}

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