#include "..\Utils.h"
#include <assert.h>
#include "enemy.h"

extern unordered_map<uint8, NetworkPlayerData> sPlayerMap;

// 적 객체들을 관리하는 클래스로 스테이지 상태에 따라서 적 오브젝트 초기화
EnemyController::EnemyController()
{
	switch (sPlayerMap.begin()->second.mStageData.Stage)
	{
	case StageElement::Elec:
		createDelay_Melee = 1500;
		createDelay_Range = 3000;
		createAmount_Melee = 5;
		createAmount_Range = 5;
		break;
	case StageElement::Water:
		createDelay_Melee = 1350;
		createDelay_Range = 3250;
		createAmount_Melee = 7;
		createAmount_Range = 5;
		break;
	case StageElement::Fire:
		createDelay_Melee = 2000;
		createDelay_Range = 3000;
		createAmount_Melee = 5;
		createAmount_Range = 4;
		break;
	case StageElement::Dark:
		createDelay_Melee = 2000;
		createDelay_Range = 3000;
		createAmount_Melee = 10;
		createAmount_Range = 4;
		break;
	default:
		assert(0);
		break;
	}
}
EnemyController::~EnemyController()
{

}

// 배틀 타이머당 생성되는 근거리 적 생성 함수
void EnemyController::CreateCheckMelee()
{
	bool isFieldEnd = std::any_of(sPlayerMap.begin(), sPlayerMap.end(), [](const auto& a) {
		return a.second.mBattleData.IsFieldEnd == true;
		});
	if (isFieldEnd == true)
	{
		return;
	}

	// 현재 적을 생성하고 난 다음 지난 시간이 적 생성 시간을 넘겼을 경우에만 새로운 적을 생성한다.
	delay_Melee += ELAPSE_BATTLE_INVALIDATE;
	if (delay_Melee < createDelay_Melee)
	{
		return;
	}
	delay_Melee = 0;

	// 최대 생성 적 개수에 따라서 적 객체를 생성하여 객체 자료구조에 넣는다.
	for (int i = 0; i < createAmount_Melee; ++i)
	{
		Vector2 pos{ rand() % WINDOWSIZE_X, -(rand() % 100) };

		float minLength = numeric_limits<float>::infinity();
		uint8 targetIndex = 0;

		// 가장 가까운 적을 타겟으로 설정
		for (auto& player : sPlayerMap) {
			float length = Vector2::GetNorm(pos - player.second.mBattleData.PosCenter);
			if (minLength >= length) {
				minLength = length;
				targetIndex = player.first;
			}
		}

		NetworkEnemyData sendData{ NetworkEnemyData::AttackType::MELEE, pos, targetIndex, mAccId++ };
		for (const auto& player : sPlayerMap)
			Data::SendDataAndType<NetworkEnemyData>(player.second.mSock, sendData);
	}
}

// 배틀 타이머당 생성되는 원거리 적 생성 함수
void EnemyController::CreateCheckRange()
{
	bool isFieldEnd = std::any_of(sPlayerMap.begin(), sPlayerMap.end(), [](const auto& a) {
		return a.second.mBattleData.IsFieldEnd == true;
		});
	if (isFieldEnd == true)
	{
		return;
	}

	delay_Range += ELAPSE_BATTLE_INVALIDATE;
	if (delay_Range < createDelay_Range)
	{
		return;
	}
	delay_Range = 0;

	for (int i = 0; i < createAmount_Range; ++i)
	{
		Vector2 pos{ (rand() % (WINDOWSIZE_X - 100)) + 50, -(rand() % 100) };

		NetworkEnemyData sendData{ NetworkEnemyData::AttackType::RANGE, pos, 0, mAccId++, (rand() % 100) + 50 };
		for (const auto& player : sPlayerMap)
			Data::SendDataAndType<NetworkEnemyData>(player.second.mSock, sendData);
	}
}

