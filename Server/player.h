#pragma once
#include "object.h"

typedef struct PlayerData {
	float shotDelay = 0; // �÷��̾� ź���� ��Ÿ��
	float crntShotDelay = 0; // �÷��̾� ź���� ���� ��Ÿ��
}PlayerData;

class Player : public GameObject {
private:
	PlayerData playerData;
	Vector2 posDest = { 0, }; // ������ ��ǥ(�ٷ� �̵����� �ʰ� ���������� ���� ���ݾ� �̵��Ѵ�)
	Vector2 vectorMove = { 0, }; 
	float alpha = 0; // �������� �������� ���� ���İ��̴�. 1�̸� posDest�̰� 0�̸� ���� ��ǥ�̴�.

	int skillCount = 0;
	int deathFrame = 40;

	Pokemon pokemon = Pokemon::Null;
	SubPokemon subPokemon = SubPokemon::Null;

	inline bool IsClearShotDelay() const // ��Ÿ���� �Ϸ�Ǿ����� Ȯ��
	{
		return (playerData.crntShotDelay <= 0);
	}
	inline void ResetShotDelay() // ��Ÿ�� �Ϸ�� ź�� ����
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