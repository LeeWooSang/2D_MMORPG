#include "Trade.h"
#include <iostream>
#include "../Core/Core.h"
#include "../Character/Character.h"

Trade::Trade()
{
	mMyId = -1;
	mTradeUserId = -1;
	memset(mSlotA, -1, sizeof(int) * MAX_TRADE_SLOT);
	memset(mSlotB, -1, sizeof(int) * MAX_TRADE_SLOT);
	mMyMeso = 0;
	mTradeUserMeso = 0;
	mReadyA = false;
	mReadyB = false;
}

Trade::~Trade()
{
	Reset();
}

void Trade::Reset()
{
	mTradeUserId = -1;
	memset(mSlotA, -1, sizeof(int) * MAX_TRADE_SLOT);
	memset(mSlotB, -1, sizeof(int) * MAX_TRADE_SLOT);
	mMyMeso = 0;
	mTradeUserMeso = 0;
	mReadyA = false;
	mReadyB = false;
}

void Trade::StartTrade(int tradeUserId)
{
	int myId = mMyId;
	Player* users = GET_INSTANCE(Core)->GetUsers();

	// 교환 상대가 없으면, 없다고 나에게 전송
	if (users[tradeUserId].GetIsConnect() == false)
	{
		return;
	}

	// 교환할 수 없는 상태라고 나에게 전송
	if (users[myId].GetState() != Player::BEHAVIOR_STATE::IDLE || users[tradeUserId].GetState() != Player::BEHAVIOR_STATE::IDLE)
	{
		return;
	}

	users[myId].SetState(Player::BEHAVIOR_STATE::TRADE);
	users[tradeUserId].SetState(Player::BEHAVIOR_STATE::TRADE);

	// 교환 아이디 설정
	users[myId].SetTradeId(myId);
	users[tradeUserId].SetTradeId(myId);

	mTradeUserId = tradeUserId;
	
	// 슬롯을 -1로 초기화
	memset(mSlotA, -1, sizeof(int) * MAX_TRADE_SLOT);
	memset(mSlotB, -1, sizeof(int) * MAX_TRADE_SLOT);

	// 추후에 교환 거절 구현
	GET_INSTANCE(Core)->SendRequestTradePacket(tradeUserId, myId);
}

void Trade::AddItem(int id, int slotNum, int itemId)
{	
	int myId = mMyId;
	int tradeUserId = mTradeUserId;

	// 교환중인 상대 아이디가 맞는지 확인
	if (id != myId && id != tradeUserId)
	{
		return;
	}

	Player* users = GET_INSTANCE(Core)->GetUsers();
	if (users[tradeUserId].GetIsConnect() == false)
	{
		Reset();
		return;
	}

	// 내가 아이템을 올린 경우
	if (id == myId && checkMySlot(slotNum) == true)
	{
		mSlotA[slotNum] = itemId;
		GET_INSTANCE(Core)->SendAddTradeItemPacket(tradeUserId, itemId, slotNum);
	}
	// 상대가 아이템을 올린 경우
	else if(id == tradeUserId && checkUserSlot(slotNum) == true)
	{
		mSlotB[slotNum] = itemId;
		GET_INSTANCE(Core)->SendAddTradeItemPacket(myId, itemId, slotNum);
	}
}

void Trade::AddMeso(int id, long long meso)
{
	int myId = mMyId;
	int tradeUserId = mTradeUserId;

	// 교환중인 상대 아이디가 맞는지 확인
	if (id != myId && id != tradeUserId)
	{
		return;
	}

	Player* users = GET_INSTANCE(Core)->GetUsers();
	if (users[tradeUserId].GetIsConnect() == false)
	{
		Reset();
		return;
	}

	// 내가 아이템을 올린 경우
	if (id == myId)
	{
		mMyMeso += meso;
		GET_INSTANCE(Core)->SendAddTradeMesoPacket(tradeUserId, mMyMeso);
	}
	// 상대가 아이템을 올린 경우
	else if (id == tradeUserId)
	{
		mTradeUserMeso += meso;
		GET_INSTANCE(Core)->SendAddTradeMesoPacket(myId, mTradeUserMeso);
	}
}

void Trade::ProcessTrade(int id)
{
	int myId = mMyId;
	int tradeUserId = mTradeUserId;

	// 교환중인 상대 아이디가 맞는지 확인
	if (id != myId && id != tradeUserId)
	{
		return;
	}

	Player* users = GET_INSTANCE(Core)->GetUsers();
	if (users[tradeUserId].GetIsConnect() == false)
	{
		Reset();
		return;
	}

	// 한명이 교환 눌렀을 때,
	if (id == myId && mReadyA == false)
	{
		mReadyA = true;
		std::cout << id << "client trade ready ok" << std::endl;
	}
	else if (id == tradeUserId && mReadyB == false)
	{
		mReadyB = true;
		std::cout << id << "client trade ready ok" << std::endl;
	}

	// 둘다 교환을 누른 경우
	mTradeMtx.lock();
	if (mReadyA == true && mReadyB == true)
	{
		if (tradeUserId == -1)
		{
			mTradeMtx.unlock();
			return;
		}

		// 교환할 수 없는 상태라고 나에게 전송
		if (users[myId].GetState() != Player::BEHAVIOR_STATE::TRADE && users[tradeUserId].GetState() != Player::BEHAVIOR_STATE::TRADE)
		{
			mTradeMtx.unlock();
			return;
		}

		users[myId].SetState(Player::BEHAVIOR_STATE::IDLE);
		users[tradeUserId].SetState(Player::BEHAVIOR_STATE::IDLE);		
		users[myId].SetTradeId(-1);
		users[tradeUserId].SetTradeId(-1);
		mTradeMtx.unlock();

		// 교환창 아이템을 전송한다. (클라이언트에서는 교환창까지 닫는다)
		GET_INSTANCE(Core)->SendTradePacket(myId, mSlotB, mTradeUserMeso);
		GET_INSTANCE(Core)->SendTradePacket(tradeUserId, mSlotA, mMyMeso);
		Reset();
	}
	else
	{
		mTradeMtx.unlock();
	}
}

void Trade::ProcessCancel()
{
	Player* users = GET_INSTANCE(Core)->GetUsers();

	if (mTradeUserId == -1)
	{
		return;
	}

	// 교환할 수 없는 상태라고 나에게 전송
	if (users[mMyId].GetState() != Player::BEHAVIOR_STATE::TRADE && users[mTradeUserId].GetState() != Player::BEHAVIOR_STATE::TRADE)
	{
		return;
	}

	mTradeMtx.lock();
	int myId = mMyId;
	int tradeUserId = mTradeUserId;

	users[myId].SetState(Player::BEHAVIOR_STATE::IDLE);
	users[tradeUserId].SetState(Player::BEHAVIOR_STATE::IDLE);
	users[myId].SetTradeId(-1);
	users[tradeUserId].SetTradeId(-1);
	Reset();
	mTradeMtx.unlock();

	GET_INSTANCE(Core)->SendTradeCancelPacket(myId);
	GET_INSTANCE(Core)->SendTradeCancelPacket(tradeUserId);
	
}

bool Trade::checkMySlot(int slotNum)
{
	if (slotNum >= MAX_TRADE_SLOT || mSlotA[slotNum] != -1)
	{
		return false;
	}
	return true;
}

bool Trade::checkUserSlot(int slotNum)
{
	if (slotNum >= MAX_TRADE_SLOT || mSlotB[slotNum] != -1)
	{
		return false;
	}
	return true;
}