#pragma once
#include "Protocol.h"
#include "Timer.h"

class CServer
{
public:
	enum MOVE_DIR { DIR_U, DIR_UR, DIR_UL, DIR_R, DIR_L, DIR_D, DIR_DR, DIR_DL, DIR_END };
	enum STATE_SOLDIER { SOLDIER_IDLE, SOLDIER_MOVE, SOLDIER_LYING, SOLDIER_ROLL, SOLDIER_JUMP, SOLDIER_END };
private:

	//D3DXVECTOR3 vPosition;
	unsigned int m_ClientCount[MAX_USER];
	vector<thread*> m_vecWorker_Thread;
	vector<PLAYER_INFO*> m_Client;
	HANDLE g_hIocp;
	bool g_bShoutdown = false;
	int iCpuCore;
	unsigned int playerIndex{ UINT_MAX };

	bool m_bReady = false;
	CRITICAL_SECTION cs;
	float startTime;

	//DWORD KeyValue;
	//CPlayer*	m_pPlayer;

public:
	void error_display(char *msg, int err_no);
	void error_quit(wchar_t* msg, int err_no);

	void ServerIpAddress(void);
	void CheckCPUCoreCount();
	void MakeWorkerThread_AcceptThread();
	void Accept_thread();
	void Worker_thread();
	void Timer_Thread();
	void Add_Timer(int id, int do_event, int wakeup);
	
	void SendRemovePlayerPacket(DWORD dwKey);


	void Initialize(void);
	void SendPacket(unsigned int id, const Packet* packet);
	void ProcessPacket(const Packet* buf, const unsigned int& id);
	
private:
	vector<Ser_PLAYER_DATA> m_vecPlayer;

public:
	priority_queue<event_type, vector<event_type>, mycomp> timer_queue;
	mutex timer_lock;

public:
	CServer();
	~CServer();
};

