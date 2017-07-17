#include "stdafx.h"
#include "Station.h"
#include "Transform.h"
#include "MeshMgr.h"
#include "ShaderMgr.h"
#include "Management.h"
#include "GraphicDev.h"
#include "CameraMgr.h"
#include "Cloth.h"
#include "Layer.h"
#include "Scene.h"
#include "GageUI.h"

CStation::CStation(ID3D11DeviceContext* pContext)
: CGameObject(pContext)
, m_pFlag(NULL)
, m_pPxActor(NULL)
, m_pPlayer(NULL)
, m_fFlagDist(100.f)
, m_eFlagState(FLAG_EMPTY)
, m_pGage(NULL)
{
	m_uiObjNum = MESHNUM_TOWER;
}

CStation::~CStation(void)
{
}

CStation* CStation::Create(ID3D11DeviceContext* pContext)
{
	CStation* pObject = new CStation(pContext);

	if (FAILED(pObject->Initialize()))
		Safe_Release(pObject);

	return pObject;
}

HRESULT CStation::Initialize()
{
	if (FAILED(Ready_Component()))
		return E_FAIL;


	return S_OK;
}

_int CStation::Update(const _float& fTimeDelta)
{
	//거점에서 일정거리 이하로 객체가 있는지 확인
	CollisionObject();

	CGameObject::Update(fTimeDelta);

	CManagement::GetInstance()->Add_RenderGroup(CRenderer::RENDER_ZSORT, this);
	return 0;
}

void CStation::Render(void)
{
	m_pContext->IASetInputLayout(CShaderMgr::GetInstance()->Get_InputLayout(L"Shader_Default"));

	ID3D11Buffer* pBaseShaderCB = CGraphicDev::GetInstance()->GetBaseShaderCB();
	ID3D11SamplerState* pBaseSampler = CGraphicDev::GetInstance()->GetBaseSampler();

	BASESHADER_CB tBaseShaderCB;

	tBaseShaderCB.matWorld = XMMatrixTranspose(XMLoadFloat4x4(&m_pTransform->m_matWorld));
	tBaseShaderCB.matView = XMMatrixTranspose(XMLoadFloat4x4(CCameraMgr::GetInstance()->Get_CurCameraView()));
	tBaseShaderCB.matProj = XMMatrixTranspose(XMLoadFloat4x4(CCameraMgr::GetInstance()->Get_CurCameraProj()));

	m_pContext->UpdateSubresource(pBaseShaderCB, 0, NULL, &tBaseShaderCB, 0, 0);

	m_pContext->VSSetShader(CShaderMgr::GetInstance()->Get_VertexShader(L"Shader_Default"), NULL, 0);
	m_pContext->VSSetConstantBuffers(0, 1, &pBaseShaderCB);
	m_pContext->PSSetShader(CShaderMgr::GetInstance()->Get_PixelShader(L"Shader_Default"), NULL, 0);
	m_pContext->PSSetSamplers(0, 1, &pBaseSampler);


	CMeshMgr::GetInstance()->Render_MeshMgr(m_uiObjNum, m_iTextureNumber, TRUE);

}

void CStation::Release(void)
{
	if (m_pPxActor)
		m_pPxActor->release();

	CGameObject::Release();
	delete this;
}

HRESULT CStation::Ready_Component()
{
	CComponent* pComponent = NULL;

	// Transform
	pComponent = CTransform::Create();
	m_pTransform = dynamic_cast<CTransform*>(pComponent);
	if (pComponent == NULL) return E_FAIL;
	m_mapComponent.insert(MAPCOMPONENT::value_type(L"Com_Transform", pComponent));

	return S_OK;
}

