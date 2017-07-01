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
	_int			m_iCurrentTextureNumber; //���� �ؽ��� ��ȣ
	_int			m_iLastTextureNumber; //�� �ؽ��� ����
	_float			m_fFrameSpeed;

};

#endif