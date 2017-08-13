#ifndef BLOODYSCREEN_h__
#define BLOODYSCREEN_h__

#include "UI.h"

class CPlayer;

class CBloodyScreen
	: public CUI
{
private:
	explicit CBloodyScreen(ID3D11DeviceContext* pContext);
	virtual ~CBloodyScreen(void);

public:
	static CBloodyScreen* Create(ID3D11DeviceContext* pContext);

private:
	CPlayer*	m_pPlayer;
	_float		m_fAlpha;
	_bool		m_bCheck;

public:
	virtual HRESULT	Initialize(void);
	virtual _int Update(const _float& fTimeDelta);
	virtual void Render(void);
	virtual void Release(void);

protected:
	virtual HRESULT Ready_Component(void);

public:
	void	CheckPlayer(void);



};


#endif