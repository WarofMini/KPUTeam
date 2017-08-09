#ifndef MOUSECURSOR_h__
#define MOUSECURSOR_h__

#include "UI.h"


class CMouseUI
	: public CUI
{
private:
	explicit CMouseUI(ID3D11DeviceContext* pContext);
	virtual ~CMouseUI(void);

public:
	static CMouseUI* Create(ID3D11DeviceContext* pContext);

public:
	virtual HRESULT	Initialize(void);
	virtual _int Update(const _float& fTimeDelta);
	virtual void Render(void);
	virtual void Release(void);

protected:
	virtual HRESULT Ready_Component(void);

public:
	void	MoveFollowMousePos(void);

};


#endif