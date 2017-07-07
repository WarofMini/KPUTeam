#include "stdafx.h"
#include "Effect.h"
#include "Management.h"
#include "ResourcesMgr.h"
#include "ShaderMgr.h"
#include "GraphicDev.h"
#include "RcTex.h"
#include "CameraMgr.h"
#include "Transform.h"


CEffect::CEffect(ID3D11DeviceContext * pContext)
:CGameObject(pContext)
, m_pBuffer(NULL)
, m_pTexture(NULL)
, m_pTransform(NULL)
, m_iFrame(0)
, m_iSizeX(0)
, m_iSizeY(0)
, m_fLifeTime(1.0f)
, m_fRealTime(0.0f)
, m_fDelayTime(0.0f)
, m_bAllBillboardCheck(false)
, m_bYBillboardCheck(false)
{

}

CEffect::CEffect(const CEffect & rhs)
:CGameObject(rhs.m_pContext)
, m_pBuffer(rhs.m_pBuffer)
, m_pTexture(rhs.m_pTexture)
, m_pTransform(rhs.m_pTransform)
, m_iFrame(rhs.m_iFrame)
, m_iSizeX(rhs.m_iSizeX)
, m_iSizeY(rhs.m_iSizeY)
, m_fLifeTime(rhs.m_fLifeTime)
, m_fRealTime(rhs.m_fRealTime)
, m_fDelayTime(rhs.m_fDelayTime)
, m_bAllBillboardCheck(rhs.m_bAllBillboardCheck)
, m_bYBillboardCheck(rhs.m_bYBillboardCheck)
{

}

CEffect::~CEffect(void)
{
}

HRESULT CEffect::Initialize(void)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	return S_OK;
}

INT CEffect::Update(const FLOAT & fTimeDelta)
{
	return 0;
}

void CEffect::Render(void)
{	
}

void CEffect::Release(void)
{
	CGameObject::Release();
	delete this;

}

HRESULT CEffect::Ready_Component(void)
{
	return S_OK;
}

void CEffect::ComputeBillboard(void)
{
	if (m_bAllBillboardCheck)
	{
		XMFLOAT4X4 matBill;
		XMStoreFloat4x4(&matBill, XMMatrixIdentity());

		XMStoreFloat4x4(&matBill, XMLoadFloat4x4(CCameraMgr::GetInstance()->Get_CurCameraView()));

		ZeroMemory(&matBill.m[3][0], sizeof(XMFLOAT3));

		XMVECTOR    vecTest = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

		XMStoreFloat4x4(&matBill, XMMatrixInverse(&vecTest, XMLoadFloat4x4(&matBill)));

		XMStoreFloat4x4(&m_pTransform->m_matWorld, XMMatrixMultiply(XMLoadFloat4x4(&m_pTransform->m_matWorld), XMLoadFloat4x4(&matBill)));

		memcpy(&m_pTransform->m_matWorld.m[3][0], &m_pTransform->m_vPos, sizeof(XMFLOAT3));
	}

	if (m_bYBillboardCheck)
	{
		XMFLOAT4X4 matBill, matView;
		XMStoreFloat4x4(&matBill, XMMatrixIdentity());
		XMStoreFloat4x4(&matView, XMMatrixIdentity());

		XMStoreFloat4x4(&matView, XMLoadFloat4x4(CCameraMgr::GetInstance()->Get_CurCameraView()));


		matBill._11 = matView._11;
		matBill._13 = matView._13;
		matBill._31 = matView._31;
		matBill._33 = matView._33;

		XMVECTOR    vecTest = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

		XMStoreFloat4x4(&matBill, XMMatrixInverse(&vecTest, XMLoadFloat4x4(&matBill)));

		XMStoreFloat4x4(&m_pTransform->m_matWorld, XMMatrixMultiply(XMLoadFloat4x4(&m_pTransform->m_matWorld), XMLoadFloat4x4(&matBill)));

		memcpy(&m_pTransform->m_matWorld.m[3][0], &m_pTransform->m_vPos, sizeof(XMFLOAT3));
	}
}

void CEffect::SetPosition(XMFLOAT3 vPos)
{
	m_pTransform->m_vPos = vPos;
}

void CEffect::SetScale(XMFLOAT3 vScale)
{
	m_pTransform->m_vScale = vScale;
}

void CEffect::SetRotate(XMFLOAT3 vRotate)
{
	m_pTransform->m_vAngle = vRotate;
}

CEffect * CEffect::Clone_Effect(const CEffect & rhs)
{
	return new CEffect(rhs);
}
