#include "stdafx.h"
#include "Camera.h"

CCamera::CCamera(ID3D11DeviceContext* pContext)
: CGameObject(pContext)
, m_pProj(nullptr)
, m_pView(nullptr)
, m_pPos(nullptr)
, m_pTarget(nullptr)
{
}

CCamera::~CCamera(void)
{
}

HRESULT CCamera::Initialize(_float fNear, _float fFar, XMFLOAT3 vPos, XMFLOAT3 vTarget)
{
	m_pProj = new XMFLOAT4X4;
	m_pView = new XMFLOAT4X4;
	m_pPos = new XMFLOAT3(XMFLOAT3(0.f, 0.f, 0.f));
	m_pTarget = new XMFLOAT3(XMFLOAT3(0.f, 0.f, 1.f));

	XMStoreFloat4x4(m_pProj, XMMatrixIdentity());
	XMStoreFloat4x4(m_pView, XMMatrixIdentity());

	Set_Proj(fNear, fFar);
	Set_View(vPos, vTarget);

	return S_OK;
}

void CCamera::Release(void)
{
	CGameObject::Release();

	Safe_Delete(m_pPos);
	Safe_Delete(m_pTarget);
	Safe_Delete(m_pProj);
	Safe_Delete(m_pView);
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
	XMMATRIX matProj = XMMatrixPerspectiveFovLH(   (_float)D3DXToRadian(45.f), _float(WINCX) / _float(WINCY), fNear, fFar);
	XMStoreFloat4x4(m_pProj, matProj);
}

void CCamera::Set_View(XMFLOAT3 vPos, XMFLOAT3 vTarget)
{
	(*m_pPos) = vPos;
	(*m_pTarget) = vTarget;

	XMMATRIX matView = XMMatrixLookAtLH(XMLoadFloat3(m_pPos), XMLoadFloat3(m_pTarget), XMVectorSet(0.f, 1.f, 0.f, 0.f));
	XMStoreFloat4x4(m_pView, matView);
}
