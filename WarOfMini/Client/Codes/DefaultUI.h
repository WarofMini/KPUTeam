#ifndef DefaultUI_h__
#define DefaultUI_h__

#include "UI.h"

class CDefaultUI
	: public CUI
{
private:
	explicit CDefaultUI(ID3D11DeviceContext* pContext);
	virtual ~CDefaultUI(void);

private:
	_bool	m_bCameraCheck;

public:
	static CDefaultUI* Create(ID3D11DeviceContext* pContext, wstring strName);


public:
	virtual HRESULT	Initialize(void);
	virtual _int Update(const _float& fTimeDelta);
	virtual void Render(void);
	virtual void Release(void);

protected:
	virtual HRESULT Ready_Component(void);

public:
	void	SetCameraCheck(_bool bCheck) { m_bCameraCheck = bCheck; }

};


#endif


