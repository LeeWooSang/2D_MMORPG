#pragma once
#include "../Common/Macro.h"
#include <chrono>
#include "../Common/Defines.h"
#include "../Common/Protocol.h"

struct TimerEvent
{
	TimerEvent()
		: startTime(std::chrono::high_resolution_clock::now()), eventType(SERVER_EVENT::DEFAULT), myId(-1)
	{}

	TimerEvent(std::chrono::high_resolution_clock::time_point s, SERVER_EVENT e, int id)
		: startTime(s), eventType(e), myId(id) {}

	std::chrono::high_resolution_clock::time_point startTime;
	SERVER_EVENT eventType;
	int	 myId;
	//int		target_id = TARGET_IS_NONE;

	// 우선순위 큐에 데이터를 넣을 때의 비교연산자
	constexpr bool operator < (const TimerEvent& left)	const { return ((*this).startTime > left.startTime); };
};

class GameTimer
{
	SINGLE_TONE(GameTimer)

public:
	bool Initialize();
	void AddTimer(std::chrono::high_resolution_clock::time_point t, SERVER_EVENT e, int id) { mTimerQueue.push(TimerEvent(t, e, id)); }

private:
	void run();

private:
	std::thread mTimerThread;
	tbb::concurrent_priority_queue<TimerEvent> mTimerQueue;
};

