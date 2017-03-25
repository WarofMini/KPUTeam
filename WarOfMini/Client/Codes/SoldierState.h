#ifndef SoldierState_h__
#define SoldierState_h__

#include "State.h"
#include "Include.h"

class CPlayer;

class CSoldierState : public CState
{
protected:
	explicit	CSoldierState(CPlayer*	pSoldier);
	virtual		~CSoldierState()PURE;

protected:
	virtual int	InState();
	virtual int	OnState();
	virtual int	OutState();

protected:
	CPlayer*		m_pSoldier;

protected:
	virtual void Release(void);
};

#endif // SoldierState_h__
