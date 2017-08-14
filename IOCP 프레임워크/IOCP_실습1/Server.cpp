#include "Server.h"
#include <bitset>
//������ �Է�


CServer::CServer()
	: m_iStarterCnt(0)
	, m_bGameStart(false)
{
	ServerIpAddress();
	CheckCPUCoreCount();
	Initialize();
	MakeWorkerThread_AcceptThread();
}

CServer::~CServer()
{
	WSACleanup();
}

void CServer::error_display(char * msg, int err_no)
{
	WCHAR *lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("%s :", msg);
	wprintf(L"Error ! %s\n", lpMsgBuf);
	LocalFree(lpMsgBuf);
}

void CServer::error_quit(wchar_t* msg, int err_no)
{
	WCHAR *lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
	//MessageBox(NULL, (LPCTSTR)lpMsgBuf,msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(-1);
}

void CServer::ServerIpAddress(void)
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	PHOSTENT hostInfo;
	char hostname[50];
	char ipaddr[50];
	memset(hostname, 0, sizeof(hostname));
	memset(ipaddr, 0, sizeof(ipaddr));

	int Error = gethostname(hostname, sizeof(hostname));
	if (Error == 0)
	{
		hostInfo = gethostbyname(hostname);
		strcpy(ipaddr, inet_ntoa(*reinterpret_cast<struct in_addr*>(hostInfo->h_addr_list[0])));
	}
	WSACleanup();
	cout << "This Server's IP address : " << ipaddr << endl;

}

void CServer::Process_Event(event_type ev_Now)
{
	//switch (ev_Now.do_event)
	//{
	//case EV_GAMECOUNT:
	//{
	//	Overlap_ex* over = new Overlap_ex;
	//	over->operation_type = EV_GAMECOUNT;
	//	PostQueuedCompletionStatus(g_hIocp, 1, ev_Now.obj_id, &(over->Original_Overlap));
	//	break;
	//}
	//default:
	//	break;
	//}
	//

}

void CServer::Initialize(void)
{
	startTime = 0.f;
	WSADATA wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);

	g_hIocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
	if (g_hIocp == NULL)
	{
		int Error_No = WSAGetLastError();
		error_quit(L"Server Initalize Fail", Error_No);
	}

	ZeroMemory(m_strStation, sizeof(strStation) * 5);
	for (int i = 0; i < 5; ++i)
		m_strStation[i].stationID = i;
	m_strStation[0].flagState = 1;
	m_strStation[2].flagState = 2;
	Ateam = 0;
	Bteam = 0;

}

void CServer::CheckCPUCoreCount()
{
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	iCpuCore = static_cast<int>(si.dwNumberOfProcessors) * 2;
	//cout << "CPU Core Count : " << iCpuCore / 2 << endl;
	//cout << "Thread : " << iCpuCore << endl;

}

void CServer::MakeWorkerThread_AcceptThread()
{
	m_vecWorker_Thread.reserve(iCpuCore);

	for (int i = 0; i < iCpuCore; ++i)
	{
		m_vecWorker_Thread.push_back(new thread{ &CServer::Worker_thread, this });
	}

	thread acceptThread{ &CServer::Accept_thread, this };

	thread timer_thread{ &CServer::Timer_Thread, this };

	while (g_bShoutdown)
	{
		Sleep(1000);
	}

	for (auto thread : m_vecWorker_Thread)
	{
		thread->join();
		delete thread;
	}
	
	acceptThread.join();
	timer_thread.join();
}

