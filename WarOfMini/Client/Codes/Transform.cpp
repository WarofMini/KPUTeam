#include "stdafx.h"
#include "Transform.h"


CTransform::CTransform(void)
:m_vAngle(0.0f, 0.0f, 0.0f)
,m_vScale(1.f, 1.f, 1.f)
,m_vPos(0.f, 0.f, 0.f)
,m_vDir(0.f, 0.f, 1.f)
{
	D3DXMatrixIdentity(&m_matWorld);
}

CTransform::~CTransform(void)
{
}

_int CTransform::Update(const _float& fTime)
{
	_matrix matScale;
	_matrix matRotX, matRotY, matRotZ;
	_matrix matTrans;

	D3DXMatrixScaling(&matScale, m_vScale.x, m_vScale.y, m_vScale.z);


	D3DXMatrixRotationX(&matRotX, (_float)(D3DXToRadian(m_vAngle.x)));
	D3DXMatrixRotationY(&matRotY, (_float)(D3DXToRadian(m_vAngle.y)));
	D3DXMatrixRotationZ(&matRotZ, (_float)(D3DXToRadian(m_vAngle.z)));

	D3DXMatrixTranslation(&matTrans, m_vPos.x, m_vPos.y, m_vPos.z);


	m_matWorld = matScale * matRotX * matRotY * matRotZ * matTrans;

	return 0;
}

void CTransform::Release(void)
{
	CComponent::Release();

	delete this;
}

CTransform * CTransform::Create(void)
{
	CTransform* pTransform = new CTransform;

	return  pTransform;
}
