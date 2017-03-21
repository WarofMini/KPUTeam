#include "stdafx.h"
#include "Stage.h"
#include "RenderTargetMgr.h"
#include "CameraMgr.h"
#include "FontMgr.h"
#include "LogoBack.h"
#include "StaticObject.h"

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
	if (FAILED(Ready_Environment()))	return E_FAIL;
	if (FAILED(Ready_GameLogic()))		return E_FAIL;

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



	//StatocObject
	pGameObject = CStaticObject::Create(m_pContext);

	if (NULL == pGameObject)
		return E_FAIL;

	pLayer->Ready_Object(L"Object", pGameObject);


	m_mapLayer.insert(MAPLAYER::value_type(L"Layer_GameLogic", pLayer));

	return S_OK;
}

HRESULT CStage::Ready_Environment(void)
{
	CLayer* pLayer = CLayer::Create();
	CGameObject* pGameObject = NULL;

	//다이나믹 카메라 적용
	CCameraMgr::GetInstance()->Ready_DynamicCamera(m_pContext, CCameraMgr::CAMERA_DYNAMIC, 0.1f, 1000.f, _vec3(0.f, 5.f, -10.f), _vec3(0.f, 0.f, 0.f));


	// RenderTarget
	//CRenderTargetMgr::GetInstance()->Ready_RenderTarget(m_pGraphicDev, m_pContext, L"RT_Blend", DXGI_FORMAT_R8G8B8A8_UNORM, WINCX, WINCY, -0.8f, 0.8f);

	m_mapLayer.insert(MAPLAYER::value_type(L"Layer_Environment", pLayer));

	return S_OK;
}

void CStage::Release(void)
{
	CScene::Release();

	delete this;
}