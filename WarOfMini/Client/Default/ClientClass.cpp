#include "stdafx.h"
#include "Include.h"
#include "Management.h"
#include "Scene.h"
#include "OtherPlayer.h"
#include "GraphicDev.h"
#include "Player.h"
#include "Bomb.h"
#include "Station.h"

int		g_myid;
XMFLOAT3 g_vPos;

AsynchronousClientClass::AsynchronousClientClass()
{
	ZeroMemory(&m_time, sizeof(Ser_Time_DATA));
	ZeroMemory(&m_state, sizeof(Ser_ANIMATION_DATA));
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
	serveraddr.sin_port = htons(SERVER_PORT);

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
	m_wsa_recvbuf.len = MAX_BUFFER_SIZE;
	m_wsa_sendbuf.buf = reinterpret_cast<CHAR*>(m_sendbuf);
	m_wsa_sendbuf.len = MAX_BUFFER_SIZE;
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
				if (buf[0] > MAX_BUFFER_SIZE) {
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
			error_quit(L"Sendpacket_TEST()", err_no);
		}
	}
}

void AsynchronousClientClass::sendPacket(const BYTE data_size, const BYTE type, BYTE * data_start_pointer)
{
	// 실제 최대 버퍼 사이즈 보다 데이터 길이가 커지면 안된다.
	if (MAX_BUFFER_SIZE < (data_size + 2)) {
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
	InputServerIP_ReadtxtFile();
#endif
}

void AsynchronousClientClass::InputServerIP_ReadtxtFile()
{
	ifstream getServerIP;
	getServerIP.open("../ServerIP.txt", ios::in);
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
	int left = MAX_BUFFER_SIZE;

	while (left > 0) {
		received = recv(m_sock, reinterpret_cast<char*>(&ptr), left, 0);
		if (SOCKET_ERROR == received)
			return SOCKET_ERROR;
		else if (received == 0)
			break;
		left -= received;
		ptr += received;
	}

	return (MAX_BUFFER_SIZE - left);
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
	static bool first_time = true;
	int id = 0;

	CScene* pScene = NULL;
	CLayer* pLayer = NULL;
	CGameObject* pGameObject = NULL;


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
		m_pPlayerData = reinterpret_cast<Ser_PLAYER_DATA*>((Ser_PLAYER_DATA*)buf);

		//첫 입장시 플레이어 id 값. 

		id = m_pPlayerData->ID;

		//		sendPacket(sizeof(Ser_PLAYER_DATA), INIT_CLIENT, reinterpret_cast<BYTE*>(&id));

		if (first_time)
		{
			first_time = false;
			g_myid = id;
			g_vPos = m_pPlayerData->vPos;
		}
		else
		{
			CGameObject* pGameObject = NULL;
			pGameObject = COtherPlayer::Create(CGraphicDev::GetInstance()->GetGraphicDevice(), CGraphicDev::GetInstance()->GetContext(), m_pPlayerData->vPos, m_pPlayerData->ID);

			pScene = CManagement::GetInstance()->GetScene();

			((COtherPlayer*)pGameObject)->BuildObject(pScene->GetPhysicsSDK(), pScene->GetPhysicsScene(), pScene->GetPxMaterial(), pScene->GetPxControllerManager());

			pLayer = pScene->FindLayer(L"Layer_GameLogic");
			pLayer->Ready_Object(L"OtherPlayer", pGameObject);
		}

	}
	break;
	case INIT_OTHER_PLAYER:
	{
		Ser_Vec_PLAYER_DATA vecPlayerData = *reinterpret_cast<Ser_Vec_PLAYER_DATA*>((Ser_Vec_PLAYER_DATA*)buf);


		if (g_myid == vecPlayerData.ID)
		{
			for (int i = 0; i < vecPlayerData.PlayerSize; ++i)
			{
				if (g_myid == vecPlayerData.vecPlayerData[i].ID)
					continue;
				pGameObject = COtherPlayer::Create(CGraphicDev::GetInstance()->GetGraphicDevice(), CGraphicDev::GetInstance()->GetContext(), vecPlayerData.vecPlayerData[i].vPos, vecPlayerData.vecPlayerData[i].ID);

				pScene = CManagement::GetInstance()->GetScene();

				((COtherPlayer*)pGameObject)->BuildObject(pScene->GetPhysicsSDK(), pScene->GetPhysicsScene(), pScene->GetPxMaterial(), pScene->GetPxControllerManager());


				pLayer = pScene->FindLayer(L"Layer_GameLogic");
				pLayer->Ready_Object(L"OtherPlayer", pGameObject);
			}
		}
	}
	break;
	case CLIENT_POSITION:
	{
		m_pPlayerData = reinterpret_cast<Ser_PLAYER_DATA*>((Ser_PLAYER_DATA*)buf);

		pScene = CManagement::GetInstance()->GetScene();
		pLayer = pScene->FindLayer(L"Layer_GameLogic");
		list<CGameObject*>* pObjList = pLayer->Find_ObjectList(L"OtherPlayer");
		if (pObjList == NULL)
			break;

		list<CGameObject*>::iterator iter = pObjList->begin();
		list<CGameObject*>::iterator iter_end = pObjList->end();

		for (iter; iter != iter_end; ++iter)
		{
			if (((COtherPlayer*)*iter)->GetID() == m_pPlayerData->ID)
			{
				((COtherPlayer*)*iter)->SetPlayerData(m_pPlayerData->vPos, m_pPlayerData->vDir);
				if (m_pPlayerData->strColllayData.bShoot)
				{
					CGameObject* pGameObject = CBomb::Create(CGraphicDev::GetInstance()->GetContext());
					pGameObject->SetTransformPosition(m_pPlayerData->strColllayData.xmf3CollPos);
					pLayer->Ready_Object(L"Effect", pGameObject);
					if (g_myid == m_pPlayerData->strColllayData.iCollPlayerID)
					{
						list<CGameObject*>* pObjList = pLayer->Find_ObjectList(L"Player");
						list<CGameObject*>::iterator iter = pObjList->begin();
						((CPlayer*)*iter)->SetHP();
					}
				}
			}
		}
	}
	break;
	case CLIENT_DIRECTION:
		break;
	case CLIENT_ANIMATION:
	{
		Ser_ANIMATION_DATA AnimationData = *reinterpret_cast<Ser_ANIMATION_DATA*>((Ser_ANIMATION_DATA*)buf);

		pScene = CManagement::GetInstance()->GetScene();
		pLayer = pScene->FindLayer(L"Layer_GameLogic");
		list<CGameObject*>* pObjList = pLayer->Find_ObjectList(L"OtherPlayer");
		if (pObjList == NULL)
			break;

		list<CGameObject*>::iterator iter = pObjList->begin();
		list<CGameObject*>::iterator iter_end = pObjList->end();

		for (iter; iter != iter_end; ++iter)
		{
			if (((COtherPlayer*)*iter)->GetID() == AnimationData.ID)
			{
				if (((COtherPlayer*)*iter)->IsSoldier() == AnimationData.bIsSoldier)
					((COtherPlayer*)*iter)->PlayAnimation(AnimationData.dwAniIdx, AnimationData.bImmediate);
				else
					((COtherPlayer*)*iter)->SoldierChange();
			}
		}
	}
	break;
	case COLLISION_LAY:
	{
		/*Ser_COLLLAY_DATA strCollData = *reinterpret_cast<Ser_COLLLAY_DATA*>((Ser_COLLLAY_DATA*)buf);

		pScene = CManagement::GetInstance()->GetScene();
		pLayer = pScene->FindLayer(L"Layer_GameLogic");
		if (g_myid == strCollData.ID)
		{
			list<CGameObject*>* pObjList = pLayer->Find_ObjectList(L"Player");
			list<CGameObject*>::iterator iter = pObjList->begin();
			((CPlayer*)*iter)->SetHP();
		}

		CGameObject* pGameObject = CBomb::Create(CGraphicDev::GetInstance()->GetContext());
		pGameObject->SetTransformPosition(strCollData.xmf3CollPos);
		pLayer->Ready_Object(L"Effect", pGameObject);*/
	}
	break;
	case INGAME_CUR_STATION:
	{
		Ser_CurStation_DATA curStationdata = *reinterpret_cast<Ser_CurStation_DATA*>((Ser_CurStation_DATA*)buf);

		pScene = CManagement::GetInstance()->GetScene();
		pLayer = pScene->FindLayer(L"Layer_GameLogic");
		list<CGameObject*>* pObjList = pLayer->Find_ObjectList(L"Station");
		if (pObjList == NULL)
			break;
		
		list<CGameObject*>::iterator iter = pObjList->begin();
		list<CGameObject*>::iterator iter_end = pObjList->end();
		for (int i = 0; iter != iter_end; ++iter, ++i)
		{//
			((CStation*)*iter)->SerSetStation(curStationdata.station[i].flagState, curStationdata.station[i].fTime);
		}
	}
	break;
	case TIMECOUNT:
	{
		Ser_Time_DATA timedata = *reinterpret_cast<Ser_Time_DATA*>((Ser_Time_DATA*)buf);
		m_time = &timedata;

		cout << "TimeCount : " << m_time->time << endl;
	}
	break;
	case CLIENT_READY:
	{

	}
	break;
	case CLIENT_STAGE:
	{

	}
	break;
	case PLAYER_DISCONNECTED:
	{
		Ser_Packet_Remove_Player PlayerRemove = *reinterpret_cast<Ser_Packet_Remove_Player*>((Ser_Packet_Remove_Player*)buf);

		pScene = CManagement::GetInstance()->GetScene();
		pLayer = pScene->FindLayer(L"Layer_GameLogic");
		list<CGameObject*>* pObjList = pLayer->Find_ObjectList(L"OtherPlayer");
		list<CGameObject*>::iterator iter = pObjList->begin();
		list<CGameObject*>::iterator iter_end = pObjList->end();

		for (iter; iter != iter_end; ++iter)
		{
			if (((COtherPlayer*)*iter)->GetID() == PlayerRemove.id)
			{
				Safe_Release(*iter);
				pObjList->erase(iter);
				break;
			}
		}

		//	list<CGameObject*>* pObjList = pLayer->Find_ObjectList(L"")
	}
	break;
	}
}
