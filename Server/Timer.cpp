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

void Timer::AddPlayerSock(SOCKET sock)
{
	mPlayerSockVec.emplace_back(sock);
}

void Timer::RemovePlayerSock(SOCKET sock)
{
	auto findIt = std::find(mPlayerSockVec.begin(), mPlayerSockVec.end(), sock);

	if (findIt != mPlayerSockVec.end())
		mPlayerSockVec.erase(findIt);
}

void Timer::SendTimerData()
{
	for (const auto& sock : mPlayerSockVec) {
		Data::SendDataAndType<TimerData>(sock, TimerData{ mDeltaTime, mTotalTime });
	}
}

float Timer::GetTotalTime()
{
	return mTotalTime;
}

float Timer::GetDeltaTime() const
{
	return (float)mDeltaTime;
}

float Timer::GetFps() const
{
	return mFps;
}
