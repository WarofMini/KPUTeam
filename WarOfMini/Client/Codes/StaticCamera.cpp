#include "stdafx.h"
#include "StaticCamera.h"
#include "Input.h"


CStaticCamera::CStaticCamera(ID3D11DeviceContext* pContext)
: CCamera(pContext)
{

}

CStaticCamera::~CStaticCamera(void)
{

}

CStaticCamera* CStaticCamera::Create(ID3D11DeviceContext* pContext, _float fNear, _float fFar, XMFLOAT3& vEye, XMFLOAT3& vAt)
{
	CStaticCamera* pCamera = new CStaticCamera(pContext);

	if (FAILED(pCamera->Initialize(fNear, fFar, vEye, vAt)))
		Safe_Release(pCamera);

	return pCamera;
}

_int CStaticCamera::Update(const _float& fTimeDelta)
{
	return 0;
}

void CStaticCamera::Release(void)
{
	CCamera::Release();

	delete this;
}
