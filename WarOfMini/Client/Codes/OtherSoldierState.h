#ifndef OtherSoldierState_h__
#define OtherSoldierState_h__

#include "State.h"
#include "Include.h"

#include "OtherPlayer.h"

class COtherSoldierState : public CState
{
public:
	enum eKey { KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, KEY_END };

protected:
	explicit	COtherSoldierState(COtherPlayer*	pSoldier);
	virtual		~COtherSoldierState()PURE;

protected:
	virtual int	InState();
	virtual int	OnState();
	virtual int	OutState();

	virtual void ShootCheck(void);

protected:
	COtherPlayer*	m_pSoldier;
	DWORD*			m_pAniIdx;
	bool			m_bShoot;

	//
	MOVE_DIR*		m_pMoveDir;
	_bool*			m_pkey;

protected:
	virtual void Release(void);
};

#endif // SoldierState_h__
