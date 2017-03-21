#ifndef StaticObject_h__
#define StaticObject_h__


#include "GameObject.h"

class CTransform;

class CStaticObject : public CGameObject
{
private:
	explicit CStaticObject(ID3D11DeviceContext* pContext);
	virtual ~CStaticObject(void);

public:
	static CStaticObject* Create(ID3D11DeviceContext* pContext);


private:
	_uint			m_uiObjNum;
	CTransform*		m_pTransform;


public:
	virtual HRESULT		Initialize(void);
	virtual	_int		Update(const _float& fTimeDelta);
	virtual void		Render(void);
	virtual void		Release(void);

private:
	virtual HRESULT Ready_Component(void);

};


#endif