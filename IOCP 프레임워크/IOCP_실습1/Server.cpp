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
		PlayerTemp.vPos = XMFLOAT3(20.f * User->id, 20.f * User->id, 20.f * User->id);
		
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

		//여기에 넣으면
		// 쟞댄다일ㄷ
		// 왜냐?
		// 음... 상관없나//// 상관이 없을수도 있긴 하겠지만.
		// 위에서 해주는게

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
			closesocket(m_Client[key]->sock);
			m_Client[key]->connected = false;
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
		else
		{
			cout << "No Event !! " << endl;
			exit(-1);

		}
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
	

	//int packet_size = reinterpret_cast<unsigned char*>(packet)[0];

	////함수가 끝난 다음에 보내는 데이터가 날아가면 안됨.. 비동기 포인터로 처리한다.
	//Overlap_ex* over = new Overlap_ex;
	//ZeroMemory(over, sizeof(Overlap_ex));
	//over->operation_type = OP_SEND;

	//over->wsabuf.buf = reinterpret_cast<CHAR*>(over->IOCPbuf);
	//over->wsabuf.len = packet_size;
	//memcpy(over->IOCPbuf, packet, packet_size);

	//int retval = WSASend(m_Client[id].s, &over->wsabuf, 1, 0, NULL, &over->Original_Overlap, NULL);

	//if (retval != 0)
	//{
	//	int error_no = WSAGetLastError();
	//	error_display("SendPacket: WSASend", error_no);
	//	while (true);

	//}
}

