#pragma once
#pragma comment(lib, "ws2_32")
#include <Winsock2.h>
#include <Windows.h>
#include <iostream>
#include <thread>
#include <vector>
#include <D3DX10math.h>
#include<cstdlib>


using namespace std;

#define SERVER_PORT	9000
#define SERVER_IP "127,0,0,1"

#define MAX_BUFFER_SIZE	4000
#define MAX_PACKET_SIZE 255

#define MAX_USER 10

#define OP_RECV 1
#define OP_SEND 2


const DWORD KEY_UP		= 0x00000000;
const DWORD KEY_DOWN	= 0x00000001;
const DWORD KEY_LEFT	= 0x00000002;
const DWORD KEY_RIGHT	= 0x00000004;

struct Overlap_ex
{
	WSAOVERLAPPED Original_Overlap;
	int operation_type;
	WSABUF wsabuf;
	unsigned char IOCPbuf[MAX_BUFFER_SIZE];
	unsigned char Packetbuf[MAX_PACKET_SIZE];
};

struct PLAYER_INFO
{
	D3DXVECTOR3 position;
	SOCKET sock;
	bool connected;
	unsigned int prev_received;
	unsigned int curr_paket_size;
	Overlap_ex my_overapped;
	unsigned char Packetbuf[MAX_PACKET_SIZE];
	int id;
	CRITICAL_SECTION cs;
};
//////////////////////////////////////////


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
};

struct Packet_remove_Player
{
	BYTE size;
	BYTE type;
	WORD id;
};
