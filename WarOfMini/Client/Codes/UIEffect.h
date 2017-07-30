#ifndef UIEffect_h__
#define UIEffect_h__

#include "UI.h"

class CLoadingBarUI;

class CUIEffect
	: public CUI
{
private:
	explicit CUIEffect(ID3D11DeviceContext* pContext);
	virtual ~CUIEffect(void);

private:
	CLoadingBarUI*		m_pLoadingBar;

public:
	static CUIEffect* Create(ID3D11DeviceContext* pContext, wstring strName);


public:
	virtual HRESULT	Initialize(void);
	virtual _int Update(const _float& fTimeDelta);
	virtual void Render(void);
	virtual void Release(void);

protected:
	virtual HRESULT Ready_Component(void);

public:
	void	SetLoadingBar(CLoadingBarUI* pUI) { m_pLoadingBar = pUI; }

};


#endif


