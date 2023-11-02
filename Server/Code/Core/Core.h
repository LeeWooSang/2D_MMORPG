#pragma once
#include "../Common/Macro.h"
#include "../Common/Defines.h"
#include "../Common/Protocol.h"
#include "../Character/Character.h"
#include "../Channel/Channel.h"

class Core
{
	SINGLE_TONE(Core)

public:
	bool Initialize();
	void ServerQuit();
	void* GetIOCP()	const { return mIOCP; }
	volatile bool GetIsRun()	const { return mIsRun; }
	void PushLeafWork(OverEx* overEx) { mLeafWorks.push(overEx); }
	
	int GetObjectIndex(int id) const
	{ 
		tbb::concurrent_hash_map<int, int>::const_accessor acc;
		if (mObjectIds.find(acc, id))
		{
			return acc->second;
		}
		acc.release();
		return -1;
	}

	Player* GetUsers() { return mUsers; }
	Player& GetUser(int index) { return mUsers[index]; }
	Channel& GetChannel(int channel) { return mChannels[channel]; }

	void SendPositionPacket(int to, int obj);
	void SendAddObjectPacket(int to, int obj);
	void SendRemoveObjectPacket(int to, int obj);
	void SendChangeChannelPacket(int to, bool result);

private:
	void errorDisplay(const char* msg, int error);

	void acceptClient();
	void threadPool();
	void disconnect(int id);

	void recvPacket(int id);
	void sendPacket(int id, char* packet);

	void processPacket(int id, char* buf);
	void processEvent(SERVER_EVENT eventType, int id);

	int FindChannel();
	int createPlayerId()	const;

	bool popLeafWork();

private:
	HANDLE mIOCP;
	SOCKET mListenSocket;

	std::vector<std::thread> mWorkerThreads;
	std::thread mAcceptThread;
	std::thread mTimerThread;

	volatile bool mIsRun;
	
	// id, ¿Œµ¶Ω∫
	tbb::concurrent_hash_map<int, int> mObjectIds;

	Player* mUsers;
	//Monster* mMonsters;

	tbb::concurrent_queue<OverEx*> mLeafWorks;

	std::array<Channel, MAX_CHANNEL> mChannels;
};

