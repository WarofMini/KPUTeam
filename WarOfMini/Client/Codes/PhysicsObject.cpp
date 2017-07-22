#include "stdafx.h"
#include "PhysicsObect.h"
#include "Transform.h"
#include "MeshMgr.h"
#include "ShaderMgr.h"
#include "Management.h"
#include "GraphicDev.h"
#include "CameraMgr.h"

CPhysicsObect::CPhysicsObect(ID3D11DeviceContext* pContext)
: CGameObject(pContext)
, m_pPxActor(NULL)
{
}

CPhysicsObect::~CPhysicsObect(void)
{
}

HRESULT CPhysicsObect::Initialize()
{
	if (FAILED(Ready_Component()))
		return E_FAIL;


	return S_OK;
}

_int CPhysicsObect::Update(const _float& fTimeDelta)
{

	CGameObject::Update(fTimeDelta);

	PhysXUpdate(fTimeDelta);

	CManagement::GetInstance()->Add_RenderGroup(CRenderer::RENDER_ZSORT, this);


	return 0;
}


void CPhysicsObect::Render(void)
{
	m_pContext->IASetInputLayout(CShaderMgr::GetInstance()->Get_InputLayout(L"Shader_Object"));

	ID3D11Buffer* pBaseShaderCB = CGraphicDev::GetInstance()->GetBaseShaderCB();
	ID3D11Buffer* pDirShaderCB = CGraphicDev::GetInstance()->GetDirLightShaderCB();
	ID3D11Buffer* pMaterialCB = CGraphicDev::GetInstance()->GetMaterialShaderCB();


	ID3D11SamplerState* pBaseSampler = CGraphicDev::GetInstance()->GetBaseSampler();

	BASESHADER_CB tBaseShaderCB;

	tBaseShaderCB.matWorld = XMMatrixTranspose(XMLoadFloat4x4(&m_pTransform->m_matWorld));
	tBaseShaderCB.matView = XMMatrixTranspose(XMLoadFloat4x4(CCameraMgr::GetInstance()->Get_CurCameraView()));
	tBaseShaderCB.matProj = XMMatrixTranspose(XMLoadFloat4x4(CCameraMgr::GetInstance()->Get_CurCameraProj()));

	m_pContext->UpdateSubresource(pBaseShaderCB, 0, NULL, &tBaseShaderCB, 0, 0);

	DIRECTIONALIGHT_CB tDirCB;

	tDirCB.Ambient = g_tDirectionalLight.Ambient;
	tDirCB.Diffuse = g_tDirectionalLight.Diffuse;
	tDirCB.Specular = g_tDirectionalLight.Specular;
	tDirCB.Direction = XMVector4Normalize(XMLoadFloat3(&m_pTransform->m_vPos) - g_tDirectionalLight.Direction);

	m_pContext->UpdateSubresource(pDirShaderCB, 0, NULL, &tDirCB, 0, 0);

	MATERIAL_CB tMaterialCB;

	tMaterialCB.Ambient = XMVectorSet(0.8f, 0.8f, 0.8f, 1.0f);
	tMaterialCB.Diffuse = XMVectorSet(0.8f, 0.8f, 0.8f, 1.0f);
	tMaterialCB.Specular = XMVectorSet(0.8f, 0.8f, 0.8f, 10.0f);
	tMaterialCB.Eye = XMLoadFloat3(&CCameraMgr::GetInstance()->Get_CurCameraEye());

	m_pContext->UpdateSubresource(pMaterialCB, 0, NULL, &tMaterialCB, 0, 0);


	m_pContext->VSSetShader(CShaderMgr::GetInstance()->Get_VertexShader(L"Shader_Object"), NULL, 0);
	m_pContext->VSSetConstantBuffers(0, 1, &pBaseShaderCB);
	m_pContext->PSSetShader(CShaderMgr::GetInstance()->Get_PixelShader(L"Shader_Object"), NULL, 0);
	m_pContext->PSSetConstantBuffers(1, 1, &pDirShaderCB);
	m_pContext->PSSetConstantBuffers(2, 1, &pMaterialCB);

	
	m_pContext->PSSetSamplers(0, 1, &pBaseSampler);


	CMeshMgr::GetInstance()->Render_MeshMgr(m_uiObjNum, m_iTextureNumber, TRUE);

}


void CPhysicsObect::Release(void)
{
	if (m_pPxActor)
		m_pPxActor->release();

	CGameObject::Release();
	delete this;
}


HRESULT CPhysicsObect::Ready_Component()
{
	CComponent* pComponent = NULL;

	// Transform
	pComponent = CTransform::Create();
	m_pTransform = dynamic_cast<CTransform*>(pComponent);
	if (pComponent == NULL) return E_FAIL;
	m_mapComponent.insert(MAPCOMPONENT::value_type(L"Com_Transform", pComponent));

	return S_OK;
}

void CPhysicsObect::PhysXUpdate(const _float & fTimeDelta)
{
	XMMATRIX matScale = XMMatrixScaling((float)(m_pTransform->m_vScale.x), (float)(m_pTransform->m_vScale.y), (float)(m_pTransform->m_vScale.z));


	PxTransform pT = m_pPxActor->getGlobalPose();
	PxMat44 m = PxMat44(pT);
	PxVec3 RotatedOffset = m.rotate(PxVec3(0.0f, 0.0f, 0.0f));
	m.setPosition(PxVec3(m.getPosition() + RotatedOffset));

	XMStoreFloat4x4(&m_pTransform->m_matWorld, matScale * (XMMATRIX)m.front());
}


void CPhysicsObect::BuildObject(PxPhysics* pPxPhysics, PxScene* pPxScene, PxMaterial *pPxMaterial, XMFLOAT3 vScale, PxCooking* pCooking, const char* name)
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

void CPhysicsObect::SetPosition(XMFLOAT3 vPosition)
{
	PxTransform _PxTransform = m_pPxActor->getGlobalPose();

	_PxTransform.p = PxVec3(vPosition.x, vPosition.y, vPosition.z);


	m_pPxActor->setGlobalPose(_PxTransform);
}

void CPhysicsObect::SetRotate(XMFLOAT3 vRot)
{
	PxTransform _PxTransform = m_pPxActor->getGlobalPose();

	_PxTransform.q *= PxQuat(vRot.x, PxVec3(1, 0, 0));
	_PxTransform.q *= PxQuat(vRot.z, PxVec3(0, 1, 0));
	_PxTransform.q *= PxQuat(vRot.y, PxVec3(0, 0, 1));

	m_pPxActor->setGlobalPose(_PxTransform);

}
