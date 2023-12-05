#include "..\Utils.h"
#include "Battle.h"
#include "enemy.h"

Battle::Battle()
{
}

Battle::~Battle()
{
}

void Battle::Init(NetworkPlayerData* playerMap)
{
	mPlayerMap = playerMap;
	mEnemyController = make_unique<EnemyController>(playerMap);
}

void Battle::Update(float elapsedTime)
{
	static float accTime;
	accTime += elapsedTime;

	if (accTime > 0.016f) {
		CreateEnemys(elapsedTime);
		mEnemyController->Update();
		accTime = 0.f;
	}
}

void Battle::CreateEnemys(float elapsedTime)
{
	// 일정 타이머에 맞게 적 객체 생성
	mEnemyController->CreateCheckMelee();
	//mEnemyController->CreateCheckRange();

#ifdef _DEBUG 
	mEnemyController->ShowEnemyCount(); // 적 객체 개수 확인
#endif
}
