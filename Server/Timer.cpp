#include "..\Utils.h"
#include "Timer.h"
#include <algorithm>

DECLARE_SINGLE(Timer)

void Timer::Init()
{
	int64 countsPerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
	mSecondsPerCount = 1.0 / (double)countsPerSec;
	Reset();
}

void Timer::Update()
{
	std::lock_guard<std::mutex> lock(mMutex);
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

void Timer::Reset()
{
	int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

	mBaseTime = currTime;
	mPrevTime = currTime;
	mStopTime = 0;
	mStopped = false;
}

void Timer::Start()
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

void Timer::Stop()
{
	if (!mStopped)
	{
		int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

		mStopTime = currTime;
		mStopped = true;
	}
}

float Timer::GetTotalTime() const
{
	return mTotalTime;
}

float Timer::GetDeltaTime() const
{
	std::lock_guard<std::mutex> lock(mMutex);
	return (float)mDeltaTime;
}

float Timer::GetFps() const
{
	return mFps;
}
