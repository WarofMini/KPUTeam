

#include "stdafx.h"
#include "CameraMgr.h"
#include "Camera.h"
#include "DynamicCamera.h"


IMPLEMENT_SINGLETON(CCameraMgr)


CCameraMgr::CCameraMgr(void)
	: m_eCurCamera(CAMERA_DYNAMIC)
{
	m_vecCamera.reserve(CAMERA_END);

	for (size_t i = 0; i < CAMERA_END; ++i)
		m_vecCamera.push_back(nullptr);
}


CCameraMgr::~CCameraMgr(void)
{
}

void CCameraMgr::Ready_DynamicCamera(ID3D11DeviceContext* pContext, CAMERALIST eCameraName, _float fNear, _float fFar, _vec3& vEye, _vec3& vAt)
{
	if (m_vecCamera[eCameraName] == NULL)
		m_vecCamera[eCameraName] = CDynamicCamera::Create(pContext, fNear, fFar, vEye, vAt);
}

void CCameraMgr::Ready_StaticCamera(ID3D11DeviceContext* pContext, CAMERALIST eCameraName, const CTransform* pObjTrans, _float fDist, _float fHeightPivot
	, _float fNear, _float fFar, _vec3& vEye, _vec3& vAt)
{
}

void CCameraMgr::Update_CurCamera(const _float& fTimeDelta)
{
	m_vecCamera[m_eCurCamera]->Update(fTimeDelta);
}

void CCameraMgr::Release(void)
{
	for (size_t i = 0; i < CAMERA_END; ++i)
	{
		Safe_Release(m_vecCamera[i]);
	}

	m_vecCamera.clear();

	delete this;
}

const _matrix* CCameraMgr::Get_CurCameraProj(void)
{
	return m_vecCamera[m_eCurCamera]->Get_Proj();
}

const _matrix* CCameraMgr::Get_CurCameraView(void)
{
	return m_vecCamera[m_eCurCamera]->Get_View();
}

void CCameraMgr::Set_CurCamera(CAMERALIST eCameraName)
{
	m_eCurCamera = eCameraName;

	m_vecCamera[m_eCurCamera]->Update(0);
}