void CServer::Accept_thread()
{
	//�� �÷��̾��� ��ġ�� �ٸ� ��� �÷��̾�� ����
	//�ٸ� �ÿ��̾��� ��ġ�� �� �÷��̾�� ����
	int retval{ 0 };

	

	//socket()
	SOCKET listen_sock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (listen_sock == INVALID_SOCKET)
	{
		int err_no = WSAGetLastError();
		error_display("socket()", err_no);
	};

	//bind()
	struct sockaddr_in serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVER_PORT);
	retval = ::bind(listen_sock, reinterpret_cast<struct sockaddr*>(&serveraddr), sizeof(serveraddr));
	if (retval == SOCKET_ERROR)
	{
		int err_no = WSAGetLastError();
		error_display("socket()", err_no);
	}

	//listen()
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR)
	{
		int err_no = WSAGetLastError();
		error_display("listen()", err_no);
	}
	while ((!g_bShoutdown) == TRUE)
	{
		//accept()
		struct sockaddr_in clientaddr;
		int addrlen = sizeof(clientaddr);

		SOCKET client_sock = WSAAccept(listen_sock, reinterpret_cast<struct sockaddr*>(&clientaddr), &addrlen, NULL, NULL);
		if (client_sock == INVALID_SOCKET)
		{
			int err_no = WSAGetLastError();
			error_display("accept()", err_no);
			while (true);

		}

		

		// �׷� ���⼭ ���� id ���� ���� �Ǿ���

		playerIndex += 1;
		cout << "[No.  " << playerIndex << "]" << "Client IP =  " << inet_ntoa(clientaddr.sin_addr) << "PORT = " << ntohs(clientaddr.sin_port) << endl;

		CreateIoCompletionPort(reinterpret_cast<HANDLE>(client_sock), g_hIocp, playerIndex, 0);

		// ���⼭ ���� Ŭ���̾�Ʈ ���� ������ �ʱ�ȭ �ؼ� �޾Ƽ� �־���
		PLAYER_INFO* User = new PLAYER_INFO;

		User->sock = client_sock;
		User->connected = true;
		User->id = playerIndex; // ���Ⱑ id ������
		User->curr_paket_size = 0;
		User->prev_received = 0;
		memset(&User->my_overapped.Original_Overlap, 0, sizeof(User->my_overapped.Original_Overlap));
		User->my_overapped.operation_type = OP_RECV; // �װ� �������� ���� ���⼭ �ʱ�ȭ ���ذž�.
		User->my_overapped.wsabuf.buf = reinterpret_cast<char*>(&User->my_overapped.IOCPbuf);
		User->my_overapped.wsabuf.len = sizeof(User->my_overapped.IOCPbuf);
		
		//m_listPlayer
		Ser_PLAYER_DATA PlayerTemp;
		PlayerTemp.ID = User->id;
		PlayerTemp.size = sizeof(Ser_PLAYER_DATA);
		PlayerTemp.type = INIT_CLIENT;
		PlayerTemp.sHP = 20;
		
		if (m_bGameStart == false)
			PlayerTemp.SC_ID = GS_READY;//Ŭ�� ������ ���ӽ��������� �����ϰ� ���������˷��ٰ�
		else
			PlayerTemp.SC_ID = GS_START;

		float fRansPosx = ((100.f)*((float)rand() / RAND_MAX)) + -50.f;
		float fRansPosy = ((100.f)*((float)rand() / RAND_MAX)) + -50.f;

		if (Ateam <= Bteam)
		{
			++Ateam;
			User->m_bRedBlue = true;
			PlayerTemp.vPos = XMFLOAT3(200.f + fRansPosx, 57.f, 100.f + fRansPosy);
																
		}
		else
		{
			++Bteam;
			User->m_bRedBlue = false;
			PlayerTemp.vPos = XMFLOAT3(300.f + fRansPosx, 57.f, 1090.f + fRansPosy);
		}

	/*	if (User->m_bRedBlue == true)
		{
			
		}
		else
		{
			
		}*/
		//PlayerTemp.dwState = SOLDIER_IDLE;
		
		m_vecPlayer.push_back(PlayerTemp);

		// �׸��� ���⼭ ���� ��ü Ŭ���̾�Ʈ �����ϴ� �༮���� ���� �־����ݾ�.����
		m_Client.push_back(move(User));


		SendPacket(PlayerTemp.ID, reinterpret_cast<Packet*>(&PlayerTemp));

		
		//int TimeInteeger = GetTickCount() + 1000;
		//if (playerIndex >= 1)
		//{
		//	//timer_queue.push(event_type{ User->id, m_state
		//}
		
		
		DWORD flags{ 0 };

		// �׸��� ���� �� Ŭ���̾�Ʈ�� ���� � ������ ��Ŷ�̵Ǿ��� ������, ���� �� �ֵ��� �غ��ڼ��� ���� �װ� WSARecv ��

		retval = WSARecv(client_sock, &m_Client[playerIndex]->my_overapped.wsabuf, 1, NULL, &flags, &m_Client[playerIndex]->my_overapped.Original_Overlap, NULL);

		if (retval == SOCKET_ERROR)
		{
			int err_no = WSAGetLastError();
			if (err_no != ERROR_IO_PENDING)
			{
				error_display("Accept_WSARecv", err_no);
			}
		}
	}
}

