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
			// Ÿ�̸� ť���� �̺�Ʈ�� �ִ��� Ȯ��
			if (mTimerQueue.try_pop(ev) == false)
			{
				break;
			}

			// Ÿ�̸� ť���� �ش� �̺�Ʈ�� �ð��� �Ǿ����� üũ�Ѵ�.
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

			// �̺�Ʈ�� ����AI�� ���
			if (ev.eventType == SERVER_EVENT::MONSTER_MOVE)
			{
				// ����id�� ä�ΰ� ����id ������ ��Ŀ�����忡�� ����

				Over* over = new Over;
				//std::shared_ptr<Over> over = std::make_shared<Over>();

				over->eventType = SERVER_EVENT::MONSTER_MOVE;
				over->myId = ev.myId;
				over->channel = ev.channel;
				over->sectorXId = ev.sectorXId;
				over->sectorYId = ev.sectorYId;

				void* key = &over;
				over->key = key;

				{
					tbb::concurrent_hash_map<void*, Over*>::accessor acc;
					//tbb::concurrent_hash_map<void*, std::shared_ptr<Over>>::accessor acc;
					//tbb::concurrent_hash_map<void*, atomic<std::shared_ptr<Over>>>::accessor acc;
					GET_INSTANCE(Core)->mEventDatas.insert(acc, key);
					acc->second = over;
					acc.release();
				}

				PostQueuedCompletionStatus(GET_INSTANCE(Core)->GetIOCP(), 1, ev.myId, &over->overlapped);
			}
		}		
	}
}
