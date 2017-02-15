#ifndef State_h__ 
#define State_h__

#include "Include.h"

class CState
{
protected:
	explicit	CState();
	virtual		~CState() PURE;

public:
	enum STATE { STATE_IN, STATE_ON, STATE_OUT, STATE_END };

protected:
	virtual int	InState();
	virtual int	OnState();
	virtual int	OutState();

public:
	void	Begin_State();
	void	Update_State();
	STATE	Get_State();

protected:
	STATE	m_eState;

public:
	virtual	void	Release();
};

#endif	//State_h__