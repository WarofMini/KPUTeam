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
,m_pLayer(NULL)
{
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

	CreateChain(pPxPhysics, pPxScene, m_pPxActor);

	pPxScene->addActor(*m_pPxActor);

	m_pTransform->m_vScale = XMFLOAT3(vScale.x, vScale.y, vScale.z);

}

void CPhysicsDoor::CreateChain(PxPhysics* pPxPhysics, PxScene* pPxScene, PxRigidActor* pActor)
{
	PxVec3 offset(80, 0, 0);
	PxTransform localTm(-offset);

	localTm.q *= PxQuat((_float)D3DXToRadian(90.f), PxVec3(1, 0, 0));
	localTm.q *= PxQuat((_float)D3DXToRadian(90.f), PxVec3(0, 1, 0));
	localTm.q *= PxQuat(0.0f, PxVec3(0, 0, 1));


	list<CGameObject*>* pObjList = m_pLayer->Find_ObjectList(L"StaticWall");

	XMFLOAT3 m_vTargetPos = ((CTransform*)(*pObjList->begin())->Get_Component(L"Com_Transform"))->m_vPos;

	PxD6Joint* m_pJoint = PxD6JointCreate(*pPxPhysics, NULL, PxTransform(PxVec3(m_vTargetPos.x - 2.f, m_vTargetPos.y - 22.f, m_vTargetPos.z - offset.x)), pActor, localTm);

	m_pJoint->setMotion(PxD6Axis::eSWING1, PxD6Motion::eLIMITED);

	m_pJoint->setSwingLimit(PxJointLimitCone(PxPi/4, PxPi/4, 10.05f));



	//1 배우0 : 관절이 부착된 배우, NULL을 사용하여 조인트를 월드 프레임의 특정 지점에 부착가능
	//2 localFrame 배우0을 기준으로 한 조인트의 위치와 방향
	//3 배우 1 : 관절이 부착된 배우, 
	//4 localFrame 배우1에 대한 조인트와 위치와 방향
	//PxRevoluteJoint* m_pJoint = PxRevoluteJointCreate(*pPxPhysics, ((CDefaultObj*)(*pObjList->begin()))->GetPxActor(), PxTransform(PxVec3(0.0f, 0.0f, 0.0f)), pActor, PxTransform(-offset));


	//0 : 하한 낮은각도
	//upperLimit : 한도의 상한
	//dist 그것이 활동하게되는 한계로부터의 거리 . 기본값은 0.1라디안 중 작은값
	//m_pJoint->setLimit(PxJointAngularLimitPair(PxPi / 4, PxPi / 4, 0.05f));
	//m_pJoint->setRevoluteJointFlag(PxRevoluteJointFlag::eDRIVE_FREESPIN, true);
}

void CPhysicsDoor::SetLayer(CLayer * pLayer)
{
	m_pLayer = pLayer;
}
