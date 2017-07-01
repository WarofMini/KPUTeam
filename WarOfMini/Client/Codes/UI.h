#ifndef UI_h__
#define UI_h__

#include "GameObject.h"

class CTransform;
class CTextures;
class CRcTex;

class CUI
	: public CGameObject
{

private:
	explicit CUI(ID3D11DeviceContext* pContext);
	virtual ~CUI(void);

public:
	static CUI* Create(ID3D11DeviceContext* pContext);

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

private:
	_float					m_fX, m_fY;
	_float					m_fSizeX, m_fSizeY;

	XMFLOAT4X4				m_pProj;
	XMFLOAT4X4				m_pView;
};

#endif