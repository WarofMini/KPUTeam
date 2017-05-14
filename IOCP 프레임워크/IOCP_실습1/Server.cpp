#include "Server.h"


CServer::CServer()
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
}

void CServer::CheckCPUCoreCount()
{
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	iCpuCore = static_cast<int>(si.dwNumberOfProcessors) * 2;
	cout << "CPU Core Count : " << iCpuCore / 2 << endl;
	cout << "Thread : " << iCpuCore << endl;

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
}

void CServer::Accept_thread()
{
	//새 플레이어의 위치를 다른 모든 플레이어에게 전송
	//다른 플에이어의 위치를 새 플레이어에게 전송
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

		// 그럼 여기서 이제 id 값이 증가 되었찌

		playerIndex += 1;
		cout << "[No.  " << playerIndex << "]" << "Client IP =  " << inet_ntoa(clientaddr.sin_addr) << "PORT = " << ntohs(clientaddr.sin_port) << endl;

		CreateIoCompletionPort(reinterpret_cast<HANDLE>(client_sock), g_hIocp, playerIndex, 0);

		
		// 여기서 이제 클라이언트 관련 정보를 초기화 해서 받아서 넣었어
		PLAYER_INFO* User = new PLAYER_INFO;

		User->sock = client_sock;
		User->connected = true;
		User->id = playerIndex; // 여기가 id 값이지
		User->curr_paket_size = 0;
		User->prev_received = 0;
		memset(&User->my_overapped.Original_Overlap, 0, sizeof(User->my_overapped.Original_Overlap));
		User->my_overapped.operation_type = OP_RECV; // 그걸 접속하자 마자 여기서 초기화 해준거야.
		User->my_overapped.wsabuf.buf = reinterpret_cast<char*>(&User->my_overapped.IOCPbuf);
		User->my_overapped.wsabuf.len = sizeof(User->my_overapped.IOCPbuf);
		
		//m_listPlayer
		Ser_PLAYER_DATA PlayerTemp;
		PlayerTemp.ID = User->id;
		PlayerTemp.size = sizeof(Ser_PLAYER_DATA);
		PlayerTemp.type = INIT_CLIENT;
		PlayerTemp.vPos = XMFLOAT3(20.f * User->id, 0.f, 0.f);
		PlayerTemp.vMoveDir = XMFLOAT3(0.f, 0.f, 0.f);
		PlayerTemp.dwState = SOLDIER_IDLE;
		
		m_vecPlayer.push_back(PlayerTemp);

		// 그리고 여기서 이제 전체 클라이언트 관리하는 녀석으로 값을 넣어주잖아.ㅇㅇ
		m_Client.push_back(move(User));

		// 바로 여기지아니면 여기나 여기도 괜찮고 여기다 해도 상관없지
		// 왜냐하면? 어차피 데이터를 보내는데는 단 2가지만 필요하거든, 클라이언트의 소켓 주소랑
		// 패킷을 보낼 데이터
		// 하지만 요 쯤이 적절해 보이는군
		// 그냥 안정적으로 클라이언트까지 넣어주고
		// 바로 패킷보내주는 작업을 하는게 그냥 내 기분에 좋으니까
		// 나도 여기라 생각했는데 밑에 Recv는 하고 받야아할것같아서 그밑에다 할려그랬는데...ㅅㅂ

		SendPacket(PlayerTemp.ID, reinterpret_cast<Packet*>(&PlayerTemp));

		
		if (playerIndex >= 1)
		{
			m_bReady = true;
		}
		else
		{
			m_bReady = false;
		}
		

		DWORD flags{ 0 };

		// 그리고 이제 이 클라이언트가 만약 어떤 무언가의 패킷이되었든 보내면, 받을 수 있도록 준비자세를 취해 그게 WSARecv 여

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
	//접송종료시 다른 모든 플레이어에게 알림

	while (TRUE == (!g_bShoutdown))
	{
		DWORD key;
		DWORD iosize;
		Overlap_ex* overlap; // 그래서 이 overlap 포인터 변수가 존재하는 이유야

		// GetQueuedCompletionStatus 이 함수는, 뭐가 되었든 간에 그냥 무슨 이벤트가 발생하면 리턴하는 함수야
		// 클라에서 메세지 패킷을 보내도 이 함수가 갑자기 깨어나서 return 을 하게 되고
		// 서버에서 메세지 패킷을 send 하여 보내도, 이 함수가 갑자기 깨어나서 return 을해
		// 물론 클라가 접속을 일방적으로 끊었을때도 깨어나서 return 을 한단말이지
		// 여기 이 함수에서, 야 시발 형배야 나 다 보냈어 하고 알려준다고
		// 여기서 리턴을 하면 overlap 포인터에다가, WSARecv 든 WSASend 든, 거기다 사용했었던 주소를 리턴해서 요 변수에 담아주는거야
		BOOL Result = GetQueuedCompletionStatus(g_hIocp, &iosize, &key, reinterpret_cast<LPOVERLAPPED *>(&overlap), INFINITE);

		// 근데 이건 아니지, 왜냐? 서버 자기 자신이 딴 클라한테 보냈기 때문에, return 이 false 일 수가 없어
		if (FALSE == Result || 0 == iosize)
		{
			if (FALSE == Result)
			{
				int Error_no = WSAGetLastError();
				error_display("WorkerThread Start GetQueuedCompletionStatus", Error_no);
			}

			SendRemovePlayerPacket(key);

			cout << "[No. " << key << "] Disconnected "<< endl;

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
					int err_no = WSAGetLastError();
					if (ERROR_IO_PENDING != err_no)
					{
						error_display("WorkerThreadStart::WSARecv", err_no);

					}
					continue;

				}			
			}


		}
		else if (overlap->operation_type == OP_SEND)
		{
			delete overlap;
		}
		else if (overlap->operation_type == OP_TIME)
		{
			//Do_Timer(key);
			Add_Timer(key, OP_TIME, 1000);
			delete overlap;
		}
	
		else
		{

			cout << "No Event !! " << endl;
			exit(-1);

		}
	}



}
void CServer::Timer_Thread()
{
	while (1)
	{
		if (m_bReady)
		{
			for (int i = 2; i > 0; ++i)
			{
				CTimer::TimerCount(1.f);
				if (i == 1 - 1)
				{
					m_bReady = false;
					startTime = CTimer::SetTime();
					cout << "Complete Time !! " << endl;

				}
				//모든 유저들에게 시간값을 알려주자.
				/*for (int p = 0; p < MAX_USER; ++p)
				{
					SendPacket(p, &)
				}*/
				cout << "Timer : " << CTimer::SetTime() << endl;
			}
		}
		else
		{
			Sleep(1);
			timer_lock.lock();
			while (false == timer_queue.empty())
			{
				
				if (timer_queue.top().wakeup_time > GetTickCount())
					break;
				event_type ev = timer_queue.top();
				timer_queue.pop();
				timer_lock.unlock();
				Overlap_ex* over = new Overlap_ex;
				over->operation_type = ev.event_id;
				PostQueuedCompletionStatus(g_hIocp, 1, ev.obj_id, &(over->Original_Overlap));
				timer_lock.lock();
			
			}
			timer_lock.unlock();

		}

	}
	CTimer::TimerCount(0.03f);


}
void CServer::Add_Timer(int id, int do_event, int wakeup)
{
	event_type new_event;
	new_event.do_event = do_event;
	new_event.obj_id = id;
	new_event.wakeup_time = wakeup + GetTickCount();

	EnterCriticalSection(&cs);
	timer_queue.push(new_event);
	//cout << "Timer : " << new_event.wakeup_time << endl;
	LeaveCriticalSection(&cs);

	/*timer_lock.lock();
	timer_queue.push(event_type{ i, GetTickCount() + 1000, OP_TIME });
	timer_lock.unlock();*/
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
		SendPacket(m_Client[i]->id, reinterpret_cast<Packet*>(&packet));
	}
}
void CServer::SendPacket(unsigned int id, const Packet* packet)
{	
	Overlap_ex* overlap = new Overlap_ex;
	//unsigned char* packetTemp = packet;
	memset(overlap, 0, sizeof(Overlap_ex));

	// SendPacket 할때 요 아래서 OP_SEND
	// 현재 이놈 overlap 으로 내가 (서버가) 패킷을 보내는 것이다
	// 하고 값을 적어주고 
	overlap->operation_type = OP_SEND;
	memcpy(overlap->IOCPbuf, packet, packet[0]);

	overlap->wsabuf.buf = reinterpret_cast<char*>(overlap->IOCPbuf);
	overlap->wsabuf.len = packet[0];

	DWORD flags{ 0 };

	// 여기서 그냥 send 하여 보내버리는거야
	// 이게 다 보내지고 나면
	int retval = WSASend(m_Client[id]->sock, &overlap->wsabuf, 1, NULL, flags, &overlap->Original_Overlap, NULL);
	if (retval == SOCKET_ERROR)
	{
		int err_no = WSAGetLastError();

		if (err_no != ERROR_IO_PENDING)
		{
			error_display("SendPacket_WSASend", err_no);
			while (true);

		}
	}
}

