#pragma once

#include "SoldierState.h"

class CSoldierIdle : public CSoldierState
{
protected:
	explicit	CSoldierIdle(CSoldier*	pSoldier);
	virtual		~CSoldierIdle();

protected:
	virtual int	InState();
	virtual int	OnState();
	virtual int	OutState();

public:
	static	CSoldierIdle*		Create(CSoldier* pSoldier);

protected:
	virtual void Release(void);
};