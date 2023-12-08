#pragma once
#include "object.h"

enum class BossAct { Line = 0, Circle, Spiral, Sector, Spread, Skill1, Skill2, Idle };

struct BulletData;
class ObjectImage;
class EnemyBullet;
class BossSkillManager;

#define BOSS_BULLET_LIST 5
#define BOSS_SKILL_LIST 8

typedef struct BossData {
	Type type = Type::Empty;

	int actDelay = 0;
	int crntActDelay = 0;
	int attackDelay[BOSS_BULLET_LIST] = { 0, };
	int crntAttackDelay = 0;

	float hp = 0;
	float damage = 0;
	float damage_skill1 = 0;
	float damage_skill2 = 0;
	float speed = 0;
	float bulletSpeed[BOSS_BULLET_LIST] = { 0, };

	int frameNum_Idle = 0;
	int frameNum_IdleMax = 0;
	int frameNum_Atk = 0;
	int frameNum_AtkMax = 0;
	int frameNum_AtkRev = 0;

	bool isCreated = false;
	bool isDeath = false;
}BossData;

class Boss : public GameObject, public IAnimatable, public IMovable {
private:
	BossData bossData;
	ObjectImage* image = nullptr;
	EnemyBullet* bullets = nullptr;
	BossAct act = BossAct::Idle;

	Vector2 posDest = { 0, };
	Vector2 unitVector = { 0, };

	BossSkillManager* skill = nullptr;
	int maxSkillCount[BOSS_SKILL_LIST] = { 0, };
	int skillCount = 0;
	int deathFrame = 30;
	int normalSkillCount = 0;

	void SetMove(const Vector2& unitVector);
	void SetPosDest() override;
	void Death();
	void StartAttack();
	void Shot();
	BulletData GetBulletData();

	void ResetAttackDelay();
	inline bool IsClearAttackDelay()
	{
		return (bossData.crntAttackDelay <= 0);
	}
	inline void ResetActDelay()
	{
		bossData.crntActDelay = bossData.actDelay;
	}
	inline bool IsClearActDelay()
	{
		return (bossData.crntActDelay <= 0);
	}

	void ShotByLine();
	void ShotByCircle();
	void ShotBySpiral();
	void ShotBySector();
	void ShotBySpread();

	BossData GetBossData();
public:
	Boss();
	~Boss();
	void Create();
	void Paint(HDC hdc);
	void Move() override;
	void CheckAttackDelay();
	void CheckActDelay();

	void Animate(const HWND& hWnd);
	void AnimateSkill();
	bool CheckHit(const RECT& rectSrc, float damage, Type hitType, POINT effectPoint = { -1, });

	inline constexpr void ReSetBossAct()
	{
		act = BossAct::Idle;
	}
	inline constexpr Type GetType() const
	{
		return bossData.type;
	}
	inline constexpr bool IsCreated() const
	{
		return bossData.isCreated;
	}
	inline constexpr bool IsDeath() const
	{
		return bossData.isDeath;
	}
	inline constexpr BossAct GetAct() const
	{
		return act;
	}
	inline constexpr void SetAct(BossAct act)
	{
		this->act = act;
	}
	inline constexpr float GetDamage_Skill1() const
	{
		return bossData.damage_skill1;
	}
	inline constexpr float GetDamage_Skill2() const
	{
		return bossData.damage_skill2;
	}
	inline constexpr void KillBoss()
	{
		bossData.hp = 1.0f;
	}
};
