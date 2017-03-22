#ifndef Equipment_h__
#define Equipment_h__

#include "StaticObject.h"

class CTransform;

class CEquipment
	: public CStaticObject
{
protected:
	explicit CEquipment(ID3D11DeviceContext* pContext);
	virtual ~CEquipment(void);

public:
	virtual INT Update(const FLOAT& fTimeDelta);
	virtual void Release(void);

public:
	void SetParent(const XMFLOAT4X4& matParent);

protected:
	virtual HRESULT Ready_Component(void);

protected:
	XMFLOAT4X4	m_matParent;
};

#endif // Equipment_h__
