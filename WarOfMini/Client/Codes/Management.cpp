#include "stdafx.h"
#include "Management.h"
#include "Scene.h"

IMPLEMENT_SINGLETON(CManagement)

CManagement::CManagement(void)
: m_pScene(NULL)
, m_pRenderer(NULL)
, m_pGraphicDev(NULL)
, m_pContext(NULL)
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

HRESULT CManagement::Ready_Management(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext)
{
	m_pGraphicDev = pGraphicDev;
	m_pContext = pContext;

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
