#ifndef Effect_Circle_h__
#define Effect_Circle_h__


#include "Effect.h"

class CTextures;
class CRcTex;

class CCircle
	: public CEffect
{
private:
	explicit CCircle(ID3D11DeviceContext* pContext);
	virtual ~CCircle(void);


private:
	_bool			m_bActiveCheck;

public:
	static CCircle* Create(ID3D11DeviceContext* pContext);

public:
	virtual HRESULT	Initialize(void);
	virtual INT		Update(const FLOAT& fTimeDelta);
	virtual void	Render(void);
	virtual void	Release(void);

protected:
	virtual HRESULT Ready_Component(void);


public:
	void			SetCheck(_bool bCheck) { m_bActiveCheck = bCheck; }
	_bool			GetCheck(void) { return m_bActiveCheck; }
};

#endif