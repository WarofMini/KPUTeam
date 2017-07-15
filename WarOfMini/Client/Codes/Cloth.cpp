#include "stdafx.h"
#include "Cloth.h"
#include "Transform.h"
#include "MeshMgr.h"
#include "ShaderMgr.h"
#include "Management.h"
#include "GraphicDev.h"
#include "CameraMgr.h"
#include "ResourcesMgr.h"


CCloth::CCloth(ID3D11DeviceContext* pContext)
: CPhysicsObect(pContext)
, m_pCloth(NULL)
, m_Time(0.0f)
, m_pBuffer(NULL)
, m_pTexture(NULL)
, m_pClothVtx(NULL)
, m_iVtxCount(0)
{
	
}

CCloth::~CCloth(void)
{
}

CCloth * CCloth::Create(ID3D11DeviceContext * pContext, wstring strName)
{
	CCloth* pObject = new CCloth(pContext);

	pObject->SetBufferName(strName);

	if (FAILED(pObject->Initialize()))
		Safe_Release(pObject);

	return pObject;
}




HRESULT CCloth::Initialize()
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_iVtxCount = (_int)(m_pBuffer->GetResX() * m_pBuffer->GetResZ());

	m_pClothVtx = new VTXTEX[m_iVtxCount];

	m_pBuffer->GetVtxInfo(m_pClothVtx);

	return S_OK;
}

_int CCloth::Update(const _float& fTimeDelta)
{
	CGameObject::Update(fTimeDelta);

	UpdateWind(fTimeDelta);

	ClothPhysXUpdate(fTimeDelta);


	PxClothParticleData* data = m_pCloth->lockParticleData();

	PxClothParticle* pa = data->particles;

	for (int i = 0; i < m_iVtxCount; ++i)
	{
		m_pClothVtx[i].vPos = XMFLOAT3(pa[i].pos.x, pa[i].pos.y, pa[i].pos.z);
	}
	
	m_pBuffer->SetVtxInfo(m_pClothVtx);

	 //update the cloth local frame
	//gPose = PxTransform(PxVec3(0), PxQuat(PxPi / 240, PxVec3(0, 1, 0))) * gPose;
	//m_pCloth->setTargetPose(gPose);


	CManagement::GetInstance()->Add_RenderGroup(CRenderer::RENDER_ZSORT, this);

	return 0;
}


void CCloth::Render(void)
{
	CGraphicDev::GetInstance()->SetCullEnable(FALSE);

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

	m_pTexture->Render(0, 0);
	m_pBuffer->Render();

	CGraphicDev::GetInstance()->SetCullEnable(TRUE);

}


void CCloth::Release(void)
{
	if (m_pCloth)
		m_pCloth->release();

	Safe_Delete_Array(m_pClothVtx);
	CPhysicsObect::Release();
}


HRESULT CCloth::Ready_Component()
{
	CComponent* pComponent = NULL;

	//Buffer
	pComponent = CResourcesMgr::GetInstance()->Clone_ResourceMgr(RESOURCE_STAGE, m_strBufferName.c_str());
	m_pBuffer = dynamic_cast<CFlagTex*>(pComponent);
	if (pComponent == NULL) return E_FAIL;
	m_mapComponent.insert(MAPCOMPONENT::value_type(L"Com_Buffer", pComponent));

	//Texture
	pComponent = CResourcesMgr::GetInstance()->Clone_ResourceMgr(RESOURCE_STAGE, L"Texture_LogoBack");
	m_pTexture = dynamic_cast<CTextures*>(pComponent);
	if (pComponent == NULL) return E_FAIL;
	m_mapComponent.insert(MAPCOMPONENT::value_type(L"Com_Texture", pComponent));


	// Transform
	pComponent = CTransform::Create();
	m_pTransform = dynamic_cast<CTransform*>(pComponent);
	if (pComponent == NULL) return E_FAIL;
	m_mapComponent.insert(MAPCOMPONENT::value_type(L"Com_Transform", pComponent));

	return S_OK;
}


