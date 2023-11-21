#pragma once

class GameTimer
{
	SINGLETON(GameTimer)

public:
	void Init();
	void Update();
	void Reset();
	void Start(); 
	void Stop();  

public:
	float GetTotalTime();
	float GetDeltaTime() const;
	float GetFps() const;

private:
	double			mSecondsPerCount = 0.0;
	double			mDeltaTime = -1.0;
	float			mTotalTime = 0.f;
	int64			mBaseTime = 0;
	int64			mPausedTime = 0;
	int64			mStopTime = 0;
	int64			mPrevTime = 0;
	int64			mCurrTime = 0;
	float			mFps = 0.f;
	float			mSpf = 0.f;
	bool			mStopped = false;
};

#define DELTA_TIME GET_SINGLE(GameTimer)->GetDeltaTime()
#define TOTAL_TIME GET_SINGLE(GameTimer)->GetTotalTime()