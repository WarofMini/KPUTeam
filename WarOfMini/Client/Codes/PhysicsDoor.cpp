#include "stdafx.h"
#include "Management.h"
#include "PhysicsDoor.h"
#include "Transform.h"
#include "MeshMgr.h"
#include "ShaderMgr.h"
#include "CameraMgr.h"
#include "Layer.h"
#include "Scene.h"
#include "DefaultObject.h"

CPhysicsDoor::CPhysicsDoor(ID3D11DeviceContext * pContext)
:CPhysicsObect(pContext)
{
	m_vSeparation = XMFLOAT3(0.0f, 0.0f, 0.0f);
}

CPhysicsDoor::~CPhysicsDoor(void)
{
}

CPhysicsDoor * CPhysicsDoor::Create(ID3D11DeviceContext * pContext)
{
	CPhysicsDoor* pObject = new CPhysicsDoor(pContext);

	if (FAILED(pObject->Initialize()))
		Safe_Release(pObject);

	return pObject;
}



HRESULT CPhysicsDoor::Initialize(void)
{
	m_uiObjNum = MESHNUM_DOOR2;

	if (FAILED(Ready_Component()))
		return E_FAIL;


	return S_OK;
}

_int CPhysicsDoor::Update(const _float & fTimeDelta)
{
	CGameObject::Update(fTimeDelta);

	PhysXUpdate(fTimeDelta);

	CManagement::GetInstance()->Add_RenderGroup(CRenderer::RENDER_ZSORT, this);

	return 0;
}

void CPhysicsDoor::Render(void)
{
	CPhysicsObect::Render();
}

void CPhysicsDoor::Release(void)
{
	CPhysicsObect::Release();
}

HRESULT CPhysicsDoor::Ready_Component(void)
{
	if (FAILED(CPhysicsObect::Ready_Component()))
		return E_FAIL;

	return S_OK;
}

void CPhysicsDoor::BuildObject(PxPhysics * pPxPhysics, PxScene * pPxScene, PxMaterial * pPxMaterial, XMFLOAT3 vScale, PxCooking * pCooking, const char * name)
{


	PxConvexMeshDesc meshDesc;

	PxVec3*			m_pVtxTex;
	PxU32*			m_pIndex;
	_uint			m_iCount;


	//Mesh vertex Count
	m_iCount = CMeshMgr::GetInstance()->Get_MeshVtxCnt(m_uiObjNum);

	m_pVtxTex = CMeshMgr::GetInstance()->Get_MeshPxVtx(m_uiObjNum);
	m_pIndex = CMeshMgr::GetInstance()->Get_MeshPxIndex(m_uiObjNum);


	meshDesc.points.count = m_iCount;
	meshDesc.points.stride = sizeof(PxVec3);
	meshDesc.points.data = &(*m_pVtxTex);


	//meshDesc.triangles.count = m_iCount / 3;
	//meshDesc.triangles.stride = sizeof(PxU32) * 3;
	//meshDesc.triangles.data = &(*m_pIndex);



	meshDesc.flags = PxConvexFlags(PxConvexFlag::eCOMPUTE_CONVEX);

	PxDefaultMemoryOutputStream stream;
	bool ok = pCooking->cookConvexMesh(meshDesc, stream);

	if (!ok)
		return;

	PxDefaultMemoryInputData rb(stream.getData(), stream.getSize());

	PxConvexMesh* ConvexMesh = pPxPhysics->createConvexMesh(rb);

	PxVec3 ScaleTemp = PxVec3(vScale.x, vScale.y, vScale.z);

	//Scale
	PxMeshScale PxScale;
	PxScale.scale = ScaleTemp;

	PxConvexMeshGeometry meshGeom(ConvexMesh, PxScale);


	PxTransform _PxTransform(0, 0, 0);
	
	m_pPxActor = PxCreateDynamic(*pPxPhysics, _PxTransform, meshGeom, *pPxMaterial, 1.0f);

	m_pPxActor->setName(name);

	pPxScene->addActor(*m_pPxActor);

	m_pTransform->m_vScale = XMFLOAT3(vScale.x, vScale.y, vScale.z);

}

void CPhysicsDoor::CreateChain(PxPhysics* pPxPhysics, PxScene* pPxScene)
{
	PxVec3 offset(m_vSeparation.x, m_vSeparation.y, m_vSeparation.z);
	PxTransform localTm(offset);

	PxTransform m_pTemp = m_pPxActor->getGlobalPose();

	PxD6Joint* m_pJoint = PxD6JointCreate(*pPxPhysics, NULL, m_pTemp, m_pPxActor, PxTransform(-offset));

	m_pJoint->setMotion(PxD6Axis::eSWING2, PxD6Motion::eLIMITED);
	m_pJoint->setSwingLimit(PxJointLimitCone(PxPi/2, PxPi/2, 0.001f));
	m_pJoint->setConstraintFlag(PxConstraintFlag::eVISUALIZATION, true);

	//m_pJoint->setLinearLimit(PxJointLinearLimit(PxPi/2, PxSpring(100, 100)));

}