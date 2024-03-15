#pragma once
#include "../Common/Macro.h"
constexpr int MAX_SAMPLE_COUNT = 50;

class GameTimer
{
	SINGLE_TONE(GameTimer)

	void Tick(int fps = 0);
	void Start();
	void Stop();
	void Reset();

	unsigned int GetFrameRate()	const { return m_nCurrentFrameRate; }
	float GetElapsedTime() const { return mElapsedTime; }
	float GetTotalTime();

private:
	bool								mStopped;
	double							mTimeScale;
	float								mElapsedTime;		

	__int64							m_nBasePerformanceCounter;
	__int64							m_nPausedPerformanceCounter;
	__int64							m_nStopPerformanceCounter;
	__int64							m_nCurrentPerformanceCounter;
    __int64							m_nLastPerformanceCounter;

	__int64							m_nPerformanceFrequencyPerSec;				

    float								m_fFrameTime[MAX_SAMPLE_COUNT];
	unsigned int					m_nSampleCount;

    unsigned int					m_nCurrentFrameRate;				
	unsigned int					m_nFramesPerSecond;					
	float								m_fFPSTimeElapsed;
};
