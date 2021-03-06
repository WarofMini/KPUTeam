#ifndef Effect_Bomb_h__
#define Effect_Bomb_h__

#include "Effect.h"

class CTextures;
class CRcTex;

class CBomb
	: public CEffect
{
private:
	explicit CBomb(ID3D11DeviceContext* pContext);
	virtual ~CBomb(void);

private:
	_bool		m_bSoundStart;

public:
	static CBomb* Create(ID3D11DeviceContext* pContext);

public:
	virtual HRESULT	Initialize(void);
	virtual INT		Update(const FLOAT& fTimeDelta);
	virtual void	Render(void);
	virtual void	Release(void);

protected:
	virtual HRESULT Ready_Component(void);
};

#endif