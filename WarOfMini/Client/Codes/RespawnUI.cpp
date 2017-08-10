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
#include "CameraMgr.h"

CRespawnUI::CRespawnUI(ID3D11DeviceContext * pContext)
: CUI(pContext)
, m_pPanel(NULL)
, m_pMiniMap(NULL)
, m_pPlayer(NULL)
, m_bRespawnSelect(false)
, m_fTimeCnt(0.f)
, m_bUIOpen(false)
, m_bClearPlayer(false)
{
	ZeroMemory(m_pStationUI, sizeof(CStationUI*) * 5);
	ZeroMemory(m_pFlagUI, sizeof(CFlagUI*) * 5);
	ZeroMemory(m_pStation, sizeof(CStation*) * 5);
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


	//Station One======================================
	m_pStationUI[0] = CStationUI::Create(m_pContext);
	((CUI*)m_pStationUI[0])->SetSizeX(80);
	((CUI*)m_pStationUI[0])->SetSizeY(70);
	((CUI*)m_pStationUI[0])->SetMoveX(-300.f);
	((CUI*)m_pStationUI[0])->SetMoveY(180.f);
	((CUI*)m_pStationUI[0])->ComputeFXFY();

	//Station One Flag
	m_pFlagUI[0] = CFlagUI::Create(m_pContext);
	((CUI*)m_pFlagUI[0])->SetSizeX(50);
	((CUI*)m_pFlagUI[0])->SetSizeY(40);
	((CUI*)m_pFlagUI[0])->SetMoveX(-275.f);
	((CUI*)m_pFlagUI[0])->SetMoveY(140.f);
	((CUI*)m_pFlagUI[0])->ComputeFXFY();
	//================================================


	//Station Two===================================
	m_pStationUI[1] = CStationUI::Create(m_pContext);
	((CUI*)m_pStationUI[1])->SetSizeX(80);
	((CUI*)m_pStationUI[1])->SetSizeY(70);
	((CUI*)m_pStationUI[1])->SetMoveX(-30.f);
	((CUI*)m_pStationUI[1])->SetMoveY(180.f);
	((CUI*)m_pStationUI[1])->ComputeFXFY();

	//Station Two Flag
	m_pFlagUI[1] = CFlagUI::Create(m_pContext);
	((CUI*)m_pFlagUI[1])->SetSizeX(50);
	((CUI*)m_pFlagUI[1])->SetSizeY(40);
	((CUI*)m_pFlagUI[1])->SetMoveX(-5.f);
	((CUI*)m_pFlagUI[1])->SetMoveY(140.f);
	((CUI*)m_pFlagUI[1])->ComputeFXFY();
	//================================================


	//Station Three==================================
	m_pStationUI[2] = CStationUI::Create(m_pContext);
	((CUI*)m_pStationUI[2])->SetSizeX(80);
	((CUI*)m_pStationUI[2])->SetSizeY(70);
	((CUI*)m_pStationUI[2])->SetMoveX(-300.f);
	((CUI*)m_pStationUI[2])->SetMoveY(-200.f);
	((CUI*)m_pStationUI[2])->ComputeFXFY();

	//Station Three Flag
	m_pFlagUI[2] = CFlagUI::Create(m_pContext);
	((CUI*)m_pFlagUI[2])->SetSizeX(50);
	((CUI*)m_pFlagUI[2])->SetSizeY(40);
	((CUI*)m_pFlagUI[2])->SetMoveX(-275.f);
	((CUI*)m_pFlagUI[2])->SetMoveY(-240.f);
	((CUI*)m_pFlagUI[2])->ComputeFXFY();
	//================================================


	//Station Four====================================
	m_pStationUI[3] = CStationUI::Create(m_pContext);
	((CUI*)m_pStationUI[3])->SetSizeX(80);
	((CUI*)m_pStationUI[3])->SetSizeY(70);
	((CUI*)m_pStationUI[3])->SetMoveX(-30.f);
	((CUI*)m_pStationUI[3])->SetMoveY(-200.f);
	((CUI*)m_pStationUI[3])->ComputeFXFY();

	//Station Four Flag
	m_pFlagUI[3] = CFlagUI::Create(m_pContext);
	((CUI*)m_pFlagUI[3])->SetSizeX(50);
	((CUI*)m_pFlagUI[3])->SetSizeY(40);
	((CUI*)m_pFlagUI[3])->SetMoveX(-5.f);
	((CUI*)m_pFlagUI[3])->SetMoveY(-240.f);
	((CUI*)m_pFlagUI[3])->ComputeFXFY();
	//================================================

	//Station Five====================================
	m_pStationUI[4] = CStationUI::Create(m_pContext);
	((CUI*)m_pStationUI[4])->SetSizeX(80);
	((CUI*)m_pStationUI[4])->SetSizeY(70);
	((CUI*)m_pStationUI[4])->SetMoveX(330.f);
	((CUI*)m_pStationUI[4])->SetMoveY(15.f);
	((CUI*)m_pStationUI[4])->ComputeFXFY();

	//Station Five Flag
	m_pFlagUI[4] = CFlagUI::Create(m_pContext);
	((CUI*)m_pFlagUI[4])->SetSizeX(50);
	((CUI*)m_pFlagUI[4])->SetSizeY(40);
	((CUI*)m_pFlagUI[4])->SetMoveX(355.f);
	((CUI*)m_pFlagUI[4])->SetMoveY(-25.f);
	((CUI*)m_pFlagUI[4])->ComputeFXFY();
	//================================================

	return S_OK;
}

