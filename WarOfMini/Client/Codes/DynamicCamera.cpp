#include "stdafx.h"
#include "DynamicCamera.h"
#include "Input.h"

CDynamicCamera::CDynamicCamera(ID3D11DeviceContext* pContext)
	: CCamera(pContext)
	, m_fSpeed(500.f)
{
}

CDynamicCamera::~CDynamicCamera(void)
{
}

CDynamicCamera* CDynamicCamera::Create(ID3D11DeviceContext* pContext, _float fNear, _float fFar, _vec3& vEye, _vec3& vAt)
{
	CDynamicCamera* pCamera = new CDynamicCamera(pContext);

	if (FAILED(pCamera->Initialize(fNear, fFar, vEye, vAt)))
		Safe_Release(pCamera);

	return pCamera;
}

_int CDynamicCamera::Update(const _float& fTimeDelta)
{
	KeyCheck(fTimeDelta);
	MouseMove(fTimeDelta);
	FixMouse();

	MakeView();
	MakeProjection();

	return 0;
}

void CDynamicCamera::Release(void)
{
	CCamera::Release();

	delete this;
}

void CDynamicCamera::KeyCheck(const _float & fTimeDelta)
{
	_matrix			matWorld;
	D3DXMatrixInverse(&matWorld, NULL, &m_matView);

	if (CInput::GetInstance()->GetDIKeyState(DIK_W) & 0x80)
	{
		_vec3 vLook;
		memcpy(&vLook, &matWorld.m[2][0], sizeof(_vec3));

		_vec3	vLength = *D3DXVec3Normalize(&vLook, &vLook) * m_fSpeed * fTimeDelta;


		m_vEye += vLength;
		m_vAt += vLength;
	}

	if (CInput::GetInstance()->GetDIKeyState(DIK_S) & 0x80)
	{
		_vec3		vLook;
		memcpy(&vLook, &matWorld.m[2][0], sizeof(_vec3));

		_vec3	vLength = *D3DXVec3Normalize(&vLook, &vLook) * m_fSpeed * fTimeDelta;

		m_vEye -= vLength;
		m_vAt -= vLength;
	}

	if (CInput::GetInstance()->GetDIKeyState(DIK_A) & 0x80)
	{
		D3DXVECTOR3	   vLook;
		memcpy(&vLook, &matWorld.m[0][0], sizeof(D3DXVECTOR3));

		D3DXVECTOR3		vLength = *D3DXVec3Normalize(&vLook, &vLook) * m_fSpeed * fTimeDelta;

		m_vEye -= vLength;
		m_vAt -= vLength;
	}


	if (CInput::GetInstance()->GetDIKeyState(DIK_D) & 0x80)
	{
		D3DXVECTOR3			vLook;
		memcpy(&vLook, &matWorld.m[0][0], sizeof(D3DXVECTOR3));

		D3DXVECTOR3			vLength = *D3DXVec3Normalize(&vLook, &vLook) * m_fSpeed * fTimeDelta;

		m_vEye += vLength;
		m_vAt += vLength;
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

void CDynamicCamera::MouseMove(const _float & fTimeDelta)
{
	long dwMouseMove = 0;

	_matrix			matWorld;
	D3DXMatrixInverse(&matWorld, NULL, &m_matView);


	//R Btn
	if (CInput::GetInstance()->GetDIMouseState(CInput::DIM_RB))
	{
		if (dwMouseMove = CInput::GetInstance()->GetDIMouseMove(CInput::DIMS_Y))
		{
			_vec3		vRight;
			memcpy(&vRight, &matWorld.m[0][0], sizeof(_vec3));
			_vec3			vLook = m_vAt - m_vEye;

			_matrix			matRot;
			D3DXMatrixRotationAxis(&matRot, &vRight, (_float)D3DXToRadian(dwMouseMove / 10.0f));

			D3DXVec3TransformNormal(&vLook, &vLook, &matRot);

			m_vAt = m_vEye + vLook;
		}


		if (dwMouseMove = CInput::GetInstance()->GetDIMouseMove(CInput::DIMS_X))
		{
			_vec3			vAxis = _vec3(0.f, 1.f, 0.f);

			_vec3			vLook = m_vAt - m_vEye;

			_matrix			matRot;

			D3DXMatrixRotationAxis(&matRot, &vAxis, (float)D3DXToRadian(dwMouseMove / 10.0f));

			D3DXVec3TransformNormal(&vLook, &vLook, &matRot);

			m_vAt = m_vEye + vLook;
		}
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

