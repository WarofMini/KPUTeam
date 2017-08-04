#ifndef Panel_h__
#define Panel_h__

#include "UI.h"

class CPanel
	: public CUI
{
private:
	explicit CPanel(ID3D11DeviceContext* pContext);
	virtual ~CPanel(void);

private:
	_float	m_fAlpha;
	_bool	m_bStart;
	_bool	m_bEnd;

public:
	static CPanel* Create(ID3D11DeviceContext* pContext, wstring strName);


public:
	virtual HRESULT	Initialize(void);
	virtual _int Update(const _float& fTimeDelta);
	virtual void Render(void);
	virtual void Release(void);

protected:
	virtual HRESULT Ready_Component(void);

public:
	void	SetStart(_bool bCheck) { m_bStart = bCheck; }
	_bool	GetStart(void) { return m_bStart; }
	void	SetEnd(_bool bCheck) { m_bEnd = bCheck; }
	_bool	GetEnd(void) { return m_bEnd; }
	_float	GetAlpha(void) { return m_fAlpha; }
	void	PanelUpdate(const _float & fTimeDelta);
	_bool	GetResetCheck(void);

};


#endif


