#ifndef Particle_h__
#define Particle_h__

#include "Effect.h"

class CTextures;
class CRcTex;

class CParticle
	: public CEffect
{
private:
	explicit CParticle(ID3D11DeviceContext* pContext);
	virtual ~CParticle(void);


private:
	_float	m_fAlpha;
	_float	m_fAlphaSpeed;

private:
	XMFLOAT3	m_vIntP; //초기 위치
	XMFLOAT3	m_vIntV; //초기 속도
	XMFLOAT3	m_vIntA; // 초기 기가속도

	XMFLOAT3	m_vCrnP; // 현재 위치
	XMFLOAT3	m_vCrnV; // 현재 속도
	XMFLOAT3	m_vCrnA; // 현재 가속도
	_float		m_fElst; // 탄성(Elastic) 계수
	_float		m_fDamp; // 공기 저항(Air Registance) 계수
	_float		m_fParticleTime;

public:
	static CParticle* Create(ID3D11DeviceContext* pContext);

public:
	virtual HRESULT	Initialize(void);
	virtual INT		Update(const FLOAT& fTimeDelta);
	virtual void	Render(void);
	virtual void	Release(void);

protected:
	virtual HRESULT Ready_Component(void);

public:
	void	SetAlphaSpeed(_float fSpeed) { m_fAlphaSpeed = fSpeed; }
	void	SetAlpha(_float fAlpha) { m_fAlpha = fAlpha; }
	_float	GetAlpha(void) { return m_fAlpha; }

	void	InitParticle(void);

	void	SetCrnP(XMFLOAT3 vPos) { m_vCrnP = vPos; }
};

#endif