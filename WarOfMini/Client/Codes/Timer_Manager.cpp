#include "stdafx.h"
#include "Timer_Manager.h"
#include "Timer.h"

IMPLEMENT_SINGLETON(CTimeMgr)

CTimeMgr::CTimeMgr()
{
}


CTimeMgr::~CTimeMgr()
{
}

_float CTimeMgr::Get_TimeDelta(const _tchar * pTimerTag) const
{
	CTimer* pTimer = Find_Timer(pTimerTag);

	if (pTimer == NULL)
		return 0.0f;

	return pTimer->Get_TimeDelta();
}

void CTimeMgr::SetUp_TimeDelta(const _tchar * pTimerTag)
{
	CTimer* pTimer = Find_Timer(pTimerTag);

	if (pTimer == NULL)
		return;

	pTimer->SetUp_TimeDelta();
}

HRESULT CTimeMgr::Ready_Timer(const _tchar * pTimerTag)
{
	CTimer* pTimer = Find_Timer(pTimerTag);

	if (pTimer != NULL)
		return E_FAIL;

	pTimer = CTimer::Create();

	if (pTimer == NULL)
		return E_FAIL;

	m_mapTimers.insert(MAPTIMER::value_type(pTimerTag, pTimer));

	return S_OK;
}

CTimer * CTimeMgr::Find_Timer(const _tchar * pTimerTag) const
{
	MAPTIMER::const_iterator iter = find_if(m_mapTimers.begin(), m_mapTimers.end(), CTag_Finder(pTimerTag));

	if (iter == m_mapTimers.end())
		return NULL;


	return iter->second;
}

void CTimeMgr::Release(void)
{
	for_each(m_mapTimers.begin(), m_mapTimers.end(), CRelease_Pair());
	m_mapTimers.clear();

	delete this;
}
