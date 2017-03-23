#include "stdafx.h"
#include "DynamicCamera.h"
#include "Input.h"

CDynamicCamera::CDynamicCamera(ID3D11DeviceContext* pContext)
: CCamera(pContext)
{
}

CDynamicCamera::~CDynamicCamera(void)
{
}

CDynamicCamera* CDynamicCamera::Create(ID3D11DeviceContext* pContext, _float fNear, _float fFar, XMFLOAT3& vEye, XMFLOAT3& vAt)
{
	CDynamicCamera* pCamera = new CDynamicCamera(pContext);

	if (FAILED(pCamera->Initialize(fNear, fFar, vEye, vAt)))
		Safe_Release(pCamera);

	return pCamera;
}

INT CDynamicCamera::Update(const _float& fTimeDelta)
{
	KeyState(fTimeDelta);
	MouseMove(fTimeDelta);
	FixMouse();


	MakeView();

	return 0;
}

void CDynamicCamera::Release(void)
{
	CCamera::Release();

	delete this;
}

void CDynamicCamera::KeyState(const _float& fTimeDelta)
{
	XMMATRIX	matWorld;
	XMVECTOR    vecTest = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);


	matWorld = XMMatrixInverse(&vecTest, XMLoadFloat4x4(m_pView));

	if (CInput::GetInstance()->Get_DIKeyState(DIK_W) & 0x80)
	{
		XMVECTOR vLook;

		memcpy(&vLook, &matWorld.m[2][0], sizeof(XMVECTOR));

		vLook = XMVector3Normalize(vLook);


		XMVECTOR	vLength = vLook * m_fCameraSpeed * fTimeDelta;

		XMVECTOR vPos, vAt;

		vPos = XMLoadFloat3(m_pEye);
		vAt = XMLoadFloat3(m_pAt);

		vPos += vLength;
		vAt += vLength;
		
		XMStoreFloat3(m_pEye, vPos);
		XMStoreFloat3(m_pAt, vAt);
	}


	if (CInput::GetInstance()->Get_DIKeyState(DIK_S) & 0x80)
	{
		XMVECTOR vLook;

		memcpy(&vLook, &matWorld.m[2][0], sizeof(XMVECTOR));

		vLook = XMVector3Normalize(vLook);

		XMVECTOR	vLength = vLook * m_fCameraSpeed * fTimeDelta;

		XMVECTOR vPos, vAt;

		vPos = XMLoadFloat3(m_pEye);
		vAt = XMLoadFloat3(m_pAt);

		vPos -= vLength;
		vAt  -= vLength;

		XMStoreFloat3(m_pEye, vPos);
		XMStoreFloat3(m_pAt, vAt);
	}

	if (CInput::GetInstance()->Get_DIKeyState(DIK_A) & 0x80)
	{
		XMVECTOR vLook;

		memcpy(&vLook, &matWorld.m[0][0], sizeof(XMVECTOR));

		vLook = XMVector3Normalize(vLook);

		XMVECTOR	vLength = vLook * m_fCameraSpeed * fTimeDelta;

		XMVECTOR vPos, vAt;

		vPos = XMLoadFloat3(m_pEye);
		vAt = XMLoadFloat3(m_pAt);

		vPos -= vLength;
		vAt -= vLength;

		XMStoreFloat3(m_pEye, vPos);
		XMStoreFloat3(m_pAt, vAt);
	}



	if (CInput::GetInstance()->Get_DIKeyState(DIK_D) & 0x80)
	{
		XMVECTOR vLook;

		memcpy(&vLook, &matWorld.m[0][0], sizeof(XMVECTOR));

		vLook = XMVector3Normalize(vLook);

		XMVECTOR	vLength = vLook * m_fCameraSpeed * fTimeDelta;

		XMVECTOR vPos, vAt;

		vPos = XMLoadFloat3(m_pEye);
		vAt = XMLoadFloat3(m_pAt);

		vPos += vLength;
		vAt  += vLength;

		XMStoreFloat3(m_pEye, vPos);
		XMStoreFloat3(m_pAt, vAt);
	}

	//Mouse Fix
	if (GetAsyncKeyState('Q') & 1)
	{
		if (m_bMouseFix == true)
			m_bMouseFix = false;
		else
			m_bMouseFix = true;
	}

}

void CDynamicCamera::FixMouse(void)
{
	POINT ptMouse;
	ZeroMemory(&ptMouse, sizeof(POINT));

	ptMouse = { WINCX >> 1, WINCY >> 1 };

	ClientToScreen(g_hWnd, &ptMouse);

	if (m_bMouseFix == true)
		SetCursorPos(ptMouse.x, ptMouse.y);
}

void CDynamicCamera::MouseMove(const _float& fTimeDelta)
{
	_long dwMouseMove = 0;
	
	XMMATRIX	matWorld;

	XMVECTOR    vecTest = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

	matWorld = XMMatrixInverse(&vecTest, XMLoadFloat4x4(m_pView));

	//R Btn
	if (CInput::GetInstance()->Get_DIMouseState(CInput::DIM_RB))
	{
		if (dwMouseMove = CInput::GetInstance()->Get_DIMouseMove(CInput::DIMS_Y))
		{
			XMVECTOR vRight;

			memcpy(&vRight, &matWorld.m[0][0], sizeof(XMVECTOR));

			XMVECTOR	vLook, vAt, vEye;

			vEye = XMLoadFloat3(m_pEye);
			vAt = XMLoadFloat3(m_pAt);

			vLook = vAt - vEye;

			XMMATRIX	matRot;

			matRot = XMMatrixRotationAxis(vRight, (_float)D3DXToRadian(dwMouseMove/ 10.0f));

			vLook = XMVector3TransformNormal(vLook, matRot);

			vAt = vEye + vLook;

			XMStoreFloat3(m_pAt, vAt);
		}


		if (dwMouseMove = CInput::GetInstance()->Get_DIMouseMove(CInput::DIMS_X))
		{
			XMVECTOR vAxis = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);


			XMVECTOR vLook, vAt, vEye;

			vEye = XMLoadFloat3(m_pEye);
			vAt = XMLoadFloat3(m_pAt);

			vLook = vAt - vEye;


			XMMATRIX	matRot;

			matRot = XMMatrixRotationAxis(vAxis, (_float)D3DXToRadian(dwMouseMove / 10.0f));

			vLook = XMVector3TransformNormal(vLook, matRot);

			vAt = vEye + vLook;

			XMStoreFloat3(m_pAt, vAt);
		}

	}

}