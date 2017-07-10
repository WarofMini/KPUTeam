#include "stdafx.h"
#include "Cloth.h"
#include "Transform.h"
#include "MeshMgr.h"
#include "ShaderMgr.h"
#include "Management.h"
#include "GraphicDev.h"
#include "CameraMgr.h"


CCloth::CCloth(ID3D11DeviceContext* pContext)
: CPhysicsObect(pContext)
, m_pCloth(NULL)
{
}

CCloth::~CCloth(void)
{
}

CCloth * CCloth::Create(ID3D11DeviceContext * pContext)
{
	CCloth* pObject = new CCloth(pContext);

	if (FAILED(pObject->Initialize()))
		Safe_Release(pObject);

	return pObject;
}




HRESULT CCloth::Initialize()
{
	if (FAILED(Ready_Component()))
		return E_FAIL;


	return S_OK;
}

_int CCloth::Update(const _float& fTimeDelta)
{

	CGameObject::Update(fTimeDelta);

	PhysXUpdate(fTimeDelta);

	CManagement::GetInstance()->Add_RenderGroup(CRenderer::RENDER_ZSORT, this);


	return 0;
}


void CCloth::Render(void)
{
	CPhysicsObect::Render();
}


void CCloth::Release(void)
{
	CPhysicsObect::Release();
}


HRESULT CCloth::Ready_Component()
{
	if (FAILED(CPhysicsObect::Ready_Component()))
		return E_FAIL;

	return S_OK;
}


void CCloth::BuildObject(PxPhysics* pPxPhysics, PxScene* pPxScene, PxMaterial *pPxMaterial, XMFLOAT3 vScale, PxCooking* pCooking, const char* name)
{
	PxU32 resolution = 10;
	PxU32 numParticles = resolution*resolution;


}