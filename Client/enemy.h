#pragma once
#include "object.h"
struct BulletData;
class Player;
class EnemyBullet;

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
	uint32 id = 0;
}EnemyData;

class Enemy abstract : public GameObject, public IAnimatable, public IMovable {
protected:
	EnemyData data; // 적 데이터로 생성자에서 초기화
	Vector2 posDest = { 0, }; // 포지션 위치 벡터
	Vector2 unitVector = { 0, }; // 플레이어의 방향 벡터
	//NetworkEnemyData mRecvData;

	void Paint(const HDC& hdc, int spriteRow);
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
	Enemy(ObjectImage& image, const Vector2& pos, const EnemyData& data, uint32 id);
	virtual void Paint(const HDC& hdc) abstract;
	virtual void Move() override;
	//virtual void Update() override;
	//void SetSpriteRow(int spriteRow);

	virtual void CheckAttackDelay() abstract;
	//void SetRecvData(NetworkEnemyData&& recvData) { mRecvData = recvData; }
	//NetworkEnemyData GetRecvData() { return mRecvData; }

	int GetSpriteRow();
	void Animate();
	bool Hit(float damage);

	inline Type GetType() const
	{
		return data.type;
	}
	inline uint32 GetId() const
	{
		return data.id;
	}
	inline void SetAction(Action action)
	{
		switch (action)
		{
		case Action::Idle:
			IAnimatable::SetAction(action, data.frameNum_Idle);
			break;
		case Action::Attack:
			IAnimatable::SetAction(action, data.frameNum_Atk);
			break;
		default:
			break;
		}
	}
};

class Melee : public Enemy {
private:
	void SetPosDest();
	int8 CheckCollidePlayer();
	bool CheckRecvCollidePlayer();
public:
	Melee(ObjectImage& image, const Vector2& pos, const EnemyData& data, uint8 targetIndex, uint32 id) : Enemy(image, pos, data, id) { mTargetIndex = targetIndex; }
	void Paint(const HDC& hdc) override;
	void Move() override;
	void CheckAttackDelay() override;
	uint8 mTargetIndex = 0;
};

class Range : public Enemy {
private:
	const ObjectImage* bulletImage = nullptr;

	void SetPosDest();
	void Fire();
public:
	Range(ObjectImage& image, const Vector2& pos, const EnemyData& data, uint32 id) : Enemy(image, pos, data, id) {};
	void Paint(const HDC& hdc) override;
	void Move() override;
	void CheckAttackDelay() override;
};

class EnemyController {
private:
	std::vector<Enemy*> enemies;

	EnemyBullet* bullets = nullptr;

	EnemyData meleeData;
	EnemyData rangeData;

	int createDelay_Melee = 0;
	int createDelay_Range = 0;
	int delay_Melee = 0;
	int delay_Range = 0;
	int createAmount_Melee = 0;
	int createAmount_Range = 0;

	ObjectImage imgMelee;
	ObjectImage imgRange;

public:
	EnemyController();
	~EnemyController();
	void Pop(int32 index);
	void CreateCheckMelee();
	void CreateCheckRange();
	void CreateRecvMelee(NetworkEnemyData& recvData);
	void CreateRecvRange(NetworkEnemyData& recvData);
	void Paint(HDC hdc);
	void Move();
	void Animate();
	bool CheckHit(const RECT& rectSrc, float damage, Type hitType, const POINT& effectPoint);
	void CheckHitAll(const RECT& rectSrc, float damage, Type hitType);

	void CreateBullet(const POINT& center, const BulletData& data, const Vector2& unitVector);
	void CreateBullet(const POINT& center, const BulletData& data, Dir dir);
	void MoveBullets();
	void DestroyCollideBullet(const RECT& rect);

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
};