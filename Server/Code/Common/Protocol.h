#pragma once
#define LOCAL_MODE

#ifdef LOCAL_MODE
#define SERVER_IP "127.0.0.1"
#else
#define SERVER_IP "192.168.219.103"
#endif 

constexpr int SERVER_PORT = 9000;
constexpr int MAX_BUFFER = 1024;

constexpr int WIDTH = 500;
constexpr int HEIGHT = 500;

// 채널 개수
constexpr int MAX_CHANNEL = 1;
constexpr int MAX_CHANNEL_USER = 400;

// 섹터 크기
constexpr int SECTOR_WIDTH = WIDTH / 10;
constexpr int SECTOR_HEIGHT = HEIGHT / 10;

// 섹터 개수
constexpr int SECTOR_WIDTH_SIZE = WIDTH / SECTOR_WIDTH;
constexpr int SECTOR_HEIGHT_SIZE = HEIGHT / SECTOR_HEIGHT;

constexpr int USER_START_ID = 0;
constexpr int MAX_USER = 2000;

constexpr int MONSTER_START_ID = MAX_USER;
constexpr int SECTOR_MAX_MONSTER = 40;

constexpr int MAX_OBJECT = MAX_USER + (SECTOR_WIDTH_SIZE * SECTOR_HEIGHT_SIZE * SECTOR_MAX_MONSTER);

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

		channel = 0;
		sectorXId = 0;
		sectorYId = 0;
	}

	WSAOVERLAPPED	overlapped;
	SERVER_EVENT			eventType;
	int					myId;

	int					channel;
	int					sectorXId;
	int					sectorYId;
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
	CS_MOVE,
	CS_CHANGE_CHANNEL,
	CS_BROADCASTING_CHAT,
	CS_WHISPERING_CHAT,
	CS_ATTACK,
	CS_CHANGE_AVATAR,
	CS_REQUEST_TRADE,
	CS_TRADE
};

enum SC_PACKET_TYPE
{
	SC_LOGIN_OK = 0,
	SC_LOGIN_FAIL,
	SC_POSITION,
	SC_ADD_OBJECT,
	SC_REMOVE_OBJECT,
	SC_CHANGE_CHANNEL,
	SC_CHAT,
	SC_CHANGE_AVATAR,
	SC_REQUEST_TRADE,
	SC_TRADE,
	SC_TRADE_POST_PROCESSING
};

#pragma pack(push, 1)
// size : 패킷 사이즈
// type : 어떤 패킷인지
constexpr int MAX_LOGIN_ID_LENGTH = 20;
constexpr int MAX_LOGIN_PASSWORD_LENGTH = 30;
constexpr int MAX_CHAT_LENGTH = 50;
struct CSLoginPacket
{
	char size;
	char type;
	char loginId[MAX_LOGIN_ID_LENGTH];
	char loginPassword[MAX_LOGIN_PASSWORD_LENGTH];
};
struct CSMovePacket
{
	char	size;
	char	type;
	char	direction;
};
struct CSChangeChannelPacket
{
	char	size;
	char	type;
	char channel;
};
struct CSBroadcastingChatPacket
{
	char size;
	char type;
	wchar_t chat[MAX_CHAT_LENGTH];
};
struct CSWhisperingChatPacket
{
	char size;
	char type;
	int id;
	wchar_t chat[MAX_CHAT_LENGTH];
};
struct CSAttackPacket
{
	char size;
	char type;
};
struct CSChangeAvatarPacket
{
	char size;
	char type;
	// 슬롯번호
	char slotType;
	int texId;
};
struct CSRequestTradePacket
{
	char size;
	char type;
	int id;
};
struct CSTradePacket
{
	char size;
	char type;
	int id;
	int items[9];
};
///////////////////////////////////////////////////
struct SCLoginOkPacket
{
	char size;
	char type;
	int id;
	int channel;
};
struct SCLoginFailPacket
{
	char size;
	char type;
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
	int texId;
};
struct SCRemoveObjectPacket
{
	char size;
	char type;
	int id;
};
struct SCChangeChannelPacket
{
	char size;
	char type;
	bool result;
};
struct SCChatPacket
{
	char size;
	char type;
	int id;
	wchar_t chat[MAX_CHAT_LENGTH];
};
struct SCChangeAvatarPacket
{
	char size;
	char type;
	int id;
	int texId;
};
struct SCRequestTradePacket
{
	char size;
	char type;
	int id;
};
struct SCTradePacket
{
	char size;
	char type;
	int id;
	int items[9];
};
struct SCTradePostProcessingPacket
{
	char size;
	char type;
};