void CCloth::BuildObject(PxPhysics* pPxPhysics, PxScene* pPxScene, PxMaterial *pPxMaterial, XMFLOAT3 vScale, PxCooking* pCooking, const char* name)
{
	PxQuat q = PxQuat(PxIdentity);

	PxTransform m_tPxPos = PxTransform(PxVec3(0.f, 0.f, 0.f), q);

	PxU32 resX = (PxU32)m_pBuffer->GetResX();
	PxU32 resY = (PxU32)m_pBuffer->GetResZ();

	PxReal sizeX = (PxReal)m_pBuffer->GetSizeX();
	PxReal sizeY = (PxReal)m_pBuffer->GetSizeZ();
	PxReal height = 5.f;

	vector<PxVec4> vertices;
	vector<PxU32> primitives;
	vector<PxVec2>     mUVs;
	
	PxClothMeshDesc meshDesc = CreateMeshGrid(PxVec3(sizeX, 0, 0), PxVec3(0, -sizeY, 0), resX, resY, vertices, primitives, mUVs);

	// attach two corners on the left
	for (PxU32 i = 0; i < meshDesc.points.count; i++)
	{
		PxReal u = mUVs[i].x, v = mUVs[i].y;
		bool constrain = ((u < 0.01) && (v < 0.01)) || ((u < 0.01) && (v > 0.99));
		vertices[i].w = constrain ? 0.0f : 1.0f;
	}

	// create cloth fabric
	PxClothFabric* clothFabric = PxClothFabricCreate(*pPxPhysics, meshDesc, PxVec3(0, -1, 0));
	PX_ASSERT(meshDesc.points.stride == sizeof(PxVec4));


	// create the cloth actor
	const PxClothParticle* particles = (const PxClothParticle*)meshDesc.points.data;
	m_pCloth = pPxPhysics->createCloth(m_tPxPos, *clothFabric, particles, PxClothFlags());
	PX_ASSERT(m_pCloth);

	// add this cloth into the scene
	pPxScene->addActor(*m_pCloth);

	// set solver settings
	m_pCloth->setSolverFrequency(240);
	m_pCloth->setDampingCoefficient(PxVec3(0.0f));

	m_pCloth->setStretchConfig(PxClothFabricPhaseType::eBENDING, PxClothStretchConfig(0.1f));
	m_pCloth->setTetherConfig(PxClothTetherConfig(1.0f, 1.0f));

	m_pTransform->m_vScale = XMFLOAT3(vScale.x, vScale.y, vScale.z);


}

PxClothMeshDesc CCloth::CreateMeshGrid(PxVec3 dirU, PxVec3 dirV, PxU32 numU, PxU32 numV, vector<PxVec4>& vertices, vector<PxU32>& indices, vector<PxVec2>& texcoords)
{
	int numVertices = numU * numV; //Vertex의 개수
	int numQuads = (numU - 1) * (numV - 1); //4각형의 개수

	vertices.resize(numVertices);
	indices.resize(numQuads * 4);
	texcoords.resize(numVertices);

	// fill in point data
	PxReal scaleU = 1 / PxReal(numU - 1);   //1/5
	PxReal scaleV = 1 / PxReal(numV - 1);  // 1/5


	PxVec4* posIt = &(*vertices.begin());
	PxVec2* texIt = &(*texcoords.begin());

	for (PxU32 i = 0; i < numV; i++)
	{
		PxReal texV = i * scaleV;
		PxVec3 posV = (texV - 0.5f) * dirV;
		for (PxU32 j = 0; j < numU; j++)
		{
			PxReal texU = j * scaleU;
			PxVec3 posU = (texU - 0.5f) * dirU;

			PxVec4 pos = PxVec4(posU + posV, 1.0f);
			PxVec2 tex = PxVec2(texU, 1.0f - texV);

			*posIt++ = pos;
			*texIt++ = tex;
		}
	}

	// fill in quad index data
	PxU32* idxIt = &(*indices.begin());
	for (PxU32 i = 0; i < numV - 1; i++)
	{
		for (PxU32 j = 0; j < numU - 1; j++)
		{
			PxU32 i0 = i * numU + j;
			*idxIt++ = i0;
			*idxIt++ = i0 + 1;
			*idxIt++ = i0 + numU + 1;
			*idxIt++ = i0 + numU;
		}
	}

	PxClothMeshDesc meshDesc;

	// convert vertex array to PxBoundedData (desc.points)
	meshDesc.points.data = &(*vertices.begin());
	meshDesc.points.count = static_cast<PxU32>(numVertices);
	meshDesc.points.stride = sizeof(PxVec4);

	meshDesc.invMasses.data = &(*vertices.begin()).w;
	meshDesc.invMasses.count = static_cast<PxU32>(numVertices);
	meshDesc.invMasses.stride = sizeof(PxVec4);

	// convert index array to PxBoundedData (desc.quads)
	meshDesc.quads.data = &(*indices.begin());
	meshDesc.quads.count = static_cast<PxU32>(numQuads);
	meshDesc.quads.stride = sizeof(PxU32) * 4; // <- stride per quad

	return meshDesc;


}

