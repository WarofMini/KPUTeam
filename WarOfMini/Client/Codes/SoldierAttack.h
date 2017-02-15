#pragma once

#include "SoldierState.h"

class CSoldierAttack : public CSoldierState
{
protected:
	explicit	CSoldierAttack(CSoldier*	pSoldier);
	virtual		~CSoldierAttack();

protected:
	virtual int	InState();
	virtual int	OnState();
	virtual int	OutState();

public:
	static	CSoldierAttack*		Create(CSoldier* pSoldier);

protected:
	virtual void Release(void);
};