#include "stdafx.h"
#include "Tank.h"
#include "Truck.h"
#include "Tire.h"

CTank::CTank(ID3D11DeviceContext* pContext)
	: CGameObject(pContext)
	, m_pTruck(NULL)
	, m_pTire(NULL)
{

}

CTank::~CTank(void)
{

}

CTank* CTank::Create(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext)
{
	CTank* pObject = new CTank(pContext);

	if (FAILED(pObject->Initialize(pGraphicDev)))
		Safe_Release(pObject);

	pObject->m_pTruck = CTruck::Create(pGraphicDev, pContext);
	pObject->m_pTire = CTire::Create(pGraphicDev, pContext);

	return pObject;
}

HRESULT CTank::Initialize(ID3D11Device* pGraphicDev)
{
	CComponent* pComponent = NULL;

	return S_OK;
}

INT CTank::Update(const FLOAT& fTimeDelta)
{
	m_pTire->Update(fTimeDelta);
	m_pTruck->Update(fTimeDelta);
	return 0;
}

void CTank::Render(void)
{
	m_pTire->Render();
	m_pTruck->Render();
}

void CTank::Release(void)
{
	m_pTruck->Release();
	m_pTire->Release();
}
