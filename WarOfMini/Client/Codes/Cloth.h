#ifndef Cloth_h__
#define Cloth_h__

#include "PhysicsObect.h"


class CFlagTex;
class CTextures;

class CCloth
	: public CPhysicsObect
{
private:
	explicit CCloth(ID3D11DeviceContext* pContext);
	virtual ~CCloth(void);

private:
	PxCloth*				 m_pCloth;
	PxVec3                   m_WindDir;
	PxVec3                   m_WindRange;
	PxReal                   m_WindStrength;
	PxReal                   m_Time;
	CFlagTex*				 m_pBuffer;
	CTextures*				 m_pTexture;
	VTXTEX*					 m_pClothVtx;
	wstring					 m_strBufferName;
	_int					 m_iVtxCount;
	_float					 m_fWindCount;
	

public:
	static CCloth* Create(ID3D11DeviceContext* pContext, wstring strName);

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

	void SetWind(const PxVec3 &dir, PxReal strength, const PxVec3& range);

	PxF32 WindRand(PxF32 a, PxF32 b);

	void UpdateWind(const _float& fTimeDelta);

	void ClothPhysXUpdate(const _float & fTimeDelta);

	void ClothSetPosition(XMFLOAT3 vPosition);
	void ClothSetRotate(XMFLOAT3 vRot);

	void SetBufferName(wstring strName) { m_strBufferName = strName; }

};


#endif // Cloth

