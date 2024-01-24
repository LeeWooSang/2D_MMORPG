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

	// ��ȯ ��밡 ������, ���ٰ� ������ ����
	if (users[tradeUserId].GetIsConnect() == false)
	{
		return;
	}

	// ��ȯ�� �� ���� ���¶�� ������ ����
	if (users[myId].GetState() != Player::BEHAVIOR_STATE::IDLE || users[tradeUserId].GetState() != Player::BEHAVIOR_STATE::IDLE)
	{
		return;
	}

	users[myId].SetState(Player::BEHAVIOR_STATE::TRADE);
	users[tradeUserId].SetState(Player::BEHAVIOR_STATE::TRADE);

	// ��ȯ ���̵� ����
	users[myId].SetTradeId(myId);
	users[tradeUserId].SetTradeId(myId);

	mTradeUserId = tradeUserId;
	
	// ������ -1�� �ʱ�ȭ
	memset(mSlotA, -1, sizeof(int) * MAX_TRADE_SLOT);
	memset(mSlotB, -1, sizeof(int) * MAX_TRADE_SLOT);

	// ���Ŀ� ��ȯ ���� ����
	GET_INSTANCE(Core)->SendRequestTradePacket(tradeUserId, myId);
}

void Trade::AddItem(int id, int slotNum, int itemId)
{	
	int myId = mMyId;
	int tradeUserId = mTradeUserId;

	// ��ȯ���� ��� ���̵� �´��� Ȯ��
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

	// ���� �������� �ø� ���
	if (id == myId && checkMySlot(slotNum) == true)
	{
		mSlotA[slotNum] = itemId;
		GET_INSTANCE(Core)->SendAddTradeItemPacket(tradeUserId, itemId, slotNum);
	}
	// ��밡 �������� �ø� ���
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

	// ��ȯ���� ��� ���̵� �´��� Ȯ��
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

	// ���� �������� �ø� ���
	if (id == myId)
	{
		mMyMeso += meso;
		GET_INSTANCE(Core)->SendAddTradeMesoPacket(tradeUserId, mMyMeso);
	}
	// ��밡 �������� �ø� ���
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

	// ��ȯ���� ��� ���̵� �´��� Ȯ��
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

	// �Ѹ��� ��ȯ ������ ��,
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

	// �Ѵ� ��ȯ�� ���� ���
	mTradeMtx.lock();
	if (mReadyA == true && mReadyB == true)
	{
		if (tradeUserId == -1)
		{
			mTradeMtx.unlock();
			return;
		}

		// ��ȯ�� �� ���� ���¶�� ������ ����
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

		// ��ȯâ �������� �����Ѵ�. (Ŭ���̾�Ʈ������ ��ȯâ���� �ݴ´�)
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

	// ��ȯ�� �� ���� ���¶�� ������ ����
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