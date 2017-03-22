#ifndef LogoBack_h__
#define LogoBack_h__

#include "GameObject.h"

class CTransform;
class CTextures;
class CRcTex;

class CLogoBack 
	: public CGameObject
{

private:
	explicit CLogoBack(ID3D11DeviceContext* pContext);
	virtual ~CLogoBack(void);

public:
	static CLogoBack* Create(ID3D11DeviceContext* pContext);

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

};

#endif