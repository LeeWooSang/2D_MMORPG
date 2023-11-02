#include "GameTimer.h"
#include "../Core/Core.h"
using namespace std;

INIT_INSTACNE(GameTimer)
GameTimer::GameTimer()
{
}

GameTimer::~GameTimer()
{
	if (mTimerThread.joinable() == true)
	{
		mTimerThread.join();
	}

	mTimerQueue.clear();
}

bool GameTimer::Initialize()
{
	mTimerQueue.clear();

	mTimerThread = std::thread{ &GameTimer::run, this };

	return true;
}

void GameTimer::run()
{
	while (GET_INSTANCE(Core)->GetIsRun() == true)
	{
		std::this_thread::sleep_for(10ms);
		while (true)
		{
			TimerEvent ev;
			if (mTimerQueue.try_pop(ev) == false)
			{
				break;
			}

			// 타이머 큐에서 해당 이벤트를 얻어옴
			if (ev.startTime > chrono::high_resolution_clock::now())
			{
				mTimerQueue.push(TimerEvent(ev.startTime, ev.eventType, ev.myId, ev.channel));
				break;
			}

			//if (ev.m_eventType == Core::EVENT_TYPE::PLAYER_STATUS_UPDATE)
			//{
			//	GET_INSTANCE(DataBase)->AddDBTransactionQueue(DB_TRANSACTION_TYPE::UPDATE_PLAYER_STATUS_INFO, ev.m_objID);
			//	break;
			//}

			if (ev.eventType == SERVER_EVENT::MONSTER_MOVE)
			{
				Over* over = new Over;
				over->eventType = ev.eventType;
				over->myId = ev.myId;
				over->channel = ev.channel;

				GET_INSTANCE(Core)->PushLeafWork(over);
				// 워커스레드에게 ProcessEvent를 넘겨야 됨
				PostQueuedCompletionStatus(GET_INSTANCE(Core)->GetIOCP(), 1, ev.myId, &over->overlapped);
			}
		}		
	}
}
