#ifndef OtherSoldierLying_h__
#define OtherSoldierLying_h__

#include "OtherSoldierState.h"

class COtherSoldierLying : public COtherSoldierState
{
protected:
	explicit	COtherSoldierLying(COtherPlayer*	pSoldier);
	virtual		~COtherSoldierLying();

protected:
	virtual int	InState();
	virtual int	OnState();
	virtual int	OutState();

	virtual void ShootCheck(void);

public:
	static	COtherSoldierLying*		Create(COtherPlayer* pSoldier);

protected:
	virtual void Release(void);

private:
	bool	ISLying(void);
	void	MoveKeyCheck(void);

private:
	bool		m_bIsLying;
};

#endif // SoldierLying_h__