// 수신한 패킷을 잘 조립해서 끝냈으니, 이제 여기서 클라이언트가 보낸 값을 잘 읽기만 하고
// 그 읽은값에 맞춰서 잘 데이터를 처리하는 공간인거야.
void CServer::ProcessPacket(const Packet* buf, const unsigned int& id)	//근데 얘가 꼭 const인 이유가 있나 ?
{

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

		for (int i = 0; i < m_vecPlayer.size(); ++i)
		{
			if (m_vecPlayer[i].ID == strPlayerData.ID)
			{
				Ser_Vec_PLAYER_DATA vecPlayerData;
				vecPlayerData.ID = m_vecPlayer[i].ID;
				vecPlayerData.size = sizeof(Ser_Vec_PLAYER_DATA);
				vecPlayerData.type = INIT_OTHER_PLAYER;
				vecPlayerData.PlayerSize = m_vecPlayer.size();

				for (int i = 0; i < vecPlayerData.PlayerSize; ++i)
				{
					vecPlayerData.vecPlayerData[i].ID = m_vecPlayer[i].ID;
					vecPlayerData.vecPlayerData[i].size = m_vecPlayer[i].size;
					vecPlayerData.vecPlayerData[i].type = m_vecPlayer[i].type;
					vecPlayerData.vecPlayerData[i].vPos = m_vecPlayer[i].vPos;
				}
				SendPacket(m_vecPlayer[i].ID, reinterpret_cast<Packet*>(&vecPlayerData));
			}
			else
			{
				SendPacket(m_vecPlayer[i].ID, reinterpret_cast<Packet*>(&strPlayerData));
			}
		}		
		
		//SendPacket(m_PlayerData->ID)

		// 그러면, 여기 process packet 함수는 어디에 위치해 있냐하면
		// 그러면, 만약에 여기서 클라이언트에게 초기화 하는 코드를 보내주고 싶다고 한다면..
		// 어떻게 해야 이 안으로 들어오겠어?
		// 똑같은 크기로 id값을 처리 해줘야곘지 ?;;
		// 이해를 잘못했나보네 그게 아니라, 이 INIT_CLIENT 위치로 들어와서 값을 초기화 하는 패킷을 클라에게 보내주고 싶다면
		// 어쨌든 이 processPacket 함수 내로 들어오고, 그 다음에 INIT_CLIENT 라는 패킷을 분류해야 여길 들어오잖아
		// 그러면..

		// 처음 서버가 실행되고, accept 한 다음에 어떤 순서로 진행이 되어야
		// 이 함수의 INIT_CLIENT 코드 내부로 들어오겠엉?
		// 클라에서 메시지를 보내야 그 데이터들을 처리하고서 Processpacket으로 들어오겠지

		// 그래 매우 훌륭하군
		// 박수를 쳐줘야 겠어
		// 짝짝짝작

		// 그러니까 다시 말하게 되면, 서버에서 accept 를 하고 나서 그냥 아무것도 안하고
		// 클라이언트에서 먼저 INIT_CLIENT 패킷이 날아와야
		// 여기서 일처리를 하고 다시 보내줄 수 있다는 말이 되는거지
		// 여기까지는 이해가 되었니?ㅇㅇㅇ

		// 그래 그러면
		// 클라이언트에서 INIT_CLIENT 관련 패킷을 구현해서 보내야 하는게 맞지만..
		// 접속을 하고, 바로 클라이언트에 서버가 그냥 보내줘도 되는 정보잖아
		// 왜냐하면 어차피 여기 서버 데이터를 클라에게 보내줘야 하는거니까

		// 이해가 되었을라나?
		// 다시 말하면, 애초에 이 process packet 내부에 들어오지 않고
		// 그냥 바로 accept 해서 새로운 클라가 들어오자마자, 바로 초기화 값을 보내주면 되는거지
		// 잠마.. 그럼
		// 아까 저기 클라에서 Init_client를 id값을 보냈지
		// 그러고서 여기서 응 잘받앗어 하고 여기서도 그 받은 id 값을 보내주면 되는거 아냐 ?

		// 여기서 너가 클라이언트로 패킷을 보내고 싶어
		// 한번 간단히 코드 몇줄 쳐봐

		//
		////id = reinterpret_cast<int>(m_PlayerData->ID);
		// m_PlayerData->ID = id;

		// SendPacket(m_PlayerData->ID, buf);

		// 자, 우리 형배를 위해서, 다시 포인터로 넘어가야겠네
		// 안그래도 좀전까지 상직이 포인터 알려주다가 암걸려 사망할뻔했는데

		// 굳이 그림을 그려주자면
		// [][][][] 이렇게 공간을 차지하지

		// 그리고 char 는 1 byte 를 냠냠하는 녀석이야

		// 그렇다면, char c 를 int a 에 넣을 수 있을까?
		// 답은 넣을 수 있다야.
		// 왜냐하면 int 는 4바이트를 차지하고, char 는 1바이트라서 
		// 무려 4개나 쳐넣을 수 있거든

		// 포인터는? 그냥 주소 값이야. 사실 앞에 붙는 int 니 char 니, 구조체니 클래스니 등은
		// 존나 아무 관계가 없어, 그냥 명시적으로, 이 포인터는 int 형 데이터를 쓰는 포인터야
		// 이 포인터는 char 형을 가르키는 포인터야. 라고 명시해줘서
		// 존나 말도안되는 이상한 값을 가르키면, 스스로 매칭해서 에러를 내뱉는 것일 뿐이지

		// 강제캐스팅 하면, 컴파일러 이새낀 장님이라, 포인터 강제 캐스팅하면 몰라 걍 써버려
		// 예를 들어보도록 하자

		// 위에 a 를 선언했으니, 한번 c 를 4개 넣어 보자
		// 이게 ptr 변수는 a 주소값을 담는거야
		// 근데 컴파일러는 존나 불편하지. 왜냐?
		// 시발 a 가 int 형인데, char 포인터 변수에 넣으려고 하니까
		// 지가 척 보니까 존나 말도 안되는거야
		// 그래서 불편한거지

		// 근데 이렇게 하면 존나 장님이 돼.
		// 왜냐하면 내가 강제 캐스팅으로 이 a 라는 놈은 char 라고 거짓말을 쳤거든
		// 그럼 이 ptr 안에 a 의 주소값이 들어가는데, char 주소값으로 알고있는거야
		// 그래서 이런 수식이 가능하지
		// 그러면...
		
		// 이게 돼. 왜냐? ptr 0 번째 위치에는 char 1 바이트가 들어가지거든
		// 그럼 int a 에는 4byte 가 있어 [] [] [] []
		// 이렇게 있는건데
		// 맨 앞에는 [c] [] [] [] 
		// 이렇게 값이 들어간거야
		// 여기 까지 이해가 되었니?ㅇㅇ

		// 그러면 똑같이

		// 이렇게 넣어서 4개 모두 [c][c][c][c] 이게 가능해져
		// 어렵지 않지?ㅇㅇㅇ

		// 자 그러면, 이제 int 를 char 배열에 넣어보자
		// int a 를 아까 m_sendbuf 에 넣어보자

		// m_Sendbuf 라는 놈은 1 바이트 녀석이 256개 나란히 붙은 놈이고
		// int a 는 4바이트 녀석이 끼리끼리 붙은 놈들이야

		// 그러면, 위의 예제를 참고해서, m_sendbuf 에다가 한번 넣어봐
		
		// 자 위에서는 ptr 이라는 포인터 변수에다가 a 의 주소값을 넣었어
		// 그러면 이번엔, a의 값을 한번 c 에다 넣어보렴
		// 이거 먼저 하고 난 다음에 다음껄 하자
		// int 형의 변수 값을, c 변수에 넣으려면 어떻게 해야할까
		//강제캐스팅하면 안돼 ?
		// 그래 이건 a 변수를 강제 캐스팅 해서 넣은거야

		// 잘했어
		// 그러면 조금 난이도를 높여 보도록 하지
		// 자 이번엔
		
		// 저 i_ptr 값을 넣어보렴
		// 이렇게 하면 c에 &a 의 주소가 담긴거잖아.
		// a의 주소가 아니라, i_ptr 포인터 변수의 주소값을 charㅇ 로 강제 캐스팅 한거야

	}
	break;
	case CLIENT_POSITION:
	{
		int iA = 0;
	}
	break;
	}
	
}


