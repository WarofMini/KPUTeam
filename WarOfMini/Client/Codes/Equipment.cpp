#include "stdafx.h"
#include "Equipment.h"
#include "Transform.h"


CEquipment::CEquipment(ID3D11DeviceContext* pContext)
	: CStaticObject(pContext)
{
	XMStoreFloat4x4(&m_matParent, XMMatrixIdentity());
}

CEquipment::~CEquipment(void)
{
}

INT CEquipment::Update(const FLOAT& fTimeDelta)
{
	return CStaticObject::Update(fTimeDelta);
}

void CEquipment::Release(void)
{
	CGameObject::Release();
}

HRESULT CEquipment::Ready_Component(void)
{
	if (FAILED(CStaticObject::Ready_Component()))
		return E_FAIL;

	return S_OK;
}

void CEquipment::SetParent(const XMFLOAT4X4& matParent)
{
	m_matParent = matParent;
}
