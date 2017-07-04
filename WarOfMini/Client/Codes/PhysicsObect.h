#ifndef PhysicsObject_h__
#define PhysicsObject_h__

#include "GameObject.h"

class CTransform;


class CPhysicsObect abstract
	: public CGameObject
{
protected:
	explicit CPhysicsObect(ID3D11DeviceContext* pContext);
	virtual ~CPhysicsObect(void);

public:
	virtual HRESULT	Initialize(void);
	virtual _int Update(const _float& fTimeDelta);
	virtual void Render(void);
	virtual void Release(void);

protected:
	virtual HRESULT Ready_Component(void);

protected:
	_uint			m_uiObjNum;
	CTransform*		m_pTransform;
	//Physx SDK Member Variables =========================
	PxRigidDynamic	*m_pPxActor;

public:
	virtual void	BuildObject(PxPhysics* pPxPhysics, PxScene* pPxScene, PxMaterial *pPxMaterial, XMFLOAT3 vScale, PxCooking* pCooking, const char* name);

public:
	void			SetObjNum(_uint uNum);
	_uint			GetObjNum(void);
	void			PhysXUpdate(const _float& fTimeDelta);
	

public:
	void			SetPosition(XMFLOAT3 vPosition);
	void			SetRotate(XMFLOAT3 vRot);
};


#endif // PhysicsObject

