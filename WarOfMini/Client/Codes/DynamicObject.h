#ifndef DynamicObject_h__
#define DynamicObject_h__

#include "GameObject.h"

class CAnimationInfo;


class CDynamicObject abstract
	: public CGameObject
{
protected:
	explicit CDynamicObject(ID3D11DeviceContext* pContext);
	virtual ~CDynamicObject(void);

public:
	virtual HRESULT	Initialize(void);
	virtual _int Update(const _float& fTimeDelta);
	virtual void Render(void);
	virtual void Release(void);

protected:
	virtual HRESULT Ready_Component(void);

protected:
	CAnimationInfo* m_pAnimInfo;
	MATNODE*		m_pMatBoneNode;
	_float			m_fSpeed;
	_float			m_fHeight;
	_ubyte			m_byColor;

public:
	void	SetSpeed(_float fSpeed) { m_fSpeed = fSpeed; }
	_float	GetSpeed(void) { return m_fSpeed; }
};

#endif // DynamicObject_h__
