#ifndef GageUI_h__
#define GageUI_h__

#include "UI.h"



class CLoadingBarUI
	: public CUI
{
private:
	explicit CLoadingBarUI(ID3D11DeviceContext* pContext);
	virtual ~CLoadingBarUI(void);

public:
	static CLoadingBarUI* Create(ID3D11DeviceContext* pContext);


private:
	_float		m_fXGage;

public:
	virtual HRESULT	Initialize(void);
	virtual _int Update(const _float& fTimeDelta);
	virtual void Render(void);
	virtual void Release(void);

protected:
	virtual HRESULT Ready_Component(void);

public:
	_float	GetGage(void) { return m_fXGage; }

};


#endif


