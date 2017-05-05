#include "stdafx.h"
#include "Tire.h"
#include "Management.h"
#include "Renderer.h"
#include "Transform.h"

CTire::CTire(ID3D11DeviceContext* pContext)
	: CDynamicObject(pContext)
{

}

CTire::~CTire(void)
{

}

CTire* CTire::Create(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext)
{
	CTire* pObject = new CTire(pContext);

	if (FAILED(pObject->Initialize(pGraphicDev)))
		Safe_Release(pObject);

	return pObject;
}

HRESULT CTire::Initialize(ID3D11Device* pGraphicDev)
{
	m_uiObjNum = MESHNUM_TIRE;

	if (FAILED(Ready_Component(pGraphicDev)))
		return E_FAIL;

	m_pTransform->m_vScale = XMFLOAT3(1.f, 1.f, 1.f);

	return S_OK;
}

INT CTire::Update(const FLOAT& fTimeDelta)
{
	CDynamicObject::Update(fTimeDelta);

	CManagement::GetInstance()->Add_RenderGroup(CRenderer::RENDER_ZSORT, this);

	return 0;
}

void CTire::Release(void)
{
	CDynamicObject::Release();
}

HRESULT CTire::Ready_Component(ID3D11Device* pGraphicDev)
{
	if (FAILED(CDynamicObject::Ready_Component()))
		return E_FAIL;

	return S_OK;
}

