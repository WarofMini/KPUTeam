#ifndef OtherSoldierRoll_h__
#define OtherSoldierRoll_h__

#include "OtherSoldierState.h"

class COtherSoldierRoll : public COtherSoldierState
{
protected:
	explicit	COtherSoldierRoll(COtherPlayer*	pSoldier);
	virtual		~COtherSoldierRoll();

protected:
	virtual int	InState();
	virtual int	OnState();
	virtual int	OutState();

public:
	static	COtherSoldierRoll*		Create(COtherPlayer* pSoldier);

protected:
	virtual void Release(void);

private:
	void EndRoll(void);
	
};

#endif // SoldierRoll_h__
