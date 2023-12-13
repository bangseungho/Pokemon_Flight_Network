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

// 탄막 발사 함수
void Player::Shot()
{
	// 플레이어가 탄막을 발사했다면 자신이 발사했다는 인덱스를 모든 플레이어에게 송신해야 한다.
	for (const auto& player : sPlayerMap) {
		NetworkBulletData sendData{ mSendData->mBattleData.PlayerIndex, NetworkBulletData::Status::CREATE };
		Data::SendDataAndType<NetworkBulletData>(player.second.mSock, sendData);
	}
}

// 플레이어의 기본 공격에 쿨타임을 주는 함수이다
void Player::CheckShot()
{
	if (mSendData->mBattleData.IsDeath == true)
		return;

	if (mSendData->mBattleData.IsFieldEnd == true && mSendData->mBattleData.IsEnemyClear == true)
		return;

	// 타이머를 통해서 crntShotDelay 값을 줄이며 만약 0보다 작아지면 그 때 탄막을 발사하도록 하고 다시 리셋한다.
	playerData.crntShotDelay -= ELAPSE_BATTLE_CREATE_BULLET;
	if (IsClearShotDelay() == true)
	{
		Shot();
		ResetShotDelay();
	}
}