void CServer::Worker_thread()
{
	//��������� �ٸ� ��� �÷��̾�� �˸�

	while (TRUE == (!g_bShoutdown))
	{
		DWORD key;
		DWORD iosize;
		Overlap_ex* overlap; // �׷��� �� overlap ������ ������ �����ϴ� ������

		// GetQueuedCompletionStatus �� �Լ���, ���� �Ǿ��� ���� �׳� ���� �̺�Ʈ�� �߻��ϸ� �����ϴ� �Լ���
		// Ŭ�󿡼� �޼��� ��Ŷ�� ������ �� �Լ��� ���ڱ� ����� return �� �ϰ� �ǰ�
		// �������� �޼��� ��Ŷ�� send �Ͽ� ������, �� �Լ��� ���ڱ� ����� return ����
		// ���� Ŭ�� ������ �Ϲ������� ���������� ����� return �� �Ѵܸ�����
		// ���� �� �Լ�����, �� �ù� ����� �� �� ���¾� �ϰ� �˷��شٰ�
		// ���⼭ ������ �ϸ� overlap �����Ϳ��ٰ�, WSARecv �� WSASend ��, �ű�� ����߾��� �ּҸ� �����ؼ� �� ������ ����ִ°ž�
		BOOL Result = GetQueuedCompletionStatus(g_hIocp, &iosize, &key, reinterpret_cast<LPOVERLAPPED *>(&overlap), INFINITE);

		// �ٵ� �̰� �ƴ���, �ֳ�? ���� �ڱ� �ڽ��� �� Ŭ������ ���±� ������, return �� false �� ���� ����
		if (FALSE == Result || 0 == iosize)
		{
			if (FALSE == Result)
			{
				int Error_no = WSAGetLastError();
				error_display("WorkerThread Start GetQueuedCompletionStatus", Error_no);
			}

			//SendRemovePlayerPacket(key);
			//cout << "[No. " << key << "] Disconnected "<< endl;
			continue;

		}

		else if (OP_RECV == overlap->operation_type)
		{
			unsigned char* buf_ptr = m_Client[key]->my_overapped.IOCPbuf;

			int remained = iosize;

			while (remained > 0)
			{
				if (m_Client[key]->curr_paket_size == 0)
				{
					m_Client[key]->curr_paket_size = buf_ptr[0];
				}

				int required = m_Client[key]->curr_paket_size - m_Client[key]->prev_received;

				if (remained >= required)
				{
					memcpy(m_Client[key]->Packetbuf + m_Client[key]->prev_received, buf_ptr, required);
					ProcessPacket(m_Client[key]->Packetbuf, key);


					buf_ptr  += required;
					remained -= required;
				
					m_Client[key]->curr_paket_size = 0;
					m_Client[key]->prev_received = 0;

				}
				else
				{
					memcpy(m_Client[key]->Packetbuf + m_Client[key]->prev_received, buf_ptr, remained);
					buf_ptr += remained;
					m_Client[key]->prev_received += remained;
					remained = 0;

				}

				DWORD flags = 0;
				int retval = WSARecv(m_Client[key]->sock, &m_Client[key]->my_overapped.wsabuf, 1, NULL, &flags, &m_Client[key]->my_overapped.Original_Overlap, NULL);
				if (retval == SOCKET_ERROR)
				{
					/*int err_no = WSAGetLastError();
					if (ERROR_IO_PENDING != err_no)
					{
						error_display("WorkerThreadStart::WSARecv", err_no);

					}*/
					continue;

				}			
			}
		}
		else if (overlap->operation_type == OP_SEND)
		{
			delete overlap;
		}
		//else if (overlap->operation_type == OP_TIME)
		//{
		//	//Do_Timer(key);
		//	//Add_Timer(key, OP_TIME, 1000);
		//	delete overlap;
		//}
	
		else
		{

			cout << "No Event !! " << endl;
			exit(-1);

		}
	}



}

