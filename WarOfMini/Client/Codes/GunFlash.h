#ifndef Effect_GunFlash_h__
#define Effect_GunFlash_h__


#include "Effect.h"

class CTextures;
class CRcTex;

class CGunFlash
	: public CEffect
{
private:
	explicit CGunFlash(ID3D11DeviceContext* pContext);
	virtual ~CGunFlash(void);

private:
	_float		m_fAlpha;
	_float		m_fAlphaSpeed;

public:
	static CGunFlash* Create(ID3D11DeviceContext* pContext);

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