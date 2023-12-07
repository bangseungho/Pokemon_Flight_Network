#pragma once
#include "object.h"

struct BulletData;
class PlayerBullet;
class SkillManager;

typedef struct PlayerData {
	Type type = Type::Empty;
	Type subType = Type::Empty;

	float maxhp = 0;
	float maxmp = 0;
	float hp = 0;
	float mp = 0;
	float speed = 0;

	float bulletSpeed = 0;
	float shotDelay = 0; // 플레이어 탄막의 쿨타임
	float crntShotDelay = 0; // 플레이어 탄막의 현재 쿨타임
	float damage = 0;
	float subDamage = 0;
	float damage_Q = 0; // per sec

	bool isCanGo = false;
	bool isDeath = false;
	bool isInvincible = true;
}PlayerData;

class Player : public GameObject, public IControllable, public IAnimatable, public enable_shared_from_this<Player> {
private:
	PlayerData playerData;
	PlayerBullet* bullets = nullptr; // 메인 포켓몬의 탄막 컨트롤러(비행 포켓몬)
	PlayerBullet* subBullets = nullptr; // 서브 포켓몬의 탄막 컨트롤러(대지 포켓몬)
	Vector2 posDest = { 0, }; // 목적지 좌표(바로 이동하지 않고 선형보간을 통해 조금씩 이동한다)
	Vector2 vectorMove = { 0, }; 
	float alpha = 0; // 선형보간 움직임을 위한 알파값이다. 1이면 posDest이고 0이면 현재 좌표이다.

	SkillManager* skillManager = nullptr;
	int skillCount = 0;
	int deathFrame = 40;

	Pokemon pokemon = Pokemon::Null;
	SubPokemon subPokemon = SubPokemon::Null;
	ObjectImage img_mainPokemon{};
	ObjectImage img_subPokemon{};

	void Death();
	inline bool IsClearShotDelay() const // 쿨타임이 완료되었는지 확인
	{
		return (playerData.crntShotDelay <= 0);
	}
	inline void ResetShotDelay() // 쿨타임 완료시 탄막 장전
	{
		playerData.crntShotDelay = playerData.shotDelay;
	}
public:
	Player(NetworkPlayerData* recvData, shared_ptr<class EnemyController> enemyController);
	virtual ~Player();
	void Init();

	void SetBulletsPlayer(shared_ptr<Player> player);

	void CheckShot();
	void Shot();
	void CreateSubBullet(const POINT& center, const BulletData& data, Vector2 unitVector, bool isRotateImg, bool isSkillBullet = false);


	void ActiveSkill(Skill skill);
	void MoveBullets();
	bool IsUsingSkill() const;
	inline float GetDamage_Q() const
	{
		return playerData.damage_Q;
	}
	inline float GetDamage_WE() const
	{
		return (playerData.subDamage / 1.75f);
	}
	inline Type GetType() const
	{
		return playerData.type;
	}
	inline Type GetSubType() const
	{
		return playerData.subType;
	}
	inline float GetHP() const
	{
		return playerData.hp;
	}
	inline float GetMaxHP() const
	{
		return playerData.maxhp;
	}
	inline float GetMP() const
	{
		return playerData.mp;
	}
	inline float GetMaxMP() const
	{
		return playerData.maxmp;
	}
	inline void AddHP(float amount)
	{
		playerData.hp += amount;
		if (playerData.hp > playerData.maxhp)
		{
			playerData.hp = playerData.maxhp;
		}
	}
	inline void AddMP(float amount)
	{
		playerData.mp += amount;
		if (playerData.mp > playerData.maxmp)
		{
			playerData.mp = playerData.maxmp;
		}
	}
	inline bool ReduceMP(float amount)
	{
		if ((playerData.mp - amount) < 0)
		{
			return false;
		}
		playerData.mp -= amount;
		return true;
	}
	void InvincibleMode()
	{
		playerData.isInvincible = !playerData.isInvincible;
	}
	inline bool IsDeath() const
	{
		return playerData.isDeath;
	}

	inline void Heal()
	{
		playerData.hp = playerData.maxhp;
	}
	inline void FullMP()
	{
		playerData.mp = playerData.maxmp;
	}
public:
	NetworkPlayerData* mRecvData = nullptr;
};