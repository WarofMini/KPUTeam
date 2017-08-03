#include "stdafx.h"
#include "RespawnUI.h"
#include "Management.h"
#include "ResourcesMgr.h"
#include "ShaderMgr.h"
#include "GraphicDev.h"
#include "CameraMgr.h"
#include "Panel.h"
#include "StationUI.h"


CRespawnUI::CRespawnUI(ID3D11DeviceContext * pContext)
: CUI(pContext)
, m_pPanel(NULL)
, m_pMiniMap(NULL)
{
	for (int i = 0; i < 3; ++i)
		m_pStationUI[i] = NULL;
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


	//Station One
	m_pStationUI[0] = CStationUI::Create(m_pContext);
	((CUI*)m_pStationUI[0])->SetSizeX(80);
	((CUI*)m_pStationUI[0])->SetSizeY(70);
	((CUI*)m_pStationUI[0])->SetMoveX(-270.f);
	((CUI*)m_pStationUI[0])->SetMoveY(140.f);
	((CUI*)m_pStationUI[0])->ComputeFXFY();

	//Station Two
	m_pStationUI[1] = CStationUI::Create(m_pContext);
	((CUI*)m_pStationUI[1])->SetSizeX(80);
	((CUI*)m_pStationUI[1])->SetSizeY(70);
	((CUI*)m_pStationUI[1])->SetMoveX(10.f);
	((CUI*)m_pStationUI[1])->SetMoveY(-220.f);
	((CUI*)m_pStationUI[1])->ComputeFXFY();

	//Station Three
	m_pStationUI[2] = CStationUI::Create(m_pContext);
	((CUI*)m_pStationUI[2])->SetSizeX(80);
	((CUI*)m_pStationUI[2])->SetSizeY(70);
	((CUI*)m_pStationUI[2])->SetMoveX(330.f);
	((CUI*)m_pStationUI[2])->SetMoveY(15.f);
	((CUI*)m_pStationUI[2])->ComputeFXFY();

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

	StationUIUpdate();


	m_pPanel->Update(fTimeDelta);
	m_pMiniMap->Update(fTimeDelta);

	for (int i = 0; i < 3; ++i)
		m_pStationUI[i]->Update(fTimeDelta);

	return 0;
}

void CRespawnUI::Render(void)
{
	if (CCameraMgr::GetInstance()->Get_CurCamera() == CCameraMgr::CAMERA_DYNAMIC)
		return;

	m_pPanel->Render();
	m_pMiniMap->Render();

	for (int i = 0; i < 3; ++i)
		m_pStationUI[i]->Render();
}

void CRespawnUI::Release(void)
{
	m_pPanel->Release();
	m_pMiniMap->Release();

	for (int i = 0; i < 3; ++i)
		m_pStationUI[i]->Release();

	CUI::Release();
}

HRESULT CRespawnUI::Ready_Component(void)
{
	return S_OK;
}

void CRespawnUI::StationUIUpdate(void)
{
	for (int i = 0; i < 3; ++i)
	{
		if ((m_pMiniMap->GetAlpha() >= 0.5f) && (m_pMiniMap->GetStart()))
		{
			m_pStationUI[i]->SetStart(true);
		}
		if (m_pMiniMap->GetEnd())
		{
			m_pStationUI[i]->SetStart(false);
		}
	}
}
