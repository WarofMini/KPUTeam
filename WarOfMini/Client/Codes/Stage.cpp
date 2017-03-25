#include "stdafx.h"
#include "Stage.h"
#include "RenderTargetMgr.h"
#include "CameraMgr.h"
#include "FontMgr.h"
#include "LogoBack.h"
#include "DefaultObject.h"
#include "Player.h"
#include "Transform.h"


CStage::CStage(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext)
	: CScene(pGraphicDev, pContext)
{
}

CStage::~CStage(void)
{
}

CStage* CStage::Create(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext)
{
	//SetCursor(NULL);
	//ShowCursor(FALSE);

	CStage* pScene = new CStage(pGraphicDev, pContext);

	if (FAILED(pScene->Ready_Scene()))
	{
		MSG_BOX(L"StageScene Create Failed");
		Safe_Release(pScene);
	}

	return pScene;
}

HRESULT CStage::Ready_Scene(void)
{
	if (FAILED(Ready_GameLogic()))		return E_FAIL;
	if (FAILED(Ready_Environment()))	return E_FAIL;

	InitFloor();
	InitToiletFloor();
	LoadStageMap();

	return S_OK;
}

_int CStage::Update(const _float& fTimeDelta)
{
	CScene::Update(fTimeDelta);
	CCameraMgr::GetInstance()->Update_CurCamera(fTimeDelta);

	return 0;
}

HRESULT CStage::Ready_GameLogic(void)
{
	CLayer* pLayer = CLayer::Create();
	CGameObject* pGameObject = NULL;


	pGameObject = CPlayer::Create(m_pGraphicDev, m_pContext);
	if (NULL == pGameObject) return E_FAIL;
	pLayer->Ready_Object(L"Player", pGameObject);

	m_mapLayer.insert(MAPLAYER::value_type(L"Layer_GameLogic", pLayer));

	return S_OK;
}

HRESULT CStage::Ready_Environment(void)
{
	CLayer* pLayer = CLayer::Create();
	CGameObject* pGameObject = NULL;

	//다이나믹 카메라 적용
	CCameraMgr::GetInstance()->Ready_DynamicCamera(m_pContext, CCameraMgr::CAMERA_DYNAMIC, 0.1f, 10000.f, XMFLOAT3(0.f, 5.f, -10.f), XMFLOAT3(0.f, 0.f, 0.f));

	CComponent* pTransform = (CComponent*)FindLayer(L"Layer_GameLogic")->Get_Component(L"Player", L"Com_Transform");

	if (pTransform != NULL)
	{
		CCameraMgr::GetInstance()->Ready_StaticCamera(m_pContext, CCameraMgr::CAMERA_STATIC, (CTransform*)pTransform, 10.f, 0.1f, 10000.f, XMFLOAT3(0.f, 5.f, -10.f), XMFLOAT3(0.f, 0.f, 0.f));
	}

	m_mapLayer.insert(MAPLAYER::value_type(L"Layer_Environment", pLayer));

	return S_OK;
}

void CStage::Release(void)
{
	CScene::Release();

	delete this;
}

HRESULT CStage::InitFloor(void)
{
	CLayer* pLayer = FindLayer(L"Layer_GameLogic");


	CGameObject* pGameObject = NULL;
	//InitFloor
	float m_iSize = 500.f;
	float m_fSize = 0.4f;

	m_iSize *= m_fSize;

	for (int i = 0; i < 8; ++i)
	{
		for (int j = 0; j < 6; ++j)
		{
			pGameObject = CDefaultObj::Create(m_pContext);
			if (NULL == pGameObject)
				return E_FAIL;

			((CDefaultObj*)pGameObject)->SetObjNum(MESHNUM_FLOOR1);


			((CTransform*)pGameObject->Get_Component(L"Com_Transform"))->m_vAngle = XMFLOAT3(0.f, 90.f, 0.f);
			((CTransform*)pGameObject->Get_Component(L"Com_Transform"))->m_vScale = XMFLOAT3(BITSCALE, BITSCALE, BITSCALE);
			((CTransform*)pGameObject->Get_Component(L"Com_Transform"))->m_vPos = XMFLOAT3((j % 6) * m_iSize, 0.f, i * m_iSize);
			pLayer->Ready_Object(L"StaticObject", pGameObject);

		}
	}

	return S_OK;
}

