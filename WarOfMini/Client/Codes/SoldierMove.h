#pragma once

#include "SoldierState.h"

class CSoldierMove : public CSoldierState
{
protected:
	explicit	CSoldierMove(CSoldier*	pSoldier);
	virtual		~CSoldierMove();

protected:
	virtual int	InState();
	virtual int	OnState();
	virtual int	OutState();

public:
	static	CSoldierMove*		Create(CSoldier* pSoldier);

protected:
	virtual void Release(void);
};