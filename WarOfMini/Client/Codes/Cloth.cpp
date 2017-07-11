#include "stdafx.h"
#include "Cloth.h"
#include "Transform.h"
#include "MeshMgr.h"
#include "ShaderMgr.h"
#include "Management.h"
#include "GraphicDev.h"
#include "CameraMgr.h"

PxTransform gPose = PxTransform(PxVec3(20, 20, 20), PxQuat(0, PxVec3(0, 0, 0)));

CCloth::CCloth(ID3D11DeviceContext* pContext)
: CPhysicsObect(pContext)
, m_pCloth(NULL)
, mTime(0.0f)
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

	setWind(PxVec3(1.f, 0.1f, 0.f), 40.0f, PxVec3(0.0f, 10.0f, 10.0f));

	mTime += fTimeDelta * WindRand(0.0f, 1.0f);

	float st = 1.0f + (float)sin(mTime);

	float windStrength = WindRand(1.0f, st) * mWindStrength;
	float windRangeStrength = WindRand(0.0f, 2.0f);

	PxVec3 offset(PxReal(WindRand(-1, 1)), PxReal(WindRand(-1, 1)), PxReal(WindRand(-1, 1)));
	float ct = 1.0f + (float)cos(mTime + 0.1);
	offset *= ct;
	PxVec3 windAcceleration = windStrength * mWindDir + windRangeStrength * mWindRange.multiply(offset);
	m_pCloth->setExternalAcceleration(windAcceleration);


	PxClothParticleData* data = m_pCloth->lockParticleData();

	PxClothParticle* pa = data->particles;

	PxVec3 pos1 = pa[0].pos;
	PxVec3 pos2 = pa[1].pos;
	PxVec3 pos3 = pa[2].pos;
	PxVec3 pos4 = pa[3].pos;
	PxVec3 pos5 = pa[4].pos;
	PxVec3 pos6 = pa[5].pos;
	PxVec3 pos7 = pa[6].pos;
	PxVec3 pos8 = pa[7].pos;
	PxVec3 pos9 = pa[8].pos;

	 //update the cloth local frame
	//gPose = PxTransform(PxVec3(0), PxQuat(PxPi / 240, PxVec3(0, 1, 0))) * gPose;
	//m_pCloth->setTargetPose(gPose);

	//PhysXUpdate(fTimeDelta);

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
	PxQuat q = PxQuat(PxIdentity);

	PxTransform pos = PxTransform(PxVec3(20.f, 20.f, 20.f), q);

	PxU32 numFlagRow = 2;
	PxU32 resX = 6, resY = 6;
	PxReal sizeX = 5.f, sizeY = 2.f, height = 5.f;

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
	m_pCloth = pPxPhysics->createCloth(pos, *clothFabric, particles, PxClothFlags());
	PX_ASSERT(m_pCloth);

	// add this cloth into the scene
	pPxScene->addActor(*m_pCloth);


	// set solver settings
	m_pCloth->setSolverFrequency(120);
	m_pCloth->setDampingCoefficient(PxVec3(0.0f));

	m_pCloth->setStretchConfig(PxClothFabricPhaseType::eBENDING, PxClothStretchConfig(0.1f));
	m_pCloth->setTetherConfig(PxClothTetherConfig(1.0f, 1.0f));

}

PxClothMeshDesc CCloth::CreateMeshGrid(PxVec3 dirU, PxVec3 dirV, PxU32 numU, PxU32 numV, vector<PxVec4>& vertices, vector<PxU32>& indices, vector<PxVec2>& texcoords)
{
	int numVertices = numU * numV; //Vertex의 개수
	int numQuads = (numU - 1) * (numV - 1); //4각형의 개수

	vertices.resize(numVertices);
	indices.resize(numQuads * 4);
	texcoords.resize(numVertices);

	// fill in point data
	PxReal scaleU = 1 / PxReal(numU - 1);
	PxReal scaleV = 1 / PxReal(numV - 1);


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
			*posIt++ = PxVec4(posU + posV, 1.0f);
			*texIt++ = PxVec2(texU, 1.0f - texV);
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

void CCloth::setWind(const PxVec3 & dir, PxReal strength, const PxVec3 & range)
{
	mWindStrength = strength;
	mWindDir = dir;
	mWindRange = range;
}

PxF32 CCloth::WindRand(PxF32 a, PxF32 b)
{
	return ((b - a)*((float)rand() / RAND_MAX)) + a;
}
