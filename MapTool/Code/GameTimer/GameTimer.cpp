#include "GameTimer.h"
#include <Windows.h>
#include <iostream>

INIT_INSTACNE(GameTimer)
GameTimer::GameTimer()
{
	::QueryPerformanceFrequency((LARGE_INTEGER *)&m_nPerformanceFrequencyPerSec);
	::QueryPerformanceCounter((LARGE_INTEGER *)&m_nLastPerformanceCounter); 
	mTimeScale = 1.0 / (double)m_nPerformanceFrequencyPerSec;

	mStopped = true;
	mElapsedTime = 0.0;

	m_nBasePerformanceCounter = m_nLastPerformanceCounter;
	m_nPausedPerformanceCounter = 0;
	m_nStopPerformanceCounter = 0;

	m_nSampleCount = 0;
	m_nCurrentFrameRate = 0;
	m_nFramesPerSecond = 0;
	m_fFPSTimeElapsed = 0.0f;
}

GameTimer::~GameTimer()
{
}

void GameTimer::Tick(int fps)
{
	if (mStopped == true)
	{
		mElapsedTime = 0.f;
		return;
	}

	float elapsedTime = 0.f;

	::QueryPerformanceCounter((LARGE_INTEGER *)&m_nCurrentPerformanceCounter);
	elapsedTime = float((m_nCurrentPerformanceCounter - m_nLastPerformanceCounter) * mTimeScale);

    if (fps > 0.0f)
    {
        while (elapsedTime < (1.0f / fps))
        {
	        ::QueryPerformanceCounter((LARGE_INTEGER *)&m_nCurrentPerformanceCounter);
			elapsedTime = float((m_nCurrentPerformanceCounter - m_nLastPerformanceCounter) * mTimeScale);
        }
    } 

	m_nLastPerformanceCounter = m_nCurrentPerformanceCounter;

    if (fabsf(elapsedTime - mElapsedTime) < 1.0f)
    {
        ::memmove(&m_fFrameTime[1], m_fFrameTime, (MAX_SAMPLE_COUNT - 1) * sizeof(float));
        m_fFrameTime[0] = elapsedTime;

        if (m_nSampleCount < MAX_SAMPLE_COUNT) 
			m_nSampleCount++;
    }

	m_nFramesPerSecond++;
	m_fFPSTimeElapsed += elapsedTime;
	if (m_fFPSTimeElapsed > 1.0f) 
    {
		m_nCurrentFrameRate	= m_nFramesPerSecond;
		m_nFramesPerSecond = 0;
		m_fFPSTimeElapsed = 0.0f;
	} 

    mElapsedTime = 0.0f;
    for (ULONG i = 0; i < m_nSampleCount; i++) 
		mElapsedTime += m_fFrameTime[i];
    
	if (m_nSampleCount > 0) 
		mElapsedTime /= m_nSampleCount;
}

float GameTimer::GetTotalTime()
{
	if (mStopped)
		return(float(((m_nStopPerformanceCounter - m_nPausedPerformanceCounter) - m_nBasePerformanceCounter) * mTimeScale));
	
	return(float(((m_nCurrentPerformanceCounter - m_nPausedPerformanceCounter) - m_nBasePerformanceCounter) * mTimeScale));
}

void GameTimer::Reset()
{
	__int64 nPerformanceCounter;
	::QueryPerformanceCounter((LARGE_INTEGER*)&nPerformanceCounter);

	m_nBasePerformanceCounter = nPerformanceCounter;
	m_nLastPerformanceCounter = nPerformanceCounter;
	m_nStopPerformanceCounter = 0;
	mStopped = false;
}

void GameTimer::Start()
{
	__int64 nPerformanceCounter;
	::QueryPerformanceCounter((LARGE_INTEGER *)&nPerformanceCounter);
	if (mStopped == true)
	{
		m_nPausedPerformanceCounter += (nPerformanceCounter - m_nStopPerformanceCounter);
		m_nLastPerformanceCounter = nPerformanceCounter;
		m_nStopPerformanceCounter = 0;
		mStopped = false;
	}
}

void GameTimer::Stop()
{
	if (mStopped == false)
	{
		::QueryPerformanceCounter((LARGE_INTEGER *)&m_nStopPerformanceCounter);
		mStopped = true;
	}
}
