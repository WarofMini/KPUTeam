#ifndef DefaultObj_h__
#define DefaultObj_h__

#include "GameObject.h"


class CTransform;
class CSphereMesh;

class CDefaultObj
	: public CGameObject
{
private:
	explicit CDefaultObj(ID3D11DeviceContext* pContext);
	virtual ~CDefaultObj(void);

public:
	static CDefaultObj* Create(ID3D11DeviceContext* pContext);

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
	CSphereMesh*	m_pSphereMesh;

public:
	void			SetObjNum(_uint uNum);
	_uint			GetObjNum(void);
	void			ComputeCollider(void);

	//Physx SDK Member Variables =========================
private:
	PxRigidStatic	*m_pPxActor;


public:
	void			BuildObject(PxPhysics* pPxPhysics, PxScene* pPxScene, PxMaterial *pPxMaterial, XMFLOAT3 vScale, PxCooking* pCooking,  const char* name);
	void			SetPosition(XMFLOAT3 vPosition);
	void			SetRotate(XMFLOAT3 vRot);
};

#endif // DefaultObj_h__
