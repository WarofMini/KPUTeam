#ifndef Effect_Bomb_h__
#define Effect_Bomb_h__

#include "Effect.h"

class CTextures;
class CTransform;
class CRcTex;

class CBomb
	: public CEffect
{
protected:
	explicit CBomb(ID3D11DeviceContext* pContext);
	virtual ~CBomb(void);

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