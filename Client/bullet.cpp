#include "stdafx.h"
#include "bullet.h"
#include "enemy.h"
#include "player.h"
#include "effect.h"
#include "boss.h"
#include "scene.h"

extern Player* player;
extern EnemyController* enemies;
extern Boss* boss;
extern EffectManager* effects;
extern SceneManager* sceneManager;

// =============================================
// Bullet
// =============================================

// 직선 탄막의 생성자에서 탄막의 데이터를 받아서 초기화
BulletController::Bullet::Bullet(const POINT& center, const POINT& bulletSize, const BulletData& data)
{
	this->data = data;

	rectBody.left = (float)center.x - ((float)bulletSize.x / 2);
	rectBody.right = rectBody.left + bulletSize.x;
	rectBody.top = (float)center.y - ((float)bulletSize.y / 2);
	rectBody.bottom = rectBody.top + bulletSize.y;
	rectRotBody = rectBody;
}

// 회전 탄막의 생성자에서 탄막의 데이터를 받아서 초기화
BulletController::Bullet::Bullet(const POINT& center, const POINT& bulletSize, const BulletData& data, const Vector2& unitVector, bool isRotateImg, bool isSkillBullet) : Bullet(center, bulletSize, data)
{
	this->dir = Dir::Empty;
	this->unitVector = unitVector;
	this->isRotateImg = isRotateImg;
	this->isSkillBullet = isSkillBullet;
	
	Vector2 vPoints[4];
	GetRotationPos(rectBody, unitVector, Vector2::Up(), vPoints); // 자신의 사각형 영역의 각 정점을 잇는 방향 벡터와 업 벡터간의 각도를 구해서 회전 위치를 구하는 함수
	rectRotBody = GetRotatedBody(vPoints);
}

// 탄막 렌더링
void BulletController::Bullet::Paint(const HDC& hdc, const ObjectImage& bulletImage)
{
	// 회전 탄막이 아닐 경우 그냥 렌더링
	if (isRotateImg == false)
	{
		bulletImage.Paint(hdc, rectRotBody);
	}
	// 회전 탄막일 경우 회전 객체 렌더링 
	else
	{
		Vector2 vPoints[4];
		GetRotationPos(rectBody, unitVector, Vector2::Up(), vPoints);
		bulletImage.PaintRotation(hdc, vPoints);
	}
}

// 탄막 이동 업데이트 함수
bool BulletController::Bullet::Update()
{
	const RECT rectDisplay = sceneManager->GetRectDisplay();
	float moveX = 0;
	float moveY = 0;
	switch (dir) // 방향에 따라서 탄막의 속도만큼 연산
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

	// 그냥 바로 자신의 사각형 영역에 연산을 진행함
	rectBody.left += moveX;
	rectBody.right += moveX;
	rectBody.top += moveY;
	rectBody.bottom += moveY;
	rectRotBody.left += moveX;
	rectRotBody.right += moveX;
	rectRotBody.top += moveY;
	rectRotBody.bottom += moveY;

	// 탄막이 화면 바깥으로 나갈 경우 false를 반환 
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

	// 탄막이 윈도우 화면 바깥을 나가지 않았으므로 true를 반환하며 계속 업데이트
	// 만약 false를 반환받게 될 경우 pop을 통해서 객체를 BulletController에서 제거한다.
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

// 탄막을 관리하는 탄막 컨트롤러의 생성자에서는 자신이 가지고 있는 탄막 배열을 기본 용량 200으로 예약한다.
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

// 탄막 생성 시 탄막 컨트롤러의 탄막 배열에 넣어준다.
void BulletController::CreateBullet(const POINT& center, const BulletData& data, Dir dir)
{
	Bullet* bullet = new Bullet(center, bulletSize, data, dir);
	bullets.emplace_back(bullet);
}
// 회전 탄막 생성 함수
void BulletController::CreateBullet(const POINT& center, const BulletData& data, const Vector2& unitVector, bool isRotateImg, bool isSkillBullet)
{
	Bullet* bullet = new Bullet(center, bulletSize, data, unitVector, isRotateImg, isSkillBullet);
	bullets.emplace_back(bullet);
}

// 플레이어 탄막의 업데이트 함수 충돌 처리도 여기서 진행한다.
void PlayerBullet::Update()
{
	for (size_t i = 0; i < bullets.size(); ++i)
	{
		const RECT rectBullet = bullets.at(i)->GetRect();
		const float bulletDamage = bullets.at(i)->GetDamage();
		const Type bulletType = bullets.at(i)->GetType();
		const POINT bulletPos = bullets.at(i)->GetPos();

		// 탄막이 적이나 보스에 충돌했을 경우 
		// 충돌 했거나 탄막이 윈도우 화면 바깥으로 나가면 탄막을 삭제한다.
		if ((enemies->CheckHit(rectBullet, bulletDamage, bulletType, bulletPos) == true) ||
			(boss->CheckHit(rectBullet, bulletDamage, bulletType, bulletPos) == true))
		{
			if (bullets.at(i)->IsSkillBullet() == false)
			{
				player->AddMP(0.30f);
			}
			BulletController::Pop(i);
		}
		else if(bullets.at(i)->Update() == false)
		{
			BulletController::Pop(i);
		}
	}
}

// 적 탄막의 업데이트 함수 충돌 처리도 여기서 진행한다.
void EnemyBullet::Update()
{
	for (size_t i = 0; i < bullets.size(); ++i)
	{
		if (player->IsCollide(bullets.at(i)->GetRect()) == true)
		{
			player->Hit(bullets.at(i)->GetDamage(), bullets.at(i)->GetType(), bullets.at(i)->GetPos());
			BulletController::Pop(i);
		}
		else if (bullets.at(i)->Update() == false)
		{
			BulletController::Pop(i);
		}
	}
}

// 탄막이 충돌했거나 윈도우 화면 바깥으로 나갔을 경우 해당 탄막 정보를 배열의 가장 뒤 원소로 복사하고
// 가장 뒤 원소인 탄막을 pop_back을 통해서 제거한다. 속도를 위해서이다.
void BulletController::Pop(size_t& index)
{
	bullets[index--] = bullets.back();
	bullets.pop_back();
}

// 만약 스킬이 탄막과 충돌했을 경우 탄막을 없애주며 이펙트 효과를 발생시켜야 한다.
// 탄막이 파괴되는 경우는 플레이어가 적이 날린 탄막에 스킬(번개 속성)을 사용한 경우이다.
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