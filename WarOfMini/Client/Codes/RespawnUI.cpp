#include "stdafx.h"
#include "RespawnUI.h"
#include "Management.h"
#include "ResourcesMgr.h"
#include "ShaderMgr.h"
#include "GraphicDev.h"
#include "CameraMgr.h"
#include "Panel.h"


CRespawnUI::CRespawnUI(ID3D11DeviceContext * pContext)
: CUI(pContext)
, m_pPanel(NULL)
, m_pMiniMap(NULL)
{
}

CRespawnUI::~CRespawnUI(void)
{
}

CRespawnUI* CRespawnUI::Create(ID3D11DeviceContext * pContext)
{
	CRespawnUI* pRespawn = new CRespawnUI(pContext);

	if (FAILED(pRespawn->Initialize()))
		Safe_Release(pRespawn);

	return pRespawn;
}

HRESULT CRespawnUI::Initialize(void)
{
	m_pPanel = CPanel::Create(m_pContext, L"Texture_Panel");
	((CUI*)m_pPanel)->SetSizeX(1100);
	((CUI*)m_pPanel)->SetSizeY(900);
	((CUI*)m_pPanel)->SetMoveX(0.f);
	((CUI*)m_pPanel)->SetMoveY(0.f);
	((CUI*)m_pPanel)->ComputeFXFY();

	m_pMiniMap = CPanel::Create(m_pContext, L"Texture_MiniMap");
	((CUI*)m_pMiniMap)->SetSizeX(900);
	((CUI*)m_pMiniMap)->SetSizeY(700);
	((CUI*)m_pMiniMap)->SetMoveX(0.f);
	((CUI*)m_pMiniMap)->SetMoveY(0.f);
	((CUI*)m_pMiniMap)->ComputeFXFY();


	return S_OK;
}

_int CRespawnUI::Update(const _float & fTimeDelta)
{
	if (CCameraMgr::GetInstance()->Get_CurCamera() == CCameraMgr::CAMERA_DYNAMIC)
		return 0;

	if(GetAsyncKeyState('B') & 1)
	{
		m_pPanel->SetStart(true);
		m_pMiniMap->SetStart(true);
	}
	if (GetAsyncKeyState('N') & 1)
	{
		m_pPanel->SetEnd(true);
		m_pMiniMap->SetEnd(true);
	}

	m_pPanel->Update(fTimeDelta);
	m_pMiniMap->Update(fTimeDelta);

	return 0;
}

void CRespawnUI::Render(void)
{
	if (CCameraMgr::GetInstance()->Get_CurCamera() == CCameraMgr::CAMERA_DYNAMIC)
		return;

	m_pPanel->Render();
	m_pMiniMap->Render();

}

void CRespawnUI::Release(void)
{
	m_pPanel->Release();
	m_pMiniMap->Release();

	CUI::Release();
}

HRESULT CRespawnUI::Ready_Component(void)
{
	return S_OK;
}