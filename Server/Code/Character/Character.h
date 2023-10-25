#pragma once
#include "../Common/Defines.h"
#include "../Common/Protocol.h"
#include <mutex>

class Character
{
public:
	Character();
	virtual ~Character();
	virtual void Reset();
	virtual bool Inititalize();

	Over* GetOver() { return mOver; }
	int	 GetX()	const { return mX; }
	int	 GetY()	const { return mY; }

protected:
	Over* mOver;
	int mX;
	int mY;
	std::mutex mCharacterMtx;
};

class Player : public Character
{
public:
	Player();
	~Player();
	virtual void Reset();
	virtual bool Inititalize();

	SOCKET GetSocket()	const { return mSocket; }
	void SetSocket(const SOCKET& socket) { mSocket = socket; }

	void SetPacketBuf(const char* packet, int required)	{ memcpy(mPacketBuf + mPrevSize, packet, required); }
	char* GetPacketBuf() { return mPacketBuf; }

	int GetPrevSize()	const { return mPrevSize; }
	void SetPrevSize(int size) { mPrevSize = size; }

	void PlayerConnect() { mConnect = true; }
	void PlayerDisconnect() { mConnect = false; }
	bool GetIsConnect()	const { return mConnect; }

	void ProcessMove(char dir);

private:
	SOCKET mSocket;
	char mPacketBuf[MAX_BUFFER];
	int	 mPrevSize;
	int	 mSendBytes;
	bool mConnect;
};