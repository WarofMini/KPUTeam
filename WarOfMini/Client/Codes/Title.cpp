#include "stdafx.h"
#include "Title.h"
#include "RenderTargetMgr.h"
#include "Logo.h"
#include "TitleBack.h"
#include "SceneSelector.h"
#include "Management.h"
#include "ResourcesMgr.h"
#include "CameraMgr.h"
#include "LogoBack.h"


CTitle::CTitle(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext, PxPhysics* pPxPhysicsSDK, PxScene* pPxScene, PxControllerManager*	pPxControllerManager, PxCooking* pCooking)
: CScene(pGraphicDev, pContext, pPxPhysicsSDK, pPxScene, pPxControllerManager, pCooking)
, m_bSceneCheck(NULL)
{
}

CTitle::~CTitle(void)
{
}

CTitle * CTitle::Create(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext, PxPhysics* pPxPhysicsSDK, PxScene* pPxScene, PxControllerManager*	pPxControllerManager, PxCooking* pCooking)
{
	CTitle* pScene = new CTitle(pGraphicDev, pContext, pPxPhysicsSDK, pPxScene, pPxControllerManager, pCooking);

	if (FAILED(pScene->Ready_Scene()))
	{
		MSG_BOX(L"TitleScene Create Failed");
		Safe_Release(pScene);
	}
	return pScene;
}

HRESULT CTitle::Ready_Scene(void)
{

	if (FAILED(Ready_Environment()))
		return E_FAIL;
	
	if (FAILED(Ready_GameLogic()))
		return E_FAIL;

	return S_OK;
}

_int CTitle::Update(const _float & fTimeDelta)
{
	CScene::Update(fTimeDelta);

	if ((*m_bSceneCheck) == true)
	{
		CManagement::GetInstance()->Change_Scene(CSceneSelector(SCENE_LOGO));
	}

	return 0;
}

HRESULT CTitle::Ready_GameLogic(void)
{
	CLayer* pLayer = CLayer::Create();

	CGameObject* pGameObject = NULL;


	//TitleBack
	pGameObject = CTitleBack::Create(m_pContext);

	m_bSceneCheck = ((CTitleBack*)pGameObject)->GetAlphaCheck();

	if (NULL == pGameObject)
		return E_FAIL;

	pLayer->Ready_Object(L"TitleBack", pGameObject);

	m_mapLayer.insert(MAPLAYER::value_type(L"Layer_GameLogic", pLayer));


	return S_OK;
}

HRESULT CTitle::Ready_Environment(void)
{
	CLayer* pLayer = CLayer::Create();
	CGameObject* pGameObject = NULL;

	// RenderTarget
	CRenderTargetMgr::GetInstance()->Ready_RenderTarget(m_pGraphicDev, m_pContext, L"RT_Blend", DXGI_FORMAT_R8G8B8A8_UNORM, WINCX, WINCY, -0.8f, 0.8f);


	m_mapLayer.insert(MAPLAYER::value_type(L"Layer_Environment", pLayer));

	return S_OK;
}

void CTitle::Release(void)
{
	CScene::Release();

	delete this;
}
