#ifndef Effect_BoostTrail_h__
#define Effect_BoostTrail_h__

#include "Effect.h"

class CTextures;
class CTrailTex;
class CPlayer;

class CBoostTrail
	: public CEffect
{
private:
	explicit CBoostTrail(ID3D11DeviceContext* pContext);
	virtual ~CBoostTrail(void);

private:
	_float			m_fAlpha;
	_float			m_fAlphaSpeed;
	CTrailTex*		m_pTrailBuffer;
	CPlayer*		m_pPlayer;
	VTXTEX*			m_pTrailVtx;

	XMFLOAT3		m_vStart;
	XMFLOAT3		m_vEnd;
	_bool			m_bJumpCheck;

public:
	static CBoostTrail* Create(ID3D11DeviceContext* pContext);

public:
	virtual HRESULT	Initialize(void);
	virtual INT		Update(const FLOAT& fTimeDelta);
	virtual void	Render(void);
	virtual void	Release(void);

protected:
	virtual HRESULT Ready_Component(void);

public:
	void		CheckPlayer(void);
	void		InitTrail(void);
};

#endif