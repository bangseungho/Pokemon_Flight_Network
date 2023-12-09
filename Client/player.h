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
	float shotDelay = 0; // �÷��̾� ź���� ��Ÿ��
	float crntShotDelay = 0; // �÷��̾� ź���� ���� ��Ÿ��
	float damage = 0;
	float subDamage = 0;
	float damage_Q = 0; // per sec

	bool isCanGo = false;
	bool isDeath = false;
	bool isInvincible = true;
	
	uint8 id = 0;
}PlayerData;

class Player : public GameObject, public IControllable, public IAnimatable {
private:
	PlayerData playerData;
	PlayerBullet* bullets = nullptr; // ���� ���ϸ��� ź�� ��Ʈ�ѷ�(���� ���ϸ�)
	PlayerBullet* subBullets = nullptr; // ���� ���ϸ��� ź�� ��Ʈ�ѷ�(���� ���ϸ�)
	Vector2 posDest = { 0, }; // ������ ��ǥ(�ٷ� �̵����� �ʰ� ���������� ���� ���ݾ� �̵��Ѵ�)
	Vector2 vectorMove = { 0, };
	float alpha = 0; // �������� �������� ���� ���İ��̴�. 1�̸� posDest�̰� 0�̸� ���� ��ǥ�̴�.

	SkillManager* skillManager = nullptr;
	int skillCount = 0;
	int deathFrame = 40;

	Pokemon pokemon = Pokemon::Null;
	SubPokemon subPokemon = SubPokemon::Null;
	ObjectImage img_mainPokemon{};
	ObjectImage img_subPokemon{};

	void Death();
	void SetPosDest() override;
	inline bool IsClearShotDelay() const // ��Ÿ���� �Ϸ�Ǿ����� Ȯ��
	{
		return (playerData.crntShotDelay <= 0);
	}
	inline void ResetShotDelay() // ��Ÿ�� �Ϸ�� ź�� ����
	{
		playerData.crntShotDelay = playerData.shotDelay;
	}
public:
	Player(Type type, Type subType, uint8 id);
	~Player();
	void Init();
	void Paint(HDC hdc);
	void PaintSkill(HDC hdc);

	void SetDirection(Dir dir);
	void SetMove(const HWND& hWnd, int timerID, int elpase, const TIMERPROC& timerProc) override;
	void Move(const HWND& hWnd, int timerID) override;
	void Stop(Dir dir) override;
	void CheckCollideWindow(Vector2& pos) const;

	void Animate(const HWND& hWnd);
	void CheckShot();
	void Shot();
	void Shot(NetworkBulletData& recvData);
	void BulletPop(size_t& bulletIndex);
	void CreateSubBullet(const POINT& center, const BulletData& data, Vector2 unitVector, bool isRotateImg, bool isSkillBullet = false);
	void Hit(float damage, Type hitType, uint8 memberIndex = 0, POINT effectPoint = { -1, });

	void ActiveSkill(Skill skill);
	void MoveBullets();
	bool IsUsingSkill() const;
	bool IsIdentity() const;
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
	inline uint8 GetPlayerId() const
	{
		return playerData.id;
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
	bool ReduceMP(float amount, Skill skill);
	void InvincibleMode()
	{
		playerData.isInvincible = !playerData.isInvincible;
	}
	inline bool IsDeath() const
	{
		return playerData.isDeath;
	}
	inline void SetDeath(bool death)
	{
		playerData.isDeath = death;
	}

	inline void Heal()
	{
		playerData.hp = playerData.maxhp;
	}
	inline void FullMP()
	{
		playerData.mp = playerData.maxmp;
	}
};