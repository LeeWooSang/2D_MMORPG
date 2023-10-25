#pragma once

#define SERVER_IP "127.0.0.1"
constexpr int SERVER_PORT = 9000;
constexpr int MAX_USER = 3000;
constexpr int MAX_BUFFER = 1024;

enum class SERVER_EVENT
{
	RECV = 0,
	SEND = 1,
	DEFAULT = 999
};

#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib")

struct OverEx
{
	OverEx()
	{
		memset(&overlapped, 0, sizeof(WSAOVERLAPPED));
		memset(messageBuffer, 0, sizeof(messageBuffer));
		dataBuffer.buf = messageBuffer;
		dataBuffer.len = MAX_BUFFER;
		eventType = SERVER_EVENT::DEFAULT;
		myId = 0;
	}
	WSAOVERLAPPED	overlapped;
	WSABUF					dataBuffer;
	char							messageBuffer[MAX_BUFFER];
	SERVER_EVENT			eventType;
	int								myId;
};