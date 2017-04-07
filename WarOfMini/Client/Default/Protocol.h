#pragma once


#define SERVERPORT 9000
#define MAX_BUF_SIZE 256
#define MAX_USER 500

// packet[1] operation
#define DISCONNECTED 0

// iocp buf operation
#define OP_RECV 1
#define OP_SEND 2


//struct  Ser_InitPlayer
//{
//	BYTE size;
//	BYTE type;
//	int ID;
//	D3DXVECTOR3 vMove;
//	D3DXVECTOR3 vDirection;
//	D3DXVECTOR3 vRadian;
//	int iPlayerstate;
//};

//using position = struct Position
//{
//	float x = { 100 };
//	float y = { 100 };
//	float z = { 100 };
//};

struct Ser_PLAYER_DATA
{
	BYTE size;
	BYTE type;
	int ID;
	XMFLOAT3 vPos;
	XMFLOAT3 vDir;
	// + HP값도 보내줘야함
};




// process protocol
enum PacketProtocolType_Server_ProcessPacketFunction {
	
	TEST = 1,
	KEYINPUT,
	INIT_CLIENT,		 // 클라이언트 객체 생성해주자.
	INIT_OTHER_PLAYER,	 // 다른 플레이어가 들어오게 만든다.
	CLIENT_POSITION,	 // 좌표를 주고받자.
	CLIENT_DIRECTION,	 // direction 값을 받아오자.
	PLAYER_DISCONNECTED, // 연결이 끊기면 삭제시켜주자.
	SEND_POSITION,
	
	/*TEST = 1,
	KEYINPUT,
	POSITION,*/
};


using Packet = unsigned char;
