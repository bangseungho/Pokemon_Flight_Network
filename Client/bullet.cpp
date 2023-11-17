#include "stdafx.h"
#include "bullet.h"
#include "enemy.h"
#include "player.h"
#include "effect.h"
#include "boss.h"
#include "scene.h"

extern Player* mPlayer;
extern EnemyController* enemies;
extern Boss* boss;
extern EffectManager* effects;
extern SceneManager* sceneManager;

// =============================================
// Bullet
// =============================================

// ���� ź���� �����ڿ��� ź���� �����͸� �޾Ƽ� �ʱ�ȭ
BulletController::Bullet::Bullet(const POINT& center, const POINT& bulletSize, const BulletData& data)
{
	this->data = data;

	rectBody.left = (float)center.x - ((float)bulletSize.x / 2);
	rectBody.right = rectBody.left + bulletSize.x;
	rectBody.top = (float)center.y - ((float)bulletSize.y / 2);
	rectBody.bottom = rectBody.top + bulletSize.y;
	rectRotBody = rectBody;
}

// ȸ�� ź���� �����ڿ��� ź���� �����͸� �޾Ƽ� �ʱ�ȭ
BulletController::Bullet::Bullet(const POINT& center, const POINT& bulletSize, const BulletData& data, const Vector2& unitVector, bool isRotateImg, bool isSkillBullet) : Bullet(center, bulletSize, data)
{
	this->dir = Dir::Empty;
	this->unitVector = unitVector;
	this->isRotateImg = isRotateImg;
	this->isSkillBullet = isSkillBullet;
	
	Vector2 vPoints[4];
	GetRotationPos(rectBody, unitVector, Vector2::Up(), vPoints); // �ڽ��� �簢�� ������ �� ������ �մ� ���� ���Ϳ� �� ���Ͱ��� ������ ���ؼ� ȸ�� ��ġ�� ���ϴ� �Լ�
	rectRotBody = GetRotatedBody(vPoints);
}

// ź�� ������
void BulletController::Bullet::Paint(const HDC& hdc, const ObjectImage& bulletImage)
{
	// ȸ�� ź���� �ƴ� ��� �׳� ������
	if (isRotateImg == false)
	{
		bulletImage.Paint(hdc, rectRotBody);
	}
	// ȸ�� ź���� ��� ȸ�� ��ü ������ 
	else
	{
		Vector2 vPoints[4];
		GetRotationPos(rectBody, unitVector, Vector2::Up(), vPoints);
		bulletImage.PaintRotation(hdc, vPoints);
	}
}

// ź�� �̵� ������Ʈ �Լ�
bool BulletController::Bullet::Update()
{
	const RECT rectDisplay = sceneManager->GetRectDisplay();
	float moveX = 0;
	float moveY = 0;
	switch (dir) // ���⿡ ���� ź���� �ӵ���ŭ ����
	{
	case Dir::Empty:
		moveX = unitVector.x * data.speed;
		moveY = unitVector.y * data.speed;
		break;
	case Dir::Left:
		moveX = -data.speed;
		break;
	case Dir::Right:
		moveX = data.speed;
		break;
	case Dir::Up:
		moveY = -data.speed;
		break;
	case Dir::Down:
		moveY = data.speed;
		break;
	case Dir::LU:
		moveX = -data.speed;
		moveY = -data.speed;
		break;
	case Dir::LD:
		moveX = -data.speed;
		moveY = data.speed;
		break;
	case Dir::RU:
		moveX = data.speed;
		moveY = -data.speed;
		break;
	case Dir::RD:
		moveX = data.speed;
		moveY = data.speed;
		break;
	default:
		assert(0);
		break;
	}

	// �׳� �ٷ� �ڽ��� �簢�� ������ ������ ������
	rectBody.left += moveX;
	rectBody.right += moveX;
	rectBody.top += moveY;
	rectBody.bottom += moveY;
	rectRotBody.left += moveX;
	rectRotBody.right += moveX;
	rectRotBody.top += moveY;
	rectRotBody.bottom += moveY;

	// ź���� ȭ�� �ٱ����� ���� ��� false�� ��ȯ 
	switch (dir)
	{
	case Dir::Empty:
		if (OutOfRange(rectRotBody, rectDisplay) == true)
		{
			return false;
		}
		break;
	case Dir::Left:
	case Dir::LU:
	case Dir::LD:
		if (rectRotBody.right < 0)
		{
			return false;
		}
		break;
	case Dir::Right:
	case Dir::RU:
	case Dir::RD:
		if (rectRotBody.left > rectDisplay.right)
		{
			return false;
		}
		break;
	default:
		break;
	}

	switch (dir)
	{
	case Dir::Up:
	case Dir::LU:
	case Dir::RU:
		if (rectRotBody.bottom < 0)
		{
			return false;
		}
		break;
	case Dir::Down:
	case Dir::LD:
	case Dir::RD:
		if (rectRotBody.top > rectDisplay.bottom)
		{
			return false;
		}
		break;
	default:
		break;
	}

	// ź���� ������ ȭ�� �ٱ��� ������ �ʾ����Ƿ� true�� ��ȯ�ϸ� ��� ������Ʈ
	// ���� false�� ��ȯ�ް� �� ��� pop�� ���ؼ� ��ü�� BulletController���� �����Ѵ�.
	return true;
}

