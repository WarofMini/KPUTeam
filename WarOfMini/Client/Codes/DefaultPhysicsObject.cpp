#include "stdafx.h"
#include "DefaultPhysicsObject.h"
#include "Transform.h"
#include "MeshMgr.h"
#include "ShaderMgr.h"
#include "Management.h"
#include "GraphicDev.h"
#include "CameraMgr.h"


CDefaultPhysicsObect::CDefaultPhysicsObect(ID3D11DeviceContext* pContext)
: CPhysicsObect(pContext)
{
}

CDefaultPhysicsObect::~CDefaultPhysicsObect(void)
{
}

CDefaultPhysicsObect * CDefaultPhysicsObect::Create(ID3D11DeviceContext * pContext)
{
	CDefaultPhysicsObect* pObject = new CDefaultPhysicsObect(pContext);

	if (FAILED(pObject->Initialize()))
		Safe_Release(pObject);

	return pObject;
}




HRESULT CDefaultPhysicsObect::Initialize()
{
	if (FAILED(Ready_Component()))
		return E_FAIL;


	return S_OK;
}

_int CDefaultPhysicsObect::Update(const _float& fTimeDelta)
{

	CGameObject::Update(fTimeDelta);

	PhysXUpdate(fTimeDelta);

	CManagement::GetInstance()->Add_RenderGroup(CRenderer::RENDER_ZSORT, this);


	return 0;
}


void CDefaultPhysicsObect::Render(void)
{
	CPhysicsObect::Render();
}


void CDefaultPhysicsObect::Release(void)
{
	CPhysicsObect::Release();
}


HRESULT CDefaultPhysicsObect::Ready_Component()
{
	if (FAILED(CPhysicsObect::Ready_Component()))
		return E_FAIL;

	return S_OK;
}


void CDefaultPhysicsObect::BuildObject(PxPhysics* pPxPhysics, PxScene* pPxScene, PxMaterial *pPxMaterial, XMFLOAT3 vScale, PxCooking* pCooking, const char* name)
{
	//바운딩박스
	XMFLOAT3 vMin = *(CMeshMgr::GetInstance()->Get_MeshMin(m_uiObjNum));

	vMin.x *= vScale.x;	 vMin.y *= vScale.y;  vMin.z *= vScale.z;


	XMFLOAT3 vMax = *(CMeshMgr::GetInstance()->Get_MeshMax(m_uiObjNum));
	vMax.x *= vScale.x;	 vMax.y *= vScale.y;  vMax.z *= vScale.z;


	XMFLOAT3 _d3dxvExtents =
		XMFLOAT3((abs(vMin.x) + abs(vMax.x)) / 2, (abs(vMin.y) + abs(vMax.y)) / 2, (abs(vMin.z) + abs(vMax.z)) / 2);

	//이게 객체의 최종행렬
	PxTransform _PxTransform(0, 0, 0);

	PxBoxGeometry _PxBoxGeometry(_d3dxvExtents.x, _d3dxvExtents.y, _d3dxvExtents.z);
	m_pPxActor = PxCreateDynamic(*pPxPhysics, _PxTransform, _PxBoxGeometry, *pPxMaterial, 4000.0f);
	m_pPxActor->setName(name);

	pPxScene->addActor(*m_pPxActor);

	m_pTransform->m_vScale = XMFLOAT3(vScale.x, vScale.y, vScale.z);
}