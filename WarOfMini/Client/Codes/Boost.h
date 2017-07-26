#ifndef HP_h__
#define HP_h__

#include "UI.h"

class CBoostUI
	: public CUI
{
private:
	explicit CBoostUI(ID3D11DeviceContext* pContext);
	virtual ~CBoostUI(void);

public:
	static CBoostUI* Create(ID3D11DeviceContext* pContext);


public:
	virtual HRESULT	Initialize(void);
	virtual _int Update(const _float& fTimeDelta);
	virtual void Render(void);
	virtual void Release(void);

protected:
	virtual HRESULT Ready_Component(void);


};


#endif