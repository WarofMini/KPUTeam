#ifndef DefaultObj_h__
#define DefaultObj_h__

#include "GameObject.h"

class CSound;

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

	//Physx SDK Member Variables =========================
private:
	PxRigidStatic*  m_pPxActor;
	_bool			m_bFenceCheck;
	_bool			m_bSelectFence;

public:
	void			BuildObject(PxPhysics* pPxPhysics, PxScene* pPxScene, PxMaterial *pPxMaterial, XMFLOAT3 vScale, PxCooking* pCooking,  const char* name);
	void			SetPosition(XMFLOAT3 vPosition);
	void			SetRotate(XMFLOAT3 vRot);

	PxRigidStatic*  GetPxActor(void);

	void			SoundUpdate(void);

	void			SelectFence(_bool bCheck) { m_bSelectFence = bCheck; }
};

#endif // DefaultObj_h__
