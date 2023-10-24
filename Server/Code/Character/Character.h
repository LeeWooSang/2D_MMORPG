#pragma once
#include "../Common/Protocol.h"

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