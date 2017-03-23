#include "stdafx.h"
#include "Camera.h"

CCamera::CCamera(ID3D11DeviceContext* pContext)
: CGameObject(pContext)
, m_pProj(nullptr)
, m_pView(nullptr)
, m_pEye(nullptr)
, m_pAt(nullptr)
, m_pUp(nullptr)
, m_fNear(0.0f)
, m_fFar(0.0f)
, m_fCameraSpeed(50.0f)
, m_bMouseFix(false)
{
}

CCamera::~CCamera(void)
{
}

HRESULT CCamera::Initialize(_float fNear, _float fFar, XMFLOAT3 vEye, XMFLOAT3 vAt)
{
	m_pProj = new XMFLOAT4X4;
	m_pView = new XMFLOAT4X4;
	m_pEye = new XMFLOAT3(XMFLOAT3(0.f, 0.f, 0.f));
	m_pAt = new XMFLOAT3(XMFLOAT3(0.f, 0.f, 0.f));
	m_pUp = new XMFLOAT3(XMFLOAT3(0.f, 1.f, 0.f));

	m_fNear = fNear;
	m_fFar = fFar;

	XMStoreFloat4x4(m_pProj, XMMatrixIdentity());
	XMStoreFloat4x4(m_pView, XMMatrixIdentity());

	Set_Proj(fNear, fFar);
	Set_View(vEye, vAt);

	return S_OK;
}

void CCamera::Release(void)
{
	CGameObject::Release();

	Safe_Delete(m_pEye);
	Safe_Delete(m_pAt);
	Safe_Delete(m_pProj);
	Safe_Delete(m_pView);
	Safe_Delete(m_pUp);
}

const XMFLOAT4X4* CCamera::Get_Proj(void)
{
	return m_pProj;
}

const XMFLOAT4X4* CCamera::Get_View(void)
{
	return m_pView;
}

void CCamera::Set_Proj(_float fNear, _float fFar)
{
	m_fNear = fNear;
	m_fFar = fFar;

	XMMATRIX matProj = XMMatrixPerspectiveFovLH(   (_float)D3DXToRadian(45.f), _float(WINCX) / _float(WINCY), m_fNear, m_fFar);
	XMStoreFloat4x4(m_pProj, matProj);
}

void CCamera::Set_View(XMFLOAT3 vEye, XMFLOAT3 vAt)
{
	(*m_pEye) = vEye;
	(*m_pAt) = vAt;

	XMMATRIX matView = XMMatrixLookAtLH(XMLoadFloat3(m_pEye), XMLoadFloat3(m_pAt), XMLoadFloat3(m_pUp));
	XMStoreFloat4x4(m_pView, matView);
}


void CCamera::MakeView(void)
{
	XMMATRIX matView = XMMatrixLookAtLH(XMLoadFloat3(m_pEye), XMLoadFloat3(m_pAt), XMLoadFloat3(m_pUp));
	XMStoreFloat4x4(m_pView, matView);
}

void CCamera::MakeProjection(void)
{
	XMMATRIX matProj = XMMatrixPerspectiveFovLH((_float)D3DXToRadian(45.f), _float(WINCX) / _float(WINCY), m_fNear, m_fFar);
	XMStoreFloat4x4(m_pProj, matProj);
}

_bool* CCamera::GetMouseFixCheck(void)
{
	return &m_bMouseFix;
}

void CCamera::SetMouseFixCheck(_bool bCheck)
{
	m_bMouseFix = bCheck;
}
