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

public:
	void SetPos(XMFLOAT3 vPos);
	XMFLOAT3 GetPos(void);
	void	SetUse(bool bUse, int iID);

protected:
	virtual HRESULT Ready_Component(ID3D11Device* pGraphicDev);

private:
	DWORD			m_dwAniIdx;

	bool				m_bUseable;
	int					m_iUseID;
};

#endif // Truck_h__