// ������ ��Ŷ�� �� �����ؼ� ��������, ���� ���⼭ Ŭ���̾�Ʈ�� ���� ���� �� �б⸸ �ϰ�
// �� �������� ���缭 �� �����͸� ó���ϴ� �����ΰž�.
void CServer::ProcessPacket(const Packet* buf, const unsigned int& id)	//�ٵ� �갡 �� const�� ������ �ֳ� ?
{
	vector<int> vecID;//������ִ� Ŭ���̾�Ʈ��
	for (int i = 0; i <= playerIndex; ++i)
	{
		if (m_Client[i]->connected)
			vecID.push_back(i);
	}

	//unsigned char m_sendbuf[256]{ 0 };

	// ���⼭ �� �����̰ھ�
	// �ѹ� ���ؔg Ŭ���̾�Ʈ ���ؿ��� �ѹ� ����g
	switch (buf[1])
	{
		//case TEST:
		//{
		//	//Ŭ�󿡼� ���� ��Ŷ�� �״�� �ٽ� �����ش�.
		//	cout << "[NO. " << id << "]TEST Packet Recv.. " << endl;
		//	cout << "buf[0] =  " << buf[0] << "buf[1] = " << buf[1] << "buf[2] =  " << buf[2] << endl;
		//	SendPacket(id, buf);
		//	break;
		//}

	case INIT_CLIENT:
	{
		Ser_PLAYER_DATA strPlayerData;
		strPlayerData = *reinterpret_cast<Ser_PLAYER_DATA*>((Ser_PLAYER_DATA*)&buf[2]);

		if (m_bGameStart == false)
			strPlayerData.SC_ID = GS_READY;//Ŭ�� ������ ���ӽ��������� �����ϰ� ���������˷��ٰ�
		else
			strPlayerData.SC_ID = GS_START;

		++m_iStarterCnt;

		cout << "[NO. " << strPlayerData.ID << "]ID value Recv.. " << endl;
		for (int i = 0; i < vecID.size(); ++i)
		{
			if (m_Client[vecID[i]]->id == strPlayerData.ID)
			{
				for (int j = 0; j < vecID.size(); ++j)
				{
					if (m_Client[vecID[j]]->id == strPlayerData.ID)
						continue;
					m_vecPlayer[vecID[j]].type = INIT_CLIENT;
					SendPacket(strPlayerData.ID, reinterpret_cast<Packet*>(&m_vecPlayer[vecID[j]]));
				}
			}
			else
			{
				SendPacket(m_vecPlayer[vecID[i]].ID, reinterpret_cast<Packet*>(&strPlayerData));
			}
		}

		cout << "m_StartCnt : " << m_iStarterCnt << endl;

	}
	break;
	case CLIENT_POSITION:
	{
		Ser_PLAYER_DATA strPlayerData;
		strPlayerData = *reinterpret_cast<Ser_PLAYER_DATA*>((Ser_PLAYER_DATA*)&buf[2]);

		Ser_SEND_PLAYER_DATA strSendData;
		strSendData.size = sizeof(Ser_SEND_PLAYER_DATA);
		strSendData.type = CLIENT_SEND_POSITION;
		strSendData.ID = strPlayerData.ID;
		strSendData.fAngle = strPlayerData.fAngle;
		strSendData.sBitKey = strPlayerData.sBitKey;
		strSendData.xmf3CollPos = strPlayerData.strColllayData.xmf3CollPos;

		if (strPlayerData.strColllayData.iCollPlayerID != -1)
		{
			for (int i = 0; i < vecID.size(); ++i)
				if (m_vecPlayer[vecID[i]].ID == strPlayerData.strColllayData.iCollPlayerID)
					--m_vecPlayer[vecID[i]].sHP;
		}

		if (strPlayerData.sHP >= 1000)
		{
			m_fTimeCnt = 1.4f;
			m_vecPlayer[strPlayerData.ID].sHP = 5;
		}
		strSendData.sHP = m_vecPlayer[strPlayerData.ID].sHP;

		for (int i = 0; i < vecID.size(); ++i)
		{
			if (m_vecPlayer[vecID[i]].ID == strPlayerData.ID)
			{
				m_vecPlayer[vecID[i]].vPos = strPlayerData.vPos;
				m_vecPlayer[vecID[i]].fAngle = strPlayerData.fAngle;
				/*m_vecPlayer[vecID[i]].strColllayData = strPlayerData.strColllayData;*/
				m_vecPlayer[vecID[i]].sBitKey = strPlayerData.sBitKey;
				SendPacket(m_vecPlayer[vecID[i]].ID, reinterpret_cast<Packet*>(&strSendData));
			}
			else
			{
				SendPacket(m_vecPlayer[vecID[i]].ID, reinterpret_cast<Packet*>(&strSendData));
			}
		}
	}
	break;
	case CLIENT_ANIMATION:
	{//�ϴܳ��ΰ�
	 /*Ser_ANIMATION_DATA strAnimationData;
	 strAnimationData = *reinterpret_cast<Ser_ANIMATION_DATA*>((Ser_ANIMATION_DATA*)&buf[2]);

	 for (int i = 0; i < vecID.size(); ++i)
	 {
	 if (m_vecPlayer[vecID[i]].ID == strAnimationData.ID)
	 continue;
	 m_vecPlayer[vecID[i]].type = CLIENT_ANIMATION;
	 SendPacket(m_vecPlayer[vecID[i]].ID, reinterpret_cast<Packet*>(&strAnimationData));
	 }*/
	}
	break;
	case COLLISION_LAY:
	{
		/*Ser_COLLLAY_DATA strColData;
		strColData = *reinterpret_cast<Ser_COLLLAY_DATA*>((Ser_COLLLAY_DATA*)&buf[2]);

		for (int i = 0; i < vecID.size(); ++i)
		{
		SendPacket(m_vecPlayer[vecID[i]].ID, reinterpret_cast<Packet*>(&strColData));
		}*/
	}
	break;
	case INGAME_STATION:
	{
		Ser_Station_DATA strStationData;
		strStationData = *reinterpret_cast<Ser_Station_DATA*>((Ser_Station_DATA*)&buf[2]);

		strStation& Station = m_strStation[strStationData.stationID];

		if (strStationData.team == 0)//TeamA
		{
			if (strStationData.occupation)
				++Station.ATeamCnt;
			else
				--Station.ATeamCnt;
		}
		else//TeamB
		{
			if (strStationData.occupation)
				++Station.BTeamCnt;
			else
				--Station.BTeamCnt;
		}

		Ser_CurStation_DATA curStationData;
		memcpy(&curStationData.station[0], m_strStation, sizeof(strStation) * 5);
		curStationData.size = sizeof(Ser_CurStation_DATA);
		curStationData.type = INGAME_CUR_STATION;

		for (int i = 0; i < vecID.size(); ++i)
		{
			SendPacket(m_vecPlayer[vecID[i]].ID, reinterpret_cast<Packet*>(&curStationData));
		}

		/*Ser_CurStation_DATA curStationData;
		memcpy(&curStationData.station[0], m_strStation, sizeof(strStation) * 3);
		curStationData.size = sizeof(Ser_CurStation_DATA);
		curStationData.type = INGAME_CUR_STATION;
		for (int i = 0; i < vecID.size(); ++i)
		{
		SendPacket(m_vecPlayer[vecID[i]].ID, reinterpret_cast<Packet*>(&curStationData));
		}*/
	}
	break;
	case PLAYER_DISCONNECTED:
	{
		Ser_EscapeGame EscapeGameData;
		EscapeGameData = *reinterpret_cast<Ser_EscapeGame*>((Ser_EscapeGame*)&buf[2]);

		for (int i = 0; i < vecID.size(); ++i)
		{
			if (m_vecPlayer[vecID[i]].ID == EscapeGameData.id)
				continue;
			m_vecPlayer[vecID[i]].type = PLAYER_DISCONNECTED;
			SendPacket(m_vecPlayer[vecID[i]].ID, reinterpret_cast<Packet*>(&EscapeGameData));
		}

		closesocket(m_Client[EscapeGameData.id]->sock);
		m_Client[EscapeGameData.id]->connected = false;
		cout << "[No. " << EscapeGameData.id << "] Disconnected " << endl;
		--m_iStarterCnt;
	}
	break;
	}

}

