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
		Ser_PLAYER_DATA* pPlayerTemp = new Ser_PLAYER_DATA;
		pPlayerTemp->ID = User->id;
		pPlayerTemp->size = sizeof(Ser_PLAYER_DATA);
		pPlayerTemp->type = INIT_CLIENT;
		pPlayerTemp->vPos = XMFLOAT3(20.f, 10.f, 20.f);
		
		m_listPlayer.push_back(pPlayerTemp);

		// �׸��� ���⼭ ���� ��ü Ŭ���̾�Ʈ �����ϴ� �༮���� ���� �־����ݾ�.����
		m_Client.push_back(move(User));

		// �ٷ� �������ƴϸ� ���⳪ ���⵵ ������ ����� �ص� �������
		// �ֳ��ϸ�? ������ �����͸� �����µ��� �� 2������ �ʿ��ϰŵ�, Ŭ���̾�Ʈ�� ���� �ּҶ�
		// ��Ŷ�� ���� ������
		// ������ �� ���� ������ ���̴±�
		// �׳� ���������� Ŭ���̾�Ʈ���� �־��ְ�
		// �ٷ� ��Ŷ�����ִ� �۾��� �ϴ°� �׳� �� ��п� �����ϱ�
		// ���� ����� �����ߴµ� �ؿ� Recv�� �ϰ� �޾߾��ҰͰ��Ƽ� �׹ؿ��� �ҷ��׷��µ�...����

		SendPacket(pPlayerTemp->ID, reinterpret_cast<Packet*>(pPlayerTemp));

		list<Ser_PLAYER_DATA*>::iterator iter = m_listPlayer.begin();
		list<Ser_PLAYER_DATA*>::iterator iter_end = m_listPlayer.end();

		for (iter; iter != iter_end; ++iter)
		{
			if(pPlayerTemp == *iter)
				continue;
			(*iter)->type = INIT_OTHER_PLAYER;
			SendPacket((*iter)->ID, reinterpret_cast<Packet*>(*iter));
		}

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

		//���⿡ ������
		// �a����Ϥ�
		// �ֳ�?
		// ��... �������//// ����� �������� �ֱ� �ϰ�����.
		// ������ ���ִ°�

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
			// �׷��� ���Ⱑ, Ŭ���̾�Ʈ�� ���ڱ� ������ �������� ������ ��ġ�ΰž�.
			// result ���� false �̰�, iosize ��, ó���� ��Ŷ �� ���� 0 �̸�, ����Ŷ� �����ϴ°���
			closesocket(m_Client[key]->sock);
			m_Client[key]->connected = false;
			cout << "[No. " << key << "] Disconnected "<< endl;

			continue;

		}

		// �׸��� ���⿡�� overlap ����ü�� ������ �Ǿ��ٸ�, overlap �� �ִ� Ŭ���̾�Ʈ �༮�� �̺�Ʈ�� �ߵ���Ų�ž�
		// �׸��� operation_type �� �ξ� ������ �ʰ� OP_RECV �� �ʱ�ȭ�� ���־��� ������, �̰� ������ �Ǵ°���
		// �̰� �� ��ĸ�, �� overlap �𵨿��� �̺�Ʈ�� �߻��Ѵٸ�, �̰� ������ �ٸ� Ŭ�󿡼� ��Ŷ�� ���� ���̴�!!!
		// �ϰ� �����ϱ� ���ؼ� OP_RECV ��� �Ѱž�.
		// ���⼭ Ÿ���� ���� ���Ҵ���, OP_RECV �� �ƴϴϱ� �ϴ� �Ѿ��

		// �׷��� SEND �� �����ϰ� ������ ������
		// OP_RECV �� ���� �ٸ� Ŭ���̾�Ʈ���� �޽����� �����Ŷ� ���̾�
		// �� �޼����� ��Ŷ�� ��� ����

		else if (OP_RECV == overlap->operation_type)
		{
			// ���⿡���� buf_ptr �� ��ġ�� ��� ����־��ĸ�
			// my_overapped.IOCPbuf �� ����ذž�
			// �� ���۴� �����ϸ�
			unsigned char* buf_ptr = m_Client[key]->my_overapped.IOCPbuf;

			int remained = iosize;

			// �׷��� �� �Ʒ��� �ִ°� ��Ŷ���� �� �и��ϰ� �����ؼ�
			// �츮�� ���������� ���� �� �ֵ��� ������ִ� loop while ���� �ž�
			// �� ���� �Ʒ����� ��Ŷ �����ϰ�, ���������� ��Ŷ�� ���� �� �ֵ��� �������� ���� �����ϰ� �������� ����� �а���
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
					// �����־�. �̰� ���� �ǹ̳� �ϸ�
					// �׷� ��Ŷ ������ ������ �ؼ�
					// process Packet �Լ��� ����
					// �̰� �׷� �����ǹ̰ھ�
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
		// �׷� �̰� ���ھ�.
		// �� ������, �� overlap �𵨷� SEND �ߴٴ°� �˷��ִ� �б����ΰ���
		// �Դٰ� ��ġ�� �߸� ����
		// ����
		// �� ��ġ���ٰ� �ϴ°ž礻����
		// �� �ù� ���� ���ϱ� ���⿡ OP_SEND �� �ֳ� ���� �ϰ� ����� ���°ž�.
		else if (overlap->operation_type == OP_SEND)
		{
			//�޼����� ������ ����.
			// ���⼭ �� ���ڱ� delete �� Ƣ��Գ�??
			// ���� ����?
			// �׷��� ������ �� �Ҵ��ߴ����� �޸� ������ �־�� �Ǵϱ�
			// ���⼭ �� send �� �ߴٴ� �̾߱⳪ �ٸ��� �����ϱ�
			// �׳� ���� �� ���� ��� �޸� ������ ���ִ°���
			delete overlap;
		}
		// �� �׷� �� else �� ���ھ�. �ù� �̺�Ʈ�� �ȵ����������� Ŭ�� �ƹ��� �޼����� ������ �ʾ����� ?
		// ���� �̰͵� Ʋ���ٴ� �����̱�
		// �ʴ� OP_RECV �� OP_SEND �ΰ��� ��츸 �ڵ��� �����, �׸��� Ŭ�󿡼� �Ϲ������� ���������� �ڵ�� ������ �����
		// ���� �̰� �ܿ� �ٸ� ���� ������, ���� �߸��ǵ� ���� �߸��Ȱ���
		// �׷��� ���� ������ �a�뵵 ���� �a��ž�
		// �� �ǹ��� else �ΰ���
		// �ѹ濡 ���ذ� �ȵǾ����� ������ ���ذ� �Ǿ���?����.. �ƴ� �ٵ� ���Ⱑ
		// �������� ������ ���� �� �����°��ΰ� ? �� ���� �𸣁ٳ�
		// �װ� �ƴ϶�, �� GetQueuedCompletionStatus �� �Լ����� �����Ҽ� �ۿ� ���� ���� ���� 3���� ����
		// �ʰ� �ʱ�ȭ �Ҷ�, OP_RECV ���ְ� WSARecv �Լ��� �ɾ, Ŭ���̾�Ʈ�� ���� ��Ŷ�� ���� ��� �ϳ�
		// �ƴϸ� OP_SEND
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
		int err_no = WSAGetLastError();

		if (err_no != ERROR_IO_PENDING)
		{
			error_display("SendPacket_WSASend", err_no);
			while (true);

		}
	}
	

	//int packet_size = reinterpret_cast<unsigned char*>(packet)[0];

	////�Լ��� ���� ������ ������ �����Ͱ� ���ư��� �ȵ�.. �񵿱� �����ͷ� ó���Ѵ�.
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