_int CRespawnUI::Update(const _float & fTimeDelta)
{
	RespawnSelectUpdate();

	StationCollision();

	//if(GetAsyncKeyState('B') & 1) //리스폰 UI 실행 -> 나중에 플레이어가 죽으면 알아서 뜨도록 변경
	if ((m_pPlayer!= NULL) && (m_pPlayer->GetHP() <= 0)) //리스폰 UI 실행 -> 나중에 플레이어가 죽으면 알아서 뜨도록 변경
	{
		if (m_bUIOpen == false)
		{
			m_fTimeCnt += fTimeDelta;
			if (m_fTimeCnt >= 1.f && m_bClearPlayer == false)
			{
				CCameraMgr::GetInstance()->Set_CurCamera(CCameraMgr::CAMERALIST::CAMERA_DYNAMIC);
				m_pPlayer->SetPosition(XMFLOAT3(-1111.f, -1111.f, -1111.f));
				m_bClearPlayer = true;
			}
		//마우스 커서 보이도록....
		g_bCursorShow = true;

		m_pPanel->SetStart(true);
		m_pMiniMap->SetStart(true);

			if (m_fTimeCnt >= 3.f)
			{
				m_fTimeCnt = 0.f;
				m_bUIOpen = false;
				m_bClearPlayer = false;	
				m_pPanel->SetStart(true);
				m_pMiniMap->SetStart(true);

			}
		}
		
	}
	/*
	if (GetAsyncKeyState('N') & 1)
	{
		m_pPanel->SetEnd(true);
		m_pMiniMap->SetEnd(true);

		CCameraMgr::GetInstance()->Set_CurCamera(CCameraMgr::CAMERALIST::CAMERA_STATIC);
	}
	*/

	StationFlagUIUpdate();
	StationUIUpdate();


	m_pPanel->Update(fTimeDelta);
	m_pMiniMap->Update(fTimeDelta);

	for (int i = 0; i < 5; ++i)
	{
		m_pStationUI[i]->Update(fTimeDelta);
		m_pFlagUI[i]->Update(fTimeDelta);
	}


	return 0;
}

void CRespawnUI::Render(void)
{

	m_pPanel->Render();
	m_pMiniMap->Render();

	for (int i = 0; i < 5; ++i)
	{
		m_pStationUI[i]->Render();
		m_pFlagUI[i]->Render();
	}
}

void CRespawnUI::Release(void)
{
	m_pPanel->Release();
	m_pMiniMap->Release();

	for (int i = 0; i < 5; ++i)
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
	for (int i = 0; i < 5; ++i)
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
		for (int i = 0; i < 5; ++i)
		{
			m_pFlagUI[i]->Set_TextureNumber(m_pStation[i]->GetFlagState());
		}
	}
}

void CRespawnUI::StationCollision(void)
{
	if(m_bRespawnSelect)
		return;

	POINT		ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	for (int i = 0; i < 5; ++i)
	{
		if (m_pStationUI[i]->GetStart() == false)
			continue ;
		
		RECT rcUI = { long(m_pStationUI[i]->GetfX() - m_pStationUI[i]->GetSizeX() * 0.5f)
					, long(m_pStationUI[i]->GetfY() - m_pStationUI[i]->GetSizeY() * 0.5f)
					, long(m_pStationUI[i]->GetfX() + m_pStationUI[i]->GetSizeX() * 0.5f)
					, long(m_pStationUI[i]->GetfY() + m_pStationUI[i]->GetSizeY() * 0.5f) };

		if (PtInRect(&rcUI, ptMouse)) //마우스커서와 타워가 충돌했을 경우 (해당 플레이어의 타워일경우-> 클릭시이동)
		{
			(m_pStationUI[i])->SetSizeX(90);
			(m_pStationUI[i])->SetSizeY(80);

			CStation::eFlagState eTeam = CStation::FLAG_EMPTY;
			if (g_myid % 2 == 0)
				eTeam = CStation::FLAG_TEAM1;
			else
				eTeam = CStation::FLAG_TEAM2;

			if(m_pInput->Get_DIMouseState(CInput::DIM_LB) &&
				eTeam == m_pStation[i]->GetFlagState()) //플레이어가 점령한 타워이고 L버튼을 클릭한경우 -> 해당 타워로 이동
			{
				m_vRespawnPos = m_pStation[i]->GetTransformPosition();

				m_vRespawnPos.x += PositionRand(-50.f, 50.f);
				m_vRespawnPos.z += PositionRand(-50.f, 50.f);
				m_vRespawnPos.y += 20.f;

				m_bRespawnSelect = true;

				m_pPanel->SetEnd(true);
				m_pMiniMap->SetEnd(true);
				m_bUIOpen = false;
				m_pPlayer->SetHP(9999);
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

void CRespawnUI::RespawnSelectUpdate(void)
{
	if (!m_bRespawnSelect)
		return;

	if (m_pPanel->GetResetCheck() == true)
	{
		//마우스 커서 다시 안보이도록
		g_bCursorShow = false;

		m_pPlayer->SetPosition(m_vRespawnPos);
		CCameraMgr::GetInstance()->Set_CurCamera(CCameraMgr::CAMERALIST::CAMERA_STATIC);
		m_bRespawnSelect = false;
	}
}
