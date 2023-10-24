#pragma once
#include "../Common/Macro.h"
#include <thread>

class Database
{
	SINGLE_TONE(Database)

public:
	bool Initialize();
	void Run();

private:
	std::thread mDbThread;
	tbb::concurrent_queue<int> mDbQueue;
};

