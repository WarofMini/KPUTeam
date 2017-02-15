#pragma once

#include "State.h"
#include "Include.h"

class CSoldier;

class CSoldierState : public CState
{
protected:
	explicit	CSoldierState(CSoldier*	pSoldier);
	virtual		~CSoldierState()PURE;

protected:
	virtual int	InState();
	virtual int	OnState();
	virtual int	OutState();

protected:
	CSoldier*		m_pSoldier;

protected:
	virtual void Release(void);
};