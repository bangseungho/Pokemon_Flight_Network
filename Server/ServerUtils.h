#pragma once

class PlayerData
{
public:
	PlayerData() {}
	PlayerData(SOCKET& sock, uint8 threadId) { mSock = sock, mThreadId = threadId; }
	virtual ~PlayerData() {}
	
public:
	IntroData	mIntroData;
	TownData	mTownData;
	StageData	mStageData;
	PhaseData	mPhaseData;
	BattleData	mBattleData;

	SOCKET		mSock;
	uint8		mThreadId;
};