#include "stdafx.h"
#include "DefaultObject.h"
#include "Transform.h"
#include "MeshMgr.h"
#include "ShaderMgr.h"
#include "Management.h"
#include "GraphicDev.h"
#include "CameraMgr.h"
#include "Sound.h"

CDefaultObj::CDefaultObj(ID3D11DeviceContext* pContext)
: CGameObject(pContext)
, m_pPxActor(NULL)
{
	m_uiObjNum = 0;
}

CDefaultObj::~CDefaultObj(void)
{
}

CDefaultObj* CDefaultObj::Create(ID3D11DeviceContext* pContext)
{
	CDefaultObj* pObject = new CDefaultObj(pContext);

	if (FAILED(pObject->Initialize()))
		Safe_Release(pObject);

	return pObject;
}

HRESULT CDefaultObj::Initialize()
{
	if (FAILED(Ready_Component()))
		return E_FAIL;


	return S_OK;
}

_int CDefaultObj::Update(const _float& fTimeDelta)
{
	SoundUpdate();


	if (m_uiObjNum == MESHNUM_FENCE && (g_GameState == GS_START))
	{
		PxTransform _PxTransform = m_pPxActor->getGlobalPose();

		if (_PxTransform.p.y <= -40.f)
		{
			m_bDead = true;
		}
		else
		{
			_PxTransform.p = PxVec3(_PxTransform.p.x, _PxTransform.p.y - (fTimeDelta * 5.f), _PxTransform.p.z);

			m_pPxActor->setGlobalPose(_PxTransform);

			m_pTransform->m_vPos = XMFLOAT3(_PxTransform.p.x, _PxTransform.p.y - (fTimeDelta * 5.f), _PxTransform.p.z);

		}		
	}


	CGameObject::Update(fTimeDelta);

	if (!m_bDead)
		CManagement::GetInstance()->Add_RenderGroup(CRenderer::RENDER_ZSORT, this);

	
	return m_bDead;
}

void CDefaultObj::Render(void)
{
	//IASetInputLayout : 입력 배치 설정을 사용하고자 하는 장치에 묶는 것
	m_pContext->IASetInputLayout(CShaderMgr::GetInstance()->Get_InputLayout(L"Shader_Object"));

	ID3D11Buffer* pBaseShaderCB = CGraphicDev::GetInstance()->GetBaseShaderCB();
	ID3D11Buffer* pLightPowerShaderCB = CGraphicDev::GetInstance()->GetLightPowerCB();
	
	
	ID3D11SamplerState* pBaseSampler = CGraphicDev::GetInstance()->GetBaseSampler();

	BASESHADER_CB tBaseShaderCB;

	tBaseShaderCB.matWorld = XMMatrixTranspose(XMLoadFloat4x4(&m_pTransform->m_matWorld));
	tBaseShaderCB.matView = XMMatrixTranspose(XMLoadFloat4x4(CCameraMgr::GetInstance()->Get_CurCameraView()));
	tBaseShaderCB.matProj = XMMatrixTranspose(XMLoadFloat4x4(CCameraMgr::GetInstance()->Get_CurCameraProj()));

	m_pContext->UpdateSubresource(pBaseShaderCB, 0, NULL, &tBaseShaderCB, 0, 0);

	LIGHTPOWER_CB tLightPowerCB;

	tLightPowerCB.m_fLightPower = g_fLightPower;

	/*DIRECTIONALIGHT_CB tDirCB;

	tDirCB.Ambient = g_tDirectionalLight.Ambient;
	tDirCB.Diffuse = g_tDirectionalLight.Diffuse;
	tDirCB.Specular = g_tDirectionalLight.Specular;
	tDirCB.Direction = XMVector4Normalize(XMLoadFloat3(&m_pTransform->m_vPos) - g_tDirectionalLight.Direction);*/

	m_pContext->UpdateSubresource(pLightPowerShaderCB, 0, NULL, &tLightPowerCB, 0, 0);


	m_pContext->VSSetShader(CShaderMgr::GetInstance()->Get_VertexShader(L"Shader_Object"), NULL, 0);
	m_pContext->VSSetConstantBuffers(0, 1, &pBaseShaderCB);
	
	m_pContext->PSSetShader(CShaderMgr::GetInstance()->Get_PixelShader(L"Shader_Object"), NULL, 0);
	m_pContext->PSSetConstantBuffers(1, 1, &pLightPowerShaderCB);

	m_pContext->PSSetSamplers(0, 1, &pBaseSampler);


	CMeshMgr::GetInstance()->Render_MeshMgr(m_uiObjNum, m_iTextureNumber, TRUE);

}

