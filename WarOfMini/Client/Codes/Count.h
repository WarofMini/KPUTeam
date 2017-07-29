#ifndef CountUI_h__
#define CountUI_h__

#include "UI.h"

class CCountUI
	: public CUI
{
private:
	explicit CCountUI(ID3D11DeviceContext* pContext);
	virtual ~CCountUI(void);

private:
	_float	m_fCount;
	_bool	m_bCheck;
	_float	m_fAlpha;
	_bool	m_bChangeCount;
	_bool	m_bEnd;

public:
	static CCountUI* Create(ID3D11DeviceContext* pContext);

public:
	virtual HRESULT	Initialize(void);
	virtual _int Update(const _float& fTimeDelta);
	virtual void Render(void);
	virtual void Release(void);

protected:
	virtual HRESULT Ready_Component(void);

public:
	void	SetCount(_float fCount);
	void	AlphaUpdate(const _float & fTimeDelta);
	void	ResetSize(void);
	void	LastResetSize(void);
	void	GrowSize(const _float & fTimeDelta);
	void	LastGrowSize(const _float & fTimeDelta);
};


#endif



