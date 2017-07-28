#include "stdafx.h"
#include "Logo.h"
#include "LogoBack.h"
#include "RenderTargetMgr.h"
#include "Loading.h"
#include "Stage.h"
#include "SceneSelector.h"
#include "Management.h"
#include "ResourcesMgr.h"
#include "CameraMgr.h"

CLogo::CLogo(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext, PxPhysics* pPxPhysicsSDK, PxScene* pPxScene, PxControllerManager*	pPxControllerManager, PxCooking* pCooking)
: CScene(pGraphicDev, pContext, pPxPhysicsSDK, pPxScene, pPxControllerManager, pCooking)
, m_pLoading(NULL)
{
}

CLogo::~CLogo(void)
{
}

CLogo * CLogo::Create(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext, PxPhysics* pPxPhysicsSDK, PxScene* pPxScene, PxControllerManager*	pPxControllerManager, PxCooking* pCooking)
{
	CLogo* pScene = new CLogo(pGraphicDev, pContext, pPxPhysicsSDK, pPxScene, pPxControllerManager, pCooking);

	if (FAILED(pScene->Ready_Scene()))
	{
		MSG_BOX(L"LogoScene Create Failed");
		Safe_Release(pScene);
	}
	return pScene;
}

HRESULT CLogo::Ready_Scene(void)
{
	if (m_pLoading == NULL)
		m_pLoading = CLoading::Create(CLoading::LOADING_LOGO);


	if (FAILED(Ready_GameLogic()))	
		return E_FAIL;

	if (FAILED(Ready_Environment()))	
		return E_FAIL;

	return S_OK;
}

_int CLogo::Update(const _float & fTimeDelta)
{
	CScene::Update(fTimeDelta);

	if (m_pLoading->GetComplete() == true && (GetAsyncKeyState(VK_RETURN) & 0x8000))
	{
		CManagement::GetInstance()->Change_Scene(CSceneSelector(SCENE_STAGE));	
	}
	
	return 0;
}

HRESULT CLogo::Ready_GameLogic(void)
{
	CLayer* pLayer = CLayer::Create();

	CGameObject* pGameObject = NULL;
	
	//LogoBack
	pGameObject = CLogoBack::Create(m_pContext, L"Texture_LogoBack");

	if (NULL == pGameObject)
		return E_FAIL;

	pLayer->Ready_Object(L"LogoBack", pGameObject);


	m_mapLayer.insert(MAPLAYER::value_type(L"Layer_GameLogic", pLayer));

	return S_OK;
}

HRESULT CLogo::Ready_Environment(void)
{
	CLayer* pLayer = CLayer::Create();
	CGameObject* pGameObject = NULL;

	m_mapLayer.insert(MAPLAYER::value_type(L"Layer_Environment", pLayer));
	
	return S_OK;
}

void CLogo::Release(void)
{
	Safe_Release(m_pLoading);
	CScene::Release();


	delete this;
}
