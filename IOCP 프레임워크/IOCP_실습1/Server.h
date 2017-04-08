#pragma once
#include "Protocol.h"
#include "Player.h"

class CServer
{
private:

	//D3DXVECTOR3 vPosition;
	unsigned int m_ClientCount[MAX_USER];
	vector<thread*> m_vecWorker_Thread;
	vector<PLAYER_INFO*> m_Client;
	HANDLE g_hIocp;
	bool g_bShoutdown = false;
	int iCpuCore;
	unsigned int playerIndex{ UINT_MAX };
	

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
	
	void Initialize(void);
	void SendPacket(unsigned int id, const Packet* packet);
	void ProcessPacket(const Packet* buf, const unsigned int& id);
	
private:
	list<Ser_PLAYER_DATA*> m_listPlayer;

public:
	CServer();
	~CServer();
};

