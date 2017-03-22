#ifndef Gun_h__
#define Gun_h__

#include "Equipment.h"


class CGun
	: public CEquipment
{
private:
	explicit CGun(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext);
	virtual ~CGun(void);

public:
	static CGun* Create(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext);

public:
	const XMFLOAT4X4* Get_WorldPointer(void);


public:
	virtual HRESULT	Initialize(void);
	virtual INT Update(const FLOAT& fTimeDelta);
	virtual void Release(void);

private:
	virtual HRESULT Ready_Component(void);

private:
	ID3D11Device*	m_pGraphicDev;


};


#endif //Blade_h__
