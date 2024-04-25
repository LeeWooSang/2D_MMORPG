#include "GameTimer.h"
#include "../Core/Core.h"
using namespace std;

std::mutex gMtx;
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
			// 타이머 큐에서 이벤트가 있는지 확인
			if (mTimerQueue.try_pop(ev) == false)
			{
				break;
			}

			// 타이머 큐에서 해당 이벤트가 시간이 되었는지 체크한다.
			if (ev.startTime > chrono::high_resolution_clock::now())
			{
				mTimerQueue.push(TimerEvent(ev.startTime, ev.eventType, ev.myId, ev.channel, ev.sectorXId, ev.sectorYId));
				break;
			}

			//if (ev.m_eventType == Core::EVENT_TYPE::PLAYER_STATUS_UPDATE)
			//{
			//	GET_INSTANCE(DataBase)->AddDBTransactionQueue(DB_TRANSACTION_TYPE::UPDATE_PLAYER_STATUS_INFO, ev.m_objID);
			//	break;
			//}

			// 이벤트가 몬스터AI인 경우
			if (ev.eventType == SERVER_EVENT::MONSTER_MOVE)
			{
				// 몬스터id와 채널과 섹터id 정보를 워커스레드에게 전달

				Over* over = new Over;
				over->eventType = SERVER_EVENT::MONSTER_MOVE;
				over->myId = ev.myId;
				over->channel = ev.channel;
				over->sectorXId = ev.sectorXId;
				over->sectorYId = ev.sectorYId;

				//void* key = &over;
				//over->key = key;
				//GET_INSTANCE(Core)->AddEventData(key, over);

				PostQueuedCompletionStatus(GET_INSTANCE(Core)->GetIOCP(), 1, ev.myId, &over->overlapped);
			}
		}		
	}
}
