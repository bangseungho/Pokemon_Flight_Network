#include "..\Utils.h"
#include "player.h"
#include "enemy.h"
#include "timer.h"
#include "battle.h"

extern unordered_map<uint8, NetworkPlayerData> sPlayerMap;

Player::Player(NetworkPlayerData* recvData)
{
	if (recvData != nullptr)
		mSendData = recvData;

	constexpr int damagePerSec = (1000 / ELAPSE_BATTLE_ANIMATION);

	switch (recvData->mSceneData.AirPokemon)
	{
	case Type::Elec:
		pokemon = Pokemon::Thunder;
		playerData.shotDelay = 90;
		break;
	case Type::Water:
		pokemon = Pokemon::Articuno;
		playerData.shotDelay = 110;
		break;
	case Type::Fire:
		pokemon = Pokemon::Moltres;
		playerData.shotDelay = 100;
		break;
	default:
		assert(0);
		break;
	}

	switch (recvData->mSceneData.LandPokemon)
	{
	case Type::Elec:
		subPokemon = SubPokemon::Pikachu;
		break;
	case Type::Water:
		subPokemon = SubPokemon::Squirtle;
		break;
	case Type::Fire:
		subPokemon = SubPokemon::Charmander;
		break;
	default:
		assert(0);
		break;
	}
}

Player::~Player()
{
}

void Player::Update()
{
	if (mSendData->mBattleData.IsDeath)
	{
		return;
	}

	SetPos(mSendData->mBattleData.PosCenter);
}

// ź�� �߻� �Լ�
void Player::Shot()
{
	// �÷��̾ ź���� �߻��ߴٸ� �ڽ��� �߻��ߴٴ� �ε����� ��� �÷��̾�� �۽��ؾ� �Ѵ�.
	for (const auto& player : sPlayerMap) {
		NetworkBulletData sendData{ mSendData->mBattleData.PlayerIndex, NetworkBulletData::Status::CREATE };
		Data::SendDataAndType<NetworkBulletData>(player.second.mSock, sendData);
	}
}

// �÷��̾��� �⺻ ���ݿ� ��Ÿ���� �ִ� �Լ��̴�
void Player::CheckShot()
{
	if (mSendData->mBattleData.IsDeath == true)
		return;

	if (mSendData->mBattleData.IsFieldEnd == true && mSendData->mBattleData.IsEnemyClear == true)
		return;

	// Ÿ�̸Ӹ� ���ؼ� crntShotDelay ���� ���̸� ���� 0���� �۾����� �� �� ź���� �߻��ϵ��� �ϰ� �ٽ� �����Ѵ�.
	playerData.crntShotDelay -= ELAPSE_BATTLE_CREATE_BULLET;
	if (IsClearShotDelay() == true)
	{
		Shot();
		ResetShotDelay();
	}
}
