#ifndef ResultUI_h__
#define ResultUI_h__

#include "UI.h"

class CPanel;

class CResultUI
	: public CUI
{
private:
	explicit CResultUI(ID3D11DeviceContext* pContext);
	virtual ~CResultUI(void);


private:
	CPanel*			m_pPanel;
	CInput*			m_pInput;

public:
	static CResultUI* Create(ID3D11DeviceContext* pContext);


public:
	virtual HRESULT	Initialize(void);
	virtual _int	Update(const _float& fTimeDelta);
	virtual void	Render(void);
	virtual void	Release(void);

protected:
	virtual HRESULT Ready_Component(void);

};


#endif


