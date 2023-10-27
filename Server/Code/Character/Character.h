#pragma once
#include "../Common/Defines.h"
#include "../Common/Protocol.h"
#include <mutex>
#include <shared_mutex>

class Character
{
public:
	Character();
	virtual ~Character();
	virtual void Reset();
	virtual bool Inititalize(int id);

	Over* GetOver() { return mOver; }
	int	 GetX()	const { return mX; }
	int	 GetY()	const { return mY; }
	std::mutex& GetPosMtx() { return mPosMtx; }

	virtual void ProcessMove(char dir);
	bool CheckRange(int x, int y);
	bool CheckDistance(int id);
	void CheckViewList();
	void CheckOldViewList();

	tbb::concurrent_hash_map<int, int>& GetViewList() { return mViewList; }

protected:
	Over* mOver;
	int mX;
	int mY;
	std::mutex mPosMtx;

	tbb::concurrent_hash_map<int, int> mViewList;
	std::mutex mViewListMtx;
	//std::shared_mutex mViewListMtx;
};

class Player : public Character
{
public:
	Player();
	~Player();
	virtual void Reset();
	virtual bool Inititalize(int id);

	SOCKET GetSocket()	const { return mSocket; }
	void SetSocket(const SOCKET& socket) { mSocket = socket; }

	void SetPacketBuf(const char* packet, int required)	{ memcpy(mPacketBuf + mPrevSize, packet, required); }
	char* GetPacketBuf() { return mPacketBuf; }

	int GetPrevSize()	const { return mPrevSize; }
	void SetPrevSize(int size) { mPrevSize = size; }

	void PlayerConnect() { mConnect = true; }
	void PlayerDisconnect() { Reset(); }
	bool GetIsConnect()	const { return mConnect; }

private:
	SOCKET mSocket;
	char mPacketBuf[MAX_BUFFER];
	int	 mPrevSize;
	int	 mSendBytes;
	bool mConnect;
};

class Monster : public Character
{
public:
	Monster();
	~Monster();
	virtual void Reset();
	virtual bool Inititalize(int id);

private:
	bool mSleep;
};