#ifndef SoldierRoll_h__
#define SoldierRoll_h__

#include "SoldierState.h"

class CSoldierRoll : public CSoldierState
{
protected:
	explicit	CSoldierRoll(CPlayer*	pSoldier);
	virtual		~CSoldierRoll();

protected:
	virtual int	InState();
	virtual int	OnState();
	virtual int	OutState();

public:
	static	CSoldierRoll*		Create(CPlayer* pSoldier);

protected:
	virtual void Release(void);

private:
	void EndRoll(void);
	
};

#endif // SoldierRoll_h__
