#pragma once
#include "../Common/Defines.h"

struct OverEx
{
	WSAOVERLAPPED	overlapped;
	WSABUF					dataBuffer;
	char							messageBuffer[MAX_BUFFER];
	SERVER_EVENT			eventType;
	int								myId;
};

class Character
{
public:
	Character();
	virtual ~Character();

	OverEx* GetOverEx() { return mOverEx; }

protected:
	OverEx* mOverEx;
};

class Player : public Character
{
public:
	Player();
	~Player();

	SOCKET GetSocket()	const { return mSocket; }
	void SetSocket(const SOCKET& socket) { mSocket = socket; }

	void SetPacketBuf(const char* packet, int required)	{ memcpy(mPacketBuf + mPrevSize, packet, required); }
	char* GetPacketBuf() { return mPacketBuf; }

	int GetPrevSize()	const { return mPrevSize; }
	void SetPrevSize(int size) { mPrevSize = size; }

	void PlayerConnect() { mConnect = true; }
	bool GetIsConnect()	const { return mConnect; }

private:
	SOCKET mSocket;
	char mPacketBuf[MAX_BUFFER];
	int	 mPrevSize;
	int	 mSendBytes;
	bool mConnect;
};