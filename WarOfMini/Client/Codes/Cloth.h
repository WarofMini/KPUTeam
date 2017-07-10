#ifndef Cloth_h__
#define Cloth_h__

#include "PhysicsObect.h"

class CCloth
	: public CPhysicsObect
{
private:
	explicit CCloth(ID3D11DeviceContext* pContext);
	virtual ~CCloth(void);

private:
	PxCloth*				 m_pCloth;
	PxVec3                   mWindDir;
	PxVec3                   mWindRange;
	PxReal                   mWindStrength;
	PxReal                   mTime;
public:
	static CCloth* Create(ID3D11DeviceContext* pContext);

public:
	virtual HRESULT	Initialize(void);
	virtual _int Update(const _float& fTimeDelta);
	virtual void Render(void);
	virtual void Release(void);

protected:
	virtual HRESULT Ready_Component(void);

public:
	virtual void	BuildObject(PxPhysics* pPxPhysics, PxScene* pPxScene, PxMaterial *pPxMaterial, XMFLOAT3 vScale, PxCooking* pCooking, const char* name);

	PxClothMeshDesc	CreateMeshGrid(PxVec3 dirU, PxVec3 dirV, PxU32 numU, PxU32 numV,
		vector<PxVec4>& vertices, vector<PxU32>& indices, vector<PxVec2>& texcoords);

	void setWind(const PxVec3 &dir, PxReal strength, const PxVec3& range);

	PxF32 WindRand(PxF32 a, PxF32 b);
};


#endif // Cloth

