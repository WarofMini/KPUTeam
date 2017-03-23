#include "stdafx.h"
#include "Stage.h"
#include "RenderTargetMgr.h"
#include "CameraMgr.h"
#include "FontMgr.h"
#include "LogoBack.h"
#include "DefaultObject.h"
#include "Player.h"
#include "PlayerMain.h"
#include "Player.h"
#include "PlayerMain.h"
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


	//pGameObject = CPlayerMain::Create(m_pGraphicDev, m_pContext);
	//if (NULL == pGameObject) return E_FAIL;
	//pLayer->Ready_Object(L"PlayerMain", pGameObject);



	//InitFloor
	float m_iSize = 500.f;
	float m_fSize = 0.4f;

	m_iSize *= m_fSize;

	for (int i = 0; i < 8; ++i)
	{
		for (int j = 0; j < 6; ++j)
		{
			pGameObject = CDefaultObj::Create(m_pContext);
			if (NULL == pGameObject) return E_FAIL;

			((CDefaultObj*)pGameObject)->SetObjNum(MESHNUM_FLOOR1);

			//((CTransform*)pGameObject->Get_Component(L"Com_Transform"))->m_vAngle.x = (_float)D3DXToRadian(90.f);
			((CTransform*)pGameObject->Get_Component(L"Com_Transform"))->m_vScale = XMFLOAT3(m_fSize, m_fSize, m_fSize);
			((CTransform*)pGameObject->Get_Component(L"Com_Transform"))->m_vPos = XMFLOAT3((j % 6) * m_iSize, 0.f, i * m_iSize);

			pLayer->Ready_Object(L"StaticObject", pGameObject);

		}
	}

	m_mapLayer.insert(MAPLAYER::value_type(L"Layer_GameLogic", pLayer));
	return S_OK;
}

HRESULT CStage::Ready_Environment(void)
{
	CLayer* pLayer = CLayer::Create();
	CGameObject* pGameObject = NULL;

	//다이나믹 카메라 적용
	CCameraMgr::GetInstance()->Ready_DynamicCamera(m_pContext, CCameraMgr::CAMERA_DYNAMIC, 0.1f, 1000.f, XMFLOAT3(0.f, 5.f, -10.f), XMFLOAT3(0.f, 0.f, 0.f));


	m_mapLayer.insert(MAPLAYER::value_type(L"Layer_Environment", pLayer));

	return S_OK;
}

void CStage::Release(void)
{
	CScene::Release();

	delete this;
}