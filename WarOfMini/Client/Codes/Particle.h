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
	XMFLOAT3	m_vIntP; //�ʱ� ��ġ
	XMFLOAT3	m_vIntV; //�ʱ� �ӵ�
	XMFLOAT3	m_vIntA; // �ʱ� �Ⱑ�ӵ�

	XMFLOAT3	m_vCrnP; // ���� ��ġ
	XMFLOAT3	m_vCrnV; // ���� �ӵ�
	XMFLOAT3	m_vCrnA; // ���� ���ӵ�
	_float		m_fElst; // ź��(Elastic) ���
	_float		m_fDamp; // ���� ����(Air Registance) ���
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