#ifndef Numbering_h__
#define Numbering_h__

#include "UI.h"

class CNumbering
	: public CUI
{
protected:
	explicit CNumbering(ID3D11DeviceContext* pContext);
	virtual ~CNumbering(void);

public:
	static CNumbering* Create(ID3D11DeviceContext* pContext);


public:
	virtual HRESULT	Initialize(void);
	virtual _int Update(const _float& fTimeDelta);
	virtual void Render(void);
	virtual void Release(void);

protected:
	virtual HRESULT Ready_Component(void);




};


#endif


