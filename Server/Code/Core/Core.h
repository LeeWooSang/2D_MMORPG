#pragma once
#include "../Common/Macro.h"
#include <array>
#include <vector>
#include <thread>
#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib")

#include "../Character/Character.h"

class Core
{
	SINGLE_TONE(Core)

public:
	bool Initialize();
	void ServerQuit();
	bool GetIsRun()	const { return mIsRun; }

private:
	void errorDisplay(const char* msg, int error);

	void acceptClient();
	void threadPool();
	void timer();
	void disconnect(int id);

	void recvPacket(int id);
	void sendPacket(int id, char* packet);

	void processPacket(int id, char* buf);
	void processEvent(SERVER_EVENT eventType, int id);

	int createPlayerId()	const;

private:
	HANDLE mIOCP;
	SOCKET mListenSocket;

	std::vector<std::thread> mWorkerThreads;
	std::thread mAcceptThread;
	std::thread mTimerThread;

	volatile bool mIsRun;
	
	Player* mUsers[MAX_USER];
};

