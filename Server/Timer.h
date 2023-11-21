#pragma once

class Timer
{
	SINGLETON(Timer)

public:
	void Init();
	void Update();
	void Reset();
	void Start(); 
	void Stop();  
	void AddPlayerSock(SOCKET sock);
	void RemovePlayerSock(SOCKET sock);
	void SendTimerData();

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

	std::vector<SOCKET> mPlayerSockVec;
};

#define DELTA_TIME GET_SINGLE(Timer)->GetTotalTime()
#define TOTAL_TIME GET_SINGLE(Timer)->GetDeltaTime()