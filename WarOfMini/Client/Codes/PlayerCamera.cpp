#include "stdafx.h"
#include "PlayerCamera.h"
#include "Input.h"
#include "Transform.h"

CPlayerCamera::CPlayerCamera(ID3D11DeviceContext* pContext, const CTransform* pTargetTransform, _float fGap)
	: CCamera(pContext)
	, m_pTargetTransform(pTargetTransform)
	, m_fGap(fGap)
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

	//FixMouse();

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
	XMVECTOR vEye, vAt, vUpPos, vLook, g_Look;
	XMMATRIX matWorld;

	g_Look = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	vUpPos = XMVectorSet(0.0f, m_fEpsilonY, 0.0f, 0.0f);

	vAt = vTargetPos + vUpPos;

	matWorld = XMLoadFloat4x4(&m_pTargetTransform->m_matWorld);

	//방향
	vLook = XMVector3TransformNormal(g_Look, matWorld);

	vEye = (-vLook) * m_fGap;

	vEye += vAt;

	XMStoreFloat3(m_pEye, vEye);
	XMStoreFloat3(m_pAt, vAt);
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

}
