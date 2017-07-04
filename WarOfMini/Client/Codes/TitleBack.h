#ifndef TitleBack_h__
#define TitleBack_h__

#include "GameObject.h"

class CTransform;
class CTextures;
class CRcTex;

class CTitleBack
	: public CGameObject
{

private:
	explicit CTitleBack(ID3D11DeviceContext* pContext);
	virtual ~CTitleBack(void);

public:
	static CTitleBack* Create(ID3D11DeviceContext* pContext);

public:
	virtual HRESULT	Initialize(void);
	virtual INT		Update(const FLOAT& fTimeDelta);
	virtual void	Render(void);
	virtual void	Release(void);

private:
	virtual HRESULT Ready_Component(void);

private:
	CTransform*		m_pTransform;
	CTextures*		m_pTexture;
	CRcTex*			m_pBuffer;
	float			m_fAlpha;
	float			m_fAlphaSpeed;
	_bool			m_bAlphaCheck;

public:
	_bool*			GetAlphaCheck(void) { return &m_bAlphaCheck; }
};

#endif