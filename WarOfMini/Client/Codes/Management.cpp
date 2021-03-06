#include "stdafx.h"
#include "Management.h"
#include "Scene.h"
#include "OtherPlayer.h"

IMPLEMENT_SINGLETON(CManagement)

CManagement::CManagement(void)
: m_pScene(NULL)
, m_pRenderer(NULL)
, m_pGraphicDev(NULL)
, m_pContext(NULL)
, m_pPxPhysicsSDK(NULL)
, m_pPxScene(NULL)
, m_pPxControllerManager(NULL)
, m_pCooking(NULL)
{
}

CManagement::~CManagement(void)
{
}

void CManagement::Add_RenderGroup(CRenderer::RENDERTYPE eType, CGameObject* pGameObject, _float fViewZ)
{
	if (m_pRenderer != NULL)
		m_pRenderer->Add_RenderGroup(eType, pGameObject, fViewZ);
}

void CManagement::Add_RenderInstGroup(CRenderer::RENDERTYPE eType, UINT uiObjNum, XMFLOAT4X4* pMatWorld)
{
	if (m_pRenderer != NULL)
		m_pRenderer->Add_RenderInstGroup(eType, uiObjNum, pMatWorld);
}

HRESULT CManagement::Ready_Management(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext, PxPhysics* pPxPhysics, PxScene* pPxScene, PxControllerManager* pPxControllerManager, PxCooking* pCooking)
{
	m_pGraphicDev = pGraphicDev;
	m_pContext = pContext;

	m_pPxPhysicsSDK = pPxPhysics;
	m_pPxScene = pPxScene;
	m_pPxControllerManager = pPxControllerManager;
	m_pCooking = pCooking;

	m_pRenderer = CRenderer::Create(pGraphicDev, pContext);
	NULL_CHECK_RETURN_MSG(m_pRenderer, E_FAIL, L"Renderer Create Failed");

	return S_OK;
}

_int CManagement::Update(const _float& fTimeDelta)
{
	_int iResult = 0;

	if (m_pScene != NULL)
		iResult = m_pScene->Update(fTimeDelta);

	return iResult;
}

void CManagement::Render(void)
{
	if (m_pRenderer != NULL)
		m_pRenderer->Render();

}

void CManagement::Release(void)
{
	Safe_Release(m_pRenderer);
	Safe_Release(m_pScene);

	delete this;
}

CRenderer * CManagement::GetRenderer(void)
{
	return m_pRenderer;
}

Ser_PLAYER_DATA* CManagement::Get_Server_Data(int uId)
{
	CLayer* pLayer = m_pScene->FindLayer(L"Layer_GameLogic");

	list<CGameObject*>* pObjList = pLayer->Find_ObjectList(L"OtherPlayer");
	list<CGameObject*>::iterator iter = pObjList->begin();
	list<CGameObject*>::iterator iter_end = pObjList->end();

	for (iter; iter != iter_end; ++iter)
	{
		int iCompare = ((COtherPlayer*)*iter)->GetPacketData()->ID;
		
		if (iCompare == uId)
		{
			return ((COtherPlayer*)*iter)->GetPacketData();
		}
	}

	return nullptr;
}
