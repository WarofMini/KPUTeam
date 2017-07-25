#pragma once

#include "Protocol.h"

class CTimer
{
private:
	static LARGE_INTEGER m_cpuClock;
	static LARGE_INTEGER m_cpuStop;
	static LARGE_INTEGER m_cpuFreq;
	static LARGE_INTEGER m_fixClock;


public:
	static float m_Time;

	static bool TimerCount(float fCount);
	static float FrameSec(void);
	static float GetTime(DWORD fStart);
	static float SetTime(void);

public:
	CTimer();
	~CTimer();

};


