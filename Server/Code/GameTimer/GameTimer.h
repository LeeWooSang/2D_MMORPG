#pragma once
#include "../Common/Macro.h"
#include <chrono>
#include "../Common/Defines.h"
#include "../Common/Protocol.h"

struct TimerEvent
{
	TimerEvent()
		: startTime(std::chrono::high_resolution_clock::now()), eventType(SERVER_EVENT::DEFAULT), myId(-1), channel(0)
	{
		sectorXId = 0;
		sectorYId = 0;
	}

	TimerEvent(std::chrono::high_resolution_clock::time_point s, SERVER_EVENT e, int id, int _channel, int _sectorXId, int _sectorYId)
		: startTime(s), eventType(e), myId(id), channel(_channel), sectorXId(_sectorXId), sectorYId(_sectorYId) {}

	std::chrono::high_resolution_clock::time_point startTime;
	SERVER_EVENT eventType;
	int	 myId;
	int channel;
	int sectorXId;
	int sectorYId;

	//int		target_id = TARGET_IS_NONE;

	// �켱���� ť�� �����͸� ���� ���� �񱳿�����
	constexpr bool operator < (const TimerEvent& left)	const { return ((*this).startTime > left.startTime); };
};

class GameTimer
{
	SINGLE_TONE(GameTimer)

public:
	bool Initialize();
	void AddTimer(std::chrono::high_resolution_clock::time_point t, SERVER_EVENT e, int id, int channel, int sectorXId, int sectorYId) 
	{ mTimerQueue.push(TimerEvent(t, e, id, channel, sectorXId, sectorYId)); }

private:
	void run();

private:
	std::thread mTimerThread;
	tbb::concurrent_priority_queue<TimerEvent> mTimerQueue;
};

