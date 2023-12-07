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
	mEnemyController->CreateCheckMelee();
	mEnemyController->CreateCheckRange();
	mEnemyController->CheckAttackDelay();
	mEnemyController->Update();
}
