#ifndef StaticGameObject_h__
#define StaticGameObject_h__

#include "GameObject.h"


class CTransform;

class CStaticObject abstract
	: public CGameObject
{
protected:
	explicit CStaticObject(ID3D11DeviceContext* pContext);
	virtual ~CStaticObject(void);

public:
	virtual HRESULT	Initialize(void);
	virtual _int Update(const _float& fTimeDelta);
	virtual void Render(void);
	virtual void Release(void);

protected:
	virtual HRESULT Ready_Component(void);

protected:
	CTransform*		m_pTransform;
	_uint			m_uiObjNum;
};


#endif