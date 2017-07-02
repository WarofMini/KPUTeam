#include "stdafx.h"
#include "UI.h"
#include "Management.h"
#include "ResourcesMgr.h"
#include "ShaderMgr.h"
#include "GraphicDev.h"
#include "RcTex.h"
#include "CameraMgr.h"
#include "Transform.h"

CUI::CUI(ID3D11DeviceContext * pContext)
: CGameObject(pContext)
, m_pBuffer(NULL)
, m_pTexture(NULL)
, m_pTransform(NULL)
, m_fX(0)
, m_fY(0)
, m_fSizeX(0)
, m_fSizeY(0)
, m_fMoveX(0.0f)
, m_fMoveY(0.0f)
{
	XMStoreFloat4x4(&m_pProj, XMMatrixIdentity());
	XMStoreFloat4x4(&m_pView, XMMatrixIdentity());
}

CUI::~CUI(void)
{
}

HRESULT CUI::Initialize(void)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	return S_OK;
}

INT CUI::Update(const FLOAT & fTimeDelta)
{
	return 0;
}

void CUI::Render(void)
{
}

void CUI::Release(void)
{
	CGameObject::Release();
	delete this;
}

HRESULT CUI::Ready_Component(void)
{
	return S_OK;
}

void CUI::ComputeFXFY(void)
{
	m_fX = (WINCX >> 1) + m_fMoveX;
	m_fY = (WINCY >> 1) + m_fMoveY;
}

void CUI::ComputeChangeWindowSize(void)
{
	if (WINCX != 1600)
	{
		m_fSizeX = (WINCX / 1600.f) * m_fOriginSizeX;
		m_fMoveX = (WINCX / 1600.f) * m_fOriginMoveX;
	}
	else
	{
		m_fSizeX = m_fOriginSizeX;
		m_fMoveX = m_fOriginMoveX;
	}

	if (WINCY != 900)
	{
		m_fSizeY = (WINCY / 900.f) * m_fOriginSizeY;
		m_fMoveY = (WINCY / 900.f) * m_fOriginMoveY;
	}
	else
	{
		m_fSizeY = m_fOriginSizeY;
		m_fMoveY = m_fOriginMoveY;
	}
}