// 수신한 패킷을 잘 조립해서 끝냈으니, 이제 여기서 클라이언트가 보낸 값을 잘 읽기만 하고
// 그 읽은값에 맞춰서 잘 데이터를 처리하는 공간인거야.
void CServer::ProcessPacket(const Packet* buf, const unsigned int& id)	//근데 얘가 꼭 const인 이유가 있나 ?
{
	vector<int> vecID;//연결되있는 클라이언트들
	for (int i = 0; i <= playerIndex; ++i)
	{
		if (m_Client[i]->connected)
			vecID.push_back(i);
	}

	

	//unsigned char m_sendbuf[256]{ 0 };

	// 여기서 은 무엇이겠어
	// 한번 말해봥 클라이언트 기준에서 한번 맞춰봥
	switch (buf[1])
	{
	case TEST:
	{
		//클라에서 받은 패킷을 그대로 다시 돌려준다.
		cout << "[NO. " << id << "]TEST Packet Recv.. " << endl;
		cout << "buf[0] =  " << buf[0] << "buf[1] = " << buf[1] << "buf[2] =  " << buf[2] << endl;
		SendPacket(id, buf);
		break;
	}

	case INIT_CLIENT:
	{
		Ser_PLAYER_DATA strPlayerData;
		strPlayerData = *reinterpret_cast<Ser_PLAYER_DATA*>((Ser_PLAYER_DATA*)&buf[2]);
		cout << "[NO. " << strPlayerData.ID << "]ID value Recv.. " << endl;
		for (int i = 0; i < vecID.size(); ++i)
		{
			//timer_queue.push(event_type{ i, GetTickCount() + 1000, OP_TIME });

			
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

		//전에꺼 코드
//  		for (int i = 0; i <= playerIndex; ++i)
//  		{
//  			if (m_Client[i]->connected == false)
//  				continue;
//  			if (m_vecPlayer[i].ID == strPlayerData.ID)
//  			{//내가 접속하는거면 다른 플레이어들 생성
//  				Ser_Vec_PLAYER_DATA vecPlayerData;
//  				vecPlayerData.ID = m_vecPlayer[i].ID;
//  				vecPlayerData.size = sizeof(Ser_Vec_PLAYER_DATA);
//  				vecPlayerData.type = INIT_OTHER_PLAYER;
//  				vecPlayerData.PlayerSize = vecID.size();
// 
// 				for (int j = 0; j < vecPlayerData.PlayerSize; ++j)
// 				{
// 					vecPlayerData.vecPlayerData[j].ID = m_vecPlayer[vecID[j]].ID;
// 					vecPlayerData.vecPlayerData[j].size = m_vecPlayer[vecID[j]].size;
// 					vecPlayerData.vecPlayerData[j].type = m_vecPlayer[vecID[j]].type;
// 					vecPlayerData.vecPlayerData[j].vPos = m_vecPlayer[vecID[j]].vPos;
// 				}
// 				SendPacket(m_vecPlayer[i].ID, reinterpret_cast<Packet*>(&vecPlayerData));
//  			}
//  			else
//  			{//다른놈이 들어오면 그놈 생성
//  				SendPacket(m_vecPlayer[i].ID, reinterpret_cast<Packet*>(&strPlayerData));
//  			}
//  		}

		//그 전에꺼
// 		for (int i = 0; i < m_vecPlayer.size(); ++i)
// 		{
// 			if (m_vecPlayer[i].ID == strPlayerData.ID)
// 			{
// 				Ser_Vec_PLAYER_DATA vecPlayerData;
// 				vecPlayerData.ID = m_vecPlayer[i].ID;
// 				vecPlayerData.size = sizeof(Ser_Vec_PLAYER_DATA);
// 				vecPlayerData.type = INIT_OTHER_PLAYER;
// 				vecPlayerData.PlayerSize = m_vecPlayer.size();
// 
// 				for (int i = 0; i < vecPlayerData.PlayerSize; ++i)
// 				{
// 					vecPlayerData.vecPlayerData[i].ID = m_vecPlayer[i].ID;
// 					vecPlayerData.vecPlayerData[i].size = m_vecPlayer[i].size;
// 					vecPlayerData.vecPlayerData[i].type = m_vecPlayer[i].type;
// 					vecPlayerData.vecPlayerData[i].vPos = m_vecPlayer[i].vPos;
// 				}
// 				SendPacket(m_vecPlayer[i].ID, reinterpret_cast<Packet*>(&vecPlayerData));
// 			}
// 			else
// 			{
// 				SendPacket(m_vecPlayer[i].ID, reinterpret_cast<Packet*>(&strPlayerData));
// 			}
// 		}		
	}
	break;
	case CLIENT_POSITION:
	{
		Ser_PLAYER_DATA strPlayerData;
		strPlayerData = *reinterpret_cast<Ser_PLAYER_DATA*>((Ser_PLAYER_DATA*)&buf[2]);
		for (int i = 0; i < vecID.size(); ++i)
		{
			if (m_vecPlayer[vecID[i]].ID == strPlayerData.ID)
			{
				m_vecPlayer[vecID[i]].vAngle = strPlayerData.vAngle;
				m_vecPlayer[vecID[i]].vPos = strPlayerData.vPos;
				m_vecPlayer[vecID[i]].vMoveDir = strPlayerData.vMoveDir;
				m_vecPlayer[vecID[i]].dwState = strPlayerData.dwState;
			}
			else
			{
				m_vecPlayer[vecID[i]].type = CLIENT_POSITION;
				SendPacket(m_vecPlayer[vecID[i]].ID, reinterpret_cast<Packet*>(&strPlayerData));
			}
		}
	}
	break;
	case CLIENT_ANIMATION:
	{//일단냅두고
		Ser_ANIMATION_DATA strAnimationData;
		strAnimationData = *reinterpret_cast<Ser_ANIMATION_DATA*>((Ser_ANIMATION_DATA*)&buf[2]);

		for (int i = 0; i < vecID.size(); ++i)
		{
			if (m_vecPlayer[vecID[i]].ID == strAnimationData.ID)
				continue;
			m_vecPlayer[vecID[i]].type = CLIENT_ANIMATION;
			SendPacket(m_vecPlayer[vecID[i]].ID, reinterpret_cast<Packet*>(&strAnimationData));
		}
	}
	break;
	case COLLISION_LAY:
		Ser_COLLLAY_DATA strColData;
		strColData = *reinterpret_cast<Ser_COLLLAY_DATA*>((Ser_COLLLAY_DATA*)&buf[2]);

		for (int i = 0; i < vecID.size(); ++i)
		{
			if (m_vecPlayer[vecID[i]].ID == strColData.ID)
			{
				SendPacket(strColData.ID, reinterpret_cast<Packet*>(&strColData));
			}
		}
		break;
	case PLAYER_DISCONNECTED:
	{
	/*	Ser_Packet_Remove_Player RemovePlayer;
		RemovePlayer = *reinterpret_cast<Ser_Packet_Remove_Player*>((Ser_Packet_Remove_Player*)&buf[2]);

		for (int i = 0; i < m_vecPlayer.size(); ++i)
		{
			if (m_vecPlayer[i].ID == RemovePlayer.id)
				continue;
			m_vecPlayer[i].type = PLAYER_DISCONNECTED;
			SendRemovePlayerPacket(i, i);
		}
*/
	}
	break;
	}
	
}


