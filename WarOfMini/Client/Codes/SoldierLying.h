#ifndef SoldierLying_h__
#define SoldierLying_h__

#include "SoldierState.h"

class CSoldierLying : public CSoldierState
{
protected:
	explicit	CSoldierLying(CPlayer*	pSoldier);
	virtual		~CSoldierLying();

protected:
	virtual int	InState();
	virtual int	OnState();
	virtual int	OutState();

	virtual void ShootCheck(void);

public:
	static	CSoldierLying*		Create(CPlayer* pSoldier);

protected:
	virtual void Release(void);

private:
	bool	ISLying(void);
	void	MoveKeyCheck(void);

private:
	CPlayer*	m_pSoldier;
	bool		m_bIsLying;
};

#endif // SoldierLying_h__
