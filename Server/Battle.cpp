#include "..\Utils.h"
#include "Battle.h"
#include "enemy.h"
#include "player.h"

extern unordered_map<uint8, NetworkPlayerData> sPlayerMap;

Battle::Battle()
{
}

Battle::~Battle()
{
}

void Battle::Init()
{
	for (auto& player : sPlayerMap)
		mPlayerVec.emplace_back(make_shared<Player>(&player.second));

	mEnemyController = make_shared<EnemyController>();
	Sleep(10);
}

void Battle::Update(float elapsedTime)
{
	static float accTime;
	accTime += elapsedTime;

	if (accTime >= 0.0167f) {

		for (auto& player : mPlayerVec) {
			player->Update();
			player->CheckShot();
		}

		mEnemyController->CreateCheckMelee();
		mEnemyController->CreateCheckRange();
		accTime = 0.f;
	}
}
