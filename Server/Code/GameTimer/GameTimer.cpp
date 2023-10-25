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

			// Ÿ�̸� ť���� �ش� �̺�Ʈ�� ����
			if (ev.startTime > chrono::high_resolution_clock::now())
			{
				AddTimer(ev.startTime, ev.eventType, ev.myId);
				break;
			}

			//if (ev.m_eventType == Core::EVENT_TYPE::PLAYER_STATUS_UPDATE)
			//{
			//	GET_INSTANCE(DataBase)->AddDBTransactionQueue(DB_TRANSACTION_TYPE::UPDATE_PLAYER_STATUS_INFO, ev.m_objID);
			//	break;
			//}

			OverEx* overEx = new OverEx;
			overEx->eventType = ev.eventType;
			overEx->myId = ev.myId;

			GET_INSTANCE(Core)->PushLeafWork(overEx);
			// ��Ŀ�����忡�� ProcessEvent�� �Ѱܾ� ��
			PostQueuedCompletionStatus(GET_INSTANCE(Core)->GetIOCP(), 1, ev.myId, &overEx->overlapped);
		}		
	}
}
