#include "stdafx.h"
#include "CameraMgr.h"
#include "Camera.h"
#include "DynamicCamera.h"
#include "PlayerCamera.h"


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

void CCameraMgr::Ready_DynamicCamera(ID3D11DeviceContext* pContext, CAMERALIST eCameraName, _float fNear, _float fFar, XMFLOAT3& vPos, XMFLOAT3& vTarget)
{
	if (m_vecCamera[eCameraName] == nullptr)
		m_vecCamera[eCameraName] = CDynamicCamera::Create(pContext, fNear, fFar, vPos, vTarget);
}


void CCameraMgr::Ready_StaticCamera(ID3D11DeviceContext* pContext, CAMERALIST eCameraName, const CTransform* pTargetTransform, _float fGap, _float fNear, _float fFar, XMFLOAT3& vEye, XMFLOAT3& vAt)
{
	if (m_vecCamera[eCameraName] == nullptr)
		m_vecCamera[eCameraName] = CPlayerCamera::Create(pContext, pTargetTransform, fGap, fNear, fFar, vEye, vAt);
}

void CCameraMgr::Update_CurCamera(const FLOAT& fTimeDelta)
{
	if (GetAsyncKeyState('I') & 1)
	{
		if (m_eCurCamera == CAMERA_DYNAMIC)
			m_eCurCamera = CAMERA_STATIC;
		else
			m_eCurCamera = CAMERA_DYNAMIC;
	}



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

const XMFLOAT4X4* CCameraMgr::Get_CurCameraProj(void)
{
	return m_vecCamera[m_eCurCamera]->Get_Proj();
}

const XMFLOAT4X4* CCameraMgr::Get_CurCameraView(void)
{
	return m_vecCamera[m_eCurCamera]->Get_View();
}

const _bool * CCameraMgr::Get_CurCameraFixMouseCheck(void)
{
	return m_vecCamera[m_eCurCamera]->GetMouseFixCheck();
}

_float CCameraMgr::Get_CurrentCameraSpeed(void)
{
	return m_vecCamera[m_eCurCamera]->GetCameraSpeed();
}

XMFLOAT3 CCameraMgr::Get_CurCameraLookAt(void)
{
	return m_vecCamera[m_eCurCamera]->GetCameraLookAt();
}

XMFLOAT3 CCameraMgr::Get_CurCameraEye(void)
{
	return m_vecCamera[m_eCurCamera]->GetCameraEye();
}

void CCameraMgr::Set_CurCamera(CAMERALIST eCameraName)
{
	m_eCurCamera = eCameraName;

	m_vecCamera[m_eCurCamera]->Update(0);
}

CCameraMgr::CAMERALIST CCameraMgr::Get_CurCamera(void)
{
	return m_eCurCamera;
}
