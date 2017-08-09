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
#include "CameraMgr.h"

CResultUI::CResultUI(ID3D11DeviceContext * pContext)
: CUI(pContext)
, m_pPanel(NULL)
{
	m_pInput = CInput::GetInstance();
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


	return S_OK;
}

_int CResultUI::Update(const _float & fTimeDelta)
{
	if (GetAsyncKeyState('M') & 1) //결과창UI 실행
	{
		//마우스 커서 보이도록....
		g_bCursorShow = true;
		m_pPanel->SetStart(true);
		CCameraMgr::GetInstance()->Set_CurCamera(CCameraMgr::CAMERALIST::CAMERA_DYNAMIC);
	}

	m_pPanel->Update(fTimeDelta);

	return 0;
}

void CResultUI::Render(void)
{
	m_pPanel->Render();
}

void CResultUI::Release(void)
{
	m_pPanel->Release();
	CUI::Release();
}

HRESULT CResultUI::Ready_Component(void)
{
	return S_OK;
}