void CServer::Timer_Thread()
{
	// Ŭ�󿡼� �������� ���� ���¸� ������.
	// �������� �� ���� ���¸� �޾Ƽ� ��ΰ� �������� ���¿� �����ϸ� ī��Ʈ �ٿ�.

	m_fTimeCnt = 1.f;
	bool start = false;
	while (1)
	{
		if (!start)
		{
			start = true;
			cout << " .......... Stage Entered.......... " << endl;
		}
		//if ( <= 1)
		//{
		//	continue;
		//}
		if(m_iStarterCnt > 1)
		//if (m_iStarterCnt >= 2)
		{
			for (int i = 4; i > 0; --i)
			{

				CTimer::TimerCount(1.f);
				cout << "CountDown : " << i - 1 << endl;

				Ser_Time_DATA timepacket;
				timepacket.size = sizeof(timepacket);
				timepacket.type = TIMECOUNT;
				if (i == 1)
					timepacket.gamestate = GS_START;
				else
					timepacket.gamestate = GS_READY;
				timepacket.time = i - 1;



				for (int j = 0; j < m_Client.size(); ++j)
				{
					if (m_Client[j]->connected)
						SendPacket(m_Client[j]->id, reinterpret_cast<Packet*>(&timepacket));
				}
				m_bGameStart = true;
				if (i == 1)
				{
					//startTime = CTimer::SetTime();
					cout << "Play ~!" << endl;
				}
			}

			float fTime = 0.f;
			bool  bDataChanged = false;
			while (true)
			{
				fTime = CTimer::FrameSec();
				m_fTimeCnt += fTime;

				for (int i = 0; i < 5; ++i)
				{
					if (m_strStation[i].ATeamCnt != 0 && m_strStation[i].BTeamCnt == 0)
					{
						m_strStation[i].fTime += fTime * m_strStation[i].ATeamCnt;
						if (m_strStation[i].flagState == 1)
							m_strStation[i].fTime = 0.f;

						if (m_strStation[i].fTime >= 3.f && m_strStation[i].flagState != 1)
						{
							m_strStation[i].fTime = 3.f;
							m_strStation[i].flagState = 1;// FLAG_TEAM1;
							bDataChanged = true;
						}
					}


					if (m_strStation[i].BTeamCnt != 0 && m_strStation[i].ATeamCnt == 0)
					{
						m_strStation[i].fTime -= fTime * m_strStation[i].BTeamCnt;
						if (m_strStation[i].flagState == 2)
							m_strStation[i].fTime = 0.f;

						if (m_strStation[i].fTime <= -3.f && m_strStation[i].flagState != 2)
						{
							m_strStation[i].fTime = -3.f;
							m_strStation[i].flagState = 2;// FLAG_TEAM2;
							bDataChanged = true;
						}
					}

					if (m_strStation[i].ATeamCnt == 0 && m_strStation[i].BTeamCnt == 0)
					{
						if (m_strStation[i].fTime < 0.f)
						{
							m_strStation[i].fTime += fTime;
							if (m_strStation[i].fTime > 0.f)
								m_strStation[i].fTime = 0.f;
						}
						if (m_strStation[i].fTime > 0.f)
						{
							m_strStation[i].fTime -= fTime;
							if (m_strStation[i].fTime < 0.f)
								m_strStation[i].fTime = 0.f;
						}
					}
				}

				if (m_fTimeCnt >= 1.5f)
				{
					m_fTimeCnt = 0.f;

					vector<int> vecID;//������ִ� Ŭ���̾�Ʈ��
					for (int i = 0; i <= playerIndex; ++i)
					{
						if (m_Client[i]->connected)
							vecID.push_back(i);
					}

					Ser_SEND_CHECK_PLAYER_DATA PlayerData;
					PlayerData.size = sizeof(Ser_SEND_CHECK_PLAYER_DATA);
					PlayerData.type = CLIENT_POSITION;
					PlayerData.PlayerSize = vecID.size();

					for (int i = 0; i < vecID.size(); ++i)
					{
						PlayerData.vecPositionData[i].ID = m_vecPlayer[vecID[i]].ID;
						PlayerData.vecPositionData[i].fAngle = m_vecPlayer[vecID[i]].fAngle;
						PlayerData.vecPositionData[i].vPos = m_vecPlayer[vecID[i]].vPos;
					}

					for (int i = 0; i < vecID.size(); ++i)
					{
						SendPacket(m_Client[vecID[i]]->id, reinterpret_cast<Packet*>(&PlayerData));
					}
				}

				//���ɵǰ� �ȵǰ� �� �ٲ�� �����ִ°ɷ�����
				if (bDataChanged == true)
				{
					vector<int> vecID;//������ִ� Ŭ���̾�Ʈ��
					for (int i = 0; i <= playerIndex; ++i)
					{
						if (m_Client[i]->connected)
							vecID.push_back(i);
					}

					bDataChanged = false;
					Ser_CurStation_DATA curStationData;
					memcpy(&curStationData.station[0], m_strStation, sizeof(strStation) * 5);
					curStationData.size = sizeof(Ser_CurStation_DATA);
					curStationData.type = INGAME_CUR_STATION;

					for (int i = 0; i < vecID.size(); ++i)
					{
						SendPacket(m_Client[vecID[i]]->id, reinterpret_cast<Packet*>(&curStationData));
					}
				}

				/*Ser_CurStation_DATA curStationData;
				memcpy(&curStationData.station[0], m_strStation, sizeof(strStation) * 3);
				curStationData.size = sizeof(Ser_CurStation_DATA);
				curStationData.type = INGAME_CUR_STATION;
				for (int i = 0; i < m_Client.size(); ++i)
				{
				if (m_Client[i]->connected)
				SendPacket(m_Client[i]->id, reinterpret_cast<Packet*>(&curStationData));
				}*/

			}
		}


	}

	//while (true) {
	//	Sleep(1);
	//	if (timer_queue.size() == 0)
	//		continue;

	//	timer_lock.lock();
	//	topEvent = timer_queue.top();
	//	while (topEvent.wakeup_time <= GetTickCount())
	//	{
	//		//if (timer_queue.top().wakeup_time > GetTickCount()) break;
	//		//event_type ev = timer_queue.top();
	//		timer_queue.pop();
	//		timer_lock.unlock();
	//		Overlap_ex *over = new Overlap_ex;
	//		over->operation_type = topEvent.event_id;
	//		//Process_Event(over->operation_type);
	//		PostQueuedCompletionStatus(g_hIocp, 1,
	//			topEvent.obj_id,
	//			&(over->Original_Overlap));
	//		timer_lock.lock();
	//	}
	//	timer_lock.unlock();
	//}

}
void CServer::Add_Timer(int id, int do_event, int wakeup)
{
	event_type new_event;
	new_event.do_event = do_event;
	new_event.obj_id = id;
	new_event.wakeup_time = wakeup + GetTickCount();

	timer_lock.lock();
	timer_queue.push(event_type{ id, reinterpret_cast<BYTE>(&new_event.wakeup_time) + 1000, OP_TIME });
	timer_lock.unlock();
}
void CServer::SendRemovePlayerPacket(DWORD dwKey)
{
	Ser_Packet_Remove_Player packet;
	packet.id = dwKey;
	packet.size = sizeof(packet);
	packet.type = PLAYER_DISCONNECTED;

	for (int i = 0; i <= playerIndex; ++i)
	{
		if (m_Client[i]->connected == false)
			continue;
		if (m_Client[i]->id == dwKey)
		{
			closesocket(m_Client[i]->sock);
			m_Client[i]->connected = false;
			continue;
		}
		//if(m_Client[i]->connected)
			SendPacket(m_Client[i]->id, reinterpret_cast<Packet*>(&packet));
	}
	/*if (m_Client[dwKey]->m_bRedBlue)
		Ateam--;
	else
		Bteam--;*/
}