HRESULT CStage::InitToiletFloor(void)
{
	CLayer* pLayer = FindLayer(L"Layer_GameLogic");


	CGameObject* pGameObject = NULL;

	//InitFloor
	float m_iSize = 500.f;
	float m_fSize = 0.4f;

	m_iSize *= m_fSize;


	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			pGameObject = CDefaultObj::Create(m_pContext);

			if (NULL == pGameObject)
				return E_FAIL;

			((CDefaultObj*)pGameObject)->SetObjNum(MESHNUM_FLOOR2);

			((CTransform*)pGameObject->Get_Component(L"Com_Transform"))->m_vAngle = XMFLOAT3(0.f, 90.f, 0.f);
			((CTransform*)pGameObject->Get_Component(L"Com_Transform"))->m_vScale = XMFLOAT3(BITSCALE, BITSCALE, BITSCALE);
			((CTransform*)pGameObject->Get_Component(L"Com_Transform"))->m_vPos = XMFLOAT3((j % 4) * m_iSize + 1182.f, 0.1f, i * m_iSize + 400.f);

			pLayer->Ready_Object(L"StaticObject", pGameObject);

		}
	}


	return S_OK;
}

HRESULT CStage::LoadStageMap(void)
{
	CLayer* pLayer = FindLayer(L"Layer_GameLogic");


	CGameObject* pGameObject = NULL;


	wstring tPath = L"../Bin/MapData/MapObj.dat";

	_ulong	dwByte = 0;
	HANDLE hFile = CreateFile(tPath.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	while (true)
	{
		OBJ_INFO m_tInfo;

		ReadFile(hFile, &m_tInfo, sizeof(OBJ_INFO), &dwByte, NULL);


		if (dwByte == 0)
		{
			CloseHandle(hFile);
			return E_FAIL;
		}

		MESHNUM eMeshNum = MESHNUM_END;

		wstring strName = m_tInfo.m_szName;



		eMeshNum = MatchingObject(strName);


		if (eMeshNum != MESHNUM_END)
		{
			pGameObject = CDefaultObj::Create(m_pContext);

			if (NULL == pGameObject)
				return E_FAIL;


			XMVECTOR vPos, vAngle, vScale;

			vPos = XMVectorSet(m_tInfo.m_vPos.x, m_tInfo.m_vPos.y, m_tInfo.m_vPos.z, 0.0f);
			vAngle = XMVectorSet(m_tInfo.m_vAngle.x + 90.f, m_tInfo.m_vAngle.y, m_tInfo.m_vAngle.z, 0.0f);

			vScale = XMVectorSet(m_tInfo.m_vScale.x + BITSCALE, m_tInfo.m_vScale.y + BITSCALE, m_tInfo.m_vScale.z + BITSCALE, 0.0f);

			((CDefaultObj*)pGameObject)->SetObjNum(eMeshNum);

			XMStoreFloat3(&((CTransform*)pGameObject->Get_Component(L"Com_Transform"))->m_vPos, vPos);
			XMStoreFloat3(&((CTransform*)pGameObject->Get_Component(L"Com_Transform"))->m_vAngle, vAngle);
			XMStoreFloat3(&((CTransform*)pGameObject->Get_Component(L"Com_Transform"))->m_vScale, vScale);

			pLayer->Ready_Object(L"StaticObject", pGameObject);
		}

	}




	return S_OK;
}

MESHNUM CStage::MatchingObject(wstring strMeshName)
{
	//Floor라는 글자가 있는 경우
	if (strMeshName.find(L"Floor", 0) != -1)
	{
		if (strMeshName == L"Mesh_Floor1")
			return MESHNUM_FLOOR1;
		if (strMeshName == L"Mesh_Floor2")
			return  MESHNUM_FLOOR2;
		if (strMeshName == L"Mesh_Floor3")
			return MESHNUM_FLOOR3;
	}

	//Book라는 글자가 있는 경우
	if (strMeshName.find(L"Book", 0) != -1)
	{
		if (strMeshName == L"Mesh_Book1")
			return MESHNUM_BOOK1;
		if (strMeshName == L"Mesh_Book2")
			return MESHNUM_BOOK2;
		if (strMeshName == L"Mesh_Book3")
			return MESHNUM_BOOK3;
		if (strMeshName == L"Mesh_BookBox1")
			return MESHNUM_BOOKBOX1;
		if (strMeshName == L"Mesh_BookBox2")
			return MESHNUM_BOOKBOX2;
	}

	//Chair라는 글자가 있는 경우
	if (strMeshName.find(L"Chair", 0) != -1)
	{
		if (strMeshName == L"Mesh_Chair1")
			return MESHNUM_CHAIR1;
		if (strMeshName == L"Mesh_Chair2")
			return MESHNUM_CHAIR2;
		if (strMeshName == L"Mesh_Chair3")
			return MESHNUM_CHAIR3;
		if (strMeshName == L"Mesh_Chair4")
			return MESHNUM_CHAIR4;
	}

	//Cabinet라는 글자가 있는 경우
	if (strMeshName.find(L"Cabinet", 0) != -1)
	{
		if (strMeshName == L"Mesh_Cabinet1")
			return MESHNUM_CABINET1;
		if (strMeshName == L"Mesh_Cabinet2")
			return MESHNUM_CABINET2;
	}

	if (strMeshName.find(L"Couch", 0) != -1)
	{
		if (strMeshName == L"Mesh_Couch1")
			return MESHNUM_COUCH1;
		if (strMeshName == L"Mesh_Couch2")
			return MESHNUM_COUCH2;
		if (strMeshName == L"Mesh_Couch3")
			return MESHNUM_COUCH3;
		if (strMeshName == L"Mesh_Couch4")
			return MESHNUM_COUCH4;
	}
	if (strMeshName.find(L"Desk", 0) != -1)
	{
		if (strMeshName == L"Mesh_Desk1")
			return MESHNUM_DESK1;
		if (strMeshName == L"Mesh_Desk2")
			return MESHNUM_DESK2;
		if (strMeshName == L"Mesh_Desk3")
			return MESHNUM_DESK3;
		if (strMeshName == L"Mesh_Desk4")
			return MESHNUM_DESK4;
		if (strMeshName == L"Mesh_Desk5")
			return MESHNUM_DESK5;
		if (strMeshName == L"Mesh_Desk6")
			return MESHNUM_DESK6;
		if (strMeshName == L"Mesh_Desk7")
			return MESHNUM_DESK7;
		if (strMeshName == L"Mesh_Desk8")
			return MESHNUM_DESK8;
		if (strMeshName == L"Mesh_Desk9")
			return MESHNUM_DESK9;
		if (strMeshName == L"Mesh_Desk10")
			return MESHNUM_DESK10;
		if (strMeshName == L"Mesh_Desk11")
			return MESHNUM_DESK11;
		if (strMeshName == L"Mesh_Desk12")
			return MESHNUM_DESK12;
	}
	if (strMeshName.find(L"Wall", 0) != -1)
	{
		if (strMeshName == L"Mesh_Wall1")
			return MESHNUM_WALL1;
		if (strMeshName == L"Mesh_Wall2")
			return MESHNUM_WALL2;
		if (strMeshName == L"Mesh_Wall3")
			return MESHNUM_WALL3;
		if (strMeshName == L"Mesh_Wall4")
			return MESHNUM_WALL4;
		if (strMeshName == L"Mesh_Wall5")
			return MESHNUM_WALL5;
		if (strMeshName == L"Mesh_Wall6")
			return MESHNUM_WALL6;
		if (strMeshName == L"Mesh_Wall7")
			return MESHNUM_WALL7;
		if (strMeshName == L"Mesh_Wall8")
			return MESHNUM_WALL8;
		if (strMeshName == L"Mesh_Wall9")
			return MESHNUM_WALL9;
		if (strMeshName == L"Mesh_Wall10")
			return MESHNUM_WALL10;
	}
	if (strMeshName.find(L"Plant", 0) != -1)
	{
		if (strMeshName == L"Mesh_Plant1")
			return MESHNUM_PLANT1;
		if (strMeshName == L"Mesh_Plant2")
			return MESHNUM_PLANT2;
		if (strMeshName == L"Mesh_Plant3")
			return MESHNUM_PLANT3;
		if (strMeshName == L"Mesh_Plant4")
			return MESHNUM_PLANT4;
		if (strMeshName == L"Mesh_Plant5")
			return MESHNUM_PLANT5;
	}
	if (strMeshName.find(L"Shelf", 0) != -1)
	{
		if (strMeshName == L"Mesh_Shelf1")
			return MESHNUM_SHELF1;
		if (strMeshName == L"Mesh_Shelf2")
			return MESHNUM_SHELF2;
		if (strMeshName == L"Mesh_Shelf3")
			return MESHNUM_SHELF3;
	}

	if (strMeshName == L"Mesh_Coffeetable1")
		return MESHNUM_COFFEETABLE1;
	if (strMeshName == L"Mesh_Coffeetable2")
		return MESHNUM_COFFEETABLE2;
	if (strMeshName == L"Mesh_Door1")
		return MESHNUM_DOOR1;
	if (strMeshName == L"Mesh_Door2")
		return MESHNUM_DOOR2;
	if (strMeshName == L"Mesh_Painting1")
		return MESHNUM_PAINTING1;
	if (strMeshName == L"Mesh_Painting2")
		return MESHNUM_PAINTING2;
	if (strMeshName == L"Mesh_RubbishBin1")
		return MESHNUM_RUBBISHBIN1;
	if (strMeshName == L"Mesh_RubbishBin2")
		return MESHNUM_RUBBISHBIN2;
	if (strMeshName == L"Mesh_ToiletStall1")
		return MESHNUM_TOILETSTALL1;
	if (strMeshName == L"Mesh_ToiletStall2")
		return MESHNUM_TOILETSTALL2;
	if (strMeshName == L"Mesh_Clock")
		return MESHNUM_CLOCK;
	if (strMeshName == L"Mesh_HandDryer")
		return MESHNUM_HANDDRYER;
	if (strMeshName == L"Mesh_Mirror")
		return MESHNUM_MIRROR;
	if (strMeshName == L"Mesh_PaperTowelDispenser")
		return MESHNUM_PAPERTOWELDISPENSER;
	if (strMeshName == L"Mesh_Photocopier")
		return MESHNUM_PHOTOCOPIER;
	if (strMeshName == L"Mesh_Pinboard")
		return MESHNUM_PINBOARD;
	if (strMeshName == L"Mesh_PowerOutlet")
		return MESHNUM_POWEROUTLET;
	if (strMeshName == L"Mesh_Sinks")
		return MESHNUM_SINKS;
	if (strMeshName == L"Mesh_SoapDispenser")
		return MESHNUM_SOAPDISPENSER;
	if (strMeshName == L"Mesh_Urinal")
		return MESHNUM_URINAL;
	if (strMeshName == L"Mesh_WaterCooler")
		return MESHNUM_WATERCOOLER;

	return MESHNUM_END;
}
