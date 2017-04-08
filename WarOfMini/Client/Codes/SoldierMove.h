#ifndef SoldierMove_h__
#define SoldierMove_h__

#include "SoldierState.h"

class CSoldierMove : public CSoldierState
{
protected:
	explicit	CSoldierMove(CPlayer*	pSoldier);
	virtual		~CSoldierMove();

protected:
	virtual int	InState();
	virtual int	OnState();
	virtual int	OutState();

	virtual void ShootCheck(void);

public:
	static	CSoldierMove*		Create(CPlayer* pSoldier);

protected:
	virtual void Release(void);

private:
	bool	MoveKeyCheck(void);
	bool	MoveKeyCheck_Iron(void);
	bool	RollKeyCheck(void);
	bool	LyingKeyCheck(void);
	bool	IsSoldierJump(void);

private:
	CPlayer*	m_pSoldier;
};

#endif // SoldierMove_h__
