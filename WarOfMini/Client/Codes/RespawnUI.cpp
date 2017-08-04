#include "stdafx.h"
#include "RespawnUI.h"
#include "Management.h"
#include "ResourcesMgr.h"
#include "ShaderMgr.h"
#include "Scene.h"
#include "Layer.h"
#include "GraphicDev.h"
#include "CameraMgr.h"
#include "Panel.h"
#include "StationUI.h"
#include "FlagUI.h"
#include "Station.h"
#include "Player.h"
#include "Input.h"

CRespawnUI::CRespawnUI(ID3D11DeviceContext * pContext)
: CUI(pContext)
, m_pPanel(NULL)
, m_pMiniMap(NULL)
, m_pPlayer(NULL)
{
	ZeroMemory(m_pStationUI, sizeof(CStationUI*) * 3);
	ZeroMemory(m_pFlagUI, sizeof(CFlagUI*) * 3);
	ZeroMemory(m_pStation, sizeof(CStation*) * 3);
	m_pInput = CInput::GetInstance();

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

	//Station One Flag
	m_pFlagUI[0] = CFlagUI::Create(m_pContext);
	((CUI*)m_pFlagUI[0])->SetSizeX(50);
	((CUI*)m_pFlagUI[0])->SetSizeY(40);
	((CUI*)m_pFlagUI[0])->SetMoveX(-245.f);
	((CUI*)m_pFlagUI[0])->SetMoveY(100.f);
	((CUI*)m_pFlagUI[0])->ComputeFXFY();

	//Station Two Flag
	m_pFlagUI[1] = CFlagUI::Create(m_pContext);
	((CUI*)m_pFlagUI[1])->SetSizeX(50);
	((CUI*)m_pFlagUI[1])->SetSizeY(40);
	((CUI*)m_pFlagUI[1])->SetMoveX(35.f);
	((CUI*)m_pFlagUI[1])->SetMoveY(-260.f);
	((CUI*)m_pFlagUI[1])->ComputeFXFY();


	//Station Three Flag
	m_pFlagUI[2] = CFlagUI::Create(m_pContext);
	((CUI*)m_pFlagUI[2])->SetSizeX(50);
	((CUI*)m_pFlagUI[2])->SetSizeY(40);
	((CUI*)m_pFlagUI[2])->SetMoveX(355.f);
	((CUI*)m_pFlagUI[2])->SetMoveY(-25.f);
	((CUI*)m_pFlagUI[2])->ComputeFXFY();

	return S_OK;
}

_int CRespawnUI::Update(const _float & fTimeDelta)
{

	if (CCameraMgr::GetInstance()->Get_CurCamera() == CCameraMgr::CAMERA_DYNAMIC)
		return 0;

	StationCollision();
	

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

	StationFlagUIUpdate();
	StationUIUpdate();


	m_pPanel->Update(fTimeDelta);
	m_pMiniMap->Update(fTimeDelta);

	for (int i = 0; i < 3; ++i)
	{
		m_pStationUI[i]->Update(fTimeDelta);
		m_pFlagUI[i]->Update(fTimeDelta);
	}
	return 0;
}

void CRespawnUI::Render(void)
{
	if (CCameraMgr::GetInstance()->Get_CurCamera() == CCameraMgr::CAMERA_DYNAMIC)
		return;

	m_pPanel->Render();
	m_pMiniMap->Render();

	for (int i = 0; i < 3; ++i)
	{
		m_pStationUI[i]->Render();
		m_pFlagUI[i]->Render();
	}
}

void CRespawnUI::Release(void)
{
	m_pPanel->Release();
	m_pMiniMap->Release();

	for (int i = 0; i < 3; ++i)
	{
		m_pStationUI[i]->Release();
		m_pFlagUI[i]->Release();
	}

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
			m_pFlagUI[i]->SetStart(true);
		}
		if (m_pMiniMap->GetEnd())
		{
			m_pStationUI[i]->SetStart(false);
			m_pFlagUI[i]->SetStart(false);
		}
	}
}

void CRespawnUI::StationFlagUIUpdate(void)
{
	if (!m_pStation[0])
	{
		CLayer* pLayer = CManagement::GetInstance()->GetScene()->FindLayer(L"Layer_GameLogic");
		list<CGameObject*>* pObjList = pLayer->Find_ObjectList(L"Station");

		list<CGameObject*>::iterator iter = pObjList->begin();
		list<CGameObject*>::iterator iter_end = pObjList->end();

		int m_iCount = 0;

		for (iter; iter != iter_end; ++iter)
		{
			m_pStation[m_iCount] = ((CStation*)(*iter));

			++m_iCount;
		}

		if (!m_pPlayer)
		{
			CGameObject* pObj = pLayer->Find_Object(L"Player");

			m_pPlayer = (CPlayer*)pObj;
		}

	}
	else
	{
		for (int i = 0; i < 3; ++i)
		{
			m_pFlagUI[i]->Set_TextureNumber(m_pStation[i]->GetFlagState());
		}
	}
}

void CRespawnUI::StationCollision(void)
{

	POINT		ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	for (int i = 0; i < 3; ++i)
	{
		
		RECT rcUI = { long(m_pStationUI[i]->GetfX() - m_pStationUI[i]->GetSizeX() * 0.5f)
					, long(m_pStationUI[i]->GetfY() - m_pStationUI[i]->GetSizeY() * 0.5f)
					, long(m_pStationUI[i]->GetfX() + m_pStationUI[i]->GetSizeX() * 0.5f)
					, long(m_pStationUI[i]->GetfY() + m_pStationUI[i]->GetSizeY() * 0.5f) };

		if (PtInRect(&rcUI, ptMouse)) //마우스커서와 타워가 충돌했을 경우 (해당 플레이어의 타워일경우-> 클릭시이동)
		{
			(m_pStationUI[i])->SetSizeX(90);
			(m_pStationUI[i])->SetSizeY(80);

			if(m_pInput->Get_DIMouseState(CInput::DIM_LB)) //플레이어가 점령한 타워이고 L버튼을 클릭한경우 -> 해당 타워로 이동
			{
				XMFLOAT3 m_vPos = m_pStation[i]->GetTransformPosition();

				m_vPos.x += PositionRand(-50.f, 50.f);
				m_vPos.z += PositionRand(-50.f, 50.f);
				m_vPos.y += 20.f;

				m_pPlayer->SetPosition(m_vPos);
			}
		}
		else //다른 팀의 타워일경우 -> 반응없음
		{
			(m_pStationUI[i])->SetSizeX(80);
			(m_pStationUI[i])->SetSizeY(70);
		}
	}
}


_float CRespawnUI::PositionRand(_float a, _float b)
{
	return ((b - a)*((float)rand() / RAND_MAX)) + a;
}