#include "stdafx.h"
#include "ScoreUI.h"
#include "Management.h"
#include "ResourcesMgr.h"
#include "ShaderMgr.h"
#include "Scene.h"
#include "Layer.h"
#include "GraphicDev.h"
#include "CameraMgr.h"
#include "Player.h"
#include "Panel.h"
#include "CameraMgr.h"
#include "FlagUI.h"
#include "ScoreCount.h"

int iScoreA = 1;
int iScoreB = 1;

CScoreUI::CScoreUI(ID3D11DeviceContext * pContext)
: CUI(pContext)
, m_pPanel(NULL)
{
	ZeroMemory(m_pFlag, sizeof(CFlagUI*) * 2);
	ZeroMemory(m_pCount, sizeof(CScoreCount*) * 2);
}


CScoreUI::~CScoreUI(void)
{
}

CScoreUI* CScoreUI::Create(ID3D11DeviceContext * pContext)
{
	CScoreUI* pScore = new CScoreUI(pContext);

	if (FAILED(pScore->Initialize()))
		Safe_Release(pScore);

	return pScore;
}

HRESULT CScoreUI::Initialize(void)
{
	m_pPanel = CPanel::Create(m_pContext, L"Texture_ScoreBoard");
	((CUI*)m_pPanel)->SetSizeX(500);
	((CUI*)m_pPanel)->SetSizeY(600);
	((CUI*)m_pPanel)->SetMoveX(0.f);
	((CUI*)m_pPanel)->SetMoveY(-420.f);
	((CUI*)m_pPanel)->ComputeFXFY();
	m_pPanel->SetStart(true);

	//±ê¹ß m_pFlag[0] : ³»°¡¼ÓÇÑ ÆÀ ±ê¹ß
	m_pFlag[0] = CFlagUI::Create(m_pContext);
	((CUI*)m_pFlag[0])->SetSizeX(70);
	((CUI*)m_pFlag[0])->SetSizeY(50);
	((CUI*)m_pFlag[0])->SetMoveX(-140.f);
	((CUI*)m_pFlag[0])->SetMoveY(-403.0f);
	((CUI*)m_pFlag[0])->ComputeFXFY();
	m_pFlag[0]->Set_TextureNumber(1);

	//±ê¹ß m_pFlag[1] : ÀûÆÀÀÌ ¼ÓÇÑ ÆÀ ±ê¹ß
	m_pFlag[1] = CFlagUI::Create(m_pContext);
	((CUI*)m_pFlag[1])->SetSizeX(70);
	((CUI*)m_pFlag[1])->SetSizeY(50);
	((CUI*)m_pFlag[1])->SetMoveX(140.f);
	((CUI*)m_pFlag[1])->SetMoveY(-403.0f);
	((CUI*)m_pFlag[1])->ComputeFXFY();
	m_pFlag[1]->Set_TextureNumber(2);

	//ÇÃ·¹ÀÌ¾î ÆÀ ½ºÄÚ¾î
	m_pCount[0] = CScoreCount::Create(m_pContext);
	((CUI*)m_pCount[0])->SetSizeX(70);
	((CUI*)m_pCount[0])->SetSizeY(50);
	((CUI*)m_pCount[0])->SetMoveX(40.f);
	((CUI*)m_pCount[0])->SetMoveY(-405.0f);
	((CUI*)m_pCount[0])->ComputeFXFY();

	//»ó´ëÆÀ ½ºÄÚ¾î
	m_pCount[1] = CScoreCount::Create(m_pContext);
	((CUI*)m_pCount[1])->SetSizeX(70);
	((CUI*)m_pCount[1])->SetSizeY(50);
	((CUI*)m_pCount[1])->SetMoveX(-35.f);
	((CUI*)m_pCount[1])->SetMoveY(-405.0f);
	((CUI*)m_pCount[1])->ComputeFXFY();

	return S_OK;
}

_int CScoreUI::Update(const _float & fTimeDelta)
{
	FlagUpdate();

	m_pPanel->Update(fTimeDelta);

	for (int i = 0; i < 2; ++i)
	{
		m_pFlag[i]->Update(fTimeDelta);
		m_pCount[i]->Update(fTimeDelta);
	}

	return 0;
}

void CScoreUI::Render(void)
{
	m_pPanel->Render();

	for (int i = 0; i < 2; ++i)
	{
		m_pFlag[i]->Render();
		m_pCount[i]->Render();
	}
}

void CScoreUI::Release(void)
{
	m_pPanel->Release();

	for (int i = 0; i < 2; ++i)
	{
		m_pFlag[i]->Release();
		m_pCount[i]->Release();
	}


	CUI::Release();
}

HRESULT CScoreUI::Ready_Component(void)
{
	return S_OK;
}

void CScoreUI::FlagUpdate(void)
{
	if ((m_pPanel->GetStart()) && (m_pPanel->GetAlpha() >= 0.5f))
	{
		for (int i = 0; i < 2; ++i)
		{
			m_pFlag[i]->SetStart(true);
			m_pCount[i]->SetStart(true);
		}
	}
}

void CScoreUI::SetScore(Ser_CurStation_DATA strData)
{
	int iStaCntA = 0;
	int iStaCntB = 0;

	for (int i = 0; i < 5; ++i)
	{
		if (strData.station[i].flagState == 1)
			++iStaCntA;
		if (strData.station[i].flagState == 2)
			++iStaCntB;
	}

	m_pCount[1]->Set_TextureNumber(iStaCntA);
	m_pCount[0]->Set_TextureNumber(iStaCntB);

	iScoreA = iStaCntA;
	iScoreB = iStaCntB;
}
