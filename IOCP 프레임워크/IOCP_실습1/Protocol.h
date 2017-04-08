#pragma once
#pragma comment(lib, "ws2_32")
#include <Winsock2.h>
#include <Windows.h>
#include <iostream>
#include <thread>
#include <vector>
#include <D3DX10math.h>
#include <cstdlib>
#include <xnamath.h>
#include <list>

using namespace std;

#define SERVER_PORT	9000
#define SERVER_IP "127,0,0,1"

#define MAX_BUFFER_SIZE	4000
#define MAX_PACKET_SIZE 255

#define MAX_USER 10

#define OP_RECV 1
#define OP_SEND 2


//const DWORD KEY_UP		= 0x00000000;
//const DWORD KEY_DOWN	= 0x00000001;
//const DWORD KEY_LEFT	= 0x00000002;
//const DWORD KEY_RIGHT	= 0x00000004;

struct Overlap_ex
{
	WSAOVERLAPPED Original_Overlap;
	int operation_type;
	WSABUF wsabuf;
	unsigned char IOCPbuf[MAX_BUFFER_SIZE]; // 바로 요 버퍼를 잡아준거야. -> 이 버퍼로 모든 클라이언트 패킷이 담기도록 공간을 잡기위해 패킷이 여러개 있는고야
};

struct PLAYER_INFO
{
	D3DXVECTOR3 position;
	SOCKET sock;
	bool connected;
	unsigned int prev_received;	// 이것도 이전에 받았던 패킷 size 크기를 담는거고
	unsigned int curr_paket_size;	// 이것도 현재 지금 패킷 size 가 얼만큼 왔나 확인하는 용도고
	Overlap_ex my_overapped;	// 여기안의 IOCPbuf 에 최초 recv 를 하면 
	unsigned char Packetbuf[MAX_PACKET_SIZE]; // 요기 버퍼랑은 무슨 상관일까? -> 너가말한 버퍼를 담는 공간은 여기야
	int id;
	CRITICAL_SECTION cs;
};
//////////////////////////////////////////

struct Ser_PLAYER_DATA
{
	BYTE size;	// 이게 전체 size 이고
	BYTE type;	// 너가 말한 서버의 buf[1] 이 요거고 -> 아까 process packet 에서 구분한 이벤트는 요 type 인거야.
	int ID;	// 이게 클라이언트에게 줄 id 값이야
	XMFLOAT3 vPos;
	//XMFLOAT3 vDir;
};

//클라에서 보내온 정보들을 담아서 프로토콜로 뿌려주기 ?




struct KEY_INPUT
{
	BYTE size;
	BYTE type;
	DWORD movetype;
};
enum ProcessPacket
{
	TEST = 1,
	KEYINPUT,
	INIT_CLIENT,		 // 클라이언트 객체 생성해주자.
	INIT_OTHER_PLAYER,	 // 다른 플레이어가 들어오게 만든다.
	CLIENT_POSITION,	 // 좌표를 주고받자.
	CLIENT_DIRECTION,	 // direction 값을 받아오자.
	PLAYER_DISCONNECTED, // 연결이 끊기면 삭제시켜주자.
	SEND_POSITION,
};

struct Packet_remove_Player
{
	BYTE size;
	BYTE type;
	WORD id;
};

using Packet = unsigned char;
