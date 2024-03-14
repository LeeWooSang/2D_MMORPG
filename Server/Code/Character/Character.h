#pragma once
#include "../Common/Defines.h"
#include "../Common/Protocol.h"
#include <mutex>
#include <shared_mutex>

enum class ANIMATION_MOTION_TYPE
{
	IDLE,
	WALK,
	JUMP
};

class Character
{
public:
	Character();
	virtual ~Character();
	virtual void Reset();
	virtual bool Inititalize(int id);

	Over* GetOver() { return mOver; }
	
	char GetDirection()	const { return mDirection; }
	void SetDirection(char dir) { mDirection = dir; }

	int	 GetX()	const { return mX; }
	int	 GetY()	const { return mY; }
	void SetPosition(int x, int y) { mX = x; mY = y; }

	ANIMATION_MOTION_TYPE GetAnimationType()	const { return mAnimationType; }
	void SetAnimationType(int type) { mAnimationType = static_cast<ANIMATION_MOTION_TYPE>(type); }

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
	char mDirection;
	int mX;
	int mY;
	std::mutex mPosMtx;
	ANIMATION_MOTION_TYPE mAnimationType;

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
	// 플레이어 행동 상태
	enum class BEHAVIOR_STATE
	{
		// 일반
		IDLE,
		// 전투
		BATTLE,
		// 교환
		TRADE,
		// 그 외
		DEFAULT
	};

	// 아바타 슬롯 타입
	enum class EQUIP_SLOT_TYPE
	{
		RING,
		POCKET,
		PENDANT,
		WEAPON,
		BELT,
		CAP,
		FACE_ACC,
		EYE_ACC,
		TOP_BODY,
		BOTTOM_BODY,
		SHOES,
		EAR_ACC,
		SHOULDER,
		GLOVES,
		ANDROID,
		EMBLEM,
		BADGE,
		MEDAL,
		SUB_WEAPON,
		CAPE,
		HEART
	};

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
	void ProcessBroadcastingChat(wchar_t* chat);
	void ProcessWhisperingChat(int id, wchar_t* chat);

	void ProcessAttack();
	void ProcessChangeAvatar(int slot, int texId, bool isEquip);

	int* GetAvatarTexIds()	{ return mAvatarTexIds; }
	
	BEHAVIOR_STATE GetState() const { return mState; }
	void SetState(BEHAVIOR_STATE state) { mState = state; }

	int GetTradeId()	const { return mTradeId; }
	void SetTradeId(int tradeId) { mTradeId = tradeId; }

private:
	SOCKET mSocket;
	char mPacketBuf[MAX_BUFFER];
	int	 mPrevSize;
	int	 mSendBytes;
	bool mConnect;

	int mAvatarTexIds[MAX_AVATAR_SLOT_SIZE];
	BEHAVIOR_STATE mState;
	int mTradeId;
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

protected:
	std::pair<int, int> mRangeMin;
	std::pair<int, int> mRangeMax;

	int mSectorXId;
	int mSectorYId;

	MONSTER_STATE mState;

	int mTargetId;
};

class FollowingMonster : public Monster
{
public:
	FollowingMonster();
	~FollowingMonster();
	virtual void Reset();
	virtual bool Inititalize(int id);

private:
};