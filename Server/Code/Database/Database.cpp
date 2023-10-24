#include "Database.h"
#include "../Core/Core.h"

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
	mDbThread = std::thread{ &Database::Run, this };

    return true;
}

void Database::Run()
{
	while (GET_INSTANCE(Core)->GetIsRun() == true)
	{
		while (mDbQueue.empty() == false)
		{
			//mDbQueue.try_pop()
		}
	}
}