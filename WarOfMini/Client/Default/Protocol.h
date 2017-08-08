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
#include <mutex>
#include <chrono>
#include <queue>




using namespace std;
using namespace chrono;


// packet[1] operation
#define DISCONNECTED 0

#define SERVER_PORT	9000
#define SERVER_IP "127,0,0,1"

#define MAX_BUFFER_SIZE	4000
#define MAX_PACKET_SIZE 255

#define MAX_USER 9

#define OP_RECV 1
#define OP_SEND 2
#define OP_TIME 3



// Timer Event Define !


#define EV_GAMECOUNT 0
#define EV_SUDDENDETH 1
#define EV_GAMEEXITCOUNT 2
//#define EV_GAMECOUNT 3
//#define EV_GAMECOUNT 4
//#define EV_GAMECOUNT 5


// Game State !
#define SC_LOGO					0
#define SC_STAGE				1
#define SC_END					2

// Game State !
#define GS_READY				0
#define GS_START				1
#define GS_END					2

struct Overlap_ex
{
	WSAOVERLAPPED Original_Overlap;
	int operation_type;
	WSABUF wsabuf;
	unsigned char IOCPbuf[MAX_BUFFER_SIZE]; // �ٷ� �� ���۸� ����ذž�. -> �� ���۷� ��� Ŭ���̾�Ʈ ��Ŷ�� ��⵵�� ������ ������� ��Ŷ�� ������ �ִ°��
};

struct PLAYER_INFO
{
	D3DXVECTOR3 position;
	SOCKET sock;
	bool connected;
	unsigned int prev_received;	// �̰͵� ������ �޾Ҵ� ��Ŷ size ũ�⸦ ��°Ű�
	unsigned int curr_paket_size;	// �̰͵� ���� ���� ��Ŷ size �� ��ŭ �Գ� Ȯ���ϴ� �뵵��
	Overlap_ex my_overapped;	// ������� IOCPbuf �� ���� recv �� �ϸ� 
	unsigned char Packetbuf[MAX_PACKET_SIZE]; // ��� ���۶��� ���� ����ϱ�? -> �ʰ����� ���۸� ��� ������ �����
	int id;
	bool m_bRedBlue;
	CRITICAL_SECTION cs;
};
//////////////////////////////////////////

struct Ser_COLLLAY_DATA
{
	BYTE size;	// �̰� ��ü size �̰�
	BYTE type;	// �ʰ� ���� ������ buf[1] �� ��Ű� -> �Ʊ� process packet ���� ������ �̺�Ʈ�� �� type �ΰž�.
	int iCollPlayerID;	// �̰� Ŭ���̾�Ʈ���� �� id ���̾�
	XMFLOAT3 xmf3CollPos;
};

struct Ser_PLAYER_DATA
{
	BYTE size;	// �̰� ��ü size �̰�
	BYTE type;	// �ʰ� ���� ������ buf[1] �� ��Ű� -> �Ʊ� process packet ���� ������ �̺�Ʈ�� �� type �ΰž�.
	int ID;	// �̰� Ŭ���̾�Ʈ���� �� id ���̾�
	BYTE SC_ID;
	XMFLOAT3 vPos;
	float	fAngle;
	short	sBitKey;
	short	sHP;
	//Ser_COLLLAY_DATA strColllayData;
};

struct Ser_SEND_PLAYER_DATA
{
	BYTE size;	// �̰� ��ü size �̰�
	BYTE type;	// �ʰ� ���� ������ buf[1] �� ��Ű� -> �Ʊ� process packet ���� ������ �̺�Ʈ�� �� type �ΰž�.
	int ID;	// �̰� Ŭ���̾�Ʈ���� �� id ���̾�
	float	fAngle;
	short	sBitKey;
	XMFLOAT3 xmf3CollPos;
	short	sHP;
};

struct Ser_Position
{
	int ID;	// �̰� Ŭ���̾�Ʈ���� �� id ���̾�
	float	fAngle;
	XMFLOAT3 vPos;
};
struct Ser_SEND_CHECK_PLAYER_DATA
{
	BYTE size;	// �̰� ��ü size �̰�
	BYTE type;	// �ʰ� ���� ������ buf[1] �� ��Ű� -> �Ʊ� process packet ���� ������ �̺�Ʈ�� �� type �ΰž�.
	BYTE PlayerSize;
	Ser_Position vecPositionData[10];
};

struct Ser_Vec_PLAYER_DATA
{
	BYTE size;	// �̰� ��ü size �̰�
	BYTE type;	// �ʰ� ���� ������ buf[1] �� ��Ű� -> �Ʊ� process packet ���� ������ �̺�Ʈ�� �� type �ΰž�.
	int ID;	// �̰� Ŭ���̾�Ʈ���� �� id ���̾�
	BYTE PlayerSize;
	Ser_PLAYER_DATA vecPlayerData[10];

};

struct Ser_ANIMATION_DATA
{
	BYTE size;	// �̰� ��ü size �̰�
	BYTE type;	// �ʰ� ���� ������ buf[1] �� ��Ű� -> �Ʊ� process packet ���� ������ �̺�Ʈ�� �� type �ΰž�.
	int ID;	// �̰� Ŭ���̾�Ʈ���� �� id ���̾�
	bool bImmediate;
	DWORD dwAniIdx;
	bool bIsSoldier;
};

struct Ser_Time_DATA {
	BYTE size;
	BYTE type;
	BYTE gamestate;					// 0, 1, 2, 3
	float time;						//�ð� float��
};

struct Ser_Station_DATA
{
	BYTE size;
	BYTE type;
	int team;
	bool occupation;//true �����Ϸ���-false-�ƴ�
	BYTE stationID;
};
struct strStation
{
	BYTE stationID;
	BYTE flagState;	//���� ��� ���� 0 X, 1 1��, 2 2��
	BYTE ATeamCnt;
	BYTE BTeamCnt;
	float fTime;
};
struct Ser_CurStation_DATA
{
	BYTE size;
	BYTE type;
	strStation station[3];
};

struct Ser_Packet_Remove_Player
{
	BYTE size;
	BYTE type;
	WORD id;
};

struct Ser_STATE_DATA {				// ���� ��ü ����
	BYTE size;
	BYTE type;						// 1
	BYTE gamestate;					// 0, 1, 2, 3
	BYTE timecount;					// 4, 3, 2, 1, 
};

struct Ser_EscapeGame
{
	BYTE size;
	BYTE type;						// 1
	WORD id;
};
////////////////////////////////////////////////Ÿ�̸�

struct event_type {
	int obj_id;
	int do_event;
	unsigned int wakeup_time;
	int event_id;
};

class mycomp
{
public:
	mycomp() {}
	bool operator() (const event_type lhs, const event_type rhs) const
	{
		return (lhs.wakeup_time > rhs.wakeup_time);
	}
};



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
	INIT_CLIENT,		 // Ŭ���̾�Ʈ ��ü ����������.
	INIT_OTHER_PLAYER,	 // �ٸ� �÷��̾ ������ �����.
	CLIENT_POSITION,	 // ��ǥ�� �ְ����.
	CLIENT_SEND_POSITION,
	CLIENT_DIRECTION,	 // direction ���� �޾ƿ���.
	CLIENT_ANIMATION,
	COLLISION_LAY,
	INGAME_STATION,
	INGAME_CUR_STATION,
	TIMECOUNT,
	CLIENT_READY,
	CLIENT_STAGE,
	PLAYER_DISCONNECTED, // ������ ����� ������������.
	SEND_POSITION,
};


using Packet = unsigned char;
