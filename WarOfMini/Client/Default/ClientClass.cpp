#include "stdafx.h"
#include "Include.h"
#include "Management.h"
#include "Scene.h"
#include "OtherPlayer.h"
#include "GraphicDev.h"

AsynchronousClientClass::AsynchronousClientClass()
{
}

AsynchronousClientClass::~AsynchronousClientClass()
{
	closesocket(m_sock);
	WSACleanup();
}

void AsynchronousClientClass::InitSock(const HWND& hwnd)
{
	InputServerIP();

	// init Winsock
	if (WSAStartup(MAKEWORD(2, 2), &m_wsadata) != 0) {
		int err_no = WSAGetLastError();
		error_quit(L"WSAStartup ERROR", err_no);
	}

	m_sock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
	if (INVALID_SOCKET == m_sock) {
		int err_no = WSAGetLastError();
		error_quit(L"socket()", err_no);
	}

	// connect
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(m_serverIP);
	serveraddr.sin_port = htons(SERVERPORT);

	m_retval = WSAConnect(m_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr), NULL, NULL, NULL, NULL);
	if (SOCKET_ERROR == m_retval) {
		// 비동기 소켓이라 그냥 리턴, 검사 해주어야 함
		if (WSAGetLastError() != WSAEWOULDBLOCK) {
			int err_no = WSAGetLastError();
			error_quit(L"connect()", err_no);
		}
	}

	// WSAAsyncSelect - 넌블로킹 소켓 자동 전환
	m_hWnd = hwnd;

	if (NULL == m_hWnd) {
#ifdef _DEBUG
		printf("Cannot find Consol Window, ERROR : %d\n", __LINE__);
#endif
		exit(-1);
	}
	m_retval = WSAAsyncSelect(m_sock, m_hWnd, WM_SOCKET, FD_READ | FD_CLOSE);
	if (SOCKET_ERROR == m_retval) {
		int err_no = WSAGetLastError();
		error_quit(L"ioctlsocket()", err_no);
	}

	m_wsa_recvbuf.buf = reinterpret_cast<CHAR*>(m_recvbuf);
	m_wsa_recvbuf.len = MAX_BUF_SIZE;
	m_wsa_sendbuf.buf = reinterpret_cast<CHAR*>(m_sendbuf);
	m_wsa_sendbuf.len = MAX_BUF_SIZE;
}

void AsynchronousClientClass::ProcessWinMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// 오류 발생 여부 확인
	if (WSAGETSELECTERROR(lParam)) {
		error_display("ProcessWinMessage::", WSAGETSELECTERROR(lParam), __LINE__);
		return;
	}

	// 메세지 처리
	switch (WSAGETSELECTEVENT(lParam))
	{
	case FD_READ: {	// 데이터 수신

					// m_retval = recvn();
		DWORD ioByteSize{ 0 };
		DWORD ioFlag{ 0 };

		m_retval = WSARecv(m_sock, &m_wsa_recvbuf, 1, &ioByteSize, &ioFlag, NULL, NULL);
		if (SOCKET_ERROR == m_retval) {
			int err_no = WSAGetLastError();
			error_display("FD_READ::", err_no, __LINE__);
			return;
		}
		Packet *buf = m_recvbuf;

		int current_data_processing = ioByteSize;
		while (0 < current_data_processing) {
			if (0 == m_packet_size_current) {
				m_packet_size_current = buf[0];
				if (buf[0] > MAX_BUF_SIZE) {
					cout << "AsynchronousClientClass::ProcessWinMessage() Error, recvbuf[0] is out of MAX_BUF_SIZE\n";
					exit(-1);
				}
			}
			int need_to_build = m_packet_size_current - m_packet_size_previous;
			if (need_to_build <= current_data_processing) {
				// Packet building Complete & Process Packet
				memcpy(m_data_buf + m_packet_size_previous, buf, need_to_build);

				// recvbuf 에 받은 내용을 토대로 데이터 처리
				ProcessPacket(m_data_buf);

				m_packet_size_current = 0;
				m_packet_size_previous = 0;
				current_data_processing -= need_to_build;
				buf += need_to_build;
			}
			else {
				// Packet build continue
				memcpy(m_data_buf + m_packet_size_previous, buf, current_data_processing);
				m_packet_size_previous += current_data_processing;
				current_data_processing = 0;
				buf += current_data_processing;
			}
		}
	}
		break;
	case FD_WRITE:	// 데이터 송신
		break;
	case FD_CLOSE:	// 서버에서 통신을 끊음
		break;
	default:
		break;
	}
}

