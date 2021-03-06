#include "stdafx.h"
#include "Gun.h"
#include "Transform.h"
#include "Management.h"
#include "MeshMgr.h"

CGun::CGun(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext)
: CEquipment(pContext)
, m_pGraphicDev(pGraphicDev)
, m_fScale(0.4f)
, m_iAmmo(60)
, m_iOriginAmmo(m_iAmmo)
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

	m_pTransform->m_vPos = XMFLOAT3(0.252f, -17.72f, 10.575f);
	m_pTransform->m_vAngle = XMFLOAT3(86.41f, -9.f, -176.67f);
	m_pTransform->m_vScale = XMFLOAT3(m_fScale, m_fScale, m_fScale);

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

void CGun::ChangeWeapon(_uint uiObjNum)
{
	m_uiObjNum = uiObjNum;
	if (m_uiObjNum == MESHNUM_GUN)
	{
		m_pTransform->m_vPos = XMFLOAT3(0.252f, -17.72f, 10.575f);
		m_pTransform->m_vAngle = XMFLOAT3(86.41f, -9.f, -176.67f);
	}
	else if (m_uiObjNum == MESHNUM_SPECIALGUN)
	{
		m_pTransform->m_vPos = XMFLOAT3(-18.87f, -14.0603f, 9.62835f);
		m_pTransform->m_vAngle = XMFLOAT3(140.575f, -25.445f, -160.962f);
	}
}

_uint CGun::Fire(void)
{
	if (m_iAmmo == 0)
		return m_iAmmo;
	--m_iAmmo;
	cout << m_iAmmo << endl;

	return m_iAmmo;
}

void CGun::Reload(void)
{
	m_iAmmo = 60;
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

