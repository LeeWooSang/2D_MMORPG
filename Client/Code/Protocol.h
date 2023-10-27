#pragma once

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 3500

// 최대 동접자 수
constexpr int MAX_USER = 1000;
// NPC ID 시작 번호 
constexpr int NPC_ID_START = 1000;
// NPC 개수
constexpr int NUM_NPC = 100000;
// 메세지 문자열 최대 길이
constexpr int MAX_STR_LEN = 50;

// 키 매크로
#define CS_UP			1 
#define CS_DOWN	2 
#define CS_LEFT		3 
#define CS_RIGHT	4

// 플레이어 로그인
#define SC_LOGIN_OK				1
// 플레이어를 게임에 배치해라
#define SC_PUT_PLAYER			2
// 나간 플레이어
#define SC_REMOVE_PLAYER	3
// 플레이어 위치
#define SC_POS						4  

#define SC_CHAT						5

#pragma pack(push, 1)

// 클라 -> 서버
struct CS_Packet_Up
{
	char	size;
	char type;
};
struct CS_Packet_Down
{
	char	size;
	char type;
};
struct CS_Packet_Left
{
	char	size;
	char type;
};
struct CS_Packet_Right
{
	char	size;
	char type;
};

// 서버 -> 클라

struct SC_Packet_Login_OK
{
	char size;
	char type;
	// 로그인 아이디
	char id;
};
struct SC_Packet_Put_Player
{
	char size;
	char type;
	int id;
	// 플레이어 좌표
	int x, y;
};
struct SC_Packet_Remove_Player
{
	char size;
	char type;
	int id;
};
struct SC_Packet_Pos
{
	char size;
	char type;
	int id;
	int x, y;
};
struct SC_Packet_Chat
{
	char size;
	char type;
	int id;
	wchar_t message[MAX_STR_LEN];
};
#pragma pack(pop)