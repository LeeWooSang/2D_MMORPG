#pragma once

//#define SERVER_IP "127.0.0.1"
#define SERVER_IP "192.168.219.103"
constexpr int SERVER_PORT = 9000;
constexpr int MAX_BUFFER = 1024;

constexpr int WIDTH = 500;
constexpr int HEIGHT = 500;

constexpr int MAX_MONSTER = 2000;
constexpr int MAX_USER = 2000;
constexpr int MAX_OBJECT = MAX_MONSTER + MAX_USER;

constexpr int USER_START_ID = 0;
constexpr int MONSTER_START_ID = MAX_USER;

constexpr int VIEW_DISTANCE = 7;

enum class SERVER_EVENT
{
	RECV = 0,
	SEND = 1,
	MONSTER_MOVE = 2,
	DEFAULT = 999
};

#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib")

struct Over
{
	Over()
	{
		memset(&overlapped, 0, sizeof(WSAOVERLAPPED));
		eventType = SERVER_EVENT::DEFAULT;
		myId = 0;
	}

	WSAOVERLAPPED	overlapped;
	SERVER_EVENT			eventType;
	int								myId;
};

struct OverEx : public Over
{
	OverEx()
		: Over()
	{
		memset(messageBuffer, 0, sizeof(messageBuffer));
		dataBuffer.buf = messageBuffer;
		dataBuffer.len = MAX_BUFFER;

	}

	WSABUF	dataBuffer;
	char			messageBuffer[MAX_BUFFER];
};

enum DIRECTION_TYPE
{
	UP = 0,
	DOWN,
	LEFT,
	RIGHT
};

enum CS_PACKET_TYPE
{
	CS_LOGIN = 0,
	CS_MOVE
};

enum SC_PACKET_TYPE
{
	SC_LOGIN_OK = 0,
	SC_LOGIN_FAIL,
	SC_POSITION,
	SC_ADD_OBJECT,
	SC_REMOVE_OBJECT
};

#pragma pack(push, 1)
// size : 패킷 사이즈
// type : 어떤 패킷인지
struct CSMovePacket
{
	char	size;
	char	type;
	char	direction;
};

struct SCPositionPacket
{
	char size;
	char type;
	int id;
	int x;
	int y;
};

struct SCAddObjectPacket
{
	char size;
	char type;
	int id;
	int x;
	int y;
};

struct SCRemoveObjectPacket
{
	char size;
	char type;
	int id;
};