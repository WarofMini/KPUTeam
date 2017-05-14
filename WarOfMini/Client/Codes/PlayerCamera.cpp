#include "stdafx.h"
#include "PlayerCamera.h"
#include "Input.h"
#include "Transform.h"
#include "Management.h"
#include "Scene.h"
#include "Layer.h"

CPlayerCamera::CPlayerCamera(ID3D11DeviceContext* pContext, PxScene* pPxScene, const CTransform* pTargetTransform, _float fGap)
	: CCamera(pContext)
	, m_pTargetTransform(pTargetTransform)
	, m_fGap(fGap)
	, m_fAngleY(0.f)
{
	m_fEpsilonY = 15.f;
	m_pPxScene = pPxScene;

}

CPlayerCamera::~CPlayerCamera(void)
{

}

CPlayerCamera* CPlayerCamera::Create(ID3D11DeviceContext* pContext, PxScene* pPxScene, const CTransform* pTargetTransform, _float fGap, _float fNear, _float fFar, XMFLOAT3& vEye, XMFLOAT3& vAt)
{
	CPlayerCamera* pCamera = new CPlayerCamera(pContext, pPxScene, pTargetTransform, fGap);

	if (FAILED(pCamera->Initialize(fNear, fFar, vEye, vAt)))
		Safe_Release(pCamera);

	return pCamera;
}

_int CPlayerCamera::Update(const _float& fTimeDelta)
{
	if (m_pTargetTransform == NULL)
	{
		CScene* pScene = CManagement::GetInstance()->GetScene();//->(CComponent*)
		CLayer* pLayer = pScene->FindLayer(L"Layer_GameLogic");
		CComponent* pTransform = (CComponent*)pLayer->Get_Component(L"Player", L"Com_Transform");
		m_pTargetTransform = (CTransform*)pTransform;
	}

	PlayerState(); //Player위치에 따른 보정



	KeyState(fTimeDelta);

	CheckCollision();

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

	//오른쪽으로 카메라 이동
	XMVECTOR m_vRight = XMVector3Cross(vUp, vTargetDir);
	m_vRight *= 5.f;
	vAt += m_vRight;

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

	ptMouse = { (long)WINCX >> 1, (long)WINCY >> 1 };

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

void CPlayerCamera::CheckCollision(void)
{	
		XMFLOAT3 m_vDir;
		XMStoreFloat3(&m_vDir, XMLoadFloat3(m_pEye) - XMLoadFloat3(m_pAt));
		XMStoreFloat3(&m_vDir, XMVector3Normalize(XMLoadFloat3(&m_vDir)));


		PxVec3 OriginPos = PxVec3((*m_pAt).x, (*m_pAt).y, (*m_pAt).z);
		PxVec3 m_vPxDir = PxVec3(m_vDir.x, m_vDir.y, m_vDir.z);


		PxReal maxDistance = 9999.f;
		PxRaycastBuffer hit;
		PxQueryFilterData fd;
		fd.flags |= PxQueryFlag::eANY_HIT;


		_bool m_bstatus = false;

		if (m_pPxScene == NULL)
			return;

		m_bstatus = m_pPxScene->raycast(OriginPos, m_vPxDir, maxDistance, hit, PxHitFlags(PxHitFlag::eDEFAULT), fd);

		if (m_bstatus == true)
		{

			if ((hit.block.distance < m_fGap + 1.f))
			{	
				
				XMFLOAT3 m_vNewEye = XMFLOAT3(hit.block.position.x + hit.block.normal.x, 
										   hit.block.position.y + hit.block.normal.y, 
										   hit.block.position.z + hit.block.normal.z);
								
				XMStoreFloat3(m_pEye, XMLoadFloat3(&m_vNewEye));
			}
		}
	
}
