#include "stdafx.h"
#include "MouseCol.h"
#include "ToolCamera.h"

CMouseCol::CMouseCol(void)
:m_fTime(0.0f)
{
}

CMouseCol::~CMouseCol(void)
{
	Release();
}

POINT CMouseCol::GetMousePos(void)
{
	POINT	pt;

	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);

	return pt;
}

CMouseCol * CMouseCol::Create(void)
{
	CMouseCol* pMouseCol = new CMouseCol;

	if(pMouseCol->Initialize())
		::Safe_Delete(pMouseCol);

	return pMouseCol;
}

HRESULT CMouseCol::Initialize(void)
{
	return S_OK;
}

void CMouseCol::Translation_ViewSpace(void)
{
	m_vCamEye = CToolCamera::GetInstance()->GetCamEye();

}

void CMouseCol::Translation_Local(const D3DXMATRIX * pWorld)
{
}

void CMouseCol::Release(void)
{
}
