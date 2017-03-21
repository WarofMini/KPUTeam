#include "stdafx.h"
#include "Camera.h"

CCamera::CCamera(ID3D11DeviceContext* pContext)
	: CGameObject(pContext)
	, m_fNear(0.0f)
	, m_fFar(0.0f)
	, m_bMouseFix(false)
{
	D3DXMatrixIdentity(&m_matView);
	D3DXMatrixIdentity(&m_matProj);
}

CCamera::~CCamera(void)
{
}

HRESULT CCamera::Initialize(_float fNear, _float fFar, _vec3 vEye, _vec3 vAt)
{
	m_fNear = fNear;
	m_fFar = fFar;

	m_vEye = vEye;
	m_vAt = vAt;
	m_vUp = _vec3(0.0f, 1.0f, 0.0f);



	Set_Proj(fNear, fFar);
	Set_View(vEye, vAt);

	return S_OK;
}

void CCamera::Release(void)
{
	CGameObject::Release();
}

void CCamera::MakeView(void)
{
	D3DXMatrixLookAtLH(&m_matView, &m_vEye, &m_vAt, &m_vUp);
}

void CCamera::MakeProjection(void)
{
	D3DXMatrixPerspectiveFovLH(&m_matProj, (_float)D3DXToRadian(45.f), _float(WINCX) / _float(WINCY), m_fNear, m_fFar);
}

const _matrix* CCamera::Get_Proj(void)
{
	return &m_matProj;
}

const _matrix* CCamera::Get_View(void)
{
	return &m_matView;
}

void CCamera::Set_Proj(_float fNear, _float fFar)
{
	m_fNear = fNear;
	m_fFar = fFar;

	D3DXMatrixPerspectiveFovLH(&m_matProj, (_float)D3DXToRadian(45.f), _float(WINCX) / _float(WINCY), fNear, fFar);
}

void CCamera::Set_View(_vec3 vEye, _vec3 vAt)
{
	m_vEye = vEye;
	m_vAt = vAt;

	D3DXMatrixLookAtLH(&m_matView, &m_vEye, &m_vAt, &m_vUp);

}