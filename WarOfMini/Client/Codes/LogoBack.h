#ifndef LogoBack_h__
#define LogoBack_h__

#include "Include.h"
#include "GameObject.h"

class CTextures;
class CRcTex;
class CTransform;

class CLogoBack : public CGameObject
{
private:
	explicit CLogoBack(ID3D11DeviceContext* pContext);
	virtual ~CLogoBack(void);

public:
	static CLogoBack* Create(ID3D11DeviceContext* pContext);

private:
	CTransform*		m_pTransform;
	CRcTex*			m_pBuffer;
	CTextures*		m_pTexture;

private:
	virtual HRESULT Ready_Component(void);

public:
	virtual HRESULT	Initialize(void);
	virtual _int Update(const _float& fTimeDelta);
	virtual void Render(void);
	virtual void Release(void);


};

#endif