void CStation::BuildObject(PxPhysics * pPxPhysics, PxScene * pPxScene, PxMaterial * pPxMaterial, XMFLOAT3 vScale, PxCooking * pCooking, const char * name)
{
	PxTriangleMeshDesc meshDesc;


	PxVec3*			m_pVtxTex;
	PxU32*			m_pIndex;

	_uint			m_iCount;


	//Mesh vertex Count
	m_iCount = CMeshMgr::GetInstance()->Get_MeshVtxCnt(m_uiObjNum);

	m_pVtxTex = CMeshMgr::GetInstance()->Get_MeshPxVtx(m_uiObjNum);
	m_pIndex = CMeshMgr::GetInstance()->Get_MeshPxIndex(m_uiObjNum);


	meshDesc.points.count = m_iCount;
	meshDesc.triangles.count = m_iCount / 3;
	meshDesc.points.stride = sizeof(PxVec3);
	meshDesc.triangles.stride = sizeof(PxU32) * 3;
	meshDesc.points.data = &(*m_pVtxTex);
	meshDesc.triangles.data = &(*m_pIndex);
	meshDesc.flags = PxMeshFlags(0);

	PxDefaultMemoryOutputStream stream;
	bool ok = pCooking->cookTriangleMesh(meshDesc, stream);

	if (!ok)
		return;

	PxDefaultMemoryInputData rb(stream.getData(), stream.getSize());

	PxTriangleMesh* triangleMesh = pPxPhysics->createTriangleMesh(rb);


	PxVec3 ScaleTemp = PxVec3(vScale.x, vScale.y, vScale.z);

	//Scale
	PxMeshScale PxScale;
	PxScale.scale = ScaleTemp;

	PxTriangleMeshGeometry meshGeom(triangleMesh, PxScale);


	PxQuat q = PxQuat(PxIdentity);

	PxTransform _PxTransform = PxTransform(PxVec3(0.f, 0.f, 0.f), q);

	m_pPxActor = PxCreateStatic(*pPxPhysics, _PxTransform, meshGeom, *pPxMaterial);

	m_pPxActor->setName(name);

	pPxScene->addActor(*m_pPxActor);
}

void CStation::SetPosition(XMFLOAT3 vPosition)
{
	m_pTransform->m_vPos = vPosition;

	PxTransform _PxTransform = m_pPxActor->getGlobalPose();

	_PxTransform.p = PxVec3(vPosition.x, vPosition.y, vPosition.z);
	m_pPxActor->setGlobalPose(_PxTransform);
}

void CStation::SetRotate(XMFLOAT3 vRot)
{
	m_pTransform->m_vAngle = XMFLOAT3(vRot.x, vRot.y, vRot.z);

	PxTransform _PxTransform = m_pPxActor->getGlobalPose();


	_PxTransform.q *= PxQuat((_float)D3DXToRadian(vRot.x), PxVec3(1, 0, 0));
	_PxTransform.q *= PxQuat((_float)D3DXToRadian(vRot.z), PxVec3(0, 1, 0));
	_PxTransform.q *= PxQuat((_float)D3DXToRadian(vRot.y), PxVec3(0, 0, 1));

	m_pPxActor->setGlobalPose(_PxTransform);
}

PxRigidStatic * CStation::GetPxActor(void)
{
	return m_pPxActor;
}

void CStation::CollisionObject(void) //객체 충돌
{
	if (m_pPlayer == NULL)
	{
		CLayer* pLayer = CManagement::GetInstance()->GetScene()->FindLayer(L"Layer_GameLogic");

		list<CGameObject*>* m_pObject = pLayer->Find_ObjectList(L"Player");

		m_pPlayer = ((CPlayer*)(*m_pObject->begin()));

	}
	else
	{
		_float m_fDist = 0.0f;

		m_fDist = Length(m_pPlayer->GetTransformPosition(), m_pTransform->m_vPos);

		if (m_fDist <= m_fFlagDist) //거점 범위안에 있는경우
		{
			if (m_eFlagState == FLAG_TEAM1/*객체가 어느쪽 팀인지*/) //이미 점령한 곳인 경우
			{
				
			}
			else //점령 못한 거점일 경우
			{
				m_pGage->SetGageStart(true);

				if (m_pGage->GetGoalCheck() == true)
				{
					m_eFlagState = FLAG_TEAM1;
					m_pFlag->Set_TextureNumber(m_eFlagState); //깃발의 텍스쳐 팀껄로 변환
					m_pGage->ResetValue();
				}
			}
		}
		else
		{
			m_pGage->ResetValue();
		}
	}

}

_float CStation::Length(XMFLOAT3& xmf3Vector1, XMFLOAT3& xmf3Vector2)
{
	XMFLOAT3 xmf3Result;

	XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector1) - XMLoadFloat3(&xmf3Vector2));

	XMStoreFloat3(&xmf3Result, XMVector3Length(XMLoadFloat3(&xmf3Result)));

	return (xmf3Result.x);
}
