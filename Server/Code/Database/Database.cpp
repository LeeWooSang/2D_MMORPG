#include "Database.h"
#include "../Core/Core.h"
#include "../GameTimer/GameTimer.h"

INIT_INSTACNE(Database)
Database::Database()
{
}

Database::~Database()
{
	if (mDbThread.joinable() == true)
	{
		mDbThread.join();
	}
}

bool Database::Initialize()
{
	mDbThread = std::thread{ &Database::run, this };

    return true;
}

#include <random>
using namespace std;

void Database::run()
{
	std::random_device rd;
	std::default_random_engine dre(rd());
	std::uniform_int_distribution<int> uid(0, MAX_USER - 1);

	while (GET_INSTANCE(Core)->GetIsRun() == true)
	{
		std::this_thread::sleep_for(10ms);

		GET_INSTANCE(GameTimer)->AddTimer(std::chrono::high_resolution_clock::now() + 1s, SERVER_EVENT::DEFAULT, uid(dre));
		while (mDbQueue.empty() == false)
		{
			//mDbQueue.try_pop()
		}
	}
}