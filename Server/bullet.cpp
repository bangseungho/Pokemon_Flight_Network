#include "..\Utils.h"
#include "bullet.h"
#include "enemy.h"
#include "player.h"
#include "effect.h"
#include "boss.h"

// =============================================
// Bullet
// =============================================

extern unordered_map<uint8, NetworkPlayerData> sPlayerMap;

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

// ź�� �̵� ������Ʈ �Լ�
bool BulletController::Bullet::Update()
{
	const RECT rectDisplay = { (float)(RECT_WINDOW_WIDTH / 2 - 40), (float)(RECT_WINDOW_HEIGHT / 2 - 40), (float)(RECT_WINDOW_WIDTH / 2 + 40), (float)(RECT_WINDOW_HEIGHT / 2 + 40) };
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

// ź�� ���� �� ź�� ��Ʈ�ѷ��� ź�� �迭�� �־��ش�.
void BulletController::CreateBullet(const POINT& center, BulletData& data, Dir dir)
{
	data.id = mAccId++;
	Bullet* bullet = new Bullet(center, bulletSize, data, dir);
	bullets.emplace_back(bullet);

	for (const auto& player : sPlayerMap) {
		NetworkBulletData sendData{ bullet->GetSendData().PlayerIndex, data.id, NetworkBulletData::Status::CREATE, center};
		Data::SendDataAndType<NetworkBulletData>(player.second.mSock, sendData);
	}

	bullet->GetSendData().Status = NetworkBulletData::Status::MOVE;
}
// ȸ�� ź�� ���� �Լ�
void BulletController::CreateBullet(const POINT& center, BulletData& data, const Vector2& unitVector, bool isRotateImg, bool isSkillBullet)
{
	data.id = mAccId++;
	Bullet* bullet = new Bullet(center, bulletSize, data, unitVector, isRotateImg, isSkillBullet);
	bullets.emplace_back(bullet);

	for (const auto& player : sPlayerMap) {
		NetworkBulletData sendData{ bullet->GetSendData().PlayerIndex, data.id, NetworkBulletData::Status::CREATE, center };
		Data::SendDataAndType<NetworkBulletData>(player.second.mSock, sendData);
	}

	bullet->GetSendData().Status = NetworkBulletData::Status::MOVE;
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

		//if (bullets.at(i)->Update() == false)
		//	BulletController::Pop(i);

		// ź���� ���̳� ������ �浹���� ��� 
		// �浹 �߰ų� ź���� ������ ȭ�� �ٱ����� ������ ź���� �����Ѵ�.
		if ((mEnemyController->CheckHit(rectBullet, bulletDamage, bulletType, bulletPos) == true)/* ||
			(boss->CheckHit(rectBullet, bulletDamage, bulletType, bulletPos) == true)*/)
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

// ź���� �浹�߰ų� ������ ȭ�� �ٱ����� ������ ��� �ش� ź�� ������ �迭�� ���� �� ���ҷ� �����ϰ�
// ���� �� ������ ź���� pop_back�� ���ؼ� �����Ѵ�. �ӵ��� ���ؼ��̴�.
void BulletController::Pop(size_t& index)
{
	for (const auto& player : sPlayerMap) {
		bullets[index]->GetSendData().Status = NetworkBulletData::Status::DEATH;
		Data::SendDataAndType<NetworkBulletData>(player.second.mSock, bullets[index]->GetSendData());
	}

	bullets[index] = bullets.back();
	bullets[index]->GetSendData().BulletIndex = index;
	bullets[index--]->GetSendData().Status = NetworkBulletData::Status::MOVE;
	mAccId--;

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
			//effects->CreateHitEffect(bullets.at(i)->GetPos(), bullets.at(i)->GetType());
			BulletController::Pop(i);
		}
	}
}

void EnemyBullet::Update()
{
}