#ifndef Tire_h__
#define Tire_h__

#include "DynamicObject.h"

class CTire
	: public CDynamicObject
{
public:
	enum { MOVE_FORWARD, MOVE_BACK, MOVE_END };

protected:
	explicit CTire(ID3D11DeviceContext* pContext);
	virtual ~CTire(void);

public:
	static CTire* Create(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext);

public:
	virtual HRESULT	Initialize(ID3D11Device* pGraphicDev);
	virtual INT Update(const FLOAT& fTimeDelta);
	virtual void Release(void);

public:
	void	SetPos(XMFLOAT3 vPos);
	void	SetUse(bool bUse, int iID);
protected:
	virtual HRESULT Ready_Component(ID3D11Device* pGraphicDev);

private:
	DWORD			m_dwAniIdx;
	FLOAT			m_fvelocity;
	BYTE			m_byMoveDir;

	bool				m_bUseable;
	int					m_iUseID;
};

#endif // Tire_h__
