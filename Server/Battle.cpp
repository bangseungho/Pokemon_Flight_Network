#include "..\Utils.h"
#include "Battle.h"
#include "enemy.h"

Battle::Battle()
{
}

Battle::~Battle()
{
}

void Battle::Init()
{
	mEnemyController = make_unique<EnemyController>();
}

void Battle::Update(float elapsedTime)
{
	static float accTime;
	accTime += elapsedTime;

	if (accTime >= 0.0167f) {
		Invalidata();
		accTime = 0.f;
	}
}

void Battle::Invalidata()
{
	// 일정 타이머에 맞게 적 객체 생성
	mEnemyController->CreateCheckMelee();
	mEnemyController->CreateCheckRange();
	mEnemyController->Update();
}
