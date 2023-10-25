#pragma once
#include "../Common/Macro.h"
#include "../Common/Defines.h"
#include "../Common/Protocol.h"

class Player;
class Core
{
	SINGLE_TONE(Core)

public:
	bool Initialize();
	void ServerQuit();
	void* GetIOCP()	const { return mIOCP; }
	volatile bool GetIsRun()	const { return mIsRun; }
	void PushLeafWork(OverEx* overEx) { mLeafWorks.push(overEx); }
	
private:
	void errorDisplay(const char* msg, int error);

	void acceptClient();
	void threadPool();
	void disconnect(int id);

	void recvPacket(int id);
	void sendPacket(int id, char* packet);

	void processPacket(int id, char* buf);
	void processEvent(SERVER_EVENT eventType, int id);

	int createPlayerId()	const;

	bool popLeafWork();
private:
	HANDLE mIOCP;
	SOCKET mListenSocket;

	std::vector<std::thread> mWorkerThreads;
	std::thread mAcceptThread;
	std::thread mTimerThread;

	volatile bool mIsRun;
	
	Player* mUsers;

	tbb::concurrent_queue<OverEx*> mLeafWorks;
};

