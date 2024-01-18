#pragma once
#include <mutex>

constexpr int MAX_TRADE_SLOT = 9;
class Player;
class Trade
{
public:
	Trade();
	~Trade();
	void Reset();

	void Initialize(int myId) { mMyId = myId; };
	void StartTrade(int tradeUserId);
	void AddItem(int id, int slotNum, int itemId);
	void ProcessTrade(int id);
	void ProcessCancel();


private:
	bool checkMySlot(int slotNum);
	bool checkUserSlot(int slotNum);

private:
	std::mutex mTradeMtx;
	int mMyId;
	int mTradeUserId;
	int mSlotA[MAX_TRADE_SLOT];
	int mSlotB[MAX_TRADE_SLOT];
	bool mReadyA;
	bool mReadyB;
};