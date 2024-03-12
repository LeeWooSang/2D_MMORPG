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

public:
	void SendServerSelect(GAME_SERVER_TYPE type);

	void SendLoginPacket(const std::string& loginId, const std::string& loginPassword);
	void SendChannelLoginPacket(int channel);

	void SendMovePacket(char dir);
	void SendChangeChannelPacket(int channel);
	void SendBroadCastingChatPacket(const std::wstring& chat);
	void SendWhisperingChatPacket(int id, const std::wstring& chat);

	void SendAttackPacket();
	void SendChangeAvatarPacket(char slotType, int texId);
	void SendTakeOffEquipItem(char slotType);

	void SendRequestTradePacket(int id);
	void SendAddTradeItem(int id, int texId, int slotNum);
	void SendAddTradeMeso(int id, long long meso);
	void SendTradePacket(int id, int* items, long long meso);
	void SendTradeCancelPacket(int id);

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