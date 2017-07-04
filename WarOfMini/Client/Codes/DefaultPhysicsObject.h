#ifndef DefaultPhysicsObject_h__
#define DefaultPhysicsObject_h__

#include "PhysicsObect.h"


class CDefaultPhysicsObect 
	: public CPhysicsObect
{
protected:
	explicit CDefaultPhysicsObect(ID3D11DeviceContext* pContext);
	virtual ~CDefaultPhysicsObect(void);

public:
	static CDefaultPhysicsObect* Create(ID3D11DeviceContext* pContext);


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


#endif // PhysicsObject

