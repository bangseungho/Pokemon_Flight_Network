#pragma once

struct ThreadSocket
{
	SOCKET Sock;
	int Id;
};

struct IntroData
{
	int Id;
	int Password;
};

struct TownData
{
	float	PosX;
	float	PosY;
	bool	IsReady;
};

struct StageData
{
	int Record;
};

struct PhaseData
{
	bool IsReady;
};

struct BattleData
{
	float	PosX;
	float	PosY;
	bool	IsCollider;
};

class PlayerData
{
public:
	PlayerData(ThreadSocket* threadSocket) { mThreadSocket = threadSocket; }
	virtual ~PlayerData() {}
	
public:
	IntroData mIntroData;
	TownData mTownData;
	StageData mStageData;
	PhaseData mPhaseData;
	BattleData mBattleData;

	ThreadSocket* mThreadSocket;
};