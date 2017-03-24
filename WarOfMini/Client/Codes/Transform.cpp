#include "stdafx.h"
#include "Transform.h"

CTransform::CTransform(void)
: m_vScale(1.f, 1.f, 1.f)
, m_vAngle(0.f, 0.f, 0.f)
, m_vPos(0.f, 0.f, 0.f)
, m_vDir(0.f, 0.f, 1.f)
{
	XMStoreFloat4x4(&m_matWorld, XMMatrixIdentity());
	XMStoreFloat4x4(&m_matNRotWorld, XMMatrixIdentity());
	XMStoreFloat4x4(&m_matNScaleWorld, XMMatrixIdentity());
}

CTransform::~CTransform(void)
{
}

_int  CTransform::Update(const _float& fTime)
{
	


	XMMATRIX matScale = XMMatrixScaling(m_vScale.x, m_vScale.y, m_vScale.z);
	
	XMMATRIX matRotX = XMMatrixRotationX((_float)D3DXToRadian(m_vAngle.x));
	XMMATRIX matRotY = XMMatrixRotationY((_float)D3DXToRadian(m_vAngle.y));
	XMMATRIX matRotZ = XMMatrixRotationZ((_float)D3DXToRadian(m_vAngle.z));

	XMMATRIX matTrans = XMMatrixTranslation(m_vPos.x, m_vPos.y, m_vPos.z);

	XMStoreFloat4x4(&m_matWorld, matScale * matRotX * matRotY * matRotZ * matTrans);
	XMStoreFloat4x4(&m_matNRotWorld, matScale * matTrans);
	XMStoreFloat4x4(&m_matNScaleWorld, matRotX * matRotY * matRotZ * matTrans);

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

void CTransform::Move_Position(const XMVECTOR* pDirection)
{
	XMVECTOR vPos;

	vPos = XMLoadFloat3(&m_vPos);

	vPos += (*pDirection);

	XMStoreFloat3(&m_vPos, vPos);
}

void CTransform::Move_TargetPos(const XMVECTOR* pTargetPos, const _float & fSpeed, const _float & fTimeDelta)
{
	XMVECTOR vDir, vPos;

	vPos = XMLoadFloat3(&m_vPos);

	vDir = (*pTargetPos) - vPos;

	vDir = XMVector3Normalize(vDir);

	vPos += vDir * fSpeed * fTimeDelta;

	XMStoreFloat3(&m_vPos, vPos);
}

void CTransform::Move_Forward(const _float & fSpeed, const _float & fTimeDelta)
{
	XMVECTOR vDir, vPos;

	memcpy(&vDir, &m_matWorld.m[2][0], sizeof(XMVECTOR));

	vPos = XMLoadFloat3(&m_vPos);

	vDir = XMVector3Normalize(vDir);

	XMVectorSetY(vDir, 0.0f);

	vPos += vDir * fSpeed * fTimeDelta;

	XMStoreFloat3(&m_vPos, vPos);
}

void CTransform::Move_Right(const XMVECTOR * pDirection, const _float & fSpeed, const _float & fTimeDelta)
{
	XMVECTOR vDir, vPos, vUp;

	vPos = XMLoadFloat3(&m_vPos);

	vUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	vDir = XMVector3Cross(vUp, (*pDirection));

	XMVectorSetY(vDir, 0.0f);

	vDir = XMVector3Normalize(vDir);

	vPos += vDir * fSpeed * fTimeDelta;

	XMStoreFloat3(&m_vPos, vPos);
}

void CTransform::Move_Left(const XMVECTOR * pDirection, const _float & fSpeed, const _float & fTimeDelta)
{
	XMVECTOR vDir, vPos, vUp;

	vPos = XMLoadFloat3(&m_vPos);

	vUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	vDir = XMVector3Cross((*pDirection), vUp);

	XMVectorSetY(vDir, 0.0f);

	vDir = XMVector3Normalize(vDir);

	vPos += vDir * fSpeed * fTimeDelta;

	XMStoreFloat3(&m_vPos, vPos);
}
