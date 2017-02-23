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
	D3DXVECTOR3 vPos;
	D3DXVECTOR3 vDir;
	// + HP���� ���������
};




// process protocol
enum PacketProtocolType_Server_ProcessPacketFunction {
	TEST = 1,			 // ���� ��Ŷ �״�� �����ֱ��. ( ���� ������ �״�� ���ƿ��� �ʴ´ٸ�, Ŭ�� ������ ������ �ִٴ� �� )
	KEYINPUT,			 // Ŭ���̾�Ʈ���� Ű �Է��� �޾��� ���
	INIT_CLIENT,		 // Ŭ���̾�Ʈ ��ü ����������.
	INIT_OTHER_PLAYER,	 // �ٸ� �÷��̾ ������ �����.
	CLIENT_POSITION,	 // ��ǥ�� �ְ����.
	CLIENT_DIRECTION,	 // direction ���� �޾ƿ���.
	PLAYER_DISCONNECTED, // ������ ����� ������������.
};


using Packet = unsigned char;
