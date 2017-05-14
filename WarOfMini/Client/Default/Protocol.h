#pragma once


#define SERVERPORT 9000


// packet[1] operation
#define DISCONNECTED 0

using namespace std;

#define SERVER_PORT	9000
#define SERVER_IP "127,0,0,1"
#define MAX_BUF_SIZE 256
#define MAX_PACKET_SIZE 255

#define MAX_USER 10

#define OP_RECV 1
#define OP_SEND 2


//const DWORD KEY_UP = 0x00000000;
//const DWORD KEY_DOWN = 0x00000001;
//const DWORD KEY_LEFT = 0x00000002;
//const DWORD KEY_RIGHT = 0x00000004;

struct Overlap_ex
{
	WSAOVERLAPPED Original_Overlap;
	int operation_type;
	WSABUF wsabuf;
	unsigned char IOCPbuf[MAX_BUF_SIZE]; // �ٷ� �� ���۸� ����ذž�. -> �� ���۷� ��� Ŭ���̾�Ʈ ��Ŷ�� ��⵵�� ������ ������� ��Ŷ�� ������ �ִ°��
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
	CRITICAL_SECTION cs;
};
//////////////////////////////////////////

struct Ser_PLAYER_DATA
{
	BYTE size;	// �̰� ��ü size �̰�
	BYTE type;	// �ʰ� ���� ������ buf[1] �� ��Ű� -> �Ʊ� process packet ���� ������ �̺�Ʈ�� �� type �ΰž�.
	int ID;	// �̰� Ŭ���̾�Ʈ���� �� id ���̾�
	XMFLOAT3 vPos;
	XMFLOAT3 vDir;
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
	bool bIsSoldier;//���ٸ�����
};

struct Ser_COLLLAY_DATA
{
	BYTE size;	// �̰� ��ü size �̰�
	BYTE type;	// �ʰ� ���� ������ buf[1] �� ��Ű� -> �Ʊ� process packet ���� ������ �̺�Ʈ�� �� type �ΰž�.
	int ID;	// �̰� Ŭ���̾�Ʈ���� �� id ���̾�
};

struct Ser_Packet_Remove_Player
{
	BYTE size;
	BYTE type;
	WORD id;
	Ser_PLAYER_DATA vecPlayerData[10];
};
//Ŭ�󿡼� ������ �������� ��Ƽ� �������ݷ� �ѷ��ֱ� ?




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
	CLIENT_DIRECTION,	 // direction ���� �޾ƿ���.
	CLIENT_ANIMATION,
	COLLISION_LAY,
	PLAYER_DISCONNECTED, // ������ ����� ������������.
	SEND_POSITION,
};

struct Packet_remove_Player
{
	BYTE size;
	BYTE type;
	WORD id;
};


using Packet = unsigned char;
