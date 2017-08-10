#include "stdafx.h"
#include "ResultUI.h"
#include "Management.h"
#include "ResourcesMgr.h"
#include "ShaderMgr.h"
#include "Scene.h"
#include "Layer.h"
#include "GraphicDev.h"
#include "CameraMgr.h"
#include "Station.h"
#include "Player.h"
#include "Input.h"
#include "Panel.h"
#include "OutCom.h"
#include "CameraMgr.h"
#include "FlagUI.h"
#include "Button.h"

CResultUI::CResultUI(ID3D11DeviceContext * pContext)
: CUI(pContext)
, m_pPanel(NULL)
, m_pOutCom(NULL)
, m_pFlag(NULL)
, m_bStart(false)
{
	m_pInput = CInput::GetInstance();
	ZeroMemory(m_pButton, sizeof(CButtonUI*) * 2);
}


CResultUI::~CResultUI(void)
{
}

CResultUI* CResultUI::Create(ID3D11DeviceContext * pContext)
{
	CResultUI* pResult = new CResultUI(pContext);

	if (FAILED(pResult->Initialize()))
		Safe_Release(pResult);

	return pResult;
}

HRESULT CResultUI::Initialize(void)
{
	m_pPanel = CPanel::Create(m_pContext, L"Texture_ResultPanel");
	((CUI*)m_pPanel)->SetSizeX(900);
	((CUI*)m_pPanel)->SetSizeY(700);
	((CUI*)m_pPanel)->SetMoveX(0.f);
	((CUI*)m_pPanel)->SetMoveY(0.f);
	((CUI*)m_pPanel)->ComputeFXFY();


	m_pOutCom = COutComUI::Create(m_pContext);
	((CUI*)m_pOutCom)->SetSizeX(300);
	((CUI*)m_pOutCom)->SetSizeY(150);
	((CUI*)m_pOutCom)->SetMoveX(0.f);
	((CUI*)m_pOutCom)->SetMoveY(-250.f);
	((CUI*)m_pOutCom)->ComputeFXFY();

	m_pFlag = CFlagUI::Create(m_pContext);
	((CUI*)m_pFlag)->SetSizeX(500);
	((CUI*)m_pFlag)->SetSizeY(390);
	((CUI*)m_pFlag)->SetMoveX(0.f);
	((CUI*)m_pFlag)->SetMoveY(0.0f);
	((CUI*)m_pFlag)->ComputeFXFY();

	//Try Button
	m_pButton[0] = CButtonUI::Create(m_pContext);
	((CUI*)m_pButton[0])->SetSizeX(100);
	((CUI*)m_pButton[0])->SetSizeY(100);
	((CUI*)m_pButton[0])->SetMoveX(230.f);
	((CUI*)m_pButton[0])->SetMoveY(280.0f);
	((CUI*)m_pButton[0])->ComputeFXFY();


	//ExitButton
	m_pButton[1] = CButtonUI::Create(m_pContext);
	((CUI*)m_pButton[1])->SetSizeX(100);
	((CUI*)m_pButton[1])->SetSizeY(100);
	((CUI*)m_pButton[1])->SetMoveX(380.f);
	((CUI*)m_pButton[1])->SetMoveY(280.0f);
	((CUI*)m_pButton[1])->ComputeFXFY();
	m_pButton[1]->Set_TextureNumber(1);
	return S_OK;
}

_int CResultUI::Update(const _float & fTimeDelta)
{
	if (GetAsyncKeyState('M') & 1) //결과창UI 실행
	{
		if (!m_bStart) //결과창 On
		{
			//마우스 커서 보이도록....
			g_bCursorShow = true;
			m_pPanel->SetStart(true);
			CCameraMgr::GetInstance()->Set_CurCamera(CCameraMgr::CAMERALIST::CAMERA_DYNAMIC);
			m_bStart = true;
		}
		else //결과창 Off
		{
			m_pPanel->SetEnd(true);
			m_pFlag->SetStart(false);
			m_pOutCom->SetStart(false);
			
			for (int i = 0; i < 2; ++i)
			{
				m_pButton[i]->SetStart(false);
			}

			m_bStart = false;
		}	
	}

	if (m_pPanel->GetResetCheck() == true)
	{
		g_bCursorShow = false;
		CCameraMgr::GetInstance()->Set_CurCamera(CCameraMgr::CAMERALIST::CAMERA_STATIC);
	}


	OutComUpdate();
	ButtonCollision();

	m_pPanel->Update(fTimeDelta);
	m_pFlag->Update(fTimeDelta);
	m_pOutCom->Update(fTimeDelta);

	for (int i = 0; i < 2; ++i)
	{
		m_pButton[i]->Update(fTimeDelta);
	}

	return 0;
}

