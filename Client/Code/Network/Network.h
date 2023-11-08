#pragma once
#include "../Common/Macro.h"
#include "../Common/Defines.h"
#include "../../../Server/Code/Common/Protocol.h"

struct NetworkBuffer
{
	NetworkBuffer()
	{
		memset(buffer, 0, sizeof(buffer));
		wsaBuffer.len = MAX_BUFFER;
		wsaBuffer.buf = buffer;
	}

	WSABUF	wsaBuffer;
	char 			buffer[MAX_BUFFER];
};
struct RecvBuffer : public NetworkBuffer
{
	RecvBuffer()
		: NetworkBuffer(), prevSize(0) {}
	DWORD	packetSize = 0;
	int				prevSize = 0;
};

class Network
{
	SINGLE_TONE(Network)

public:
	bool Initialize(const HWND& handle);

	void PreocessNetwork(unsigned long long wparam, long long lparam);
	void SendMovePacket(char dir);
	void SendChangeChannel(char channel);

private:
	void processPacket();
	void recvPacket();
	void sendPacket(char* packet);

private:
	SOCKET mSocket;
	NetworkBuffer mSendBuffer;
	RecvBuffer mRecvBuffer;
	char mPacketBuffer[MAX_BUFFER];
};