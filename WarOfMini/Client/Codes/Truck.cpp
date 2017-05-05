#include "stdafx.h"
#include "Truck.h"
#include "Management.h"
#include "Renderer.h"
#include "Transform.h"

CTruck::CTruck(ID3D11DeviceContext* pContext)
	: CDynamicObject(pContext)
{

}

CTruck::~CTruck(void)
{

}

CTruck* CTruck::Create(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext)
{
	CTruck* pObject = new CTruck(pContext);

	if (FAILED(pObject->Initialize(pGraphicDev)))
		Safe_Release(pObject);

	return pObject;
}

HRESULT CTruck::Initialize(ID3D11Device* pGraphicDev)
{
	m_uiObjNum = MESHNUM_TRUCK;

	if (FAILED(Ready_Component(pGraphicDev)))
		return E_FAIL;

	m_pTransform->m_vScale = XMFLOAT3(1.f, 1.f, 1.f);

	return S_OK;
}

INT CTruck::Update(const FLOAT& fTimeDelta)
{
	CDynamicObject::Update(fTimeDelta);

	CManagement::GetInstance()->Add_RenderGroup(CRenderer::RENDER_ZSORT, this);

	return 0;
}

void CTruck::Release(void)
{
	CDynamicObject::Release();
}

HRESULT CTruck::Ready_Component(ID3D11Device* pGraphicDev)
{
	if (FAILED(CDynamicObject::Ready_Component()))
		return E_FAIL;

	return S_OK;
}
