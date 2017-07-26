#include "Timer.h"


LARGE_INTEGER CTimer::m_cpuClock;
LARGE_INTEGER CTimer::m_cpuStop;
LARGE_INTEGER CTimer::m_cpuFreq;
LARGE_INTEGER CTimer::m_fixClock;

float CTimer::m_Time;

CTimer::CTimer()
{
	m_Time = 0.f;
	ZeroMemory(&m_cpuClock, sizeof(long int));
	ZeroMemory(&m_cpuStop, sizeof(long int));
	ZeroMemory(&m_cpuFreq, sizeof(long int));
	ZeroMemory(&m_fixClock, sizeof(long int));

}

CTimer::~CTimer()
{
}

bool CTimer::TimerCount(float fCount)
{
	QueryPerformanceFrequency(&m_cpuFreq);

	QueryPerformanceCounter(&m_cpuClock);
	while (1) {
		QueryPerformanceCounter(&m_cpuStop);
		if (fCount <= (m_cpuStop.QuadPart - m_cpuClock.QuadPart) / m_cpuFreq.QuadPart)
			return true;
	}
}

float CTimer::FrameSec(void)
{
	QueryPerformanceCounter(&m_cpuClock);
	if (m_cpuClock.QuadPart - m_cpuStop.QuadPart > m_cpuFreq.QuadPart) {
		QueryPerformanceFrequency(&m_cpuFreq);
		m_cpuStop.QuadPart = m_cpuClock.QuadPart;
	}
	m_Time = (float)(m_cpuClock.QuadPart - m_fixClock.QuadPart) / (float)m_cpuFreq.QuadPart;

	m_fixClock = m_cpuClock;

	return m_Time;
}

float CTimer::SetTime(void)
{
	QueryPerformanceCounter(&m_cpuClock);
	return m_cpuClock.QuadPart;
}

float CTimer::GetTime(DWORD fStart)
{
	QueryPerformanceCounter(&m_cpuClock);
	return ((m_cpuStop.QuadPart - fStart) / m_cpuFreq.QuadPart);
}



