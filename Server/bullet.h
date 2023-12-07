#pragma once
#include "image.h"

typedef struct BulletData {
	Type bulletType = Type::Empty;
	float damage = 0;
	float speed = 0;
	uint32 id = 0;
}BulletData;

class Player;
class EnemyController;
class BulletController abstract : public ISprite {
protected:
	class Bullet {
	private:
		NetworkBulletData mSendData;
		BulletData data; // 탄막 데이터
		bool isSkillBullet = false; // 스킬 탄막인지
		bool isRotateImg = false; // 회전 이미지인지

		Dir dir = Dir::Empty; // 총알 방향
		Vector2 unitVector; // 해당 탄막을 발사한 유닛의 위치

		FRECT rectBody = { 0, };
		FRECT rectRotBody = { 0, };
		Bullet(const POINT& center, const POINT& bulletSize, const BulletData& data);
	public:
		Bullet(const POINT& center, const POINT& bulletSize, const BulletData& data, Dir dir) : Bullet(center, bulletSize, data) { this->dir = dir; };
		Bullet(const POINT& center, const POINT& bulletSize, const BulletData& data, const Vector2& unitVector, bool isRotateImg, bool isSkillBullet = false);
		~Bullet() {};

		bool Update();

		POINT GetPos() const;
		inline bool IsCollide(const RECT& rect) const
		{
			return IntersectRect2(rect, rectBody);
		}
		inline constexpr float GetDamage() const
		{
			return data.damage;
		}		
		inline NetworkBulletData& GetSendData()
		{
			return mSendData;
		}
		inline constexpr Dir GetDir() const
		{
			return dir;
		}
		inline constexpr RECT GetRect() const
		{
			return rectRotBody;
		}
		inline constexpr Type GetType() const
		{
			return data.bulletType;
		}
		inline constexpr bool IsSkillBullet() const
		{
			return isSkillBullet;
		}
	};

	BulletController(const ObjectImage& bulletImage);
	~BulletController();

	std::vector<Bullet*> bullets;
	ObjectImage bulletImage;
	POINT bulletSize = { 0, };

	void Pop(size_t& index);
public:
	void CreateBullet(const POINT& center, BulletData& data, Dir dir);
	void CreateBullet(const POINT& center, BulletData& data, const Vector2& unitVector, bool isRotateImg = false, bool isSkillBullet = false);
	void DestroyCollideBullet(const RECT& rect);

	virtual void Update() abstract;

private:
	uint32 mAccId = 0;
};

class PlayerBullet : public BulletController {
public:
	PlayerBullet(const ObjectImage& bulletImage, shared_ptr<EnemyController> enemyController) : BulletController(bulletImage) { mEnemyController = enemyController; }
	void SetPlayer(shared_ptr<Player> player) { mPlayer = player; }
	void Update() override;

private:
	shared_ptr<Player>				mPlayer = nullptr;
	shared_ptr<EnemyController>		mEnemyController = nullptr;
};

class EnemyBullet : public BulletController {
public:
	EnemyBullet(const ObjectImage& bulletImage) : BulletController(bulletImage) {};
	void Update() override;
	POINT GetBulletSize() const
	{
		return bulletImage.GetDrawSize();
	}
};