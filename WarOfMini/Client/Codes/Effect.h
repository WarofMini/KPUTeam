#ifndef Effect_h__
#define Effect_h__

#include "GameObject.h"

class CTextures;
class CTransform;
class CRcTex;

class CEffect
	: public CGameObject
{
private:
	explicit CEffect(ID3D11DeviceContext* pContext);
	virtual ~CEffect(void);

public:
	static CEffect* Create(ID3D11DeviceContext* pContext);

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
	_int			m_iCurrentTextureNumber; //현재 텍스쳐 번호
	_int			m_iLastTextureNumber; //총 텍스쳐 갯수
	_float			m_fFrameSpeed;


public:
	void			SetLastTextureNumber(_int iNumber) { m_iLastTextureNumber = iNumber;}
	void			SetCurrentTextureNumber(_int iNumber) { m_iCurrentTextureNumber = iNumber; }
	void			SetFrameSpeed(_float fSpeed) { m_fFrameSpeed = fSpeed; }

	_int			GetCurrentTextureNumber(void) { return m_iCurrentTextureNumber; }
	_int			GetLastTextureNumber(void) { return m_iLastTextureNumber;  }
	_float			GetFrameSpeed(void) { return m_fFrameSpeed; }

};

#endif