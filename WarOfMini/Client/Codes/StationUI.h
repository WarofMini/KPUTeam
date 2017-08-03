#ifndef StationUI_h__
#define StationUI_h__

#include "UI.h"

class CStationUI
	: public CUI
{
private:
	explicit CStationUI(ID3D11DeviceContext* pContext);
	virtual ~CStationUI(void);

private:
	_float	m_fAlpha;
	_bool	m_bStart;
	_bool	m_bEnd;


public:
	static CStationUI* Create(ID3D11DeviceContext* pContext);


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

	void	StationUIUpdate(const _float & fTimeDelta);
};


#endif


