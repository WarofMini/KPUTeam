#ifndef SoldierState_h__
#define SoldierState_h__

#include "State.h"
#include "Include.h"

class CPlayer;
class CInput;
class CSoldierState : public CState
{
public:
	enum eKey { KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, KEY_END };

protected:
	explicit	CSoldierState(CPlayer*	pSoldier);
	virtual		~CSoldierState()PURE;

protected:
	virtual int	InState();
	virtual int	OnState();
	virtual int	OutState();

	virtual void ShootCheck(void);

protected:
	CInput*			m_pInput;
	CPlayer*		m_pSoldier;
	DWORD*			m_pAniIdx;
	bool			m_bShoot;

protected:
	virtual void Release(void);
};

#endif // SoldierState_h__
