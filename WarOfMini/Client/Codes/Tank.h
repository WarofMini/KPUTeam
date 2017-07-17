#ifndef Tank_h__
#define Tank_h__

#include "GameObject.h"

class CTank
	: public CGameObject
{
protected:
	explicit CTank(ID3D11DeviceContext* pContext);
	virtual ~CTank(void);

public:
	static CTank* Create(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext);

public:
	virtual HRESULT	Initialize(ID3D11Device* pGraphicDev);
	virtual INT Update(const FLOAT& fTimeDelta);
	virtual void Render(void);
	virtual void Release(void);

public:
	void SetPos(XMFLOAT3 vPos);
	XMFLOAT3 GetPos(void);
	void	SetUse(bool bUse, int iID);

private:
	CDynamicObject*		m_pTruck;
	CDynamicObject*		m_pTire;

public:
	bool				m_bUseable;
	int					m_iUseID;

};

#endif // Tank_h__
