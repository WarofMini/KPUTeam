#include "stdafx.h"
#include "Transform.h"

CTransform::CTransform(void)
: m_vScale(1.f, 1.f, 1.f)
, m_vAngle(0.f, 0.f, 0.f)
, m_vPos(0.f, 0.f, 0.f)
, m_vDir(0.f, 0.f, 1.f)
{
	XMStoreFloat4x4(&m_matWorld, XMMatrixIdentity());
}

CTransform::~CTransform(void)
{
}

_int  CTransform::Update(const _float& fTime)
{
	if (m_vAngle.y > (_float)D3DXToRadian(360.f))	
		m_vAngle.y -= (_float)D3DXToRadian(360.f);

	if (m_vAngle.y < (_float)D3DXToRadian(0.f))
		m_vAngle.y += (_float)D3DXToRadian(360.f);

	XMMATRIX matScale = XMMatrixScaling(m_vScale.x, m_vScale.y, m_vScale.z);
	XMMATRIX matRotX = XMMatrixRotationX(m_vAngle.x);
	XMMATRIX matRotY = XMMatrixRotationY(m_vAngle.y);
	XMMATRIX matRotZ = XMMatrixRotationZ(m_vAngle.z);
	XMMATRIX matTrans = XMMatrixTranslation(m_vPos.x, m_vPos.y, m_vPos.z);

	XMStoreFloat4x4(&m_matWorld, matScale * matRotX * matRotY * matRotZ * matTrans);

	return 0;
}

void CTransform::Update_MatrixNotXRot(void)
{
	XMMATRIX matScale = XMMatrixScaling(m_vScale.x, m_vScale.y, m_vScale.z);
	XMMATRIX matRotY = XMMatrixRotationY(m_vAngle.y);
	XMMATRIX matRotZ = XMMatrixRotationZ(m_vAngle.z);
	XMMATRIX matTrans = XMMatrixTranslation(m_vPos.x, m_vPos.y, m_vPos.z);

	XMStoreFloat4x4(&m_matWorld, matScale * matRotY * matRotZ * matTrans);
}

CTransform* CTransform::Create(void)
{
	CTransform* pTransform = new CTransform();

	return pTransform;
}

void CTransform::Release(void)
{
	CComponent::Release();

	delete this;
}
