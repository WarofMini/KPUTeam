#include "stdafx.h"
#include "PlayerCamera.h"
#include "Input.h"
#include "Transform.h"

CPlayerCamera::CPlayerCamera(ID3D11DeviceContext* pContext, const CTransform* pTargetTransform, _float fGap)
	: CCamera(pContext)
	, m_pTargetTransform(pTargetTransform)
	, m_fGap(fGap)
	, m_fAngleY(0.f)
{
	m_fEpsilonY = 15.f;

}

CPlayerCamera::~CPlayerCamera(void)
{

}

CPlayerCamera* CPlayerCamera::Create(ID3D11DeviceContext* pContext, const CTransform* pTargetTransform, _float fGap, _float fNear, _float fFar, XMFLOAT3& vEye, XMFLOAT3& vAt)
{
	CPlayerCamera* pCamera = new CPlayerCamera(pContext, pTargetTransform, fGap);

	if (FAILED(pCamera->Initialize(fNear, fFar, vEye, vAt)))
		Safe_Release(pCamera);

	return pCamera;
}

_int CPlayerCamera::Update(const _float& fTimeDelta)
{
	PlayerState(); //Player위치에 따른 보정

	KeyState(fTimeDelta);

	MakeView();
	FixMouse();

	return 0;
}

void CPlayerCamera::Release(void)
{
	CCamera::Release();

	delete this;
}

void CPlayerCamera::PlayerState(void)
{
	XMVECTOR vTargetPos = XMVectorSet(m_pTargetTransform->m_vPos.x, m_pTargetTransform->m_vPos.y, m_pTargetTransform->m_vPos.z, 0.0f);
	XMVECTOR vTargetDir = XMVectorSet(m_pTargetTransform->m_vDir.x, 0.f, m_pTargetTransform->m_vDir.z, 0.0f);
	XMVECTOR vEye, vAt, vUpPos, vLook, vUp;

	vUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	vUpPos = XMVectorSet(0.0f, m_fEpsilonY, 0.0f, 0.0f);//입실론?
	vAt = vTargetPos + vUpPos;							//바라볼위치

	XMMATRIX matWorld;
	matWorld = XMLoadFloat4x4(&m_pTargetTransform->m_matWorld);

	vLook = XMVector3Normalize(vTargetDir);				//바라볼방향
	vEye = (-vLook) * m_fGap;							//카메라위치
	//vEye += vAt;

	XMVECTOR vTargetRight = XMVectorSet(m_pTargetTransform->m_matWorld._11, m_pTargetTransform->m_matWorld._12, m_pTargetTransform->m_matWorld._13, 0.0f);
	XMVECTOR vRight = XMVector3Normalize(vTargetRight);

	XMMATRIX	matRot;
	matRot = XMMatrixRotationAxis(vRight, (_float)D3DXToRadian(m_fAngleY));

	//vEye -= vAt;
	vEye = XMVector3TransformNormal(vEye, matRot);
	vEye += vAt;
	
	XMStoreFloat3(m_pAt, vAt);
	XMStoreFloat3(m_pEye, vEye);
}

void CPlayerCamera::FixMouse(void)
{
	POINT ptMouse;
	ZeroMemory(&ptMouse, sizeof(POINT));

	ptMouse = { WINCX >> 1, WINCY >> 1 };

	ClientToScreen(g_hWnd, &ptMouse);
	SetCursorPos(ptMouse.x, ptMouse.y);
}

//키보드 입력 함수
void CPlayerCamera::KeyState(const _float& fTimeDelta)
{
	_long   dwMouseMove = 0;

	if (dwMouseMove = CInput::GetInstance()->Get_DIMouseMove(CInput::DIMS_Z))
	{
		if (dwMouseMove < 0)
		{
			if (m_fGap < 200)
				m_fGap += 1.f;  //줌아웃
		}
		else
		{
			if (m_fGap > 5)
				m_fGap -= 1.f; //줌인		
		}

		XMVECTOR vcZ, vEye, vAt;
		XMMATRIX matView;
		matView = XMLoadFloat4x4(m_pView);
		vEye = XMLoadFloat3(m_pEye);
		vAt = XMLoadFloat3(m_pAt);

		vcZ = XMVectorSet(matView._13, matView._23, matView._33, 0.0f);
		vEye = vAt - (vcZ * m_fGap);

		XMStoreFloat3(m_pEye, vEye);
		XMStoreFloat3(m_pAt, vAt);
	}

	dwMouseMove = 0;
	if (dwMouseMove = CInput::GetInstance()->Get_DIMouseMove(CInput::DIMS_Y))
	{
		m_fAngleY += dwMouseMove * fTimeDelta * 3.f;

		if (m_fAngleY <= -80.f)
			m_fAngleY = -80.f;
		if (m_fAngleY >= 80.f)
			m_fAngleY = 80.f;
	}
}
