#include "stdafx.h"
#include "Scene.h"
#include "Layer.h"

CScene::CScene(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext, PxPhysics* pPxPhysicsSDK, PxScene* pPxScene, PxControllerManager*	pPxControllerManager, PxCooking* pCooking)
: m_pGraphicDev(pGraphicDev)
, m_pContext(pContext)
, m_pPxPhysicsSDK(pPxPhysicsSDK)
, m_pPxScene(pPxScene)
, m_pPxControllerManager(pPxControllerManager)
, m_pPxMaterial(NULL)
, m_pCooking(pCooking)
{
}

CScene::~CScene(void)
{

}

INT CScene::Update(const _float &fTimeDelta)
{
	MAPLAYER::iterator iter = m_mapLayer.begin();
	MAPLAYER::iterator iter_end = m_mapLayer.end();

	_int iResult = 0;

	for (; iter != iter_end; ++iter)
	{
		iResult = iter->second->Update(fTimeDelta);

		if (iResult & 0x80000000)
			return iResult;
	}

	return iResult;
}

void CScene::Render(void)
{
	MAPLAYER::iterator iter = m_mapLayer.begin();
	MAPLAYER::iterator iter_end = m_mapLayer.end();

	for (; iter != iter_end; ++iter)
	{
		iter->second->Render();
	}
}

void CScene::Release(void)
{
	for_each(m_mapLayer.begin(), m_mapLayer.end(), CRelease_Pair());
	m_mapLayer.clear();
}

CLayer* CScene::FindLayer(const _tchar* LayerName)
{
	return m_mapLayer.find(LayerName)->second;
}

