#pragma once


#define SERVERPORT 9000
#define MAX_BUF_SIZE 256
#define MAX_USER 500

// packet[1] operation
#define DISCONNECTED 0

// iocp buf operation
#define OP_RECV 1
#define OP_SEND 2

// process protocol
enum PacketProtocolType_Server_ProcessPacketFunction {
	TEST = 1,		// ���� ��Ŷ �״�� �����ֱ��. ( ���� ������ �״�� ���ƿ��� �ʴ´ٸ�, Ŭ�� ������ ������ �ִٴ� �� )
	KEYINPUT,		// Ŭ���̾�Ʈ���� Ű �Է��� �޾��� ���

};
using Packet = unsigned char;
