#ifndef BulletNumbering_h__
#define BulletNumbering_h__

#include "UI.h"

class CNumbering;
class CGun;

class CBulletNumbering
	: public CUI
{
private:
	explicit CBulletNumbering(ID3D11DeviceContext* pContext);
	virtual ~CBulletNumbering(void);

public:
	enum BULLETNUMBERING_STATE { ORIGIN_BULLETNUMBER, CHANGE_BULLETNUMBER, BULLETNUMBER_END };


public:
	static CBulletNumbering* Create(ID3D11DeviceContext* pContext);


public:
	virtual HRESULT	Initialize(void);
	virtual _int Update(const _float& fTimeDelta);
	virtual void Render(void);
	virtual void Release(void);

protected:
	virtual HRESULT Ready_Component(void);

private:
	CNumbering*				m_pNumbering[2];
	CGun*					m_pGun;
	BULLETNUMBERING_STATE   m_eState;
public:
	void		SetNumberSize(_float fSizeX, _float fSizeY);
	void		SetNumberPosition(_float fMoveX, _float fMoveY);
	void		SetGun(CGun* pGun) { m_pGun = pGun; }
	void		SetState(BULLETNUMBERING_STATE eState) { m_eState = eState; }

public:
	void		ChangingNumbering(void);
};


#endif


