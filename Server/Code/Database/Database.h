#pragma once
#include "../Common/Macro.h"
#include "../Common/Defines.h"

class Database
{
	SINGLE_TONE(Database)

public:
	bool Initialize();

private:
	void run();

private:
	std::thread mDbThread;
	tbb::concurrent_queue<int> mDbQueue;
};