void AsynchronousClientClass::sendPacket_TEST()
{
	m_sendbuf[0] = 3;
	m_sendbuf[1] = TEST;
	m_sendbuf[2] = 1;		// true

	m_wsa_sendbuf.len = m_sendbuf[0];

	DWORD ioByteSize;
	m_retval = WSASend(m_sock, &m_wsa_sendbuf, 1, &ioByteSize, 0, NULL, NULL);
	//int retval = send(sock, reinterpret_cast<char*>(&sendbuf), sendbuf[0], 0);
	if (SOCKET_ERROR == m_retval) {
		// 비동기 소켓이라 그냥 리턴, 검사 해주어야 함
		if (WSAGetLastError() != WSAEWOULDBLOCK) {
			int err_no = WSAGetLastError();
			error_quit(L"connect()", err_no);
		}
	}
}

void AsynchronousClientClass::sendPacket(const BYTE data_size, const BYTE type, BYTE * data_start_pointer)
{
	// 실제 최대 버퍼 사이즈 보다 데이터 길이가 커지면 안된다.
	if (MAX_BUF_SIZE < (data_size + 2)) {
#ifdef _DEBUG
		// 아래와 같은 에러가 발생하게 된다면, 버퍼 사이즈를 건드리기 보다 실제 데이터 크기를 압축해 줄여 보낼 수 있도록 하자
		printf("[ code LINE %d ] [ code FUNCTION %s ] SendPacket class ERROR :: data size overed MAX_BUF_SIZE\n", __LINE__, __FUNCTION__);
#endif
	}
	else {

		// 패킷 안의 실제 내용 생성
		m_sendbuf[0] = data_size + 2;
		m_sendbuf[1] = type;
		memcpy(&m_sendbuf[2], data_start_pointer, m_sendbuf[0]);

		m_wsa_sendbuf.len = m_sendbuf[0];
		DWORD ioByteSize;
		m_retval = WSASend(m_sock, &m_wsa_sendbuf, 1, &ioByteSize, 0, NULL, NULL);
		if (SOCKET_ERROR == m_retval) {
			// 비동기 소켓이라 그냥 리턴, 검사 해주어야 함
			if (WSAGetLastError() != WSAEWOULDBLOCK) {
				int err_no = WSAGetLastError();
				error_quit(L"connect()", err_no);
			}
		}
	}
}

void AsynchronousClientClass::InputServerIP()
{
#ifdef _DEBUG
	cout << "choose Server to connect.\n\n0. Read Server IP in .txt File\n1. Input Server IP, directly\n\nchoice method : ";
	short cmd{ 0 };
	cin >> cmd;

	if (cmd & 1) { InputServerIP_cin(); }
	else { InputServerIP_ReadtxtFile(); }

	cout << "\n\n\t--==** Connecting Server, Please Wait **==--\n\n\n";
#else
	inputServerIP_ReadtxtFile();
#endif
}

void AsynchronousClientClass::InputServerIP_ReadtxtFile()
{
	ifstream getServerIP;
	getServerIP.open("ServerIP.txt", ios::in);
	getServerIP.getline(m_serverIP, 32);
	getServerIP.close();
}

void AsynchronousClientClass::InputServerIP_cin()
{
	cout << "\nexample 127.0.0.1\nInput Server's IP : ";
	cin >> m_serverIP;
}

int AsynchronousClientClass::recvn()
{
	int received;
	Packet *ptr = m_recvbuf;
	int left = MAX_BUF_SIZE;

	while (left > 0) {
		received = recv(m_sock, reinterpret_cast<char*>(&ptr), left, 0);
		if (SOCKET_ERROR == received)
			return SOCKET_ERROR;
		else if (received == 0)
			break;
		left -= received;
		ptr += received;
	}

	return (MAX_BUF_SIZE - left);
}

void AsynchronousClientClass::error_display(char * msg, int err_no, int line)
{
	WCHAR *lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
#ifdef _DEBUG
	printf("[ %s - %d ]", msg, line);
	wprintf(L"에러 %s\n", lpMsgBuf);
#endif
	LocalFree(lpMsgBuf);
}

