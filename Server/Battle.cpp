#include "..\Utils.h"
#include "Battle.h"
#include "bullet.h"
#include "enemy.h"
#include "player.h"
#include "effect.h"

extern unordered_map<uint8, NetworkPlayerData> sPlayerMap;

Battle::Battle()
{
}

Battle::~Battle()
{
}

void Battle::Init()
{
	mEnemyController = make_shared<EnemyController>();

	for (auto& player : sPlayerMap)
		mPlayerVec.emplace_back(make_shared<Player>(&player.second, mEnemyController));

	for (auto& player : mPlayerVec)
		player->SetBulletsPlayer();

	Sleep(10);
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
	for (auto& player : mPlayerVec) {
		player->Update();
		player->CheckShot();
	}

	mEnemyController->CreateCheckMelee();
	//mEnemyController->CreateCheckRange();
	mEnemyController->CheckAttackDelay();

	for (auto& player : mPlayerVec)
		player->MoveBullets();

	mEnemyController->Update();
}