// ������ ��Ŷ�� �� �����ؼ� ��������, ���� ���⼭ Ŭ���̾�Ʈ�� ���� ���� �� �б⸸ �ϰ�
// �� �������� ���缭 �� �����͸� ó���ϴ� �����ΰž�.
void CServer::ProcessPacket(const Packet* buf, const unsigned int& id)	//�ٵ� �갡 �� const�� ������ �ֳ� ?
{

	//unsigned char m_sendbuf[256]{ 0 };

	// ���⼭ �� �����̰ھ�
	// �ѹ� ���ؔg Ŭ���̾�Ʈ ���ؿ��� �ѹ� ����g
	switch (buf[1])
	{
	case TEST:
	{
		//Ŭ�󿡼� ���� ��Ŷ�� �״�� �ٽ� �����ش�.
		cout << "[NO. " << id << "]TEST Packet Recv.. " << endl;
		cout << "buf[0] =  " << buf[0] << "buf[1] = " << buf[1] << "buf[2] =  " << buf[2] << endl;
		SendPacket(id, buf);
		break;
	}

	case INIT_CLIENT:
	{
		//SendPacket(m_PlayerData->ID)

		// �׷���, ���� process packet �Լ��� ��� ��ġ�� �ֳ��ϸ�
		// �׷���, ���࿡ ���⼭ Ŭ���̾�Ʈ���� �ʱ�ȭ �ϴ� �ڵ带 �����ְ� �ʹٰ� �Ѵٸ�..
		// ��� �ؾ� �� ������ �����ھ�?
		// �Ȱ��� ũ��� id���� ó�� ����߁��� ?;;
		// ���ظ� �߸��߳����� �װ� �ƴ϶�, �� INIT_CLIENT ��ġ�� ���ͼ� ���� �ʱ�ȭ �ϴ� ��Ŷ�� Ŭ�󿡰� �����ְ� �ʹٸ�
		// ��·�� �� processPacket �Լ� ���� ������, �� ������ INIT_CLIENT ��� ��Ŷ�� �з��ؾ� ���� �����ݾ�
		// �׷���..

		// ó�� ������ ����ǰ�, accept �� ������ � ������ ������ �Ǿ��
		// �� �Լ��� INIT_CLIENT �ڵ� ���η� �����ھ�?
		// Ŭ�󿡼� �޽����� ������ �� �����͵��� ó���ϰ� Processpacket���� ��������

		// �׷� �ſ� �Ǹ��ϱ�
		// �ڼ��� ����� �ھ�
		// ¦¦¦��

		// �׷��ϱ� �ٽ� ���ϰ� �Ǹ�, �������� accept �� �ϰ� ���� �׳� �ƹ��͵� ���ϰ�
		// Ŭ���̾�Ʈ���� ���� INIT_CLIENT ��Ŷ�� ���ƿ;�
		// ���⼭ ��ó���� �ϰ� �ٽ� ������ �� �ִٴ� ���� �Ǵ°���
		// ��������� ���ذ� �Ǿ���?������

		// �׷� �׷���
		// Ŭ���̾�Ʈ���� INIT_CLIENT ���� ��Ŷ�� �����ؼ� ������ �ϴ°� ������..
		// ������ �ϰ�, �ٷ� Ŭ���̾�Ʈ�� ������ �׳� �����൵ �Ǵ� �����ݾ�
		// �ֳ��ϸ� ������ ���� ���� �����͸� Ŭ�󿡰� ������� �ϴ°Ŵϱ�

		// ���ذ� �Ǿ�����?
		// �ٽ� ���ϸ�, ���ʿ� �� process packet ���ο� ������ �ʰ�
		// �׳� �ٷ� accept �ؼ� ���ο� Ŭ�� �����ڸ���, �ٷ� �ʱ�ȭ ���� �����ָ� �Ǵ°���
		// �Ḷ.. �׷�
		// �Ʊ� ���� Ŭ�󿡼� Init_client�� id���� ������
		// �׷��� ���⼭ �� �߹޾Ѿ� �ϰ� ���⼭�� �� ���� id ���� �����ָ� �Ǵ°� �Ƴ� ?

		// ���⼭ �ʰ� Ŭ���̾�Ʈ�� ��Ŷ�� ������ �;�
		// �ѹ� ������ �ڵ� ���� �ĺ�

		//
		////id = reinterpret_cast<int>(m_PlayerData->ID);
		// m_PlayerData->ID = id;

		// SendPacket(m_PlayerData->ID, buf);

		// ��, �츮 ���踦 ���ؼ�, �ٽ� �����ͷ� �Ѿ�߰ڳ�
		// �ȱ׷��� �������� ������ ������ �˷��ִٰ� �ϰɷ� ����һ��ߴµ�

		// ���� �׸��� �׷����ڸ�
		// [][][][] �̷��� ������ ��������

		// �׸��� char �� 1 byte �� �ȳ��ϴ� �༮�̾�

		// �׷��ٸ�, char c �� int a �� ���� �� ������?
		// ���� ���� �� �ִپ�.
		// �ֳ��ϸ� int �� 4����Ʈ�� �����ϰ�, char �� 1����Ʈ�� 
		// ���� 4���� �ĳ��� �� �ְŵ�

		// �����ʹ�? �׳� �ּ� ���̾�. ��� �տ� �ٴ� int �� char ��, ����ü�� Ŭ������ ����
		// ���� �ƹ� ���谡 ����, �׳� ���������, �� �����ʹ� int �� �����͸� ���� �����;�
		// �� �����ʹ� char ���� ����Ű�� �����;�. ��� ������༭
		// ���� �����ȵǴ� �̻��� ���� ����Ű��, ������ ��Ī�ؼ� ������ ����� ���� ������

		// ����ĳ���� �ϸ�, �����Ϸ� �̻��� ����̶�, ������ ���� ĳ�����ϸ� ���� �� �����
		// ���� ������ ����

		// ���� a �� ����������, �ѹ� c �� 4�� �־� ����
		// �̰� ptr ������ a �ּҰ��� ��°ž�
		// �ٵ� �����Ϸ��� ���� ��������. �ֳ�?
		// �ù� a �� int ���ε�, char ������ ������ �������� �ϴϱ�
		// ���� ô ���ϱ� ���� ���� �ȵǴ°ž�
		// �׷��� �����Ѱ���

		// �ٵ� �̷��� �ϸ� ���� ����� ��.
		// �ֳ��ϸ� ���� ���� ĳ�������� �� a ��� ���� char ��� �������� �ưŵ�
		// �׷� �� ptr �ȿ� a �� �ּҰ��� ���µ�, char �ּҰ����� �˰��ִ°ž�
		// �׷��� �̷� ������ ��������
		// �׷���...
		
		// �̰� ��. �ֳ�? ptr 0 ��° ��ġ���� char 1 ����Ʈ�� �����ŵ�
		// �׷� int a ���� 4byte �� �־� [] [] [] []
		// �̷��� �ִ°ǵ�
		// �� �տ��� [c] [] [] [] 
		// �̷��� ���� ���ž�
		// ���� ���� ���ذ� �Ǿ���?����

		// �׷��� �Ȱ���

		// �̷��� �־ 4�� ��� [c][c][c][c] �̰� ��������
		// ����� ����?������

		// �� �׷���, ���� int �� char �迭�� �־��
		// int a �� �Ʊ� m_sendbuf �� �־��

		// m_Sendbuf ��� ���� 1 ����Ʈ �༮�� 256�� ������ ���� ���̰�
		// int a �� 4����Ʈ �༮�� �������� ���� ����̾�

		// �׷���, ���� ������ �����ؼ�, m_sendbuf ���ٰ� �ѹ� �־��
		
		// �� �������� ptr �̶�� ������ �������ٰ� a �� �ּҰ��� �־���
		// �׷��� �̹���, a�� ���� �ѹ� c ���� �־��
		// �̰� ���� �ϰ� �� ������ ������ ����
		// int ���� ���� ����, c ������ �������� ��� �ؾ��ұ�
		//����ĳ�����ϸ� �ȵ� ?
		// �׷� �̰� a ������ ���� ĳ���� �ؼ� �����ž�

		// ���߾�
		// �׷��� ���� ���̵��� ���� ������ ����
		// �� �̹���
		
		// �� i_ptr ���� �־��
		// �̷��� �ϸ� c�� &a �� �ּҰ� �����ݾ�.
		// a�� �ּҰ� �ƴ϶�, i_ptr ������ ������ �ּҰ��� char�� �� ���� ĳ���� �Ѱž�

	}
	break;
	case CLIENT_POSITION:
	{
		int iA = 0;
	}
	break;
	}
	
}