void CServer::SendPacket(unsigned int id, const Packet* packet)
{	
	Overlap_ex* overlap = new Overlap_ex;
	//unsigned char* packetTemp = packet;
	memset(overlap, 0, sizeof(Overlap_ex));

	// SendPacket �Ҷ� �� �Ʒ��� OP_SEND
	// ���� �̳� overlap ���� ���� (������) ��Ŷ�� ������ ���̴�
	// �ϰ� ���� �����ְ� 
	overlap->operation_type = OP_SEND;
	memcpy(overlap->IOCPbuf, packet, packet[0]);

	overlap->wsabuf.buf = reinterpret_cast<char*>(overlap->IOCPbuf);
	overlap->wsabuf.len = packet[0];

	DWORD flags{ 0 };

	// ���⼭ �׳� send �Ͽ� ���������°ž�
	// �̰� �� �������� ����
	int retval = WSASend(m_Client[id]->sock, &overlap->wsabuf, 1, NULL, flags, &overlap->Original_Overlap, NULL);
	if (retval == SOCKET_ERROR)
	{
		/*int err_no = WSAGetLastError();

		if (err_no != ERROR_IO_PENDING)
		{
			error_display("SendPacket_WSASend", err_no);
			while (true);

		}*/
	}
}

void CServer::CountTime()	//�ð��� ����ؼ� ��������.
{

	Ser_Time_DATA Time;
	Time.size = sizeof(Ser_Time_DATA);
	Time.type = TIMECOUNT;
	Time.time = CTimer::GetTime(startTime);
	
	cout << "TimeCount : " << Time.time << endl;
	
	//i++;
	//for (int i = 0; i <= playerIndex; ++i)
	//SendPacket(, reinterpret_cast<Packet*>(&Time));
}


