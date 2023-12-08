#include "stdafx.h"
#include "GameTimer.h"

DECLARE_SINGLE(GameTimer)

void GameTimer::Init()
{
	int64 countsPerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
	mSecondsPerCount = 1.0 / (double)countsPerSec;
	Reset();
}

void GameTimer::Move()
{
	if (mStopped) {
		mDeltaTime = 0.0;
		return;
	}

	int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	mCurrTime = currTime;

	mDeltaTime = (mCurrTime - mPrevTime) * mSecondsPerCount;

	mPrevTime = mCurrTime;

	if (mDeltaTime < 0.0)
		mDeltaTime = 0.0;

	if (mStopped)
		mTotalTime = (float)(((mStopTime - mPausedTime) - mBaseTime) * mSecondsPerCount);
	else
		mTotalTime = (float)(((mCurrTime - mPausedTime) - mBaseTime) * mSecondsPerCount);
}

void GameTimer::Reset()
{
	int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

	mBaseTime = currTime;
	mPrevTime = currTime;
	mStopTime = 0;
	mStopped = false;
}

void GameTimer::Start()
{
	int64 startTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&startTime);

	if (mStopped) {
		mPausedTime += (startTime - mStopTime);
		mPrevTime = startTime;
		mStopTime = 0;
		mStopped = false;
	}
}

void GameTimer::Stop()
{
	if (!mStopped)
	{
		int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

		mStopTime = currTime;
		mStopped = true;
	}
}

float GameTimer::GetTotalTime()
{
	return mTotalTime;
}

float GameTimer::GetDeltaTime() const
{
	return (float)mDeltaTime;
}

float GameTimer::GetFps() const
{
	return mFps;
}
