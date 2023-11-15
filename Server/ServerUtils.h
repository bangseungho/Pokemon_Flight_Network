#pragma once

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