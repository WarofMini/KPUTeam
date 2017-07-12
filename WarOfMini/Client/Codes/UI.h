#ifndef UI_h__
#define UI_h__

#include "GameObject.h"


class CTextures;
class CRcTex;

class CUI 
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
	CTextures*				m_pTexture;
	CRcTex*					m_pBuffer;

protected:
	_float					m_fX, m_fY;
	_float					m_fSizeX, m_fSizeY;
	XMFLOAT4X4				m_pProj;
	XMFLOAT4X4				m_pView;
	_float					m_fMoveX;
	_float					m_fMoveY;
	_float					m_fOriginMoveX, m_fOriginMoveY;
	_float					m_fOriginSizeX, m_fOriginSizeY;

public:
	void					SetfX(_float fX) { m_fX = fX; }
	void					SetfY(_float fY) { m_fY = fY; }
	void					SetSizeX(_float fSizeX) { m_fOriginSizeX = fSizeX;   m_fSizeX = fSizeX; }
	void					SetSizeY(_float fSizeY) { m_fOriginSizeY = fSizeY;	 m_fSizeY = fSizeY; }
	void					SetMoveX(_float fMoveX) { m_fOriginMoveX = fMoveX;   m_fMoveX = fMoveX; }
	void					SetMoveY(_float fMoveY) { m_fOriginMoveY = fMoveY;   m_fMoveY = fMoveY; }


	_float					GetfX(void) { return m_fX; }
	_float					GetfY(void) { return m_fY; }
	_float					GetSizeX(void) { return m_fSizeX; }
	_float					GetSizeY(void) { return m_fSizeY; }
	_float					GetMoveX(void) { return m_fMoveX; }
	_float					GetMoveY(void) { return m_fMoveY; }

	void					ComputeFXFY(void);

	void					ComputeChangeWindowSize(void);
};

#endif