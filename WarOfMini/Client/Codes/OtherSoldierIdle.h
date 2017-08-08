#ifndef OtherSoldierIdle_h__
#define OtherSoldierIdle_h__

#include "OtherSoldierState.h"

class COtherSoldierIdle : public COtherSoldierState
{
protected:
	explicit	COtherSoldierIdle(COtherPlayer*	pSoldier);
	virtual		~COtherSoldierIdle();

protected:
	virtual int	InState();
	virtual int	OnState();
	virtual int	OutState();

	virtual void ShootCheck(void);

public:
	static	COtherSoldierIdle*		Create(COtherPlayer* pSoldier);

protected:
	virtual void Release(void);

private:
	bool	IsSoldierMove(void);
	bool	IsSoldierLying(void);
	bool	IsSoldierJump(void);

	bool	IsSoldierIronMove(void);
	bool	IsSoldierIronLying(void);
	bool	IsSoldierIronJump(void);
};

#endif // SoldierIdle_h__
