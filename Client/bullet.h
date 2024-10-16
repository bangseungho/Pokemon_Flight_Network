#pragma once
#include "image.h"

typedef struct BulletData {
	Type bulletType = Type::Empty;
	float damage = 0;
	float speed = 0;
}BulletData;

class BulletController abstract : public ISprite {
protected:
	class Bullet {
	private:
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

		void Paint(const HDC& hdc, const ObjectImage& bulletImage);
		bool Move();

		POINT GetPos() const;
		inline bool IsCollide(const RECT& rect) const
		{
			return IntersectRect2(rect, rectBody);
		}
		inline constexpr float GetDamage() const
		{
			return data.damage;
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

public:
	void Pop(size_t& index);

	void Paint(HDC hdc);

	void CreateBullet(const POINT& center, const BulletData& data, Dir dir);
	void CreateBullet(const POINT& center, const BulletData& data, const Vector2& unitVector, bool isRotateImg = false, bool isSkillBullet = false);
	void DestroyCollideBullet(const RECT& rect);

	virtual void Move() abstract;
};

class PlayerBullet : public BulletController {
public:
	PlayerBullet(const ObjectImage& bulletImage) : BulletController(bulletImage) {};
	void Move() override;
};
class EnemyBullet : public BulletController {
public:
	EnemyBullet(const ObjectImage& bulletImage) : BulletController(bulletImage) {};
	void Move() override;
	POINT GetBulletSize() const
	{
		return bulletImage.GetDrawSize();
	}
};