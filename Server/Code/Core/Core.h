#pragma once
#include "../Common/Macro.h"
#include "../Common/Defines.h"
#include "../Common/Protocol.h"
#include "../Character/Character.h"
#include "../Channel/Channel.h"
#include "../Sector/Sector.h"

class Core
{
	SINGLE_TONE(Core)

public:
	bool Initialize();
	void ServerQuit();

	void* GetIOCP()	const { return mIOCP; }
	volatile bool GetIsRun()	const { return mIsRun; }
	
	Player* GetUsers() { return mUsers; }
	Player& GetUser(int index) { return mUsers[index]; }
	Channel& GetChannel(int channel) { return mChannels[channel]; }

public:
	void SendServerSelectPacket(int to, short* size);
	void SendLoginOkPacket(int to);
	void SendLoginFailPacket(int to);
	void SendDummyLoginPacket(int to, int channel);

	void SendChannelLoginPacket(int to, int channel);

	void SendPositionPacket(int to, int obj);
	void SendAddPlayerPacket(int to, int obj, int* texIds);
	void SendAddObjectPacket(int to, int obj);
	void SendAddMonsterPacket(int to, int obj, int x, int y);
	void SendMonsterPositionPacket(int to, int obj, int x, int y);
	void SendRemoveObjectPacket(int to, int obj);
	void SendRemoveAllObjectPacket(int to);
	void SendChangeChannelPacket(int to, bool result, int channel);
	void SendChatPacket(int to, int obj, wchar_t* chat);

	void SendChangeAvatarPacket(int to, int obj, int slot, int texId);
	void SendTakeOffEquipItemPacket(int to, int obj, int slot);

	void SendRequestTradePacket(int to, int obj);
	void SendAddTradeItemPacket(int to, int texId, int slotNum);
	void SendAddTradeMesoPacket(int to, long long meso);
	void SendTradePacket(int to, int* items, long long meso);
	void SendTradeCancelPacket(int to);

private:
	void errorDisplay(const char* msg, int error);

	void acceptClient();
	void threadPool();
	void disconnect(int id);

	void recvPacket(int id);
	void sendPacket(int id, char* packet);

	void processPacket(int id, char* buf);
	void processEvent(Over* over);

	int FindChannel();
	int createPlayerId()	const;

	void popSendData(Over* over);
	void popEventData(Over* over);

private:
	HANDLE mIOCP;
	SOCKET mListenSocket;

	std::vector<std::thread> mWorkerThreads;
	std::thread mAcceptThread;
	std::thread mTimerThread;

	volatile bool mIsRun;

	Player* mUsers;
	class Trade* mTrades;

	std::array<Channel, MAX_CHANNEL> mChannels;

	tbb::concurrent_hash_map<void*, std::shared_ptr<OverEx>> mSendDatas;

public:
	tbb::concurrent_hash_map<void*, Over*> mEventDatas;
};

