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

};


#endif // Cloth

