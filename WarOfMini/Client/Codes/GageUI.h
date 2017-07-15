#ifndef GageUI_h__
#define GageUI_h__

#include "UI.h"

class CGageUI
	: public CUI
{
private:
	explicit CGageUI(ID3D11DeviceContext* pContext);
	virtual ~CGageUI(void);

public:
	static CGageUI* Create(ID3D11DeviceContext* pContext);


public:
	virtual HRESULT	Initialize(void);
	virtual _int Update(const _float& fTimeDelta);
	virtual void Render(void);
	virtual void Release(void);

protected:
	virtual HRESULT Ready_Component(void);

};


#endif


