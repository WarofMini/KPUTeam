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

public:
	void	ChangeWeapon(_uint uiObjNum);
	_uint	Fire(void);
	void	Reload(void);

private:
	virtual HRESULT Ready_Component(void);

private:
	ID3D11Device*	m_pGraphicDev;
	float			m_fScale;
	_uint			m_iAmmo;


};


#endif //Blade_h__