void CResultUI::Render(void)
{
	m_pPanel->Render();
	m_pFlag->Render();
	m_pOutCom->Render();

	for (int i = 0; i < 2; ++i)
	{
		m_pButton[i]->Render();
	}
	
}

void CResultUI::Release(void)
{
	m_pPanel->Release();
	m_pFlag->Release();
	m_pOutCom->Release();

	for (int i = 0; i < 2; ++i)
	{
		m_pButton[i]->Release();
	}
	
	CUI::Release();
}

HRESULT CResultUI::Ready_Component(void)
{
	return S_OK;
}

void CResultUI::OutComUpdate(void)
{
	if (!m_bStart)
		return;

	if ((m_pPanel->GetStart()) && (m_pPanel->GetAlpha() >= 0.5f))
	{
		//m_pOutCom->Set_TextureNumber(1); 
		//나중에 승패판정 0 : 승리, 1: 패배
		//m_pFlag->Set_TextureNumber();
		//승리팀의 깃발로 텍스쳐변경

		if (m_pOutCom->Get_TextureNumber() == 0) //승리
		{
			((CUI*)m_pOutCom)->SetSizeX(300);
			((CUI*)m_pOutCom)->SetSizeY(150);
		}
		else
		{
			((CUI*)m_pOutCom)->SetSizeX(700);
			((CUI*)m_pOutCom)->SetSizeY(400);
		}

		m_pOutCom->SetStart(true);	
		m_pFlag->SetStart(true);

		for (int i = 0; i < 2; ++i)
		{
			m_pButton[i]->SetStart(true);
		}
	}

}

void CResultUI::ButtonCollision(void)
{

	if (!m_pButton[0]->GetStart())
		return;

	POINT		ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	for (int i = 0; i < 2; ++i)
	{
		RECT rcUI = { long(m_pButton[i]->GetfX() - m_pButton[i]->GetSizeX() * 0.5f)
			, long(m_pButton[i]->GetfY() - m_pButton[i]->GetSizeY() * 0.5f)
			, long(m_pButton[i]->GetfX() + m_pButton[i]->GetSizeX() * 0.5f)
			, long(m_pButton[i]->GetfY() + m_pButton[i]->GetSizeY() * 0.5f) };

		if (PtInRect(&rcUI, ptMouse)) //마우스커서와 버튼이 충돌했을 경우 (해당 플레이어의 타워일경우-> 클릭시이동)
		{
			
			((CUI*)m_pButton[i])->SetSizeX(110);
			((CUI*)m_pButton[i])->SetSizeY(110);
			

			if (m_pInput->Get_DIMouseState(CInput::DIM_LB))
			{
				if (m_pButton[i]->Get_TextureNumber() == 0) //TryAgain
				{

				}
				else //Exit
				{
					Ser_EscapeGame EscapeGameData;
					EscapeGameData.size = sizeof(Ser_EscapeGame);
					EscapeGameData.type = PLAYER_DISCONNECTED;
					EscapeGameData.id = g_myid;
					g_Client->sendPacket(sizeof(Ser_EscapeGame), PLAYER_DISCONNECTED, reinterpret_cast<BYTE*>(&EscapeGameData));
					Sleep(500);
					PostQuitMessage(0);
				}
			}
		}
		else
		{
			((CUI*)m_pButton[i])->SetSizeX(100);
			((CUI*)m_pButton[i])->SetSizeY(100);
		}
	}


}
