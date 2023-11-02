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
	void SetPosition(int x, int y) { mX = x; mY = y; }

	std::mutex& GetPosMtx() { return mPosMtx; }
	std::mutex& GetViewListMtx() { return mViewListMtx; }

	virtual void ProcessMove(char dir);
	bool CheckRange(int x, int y);
	bool CheckDistance(int id);

	tbb::concurrent_hash_map<int, int>& GetViewList() { return mViewList; }
	std::unordered_set<int>& GetSTLViewList() { return mSTLViewList; }
	
	int GetChannelIndex()	const { return mChannelIndex; }
	void SetChannelIndex(int index) { mChannelIndex = index; }
	int GetChannel()	const { return mChannel; }
	void SetChannel(int channel) { mChannel = channel; }

protected:
	Over* mOver;
	int mX;
	int mY;
	std::mutex mPosMtx;

	tbb::concurrent_hash_map<int, int> mViewList;
	std::unordered_set<int> mSTLViewList;
	std::mutex mViewListMtx;
	//std::shared_mutex mViewListMtx;

	int mChannelIndex;
	int mChannel;
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
	void PlayerDisconnect();
	bool GetIsConnect()	const { return mConnect; }

	void ProcessLoginViewList();
	void CheckViewList();
	void CheckOldViewList();
	void CheckSTLViewList();

	void ProcessChangeChannelViewList(int oldChannel, int newChannel);

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
	virtual void WakeUp();

	char RandomDirection()	const;
	void ProcessMoveViewList();

	void SetSleep(bool sleep) { mSleep = sleep; }

private:
	bool mSleep;
};