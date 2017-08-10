#ifndef ResultUI_h__
#define ResultUI_h__

#include "UI.h"

class CPanel;
class CFlagUI;
class COutComUI;
class CButtonUI;

class CResultUI
	: public CUI
{
private:
	explicit CResultUI(ID3D11DeviceContext* pContext);
	virtual ~CResultUI(void);


private:
	CPanel*			m_pPanel;
	CInput*			m_pInput;
	COutComUI*		m_pOutCom; //½ÂÆÐÇ¥½Ã (Win, Lose)
	CFlagUI*		m_pFlag; //½Â¸®ÆÀ ±ê¹ß
	CButtonUI*		m_pButton[2];
	_bool			m_bStart;

public:
	static CResultUI* Create(ID3D11DeviceContext* pContext);


public:
	virtual HRESULT	Initialize(void);
	virtual _int	Update(const _float& fTimeDelta);
	virtual void	Render(void);
	virtual void	Release(void);

protected:
	virtual HRESULT Ready_Component(void);

public:
	void	OutComUpdate(void);

	void	ButtonCollision(void);
};


#endif


