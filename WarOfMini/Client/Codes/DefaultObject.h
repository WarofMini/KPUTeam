#ifndef DefaultObj_h__
#define DefaultObj_h__

#include "GameObject.h"

class CTransform;

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
	virtual INT Update(const FLOAT& fTimeDelta);
	virtual void Render(void);
	virtual void Release(void);

private:
	virtual HRESULT Ready_Component(void);

private:
	UINT			m_uiObjNum;
	FLOAT			m_fRadius;
	CTransform*		m_pTransform;
};

#endif // DefaultObj_h__