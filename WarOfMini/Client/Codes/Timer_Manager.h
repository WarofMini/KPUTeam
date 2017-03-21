#ifndef Timer_Manager_h__
#define Timer_Manager_h__

#include "Include.h"

class CTimer;

class CTimeMgr
{
public:
	DECLARE_SINGLETON(CTimeMgr)

private:
	explicit CTimeMgr(void);
	~CTimeMgr(void);

public:
	_float Get_TimeDelta(const _tchar* pTimerTag) const;

public:
	void	SetUp_TimeDelta(const _tchar* pTimerTag);

public:
	HRESULT Ready_Timer(const _tchar* pTimerTag);


public:
	map<const _tchar*, CTimer*>				m_mapTimers;
	typedef map<const _tchar*, CTimer*>		MAPTIMER;

private:
	CTimer* Find_Timer(const _tchar* pTimerTag) const;

public:
	void	Release(void);
};

#endif