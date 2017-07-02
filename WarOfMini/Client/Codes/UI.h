#ifndef UI_h__
#define UI_h__

#include "GameObject.h"

class CTransform;
class CTextures;
class CRcTex;

class CUI abstract
	: public CGameObject
{

protected:
	explicit CUI(ID3D11DeviceContext* pContext);
	virtual ~CUI(void);

public:
	virtual HRESULT	Initialize(void);
	virtual INT		Update(const FLOAT& fTimeDelta);
	virtual void	Render(void);
	virtual void	Release(void);

protected:
	virtual HRESULT Ready_Component(void);

protected:
	CTransform*				m_pTransform;
	CTextures*				m_pTexture;
	CRcTex*					m_pBuffer;

protected:
	_float					m_fX, m_fY;
	_float					m_fSizeX, m_fSizeY;
	XMFLOAT4X4				m_pProj;
	XMFLOAT4X4				m_pView;
};

#endif