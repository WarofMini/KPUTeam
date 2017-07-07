#ifndef Aim_h__
#define Aim_h__

#include "UI.h"

class CAim
	: public CUI
{
private:
	explicit CAim(ID3D11DeviceContext* pContext);
	virtual ~CAim(void);

public:
	static CAim* Create(ID3D11DeviceContext* pContext);


public:
	virtual HRESULT	Initialize(void);
	virtual _int Update(const _float& fTimeDelta);
	virtual void Render(void);
	virtual void Release(void);

protected:
	virtual HRESULT Ready_Component(void);




};


#endif


