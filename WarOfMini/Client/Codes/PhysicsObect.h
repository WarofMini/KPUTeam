#ifndef PhysicsObject_h__
#define PhysicsObject_h__

#include "GameObject.h"

class CTransform;


class CPhysicsObect
	: public CGameObject
{
private:
	explicit CPhysicsObect(ID3D11DeviceContext* pContext);
	virtual ~CPhysicsObect(void);

public:
	static CPhysicsObect* Create(ID3D11DeviceContext* pContext);

public:
	virtual HRESULT	Initialize(void);
	virtual _int Update(const _float& fTimeDelta);
	virtual void Render(void);
	virtual void Release(void);

private:
	virtual HRESULT Ready_Component(void);

private:
	_uint			m_uiObjNum;
	_float			m_fRadius;
	CTransform*		m_pTransform;

public:
	void			SetObjNum(_uint uNum);
	_uint			GetObjNum(void);
	void			PhysXUpdate(const _float& fTimeDelta);

	//Physx SDK Member Variables =========================
private:
	PxRigidDynamic	*m_pPxActor;


public:
	void			BuildObject(PxPhysics* pPxPhysics, PxScene* pPxScene, PxMaterial *pPxMaterial, XMFLOAT3 vScale, PxCooking* pCooking, const char* name);
	void			SetPosition(XMFLOAT3 vPosition);
	void			SetRotate(XMFLOAT3 vRot);


};


#endif // PhysicsObject

