#ifndef EffectHit_h__
#define EffectHit_h__

#include "Effect.h"

class CTextures;
class CRcTex;

class CHitScreen
	: public CEffect
{
private:
	explicit CHitScreen(ID3D11DeviceContext* pContext);
	virtual ~CHitScreen(void);


private:
	_float	m_fAlpha;
	_float	m_fAlphaSpeed;


public:
	static CHitScreen* Create(ID3D11DeviceContext* pContext);

public:
	virtual HRESULT	Initialize(void);
	virtual INT		Update(const FLOAT& fTimeDelta);
	virtual void	Render(void);
	virtual void	Release(void);

protected:
	virtual HRESULT Ready_Component(void);

public:
	void	SetAlphaSpeed(_float fSpeed) { m_fAlphaSpeed = fSpeed; }
};

#endif