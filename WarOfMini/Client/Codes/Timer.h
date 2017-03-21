#ifndef Timer_h__
#define Timer_h__

#include "Include.h"

class CTimer
{
private:
	explicit  CTimer(void);
			 ~CTimer(void);


public:
	_float		Get_TimeDelta(void);

public:
	HRESULT		Ready_Timer(void);
	void		SetUp_TimeDelta(void);

private:
	LARGE_INTEGER	m_FrameTime;
	LARGE_INTEGER	m_FixTime;
	LARGE_INTEGER	m_LastTime;
	LARGE_INTEGER	m_CpuTick;

private:
	_float			m_fTimeDelta;

public:
	static CTimer*	Create(void);
	void			Release(void);

};


#endif