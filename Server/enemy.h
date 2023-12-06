#pragma once
#include "object.h"
//
//struct BulletData;
//class Player;
//class EnemyBullet;

typedef struct EnemyData {
	Type type = Type::Empty;

	int attackDelay = 0;
	int crntAttackDelay = 0;
	float bulletSpeed = 0;

	float hp = 0;
	float speed = 0;
	float damage = 0;

	int maxYPos = 0;
	int frameNum_Idle = 0;
	int frameNum_IdleMax = 0;
	int frameNum_Atk = 0;
	int frameNum_AtkMax = 0;
	int frameNum_AtkRev = 0;
	POINT bodySize = { 0, };
}EnemyData;

class Enemy abstract : public GameObject, public IMovable{
protected:
	EnemyData data; // 적 데이터로 생성자에서 초기화
	Vector2 posDest = { 0, }; // 포지션 위치 벡터
	Vector2 unitVector = { 0, }; // 플레이어의 방향 벡터
	uint32 id;
	NetworkEnemyData mSendData;

	Dir GetDir() const;
	virtual void SetPosDest() abstract override;

	inline void ResetAttackDelay()
	{
		data.crntAttackDelay = data.attackDelay;
	}
	inline bool IsClearAttackDelay() const
	{
		return (data.crntAttackDelay <= 0);
	}
public:
	Enemy(const Vector2& pos, const EnemyData& data);
	virtual void Update() override;
	virtual void CheckAttackDelay() abstract;

	int GetSpriteRow();
	NetworkEnemyData& GetSendData() { return mSendData; }
//	bool Hit(float damage);
//
//	inline Type GetType() const
//	{
//		return data.type;
//	}
	uint32 GetID() const { return id; }

public:
	static uint32 sId;
};

class Melee : public Enemy {
private:
	void SetPosDest();
	bool CheckCollidePlayer();
public:
	Melee(const Vector2& pos, const EnemyData& data);
	void Update() override;

private:
	void CheckAttackDelay() override;
};

class Range : public Enemy {
private:
	void SetPosDest();
//	void Fire();
public:
	Range(const Vector2& pos, const EnemyData& data);
	void Update() override;
	void CheckAttackDelay() override;
};

class EnemyController {
private:
	std::vector<Enemy*> enemies;

	//EnemyBullet* bullets = nullptr;

	EnemyData meleeData;
	EnemyData rangeData;

	int createDelay_Melee = 0;
	int createDelay_Range = 0;
	int delay_Melee = 0;
	int delay_Range = 0;
	int createAmount_Melee = 0;
	int createAmount_Range = 0;

	//void Pop(size_t& index);
public:
	EnemyController();
	~EnemyController();
	void CreateCheckMelee();
	void CreateCheckRange();
	void Update();
	//bool CheckHit(const RECT& rectSrc, float damage, Type hitType, const POINT& effectPoint);
	//void CheckHitAll(const RECT& rectSrc, float damage, Type hitType);

	//void CreateBullet(const POINT& center, const BulletData& data, const Vector2& unitVector);
	//void CreateBullet(const POINT& center, const BulletData& data, Dir dir);
	//void MoveBullets();
	//void DestroyCollideBullet(const RECT& rect);

	inline bool IsEmenyClear()
	{
		return enemies.empty();
	}
	inline void CheckAttackDelay()
	{
		for (Enemy* enemy : enemies)
		{
			enemy->CheckAttackDelay();
		}
	}

public:
	void ShowEnemyCount() const;
};