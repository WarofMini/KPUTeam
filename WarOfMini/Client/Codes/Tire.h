#ifndef Tire_h__
#define Tire_h__

#include "DynamicObject.h"

class CTire
	: public CDynamicObject
{
protected:
	explicit CTire(ID3D11DeviceContext* pContext);
	virtual ~CTire(void);

public:
	static CTire* Create(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext);

public:
	virtual HRESULT	Initialize(ID3D11Device* pGraphicDev);
	virtual INT Update(const FLOAT& fTimeDelta);
	virtual void Release(void);

protected:
	virtual HRESULT Ready_Component(ID3D11Device* pGraphicDev);
};

#endif // Tire_h__
