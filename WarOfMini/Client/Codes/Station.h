#ifndef Station_h__
#define Station_h__

#include "GameObject.h"

class CCloth;

class CStation
	: public CGameObject
{
private:
	explicit CStation(ID3D11DeviceContext* pContext);
	virtual ~CStation(void);

public:
	static CStation* Create(ID3D11DeviceContext* pContext);

public:
	virtual HRESULT	Initialize(void);
	virtual _int Update(const _float& fTimeDelta);
	virtual void Render(void);
	virtual void Release(void);

private:
	virtual HRESULT Ready_Component(void);

private:
	CCloth*			m_pFlag;
	PxRigidStatic*	m_pPxActor;

public:
	void			BuildObject(PxPhysics* pPxPhysics, PxScene* pPxScene, PxMaterial *pPxMaterial, XMFLOAT3 vScale, PxCooking* pCooking, const char* name);
	void			SetPosition(XMFLOAT3 vPosition);
	void			SetRotate(XMFLOAT3 vRot);
	PxRigidStatic*  GetPxActor(void);
	void			SetFlag(CCloth* pFlag) { m_pFlag = pFlag; };
};

#endif // Station_h__