void AsynchronousClientClass::error_quit(wchar_t * msg, int err_no)
{
	WCHAR *lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(-1);
}

SOCKET * AsynchronousClientClass::GetServerSocket()
{
	return &m_sock;
}

void AsynchronousClientClass::ProcessPacket(const Packet buf[])
{
	static int id;

	switch (buf[1])
	{
		//	case TEST:
		//#ifdef _DEBUG
		//		cout << "Server is Running. TEST Packet Recived Successfully.\n";
		//#endif //_DEBUG
		//		break;
			/*case KEYINPUT:
				break;*/
	case INIT_CLIENT:
	{
		Ser_PLAYER_DATA* m_pPlayerData = new Ser_PLAYER_DATA;

		m_pPlayerData = reinterpret_cast<Ser_PLAYER_DATA*>(buf[2]);
		id = m_pPlayerData->ID;	//첫 입장시 플레이어 id 값. 여기로 값을 받으니까 요건 좀 이따 설명해야겠따.
		// 여기서 언제 관련 패킷을 보냇엉
		// 여기서 이제 sendpacket해야곘지 ?
		// 너가 굳이 그렇게 짜고 싶으면 그렇게 짜도 되긴하는데
		// 서버에 클라이언트가 접속하자마자, 서버 -> 클라로 초기화 패킷 한번만 보내면
		// 클라이언트는 패킷 한번으로 바로 초기화가 되는데
		// 너가 말한 방법은
		// 서버 접속하고, 클라 -> 서버 ( init 패킷 보내줭 ) 하고
		// 서버가 ㅇㅋ , 서버 -> 클라 ( 초기화 담긴 정보) 보내주고
		// 다시 클라가 받아서 초기화 하는 단계를 여럿 거쳐야 되잖아그러네..

		// 그러니, 그러지 말고, 서버로 클라이언트가 바로 접속하자마자 초기화 패킷을 보내주자 이거지
		// 무슨 말인지는 이해했음?ㅇㅇㅇㅇ 그건 이해가가는데 감이 안잡히네.. 그럼 Send안하고도 서버에서 어떻게 저걸 받아서 인식을 하게되는지 모르겠어.
		// send는 무조건 해야지. 애초에 통신을 해야 데이터를 클라가 받는데?
		// 그래서 저런 식으로 밖에 이해를 못하겠어 ㄷㄷ;

		// 너가 서버에서 패킷 send 할때 어떻게 ㅎ하는데 한번 생각해봥
		// 아까 다시 init_client 서버로 가보자

		/*struct my_int_struct
		{
			int a;
			int b;
			char c;
		};

		struct my_int_struct m_i;
		char buffer[255] = { 0 };
		
		buffer[0] = (char)(char*)&m_i.a;
		buffer[8] = m_i.c;
		*((my_int_struct*)&buffer[0]) = m_i;*/
		// 자 이걸 m_i 구조체 변수를 한번 buffer 배열에 넣어보렴
	}
	break;
	case INIT_OTHER_PLAYER:
	{
		int iA = 0;
		if (m_eSceneID != SCENE_LOGO)
		{
			//내가건 주석
			Ser_PLAYER_DATA* m_pPlayerData = CManagement::GetInstance()->Get_Server_Data(reinterpret_cast<Ser_PLAYER_DATA*>(buf[2])->ID);

			if (nullptr != m_pPlayerData)
			{
				break;
			}
			else
			{
				CGameObject* pGameObject = NULL;

				pGameObject = COtherPlayer::Create(CGraphicDev::GetInstance()->GetGraphicDevice(), CGraphicDev::GetInstance()->GetContext());

				//pGameObject->SetPos(m_pPlayerData->vPos);
				pGameObject->SetPacketData(reinterpret_cast<Ser_PLAYER_DATA*>(buf[2]));

				CScene* pScene = CManagement::GetInstance()->GetScene();
				CLayer* pLayer = pScene->FindLayer(L"Layer_GameLogic");
				pLayer->Ready_Object(L"OtherPlayer", pGameObject);
			}
		}
	}
	case CLIENT_POSITION:

		break;
	case CLIENT_DIRECTION:


		break;
	case PLAYER_DISCONNECTED:

		break;
	}
}
