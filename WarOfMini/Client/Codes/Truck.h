#ifndef Truck_h__
#define Truck_h__

#include "DynamicObject.h"

class CTruck
	: public CDynamicObject
{
protected:
	explicit CTruck(ID3D11DeviceContext* pContext);
	virtual ~CTruck(void);

public:
	static CTruck* Create(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext);

public:
	virtual HRESULT	Initialize(ID3D11Device* pGraphicDev);
	virtual INT Update(const FLOAT& fTimeDelta);
	virtual void Release(void);

protected:
	virtual HRESULT Ready_Component(ID3D11Device* pGraphicDev);
};

#endif // Truck_h__
