#pragma once
#include "object.h"

typedef struct PlayerData {
	float shotDelay = 0; // 플레이어 탄막의 쿨타임
	float crntShotDelay = 0; // 플레이어 탄막의 현재 쿨타임
}PlayerData;

class Player : public GameObject {
private:
	PlayerData playerData;
	Vector2 posDest = { 0, }; // 목적지 좌표(바로 이동하지 않고 선형보간을 통해 조금씩 이동한다)
	Vector2 vectorMove = { 0, }; 
	float alpha = 0; // 선형보간 움직임을 위한 알파값이다. 1이면 posDest이고 0이면 현재 좌표이다.

	int skillCount = 0;
	int deathFrame = 40;

	Pokemon pokemon = Pokemon::Null;
	SubPokemon subPokemon = SubPokemon::Null;

	inline bool IsClearShotDelay() const // 쿨타임이 완료되었는지 확인
	{
		return (playerData.crntShotDelay <= 0);
	}
	inline void ResetShotDelay() // 쿨타임 완료시 탄막 장전
	{
		playerData.crntShotDelay = playerData.shotDelay;
	}
public:
	Player(NetworkPlayerData* recvData);
	virtual ~Player();
	void Update();

	void CheckShot();
	void Shot();

public:
	NetworkPlayerData* mSendData = nullptr;
};