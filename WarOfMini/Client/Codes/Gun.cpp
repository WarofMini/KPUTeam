#include "stdafx.h"
#include "Gun.h"
#include "Transform.h"

#include "Management.h"



CGun::CGun(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext)
: CEquipment(pContext)
, m_pGraphicDev(pGraphicDev)
{
}

CGun::~CGun(void)
{
}

CGun* CGun::Create(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext)
{
	CGun* pEquipment = new CGun(pGraphicDev, pContext);

	if (FAILED(pEquipment->Initialize()))
		Safe_Release(pEquipment);

	return pEquipment;
}

HRESULT CGun::Initialize(void)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_uiObjNum = MESHNUM_GUN;

	m_pTransform->m_vPos = XMFLOAT3(3.f, -17.f, 10.f);
	m_pTransform->m_vAngle = XMFLOAT3(-0.f, (float)D3DXToDegree(3.5), (float)D3DXToDegree(0.007));

	return S_OK;
}

INT CGun::Update(const FLOAT& fTimeDelta)
{
	CEquipment::Update(fTimeDelta);
	XMStoreFloat4x4(&m_pTransform->m_matWorld, XMLoadFloat4x4(&m_pTransform->m_matWorld) * XMLoadFloat4x4(&m_matParent));

	CManagement::GetInstance()->Add_RenderGroup(CRenderer::RENDER_ZSORT, this);

	return 0;
}

void CGun::Release(void)
{
	CEquipment::Release();


	delete this;
}

HRESULT CGun::Ready_Component(void)
{
	if (FAILED(CEquipment::Ready_Component()))
		return E_FAIL;

	return S_OK;
}

const XMFLOAT4X4* CGun::Get_WorldPointer(void)
{
	return &m_pTransform->m_matWorld;
}

