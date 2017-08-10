#ifndef ButtonUI_h__
#define ButtonUI_h__

#include "UI.h"

class CButtonUI
	: public CUI
{
private:
	explicit CButtonUI(ID3D11DeviceContext* pContext);
	virtual ~CButtonUI(void);

private:
	_bool	m_bStart;

public:
	static CButtonUI* Create(ID3D11DeviceContext* pContext);


public:
	virtual HRESULT	Initialize(void);
	virtual _int Update(const _float& fTimeDelta);
	virtual void Render(void);
	virtual void Release(void);

protected:
	virtual HRESULT Ready_Component(void);

public:
	void	SetStart(_bool bStart) { m_bStart = bStart; }
	_bool	GetStart(void) { return m_bStart; }

};


#endif


