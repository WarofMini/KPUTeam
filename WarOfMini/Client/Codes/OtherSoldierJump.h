#ifndef OtherSoldierJump_h__
#define OtherSoldierJump_h__

#include "OtherSoldierState.h"

class COtherSoldierJump : public COtherSoldierState
{
protected:
	explicit	COtherSoldierJump(COtherPlayer*	pSoldier);
	virtual		~COtherSoldierJump();

protected:
	virtual int	InState();
	virtual int	OnState();
	virtual int	OutState();

	virtual void ShootCheck(void);

public:
	static	COtherSoldierJump*		Create(COtherPlayer* pSoldier);

protected:
	virtual void Release(void);

private:
	void	LoopJump(bool bShoot);
	bool	EndJump(void);
};

#endif // SoldierJump_h__
