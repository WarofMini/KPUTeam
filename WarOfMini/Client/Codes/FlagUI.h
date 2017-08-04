#ifndef FlagUI_h__
#define FlagUI_h__

#include "UI.h"

class CFlagUI
	: public CUI
{
private:
	explicit CFlagUI(ID3D11DeviceContext* pContext);
	virtual ~CFlagUI(void);

private:
	_float	m_fAlpha;
	_bool	m_bStart;
	_bool	m_bEnd;


public:
	static CFlagUI* Create(ID3D11DeviceContext* pContext);


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
};


#endif