POINT BulletController::Bullet::GetPos() const
{
	const int width = rectBody.right - rectBody.left;
	const int height = rectBody.bottom - rectBody.top;
	
	return { (LONG)rectBody.left + (width / 2), (LONG)rectBody.top + (height / 2) };
}

// =============================================
// BulletController
// =============================================

// ź���� �����ϴ� ź�� ��Ʈ�ѷ��� �����ڿ����� �ڽ��� ������ �ִ� ź�� �迭�� �⺻ �뷮 200���� �����Ѵ�.
BulletController::BulletController(const ObjectImage& bulletImage)
{
	this->bulletImage = bulletImage;

	bulletSize = bulletImage.GetBodySize();
	bullets.reserve(200);
}

BulletController::~BulletController()
{
	for (Bullet* bullet : bullets)
	{
		delete bullet;
	}
}

void BulletController::Paint(HDC hdc)
{
	for (Bullet* bullet : bullets)
	{
		bullet->Paint(hdc, bulletImage);
	}
}

// ź�� ���� �� ź�� ��Ʈ�ѷ��� ź�� �迭�� �־��ش�.
void BulletController::CreateBullet(const POINT& center, const BulletData& data, Dir dir)
{
	Bullet* bullet = new Bullet(center, bulletSize, data, dir);
	bullets.emplace_back(bullet);
}
// ȸ�� ź�� ���� �Լ�
void BulletController::CreateBullet(const POINT& center, const BulletData& data, const Vector2& unitVector, bool isRotateImg, bool isSkillBullet)
{
	Bullet* bullet = new Bullet(center, bulletSize, data, unitVector, isRotateImg, isSkillBullet);
	bullets.emplace_back(bullet);
}

// �÷��̾� ź���� ������Ʈ �Լ� �浹 ó���� ���⼭ �����Ѵ�.
void PlayerBullet::Update()
{
	for (size_t i = 0; i < bullets.size(); ++i)
	{
		const RECT rectBullet = bullets.at(i)->GetRect();
		const float bulletDamage = bullets.at(i)->GetDamage();
		const Type bulletType = bullets.at(i)->GetType();
		const POINT bulletPos = bullets.at(i)->GetPos();

		// ź���� ���̳� ������ �浹���� ��� 
		// �浹 �߰ų� ź���� ������ ȭ�� �ٱ����� ������ ź���� �����Ѵ�.
		if ((enemies->CheckHit(rectBullet, bulletDamage, bulletType, bulletPos) == true) ||
			(boss->CheckHit(rectBullet, bulletDamage, bulletType, bulletPos) == true))
		{
			if (bullets.at(i)->IsSkillBullet() == false)
			{
				mPlayer->AddMP(0.30f);
			}
			BulletController::Pop(i);
		}
		else if(bullets.at(i)->Update() == false)
		{
			BulletController::Pop(i);
		}
	}
}

// �� ź���� ������Ʈ �Լ� �浹 ó���� ���⼭ �����Ѵ�.
void EnemyBullet::Update()
{
	for (size_t i = 0; i < bullets.size(); ++i)
	{
		if (mPlayer->IsCollide(bullets.at(i)->GetRect()) == true)
		{
			mPlayer->Hit(bullets.at(i)->GetDamage(), bullets.at(i)->GetType(), bullets.at(i)->GetPos());
			BulletController::Pop(i);
		}
		else if (bullets.at(i)->Update() == false)
		{
			BulletController::Pop(i);
		}
	}
}

// ź���� �浹�߰ų� ������ ȭ�� �ٱ����� ������ ��� �ش� ź�� ������ �迭�� ���� �� ���ҷ� �����ϰ�
// ���� �� ������ ź���� pop_back�� ���ؼ� �����Ѵ�. �ӵ��� ���ؼ��̴�.
void BulletController::Pop(size_t& index)
{
	bullets[index--] = bullets.back();
	bullets.pop_back();
}

// ���� ��ų�� ź���� �浹���� ��� ź���� �����ָ� ����Ʈ ȿ���� �߻����Ѿ� �Ѵ�.
// ź���� �ı��Ǵ� ���� �÷��̾ ���� ���� ź���� ��ų(���� �Ӽ�)�� ����� ����̴�.
void BulletController::DestroyCollideBullet(const RECT& rect)
{
	for (size_t i = 0; i < bullets.size(); ++i)
	{
		if (bullets.at(i)->IsCollide(rect) == true)
		{
			effects->CreateHitEffect(bullets.at(i)->GetPos(), bullets.at(i)->GetType());
			BulletController::Pop(i);
		}
	}
}