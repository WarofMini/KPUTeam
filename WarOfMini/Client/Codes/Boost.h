#ifndef BOOST_h__
#define BOOST_h__

#include "UI.h"

class CPlayer;

class CBoostUI
	: public CUI
{
private:
	explicit CBoostUI(ID3D11DeviceContext* pContext);
	virtual ~CBoostUI(void);

public:
	static CBoostUI* Create(ID3D11DeviceContext* pContext);

private:
	_float		m_fXGage;
	CPlayer*	m_pPlayer;

public:
	virtual HRESULT	Initialize(void);
	virtual _int Update(const _float& fTimeDelta);
	virtual void Render(void);
	virtual void Release(void);

protected:
	virtual HRESULT Ready_Component(void);

public:
	void	CheckPlayer(void);
	void	UpdateGage(void);

};


#endif