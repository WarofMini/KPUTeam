#ifndef SoldierJump_h__
#define SoldierJump_h__

#include "SoldierState.h"

class CSoldierJump : public CSoldierState
{
protected:
	explicit	CSoldierJump(CPlayer*	pSoldier);
	virtual		~CSoldierJump();

protected:
	virtual int	InState();
	virtual int	OnState();
	virtual int	OutState();

	virtual void ShootCheck(void);

public:
	static	CSoldierJump*		Create(CPlayer* pSoldier);

protected:
	virtual void Release(void);

private:
	bool	MoveKeyCheck(void);

private:
	CPlayer*	m_pSoldier;
};

#endif // SoldierJump_h__
