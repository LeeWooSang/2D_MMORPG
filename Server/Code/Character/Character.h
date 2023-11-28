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
	virtual bool CheckRange(int x, int y);

	bool CheckDistance(int x, int y);

	tbb::concurrent_hash_map<int, int>& GetViewList() { return mViewList; }
	std::unordered_set<int>& GetSTLViewList() { return mSTLViewList; }
	
	int GetChannelIndex()	const { return mChannelIndex; }
	void SetChannelIndex(int index) { mChannelIndex = index; }
	int GetChannel()	const { return mChannel; }
	void SetChannel(int channel) { mChannel = channel; }

	void ProcessChangeSector(int oldX, int oldY, int newX, int newY);

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
	volatile int mChannel;
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

	void ProcessChangeChannelViewList(int oldChannel, int newChannel);
	void ProcessChat(wchar_t* chat);
	void ProcessAttack();

private:
	SOCKET mSocket;
	char mPacketBuf[MAX_BUFFER];
	int	 mPrevSize;
	int	 mSendBytes;
	bool mConnect;
};

enum class MONSTER_STATE
{
	SLEEP,
	READY,
	MOVE
};
class Monster : public Character
{
public:
	Monster();
	~Monster();
	virtual void Reset();
	virtual bool Inititalize(int id);

	virtual void ProcessMove(char dir);
	virtual bool CheckRange(int x, int y);

	virtual void WakeUp();
	void MoveEvent();

	char RandomDirection()	const;
	void ProcessMoveViewList();

	void SetRange(std::pair<int, int> min, std::pair<int, int> max) { mRangeMin = min; mRangeMax = max; }
	
	MONSTER_STATE GetState()		const { return mState; }
	void SetState(MONSTER_STATE state) { mState = state; }
	void SetSectorId(std::pair<int, int> sectorId) { mSectorXId = sectorId.first, mSectorYId = sectorId.second; }
	void SetTargetId(int id) { mTargetId = id; }

	bool CheckCollision(int x, int y);

private:
	std::pair<int, int> mRangeMin;
	std::pair<int, int> mRangeMax;

	int mSectorXId;
	int mSectorYId;

	MONSTER_STATE mState;

	int mTargetId;
};