void CDefaultObj::Release(void)
{
	if(m_pPxActor)
		m_pPxActor->release();
	
	CGameObject::Release();
	delete this;
}

HRESULT CDefaultObj::Ready_Component()
{
	CComponent* pComponent = NULL;

	// Transform
	pComponent = CTransform::Create();
	m_pTransform = dynamic_cast<CTransform*>(pComponent);
	if (pComponent == NULL) return E_FAIL;
	m_mapComponent.insert(MAPCOMPONENT::value_type(L"Com_Transform", pComponent));


	//Sound
	pComponent = CSound::Create((CTransform*)pComponent);
	m_pSound = dynamic_cast<CSound*>(pComponent);
	if (pComponent == NULL) return E_FAIL;
	m_mapComponent.insert(MAPCOMPONENT::value_type(L"Com_Sound", pComponent));
	m_pSound->Set_Sound(L"Clock", L"Clock.wav");
	

	return S_OK;
}


void CDefaultObj::BuildObject(PxPhysics* pPxPhysics, PxScene* pPxScene, PxMaterial *pPxMaterial, XMFLOAT3 vScale, PxCooking* pCooking, const char* name)
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
	meshDesc.triangles.count = m_iCount/3;
	meshDesc.points.stride = sizeof(PxVec3);
	meshDesc.triangles.stride = sizeof(PxU32) * 3;
	meshDesc.points.data = &(*m_pVtxTex);
	meshDesc.triangles.data = &(*m_pIndex);
	meshDesc.flags = PxMeshFlags(0);

	PxDefaultMemoryOutputStream stream;
	bool ok = pCooking->cookTriangleMesh(meshDesc, stream);
	
	if (!ok)
		return ;

	PxDefaultMemoryInputData rb(stream.getData(), stream.getSize());

	PxTriangleMesh* triangleMesh = pPxPhysics->createTriangleMesh(rb);


	PxVec3 ScaleTemp = PxVec3(vScale.x, vScale.y, vScale.z);

	//Scale
	PxMeshScale PxScale;
	PxScale.scale = ScaleTemp;
	
	PxTriangleMeshGeometry meshGeom(triangleMesh, PxScale);

	PxTransform _PxTransform(0, 0, 0);
	m_pPxActor = PxCreateStatic(*pPxPhysics, _PxTransform, meshGeom, *pPxMaterial);

	m_pPxActor->setName(name);

	pPxScene->addActor(*m_pPxActor);

	/*
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
	m_pPxActor = PxCreateStatic(*pPxPhysics, _PxTransform, _PxBoxGeometry, *pPxMaterial);
	m_pPxActor->setName(name);
	pPxScene->addActor(*m_pPxActor);
	*/
}

void CDefaultObj::SetPosition(XMFLOAT3 vPosition)
{

	PxTransform _PxTransform = m_pPxActor->getGlobalPose();

	_PxTransform.p = PxVec3(vPosition.x, vPosition.y, vPosition.z);

	/*
	XMFLOAT3 vMin = *(CMeshMgr::GetInstance()->Get_MeshMin(m_uiObjNum));
	XMFLOAT3 vMax = *(CMeshMgr::GetInstance()->Get_MeshMax(m_uiObjNum));

	D3DXVECTOR3 _d3dxvExtents =
	D3DXVECTOR3((abs(vMin.x) + abs(vMax.x)) / 2, (abs(vMin.y) + abs(vMax.y)) / 2, (abs(vMin.z) + abs(vMax.z)) / 2);
	*/

	m_pPxActor->setGlobalPose(_PxTransform);
}

void CDefaultObj::SetRotate(XMFLOAT3 vRot)
{
	PxTransform _PxTransform = m_pPxActor->getGlobalPose();

	_PxTransform.q *= PxQuat(vRot.x, PxVec3(1, 0, 0));
	_PxTransform.q *= PxQuat(vRot.z, PxVec3(0, 1, 0));
	_PxTransform.q *= PxQuat(vRot.y, PxVec3(0, 0, 1));

	m_pPxActor->setGlobalPose(_PxTransform);

}

PxRigidStatic * CDefaultObj::GetPxActor(void)
{
	return m_pPxActor;
}

void CDefaultObj::SoundUpdate(void)
{
	if (m_uiObjNum == MESHNUM_CLOCK)
	{
		m_pSound->MyPlaySound(L"Clock");
		m_pSound->Check_Distance();
		m_pSound->Check_SoundEnd();
	}
}