#include "stdafx.h"
#include "Object.h"
#include "Camera.h"
#include "Info.h"
#include "Device.h"


CObject::CObject()
	:m_pInfo(NULL), m_bZSort(false), m_pGrapicDevice(CDevice::GetInstance()), m_eReleaseType(Release_End)
{
}


CObject::~CObject()
{
	Release();
}

HRESULT CObject::Initialize(void)
{
	return S_OK;
}

int CObject::Update(void)
{
	map<wstring, CComponent*>::iterator iter = m_mapComponent.begin();
	map<wstring, CComponent*>::iterator iter_end = m_mapComponent.end();

	for (iter; iter != iter_end; ++iter)
	{
		iter->second->Update();
	}


	if (m_bZSort == true)
	{
		if (m_pInfo != NULL)
			Compute_ViewZ(&m_pInfo->m_vPos);
	}

	return 0;
}

CObject * CObject::Create(ReleaseType _eType)
{
	CObject* pObj = new CObject;

	if (FAILED(pObj->Initialize()))
	{
		::Safe_Delete(pObj);
	}

	pObj->m_eReleaseType = _eType;
	return pObj;
}

void CObject::Release(void)
{
	map<wstring, CComponent*>::iterator iter = m_mapComponent.begin();
	map<wstring, CComponent*>::iterator iter_end = m_mapComponent.end();

	for (iter; iter != iter_end; ++iter)
	{
		::Safe_Delete(iter->second);
	}
	m_mapComponent.clear();
}

void CObject::SetPos(D3DXVECTOR3 vPos)
{
	if (m_pInfo != NULL)
		m_pInfo->m_vPos = vPos;
}

void CObject::Compute_ViewZ(const D3DXVECTOR3 * pPos)
{
	D3DXMATRIX		matView;
	D3DXMatrixIdentity(&matView);
	//m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	matView = CCamera::GetInstance()->m_matView;

	D3DXMatrixInverse(&matView, NULL, &matView);

	D3DXVECTOR3		vCamPos;
	memcpy(&vCamPos, &matView.m[3][0], sizeof(D3DXVECTOR3));

	float		fDistance = D3DXVec3Length(&(vCamPos - *pPos));

	m_fViewZ = fDistance;
}

const float & CObject::GetViewZ(void) const
{
	return m_fViewZ;
}

