#include "..\Utils.h"
#include <assert.h>
#include "enemy.h"

extern unordered_map<uint8, NetworkPlayerData> sPlayerMap;

// �� ��ü���� �����ϴ� Ŭ������ �������� ���¿� ���� �� ������Ʈ �ʱ�ȭ
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

// ��Ʋ Ÿ�̸Ӵ� �����Ǵ� �ٰŸ� �� ���� �Լ�
void EnemyController::CreateCheckMelee()
{
	bool isFieldEnd = std::any_of(sPlayerMap.begin(), sPlayerMap.end(), [](const auto& a) {
		return a.second.mBattleData.IsFieldEnd == true;
		});
	if (isFieldEnd == true)
	{
		return;
	}

	// ���� ���� �����ϰ� �� ���� ���� �ð��� �� ���� �ð��� �Ѱ��� ��쿡�� ���ο� ���� �����Ѵ�.
	delay_Melee += ELAPSE_BATTLE_INVALIDATE;
	if (delay_Melee < createDelay_Melee)
	{
		return;
	}
	delay_Melee = 0;

	// �ִ� ���� �� ������ ���� �� ��ü�� �����Ͽ� ��ü �ڷᱸ���� �ִ´�.
	for (int i = 0; i < createAmount_Melee; ++i)
	{
		Vector2 pos{ rand() % WINDOWSIZE_X, -(rand() % 100) };

		float minLength = numeric_limits<float>::infinity();
		uint8 targetIndex = 0;

		// ���� ����� ���� Ÿ������ ����
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

// ��Ʋ Ÿ�̸Ӵ� �����Ǵ� ���Ÿ� �� ���� �Լ�
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

