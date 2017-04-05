#ifndef SoldierIdle_h__
#define SoldierIdle_h__

#include "SoldierState.h"

class CSoldierIdle : public CSoldierState
{
protected:
	explicit	CSoldierIdle(CPlayer*	pSoldier);
	virtual		~CSoldierIdle();

protected:
	virtual int	InState();
	virtual int	OnState();
	virtual int	OutState();

	virtual void ShootCheck(void);

public:
	static	CSoldierIdle*		Create(CPlayer* pSoldier);

protected:
	virtual void Release(void);

private:
	bool	IsSoldierMove(void);
	bool	IsSoldierLying(void);
	bool	IsSoldierJump(void);

private:
	CPlayer*	m_pSoldier;
};

#endif // SoldierIdle_h__
