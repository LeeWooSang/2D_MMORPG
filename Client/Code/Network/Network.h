#pragma once
#include "../Common/Macro.h"
#include <string>
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

	void SendLoginPacket(const std::string& loginId, const std::string& loginPassword);
	void SendMovePacket(char dir);
	void SendChangeChannelPacket(char channel);
	void SendChatPacket(const std::wstring& chat);
	void SendAttackPacket();
	void SendChangeAvatarPacket(char slotType, int texId);
private:
	void processPacket();
	void recvPacket();
	void sendPacket(char* packet);

private:
	SOCKET mSocket;
	NetworkBuffer mSendBuffer;
	RecvBuffer mRecvBuffer;
	char mPacketBuffer[MAX_BUFFER];
	static int myId;
};