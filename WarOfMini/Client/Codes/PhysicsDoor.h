#ifndef PhysicsDoor_h__
#define PhysicsDoor_h__

#include "PhysicsObect.h"


class CPhysicsDoor
	: public CPhysicsObect
{
private:
	explicit CPhysicsDoor(ID3D11DeviceContext* pContext);
	virtual ~CPhysicsDoor(void);

public:
	static CPhysicsDoor* Create(ID3D11DeviceContext* pContext);

public:
	virtual HRESULT	Initialize(void);
	virtual _int Update(const _float& fTimeDelta);
	virtual void Render(void);
	virtual void Release(void);

protected:
	virtual HRESULT Ready_Component(void);

private:
	XMFLOAT3	m_vSeparation;

public:
	virtual void	BuildObject(PxPhysics* pPxPhysics, PxScene* pPxScene, PxMaterial *pPxMaterial, XMFLOAT3 vScale, PxCooking* pCooking, const char* name);
	void			CreateChain(PxPhysics * pPxPhysics, PxScene * pPxScene);
	void			SetSeparation(XMFLOAT3 vSeparation) { m_vSeparation = vSeparation;}
};

#endif //PhysicsDoor