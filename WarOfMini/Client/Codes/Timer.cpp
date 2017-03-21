#include "stdafx.h"
#include "Timer.h"


CTimer::CTimer(void)
:m_fTimeDelta(0.0f)
{

}

CTimer::~CTimer(void)
{
}

_float CTimer::Get_TimeDelta(void)
{
	return m_fTimeDelta;
}

HRESULT CTimer::Ready_Timer(void)
{
	QueryPerformanceCounter(&m_FrameTime);
	QueryPerformanceCounter(&m_FixTime);
	QueryPerformanceCounter(&m_LastTime);
	QueryPerformanceFrequency(&m_CpuTick);

	return S_OK;
}

void CTimer::SetUp_TimeDelta(void)
{
	QueryPerformanceCounter(&m_FrameTime);

	if (m_FrameTime.QuadPart - m_FixTime.QuadPart >= m_CpuTick.QuadPart)
	{
		QueryPerformanceFrequency(&m_CpuTick);
		m_FixTime = m_FrameTime;
	}

	m_fTimeDelta = _float(m_FrameTime.QuadPart - m_LastTime.QuadPart) / _float(m_CpuTick.QuadPart);

	m_LastTime = m_FrameTime;

}

CTimer * CTimer::Create(void)
{
	CTimer* pInstance = new CTimer;

	if (FAILED(pInstance->Ready_Timer()))
	{
		MSG_BOX(L"CTimer Created Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTimer::Release(void)
{
	delete this;
}

