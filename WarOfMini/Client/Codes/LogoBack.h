#ifndef LogoBack_h__
#define LogoBack_h__

#include "UI.h"

class CSound;

class CLogoBack
	: public CUI
{
private:
	explicit CLogoBack(ID3D11DeviceContext* pContext);
	virtual ~CLogoBack(void);

private:
	CSound*		m_pSound;

public:
	static CLogoBack* Create(ID3D11DeviceContext* pContext);


public:
	virtual HRESULT	Initialize(void);
	virtual _int Update(const _float& fTimeDelta);
	virtual void Render(void);
	virtual void Release(void);

protected:
	virtual HRESULT Ready_Component(void);

};

#endif