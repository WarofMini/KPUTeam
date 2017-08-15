#ifndef LightParticle_h__
#define LightParticle_h__

#include "Effect.h"


class CParticle;

class CLightParticle
	: public CEffect
{
private:
	explicit CLightParticle(ID3D11DeviceContext* pContext);
	virtual ~CLightParticle(void);

private:
	CParticle*			m_pParticle[100];
	_int				m_iParticleCount;
	_bool				m_bStart;
	_bool				m_bEnd;

public:
	static CLightParticle* Create(ID3D11DeviceContext* pContext);


public:
	virtual HRESULT	Initialize(void);
	virtual _int	Update(const _float& fTimeDelta);
	virtual void	Render(void);
	virtual void	Release(void);

protected:
	virtual HRESULT Ready_Component(void);

public:
	void	ResetParticle(void);

};


#endif


