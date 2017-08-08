#ifndef OtherSoldierMove_h__
#define OtherSoldierMove_h__

#include "OtherSoldierState.h"

class COtherSoldierMove : public COtherSoldierState
{
protected:
	explicit	COtherSoldierMove(COtherPlayer*	pSoldier);
	virtual		~COtherSoldierMove();

protected:
	virtual int	InState();
	virtual int	OnState();
	virtual int	OutState();

	virtual void ShootCheck(void);

public:
	static	COtherSoldierMove*		Create(COtherPlayer* pSoldier);

protected:
	virtual void Release(void);

private:
	bool	MoveKeyCheck(void);
	bool	MoveKeyCheck_Iron(void);
	bool	RollKeyCheck(void);
	bool	LyingKeyCheck(void);
	bool	IsSoldierJump(void);

};

#endif // SoldierMove_h__
