#include "stdafx.h"
#include "ToolCamera.h"
#include "TimeMgr.h"
#include "Input.h"
#include "SceneMgr.h"

IMPLEMENT_SINGLETON(CToolCamera)

CToolCamera::CToolCamera(void)
{
	D3DXMatrixIdentity(&m_matView);
	D3DXMatrixIdentity(&m_matProj);
	m_fCameraSpeed = 0.0f;

}


CToolCamera::~CToolCamera(void)
{

}

HRESULT CToolCamera::Initialize(void)
{
	m_vEye = D3DXVECTOR3(0.0f, 20.f, -200.0f);
	m_vAt = D3DXVECTOR3(0.f, 0.f, 0.f);
	m_vUp = D3DXVECTOR3(0.f, 1.f, 0.f);


	m_dwTime = GetTickCount();
	m_dwTime = GetTickCount();

	m_bMouseFix = true;
	
	m_fFovY = (float)D3DXToRadian(45.f);
	m_fAspect = float(WINCX) / float(WINCY);
	m_fNear = 1.f;
	m_fFar = 5000.f;
	MakeView();
	MakeProjection();

	m_fCameraDistance = 10.f;
	m_fCameraSpeed = 120.f;
	
	return S_OK;
}

int CToolCamera::Update(void)
{

	KeyState();
	MouseMove();
	FixMouse();

	MakeView();
	MakeProjection();

	return 0;
}

void CToolCamera::MakeView(void)
{
	D3DXMatrixLookAtLH(&m_matView, &m_vEye, &m_vAt, &m_vUp);
}

void CToolCamera::MakeProjection(void)
{
	D3DXMatrixPerspectiveFovLH(&m_matProj, m_fFovY, m_fAspect, m_fNear, m_fFar);
}

void CToolCamera::Release(void)
{

}

void CToolCamera::KeyState(void)
{
	float fTime = CTimeMgr::GetInstance()->GetTime();


	D3DXMATRIX			matWorld;
	D3DXMatrixInverse(&matWorld, NULL, &m_matView);

	if (CInput::GetInstance()->GetDIKeyState(DIK_W) & 0x80)
	{ 
		D3DXVECTOR3 vLook;
		memcpy(&vLook, &matWorld.m[2][0], sizeof(D3DXVECTOR3));

		D3DXVECTOR3	vLength = *D3DXVec3Normalize(&vLook, &vLook) * m_fCameraSpeed * fTime;

		m_vEye += vLength;
		m_vAt += vLength;

	}

	if (CInput::GetInstance()->GetDIKeyState(DIK_S) & 0x80)
	{
		D3DXVECTOR3		vLook;
		memcpy(&vLook, &matWorld.m[2][0], sizeof(D3DXVECTOR3));

		D3DXVECTOR3	vLength = *D3DXVec3Normalize(&vLook, &vLook) * m_fCameraSpeed * fTime;

		m_vEye -= vLength;
		m_vAt  -= vLength;
	}

	if (CInput::GetInstance()->GetDIKeyState(DIK_A) & 0x80)
	{
		D3DXVECTOR3	   vLook;
		memcpy(&vLook, &matWorld.m[0][0], sizeof(D3DXVECTOR3));

		D3DXVECTOR3		vLength = *D3DXVec3Normalize(&vLook, &vLook) * m_fCameraSpeed * fTime;

		m_vEye -= vLength;
		m_vAt -= vLength;
	}

	if (CInput::GetInstance()->GetDIKeyState(DIK_D) & 0x80)
	{
		D3DXVECTOR3			vLook;
		memcpy(&vLook, &matWorld.m[0][0], sizeof(D3DXVECTOR3));

		D3DXVECTOR3			vLength = *D3DXVec3Normalize(&vLook, &vLook) * m_fCameraSpeed * fTime;

		m_vEye += vLength;
		m_vAt += vLength;
	}
	//Mouse Fix
	if (GetAsyncKeyState('Q') & 1)
	{
		if (m_bMouseFix == true)
			m_bMouseFix = false;
		else
			m_bMouseFix = true;
	}
}

void CToolCamera::FixMouse(void)
{
	POINT ptMouse;
	ZeroMemory(&ptMouse, sizeof(POINT));

	ptMouse = { WINCX >> 1, WINCY >> 1 };

	ClientToScreen(g_hWnd, &ptMouse);

	if (m_bMouseFix == true)
		SetCursorPos(ptMouse.x, ptMouse.y);
}

void CToolCamera::MouseMove(void)
{
	//if (m_bMouseFix == false)
	//	return;

	long dwMouseMove = 0;
	float	fTime = CTimeMgr::GetInstance()->GetTime();

	D3DXMATRIX			matWorld;
	D3DXMatrixInverse(&matWorld, NULL, &m_matView);

	//R Btn
	if (CInput::GetInstance()->GetDIMouseState(CInput::DIM_RBUTTON))
	{
		if (dwMouseMove = CInput::GetInstance()->GetDIMouseMove(CInput::DIM_Y))
		{
			D3DXVECTOR3		vRight;
			memcpy(&vRight, &matWorld.m[0][0], sizeof(D3DXVECTOR3));
			D3DXVECTOR3			vLook = m_vAt - m_vEye;

			D3DXMATRIX			matRot;
			D3DXMatrixRotationAxis(&matRot, &vRight, (float)D3DXToRadian(dwMouseMove / 10.0f));

			D3DXVec3TransformNormal(&vLook, &vLook, &matRot);

			m_vAt = m_vEye + vLook;
		}

		if (dwMouseMove = CInput::GetInstance()->GetDIMouseMove(CInput::DIM_X))
		{
			D3DXVECTOR3			vAxis = D3DXVECTOR3(0.f, 1.f, 0.f);

			D3DXVECTOR3			vLook = m_vAt - m_vEye;

			D3DXMATRIX			matRot;

			D3DXMatrixRotationAxis(&matRot, &vAxis, (float)D3DXToRadian(dwMouseMove / 10.0f));

			D3DXVec3TransformNormal(&vLook, &vLook, &matRot);

			m_vAt = m_vEye + vLook;
		}

	}
}

D3DXVECTOR3 CToolCamera::GetCamEye(void)
{
	return  m_vEye;
}

D3DXMATRIX CToolCamera::GetView(void)
{
	return m_matView;
}

D3DXMATRIX CToolCamera::GetProj(void)
{
	return m_matProj;
}

bool CToolCamera::GetMouseFixCheck(void)
{
	return m_bMouseFix;
}

void CToolCamera::SetMouseFixCheck(bool bCheck)
{
	m_bMouseFix = bCheck;
}