void CCloth::SetWind(const PxVec3 & dir, PxReal strength, const PxVec3 & range)
{
	m_WindStrength = strength;
	m_WindDir = dir;
	m_WindRange = range;
}

PxF32 CCloth::WindRand(PxF32 a, PxF32 b)
{
	return ((b - a)*((float)rand() / RAND_MAX)) + a;
}

void CCloth::UpdateWind(const _float& fTimeDelta)
{
	m_Time += fTimeDelta * WindRand(0.0f, 1.0f);

	float st = 1.0f + (float)sin(m_Time);

	float windStrength = WindRand(1.0f, st) * m_WindStrength;
	float windRangeStrength = WindRand(0.0f, 2.0f);

	PxVec3 offset(PxReal(WindRand(-1, 1)), PxReal(WindRand(-1, 1)), PxReal(WindRand(-1, 1)));
	float ct = 1.0f + (float)cos(m_Time + 0.1);
	offset *= ct;
	PxVec3 windAcceleration = windStrength * m_WindDir + windRangeStrength * m_WindRange.multiply(offset);
	m_pCloth->setExternalAcceleration(windAcceleration);
}

void CCloth::ClothPhysXUpdate(const _float & fTimeDelta)
{
	XMMATRIX matScale = XMMatrixScaling((float)(m_pTransform->m_vScale.x), (float)(m_pTransform->m_vScale.y), (float)(m_pTransform->m_vScale.z));

	PxTransform pT = m_pCloth->getGlobalPose();
	PxMat44 m = PxMat44(pT);
	PxVec3 RotatedOffset = m.rotate(PxVec3(0.0f, 0.0f, 0.0f));
	m.setPosition(PxVec3(m.getPosition() + RotatedOffset));

	XMStoreFloat4x4(&m_pTransform->m_matWorld, matScale * (XMMATRIX)m.front());
}

void CCloth::ClothSetPosition(XMFLOAT3 vPosition)
{
	PxTransform _PxTransform = m_pCloth->getGlobalPose();

	_PxTransform.p = PxVec3(vPosition.x, vPosition.y, vPosition.z);

	m_pCloth->setGlobalPose(_PxTransform);
}

void CCloth::ClothSetRotate(XMFLOAT3 vRot)
{
	PxTransform _PxTransform = m_pCloth->getGlobalPose();

	_PxTransform.q *= PxQuat(vRot.x, PxVec3(1, 0, 0));
	_PxTransform.q *= PxQuat(vRot.z, PxVec3(0, 1, 0));
	_PxTransform.q *= PxQuat(vRot.y, PxVec3(0, 0, 1));

	m_pCloth->setGlobalPose(_PxTransform);
}
