#include "stdafx.h"
#include "enemy.h"
#include "player.h"
#include "bullet.h"
#include "timer.h"
#include "effect.h"
#include "interface.h"
#include "boss.h"
#include "sound.h"

#include "phase.h"

extern GameData gameData;
extern Player* player;
extern Boss* boss;
extern EffectManager* effects;
extern EnemyController* enemies;
extern SoundManager* soundManager;
extern GUIManager* gui;

extern Phase phase;

Enemy::Enemy(ObjectImage& image, const Vector2& pos, const EnemyData& data) : GameObject(image, pos)
{
	StartMove();
	this->data = data;
}

Dir Enemy::GetDir() const
{
	const float theta = atan2(unitVector.y, unitVector.x);
	const float crntDegree = RADIAN_TO_DEGREE(theta);

	constexpr int unitDegree = 45;
	float degree = (float)unitDegree / 2;
	if (crntDegree > 0)
	{
		if (crntDegree < degree)
		{
			return Dir::Left;
		}
		else if (crntDegree < (degree += unitDegree))
		{
			return Dir::LU;
		}
		else if (crntDegree < (degree += unitDegree))
		{
			return Dir::Up;
		}
		else if (crntDegree < (degree += unitDegree))
		{
			return Dir::RU;
		}
		else
		{
			return Dir::Right;
		}
	}
	else
	{
		degree = -((float)unitDegree / 2);
		if (crntDegree > degree)
		{
			return Dir::Left;
		}
		else if (crntDegree > (degree -= unitDegree))
		{
			return Dir::LD;
		}
		else if (crntDegree > (degree -= unitDegree))
		{
			return Dir::Down;
		}
		else if (crntDegree > (degree -= unitDegree))
		{
			return Dir::RD;
		}
		else
		{
			return Dir::Right;
		}
	}
}

void Melee::SetPosDest()
{
	if (IsMove() == false)
	{
		return;
	}

	const Vector2 posCenter = GetPosCenter();
	const Vector2 vectorToPlayer = posCenter - player->GetPosCenter();

	const float radius = GetRadius(vectorToPlayer.x, vectorToPlayer.y);

	unitVector = vectorToPlayer / radius;

	posDest = posCenter - (unitVector * data.speed);
}
void Range::SetPosDest()
{
	if (IsMove() == false)
	{
		return;
	}

	unitVector = Vector2::Down();
	posDest = Vector2::GetDest(GetPosCenter(), unitVector, data.speed);
	if (posDest.y > data.maxYPos)
	{
		StopMove();
	}
}

void Enemy::Paint(const HDC& hdc, int spriteRow)
{
	const RECT rectImage = ISprite::GetRectImage(GetImage(), frame, spriteRow);
	GameObject::Paint(hdc, &rectImage);
}
void Melee::Paint(const HDC& hdc)
{
	const int spriteRow = GetSpriteRow();
	Enemy::Paint(hdc, spriteRow);
}
void Range::Paint(const HDC& hdc)
{
	constexpr int spriteRow = 0;
	Enemy::Paint(hdc, spriteRow);
}

void Enemy::Move()
{
	if (IsMove() == false)
	{
		return;
	}

	SetPosDest();
	SetPos(posDest);
}
void Melee::Move()
{
	if (IsMove() == false)
	{
		return;
	}
	else if (CheckCollidePlayer() == true)
	{
		player->Hit(data.damage, GetType());
		effects->CreateHitEffect(player->GetPosCenter(), GetType());
		return;
	}

	SetPosDest();
	SetPos(posDest);
}
void Range::Move()
{
	if (IsMove() == false)
	{
		return;
	}

	SetPosDest();
	SetPos(posDest);
}

int Enemy::GetSpriteRow()
{
	int spriteRow = 0;
	switch (GetDir())
	{
	case Dir::Empty:
	case Dir::Up:
		spriteRow = 0;
		break;
	case Dir::Down:
		spriteRow = 1;
		break;
	case Dir::Left:
		spriteRow = 2;
		break;
	case Dir::Right:
		spriteRow = 3;
		break;
	case Dir::LU:
		spriteRow = 4;
		break;
	case Dir::RU:
		spriteRow = 5;
		break;
	case Dir::LD:
		spriteRow = 6;
		break;
	case Dir::RD:
		spriteRow = 7;
		break;
	default:
		assert(0);
		break;
	}

	return spriteRow;
}

void Enemy::Animate()
{
	if (isRevFrame == true)
	{
		--frame;
	}
	else
	{
		++frame;
	}

	switch (GetAction())
	{
	case Action::Idle:
		if (frame > data.frameNum_IdleMax)
		{
			isRevFrame = true;
			--frame;
		}
		else if (frame < data.frameNum_Idle)
		{
			isRevFrame = false;
			++frame;
		}
		break;
	case Action::Attack:
		if (frame > data.frameNum_AtkMax)
		{
			isRevFrame = true;
			--frame;
		}
		else if (isRevFrame == true && frame < data.frameNum_AtkRev)
		{
			isRevFrame = false;
			SetAction(Action::Idle, data.frameNum_Idle);
		}
		break;
	default:
		assert(0);
		break;
	}
}

bool Melee::CheckCollidePlayer()
{
	const RECT rectBody = GetRectBody();
	if (player->IsCollide(rectBody) == true)
	{
		StopMove();
		SetAction(Action::Attack, data.frameNum_Atk);
		ResetAttackDelay();

		return true;
	}

	return false;
}
bool Enemy::Hit(float damage)
{
	if ((data.hp -= damage) <= 0)
	{
		return true;
	}

	return false;
}

void Melee::CheckAttackDelay()
{
	if (IsMove() == false)
	{
		data.crntAttackDelay -= ELAPSE_BATTLE_INVALIDATE;
		if (IsClearAttackDelay() == true)
		{
			StartMove();
		}
	}
}
void Range::CheckAttackDelay()
{
	if (IsMove() == false)	
	{
		data.crntAttackDelay -= ELAPSE_BATTLE_INVALIDATE;
		if (IsClearAttackDelay() == true)
		{
			Fire();
			ResetAttackDelay();
		}
	}
}

void Range::Fire()
{
	SetAction(Action::Attack, data.frameNum_Atk);

	RECT rectBody = GetRectBody();
	POINT bulletPos = { 0, };
	bulletPos.x = rectBody.left + ((rectBody.right - rectBody.left) / 2);
	bulletPos.y = rectBody.bottom;

	BulletData bulletData;
	bulletData.bulletType = GetType();
	bulletData.damage = data.damage;
	bulletData.speed = data.bulletSpeed;

	Vector2 unitVector = Vector2::Down();
	int randDegree = (rand() % 10) - 5;

	unitVector = Rotate(unitVector, randDegree);
	enemies->CreateBullet(bulletPos, bulletData, unitVector);
	unitVector = Rotate(unitVector, 20);
	enemies->CreateBullet(bulletPos, bulletData, unitVector);
	unitVector = Rotate(unitVector, -40);
	enemies->CreateBullet(bulletPos, bulletData, unitVector);
}






void EnemyController::Pop(size_t& index)
{
	effects->CreateExplosionEffect(enemies.at(index)->GetPosCenter(), enemies.at(index)->GetType());
	soundManager->PlayEffectSound(EffectSound::Explosion);
	enemies[index--] = enemies.back();
	enemies.pop_back();
}
EnemyController::EnemyController()
{
	ObjectImage imgRangeBullet;
	switch (gameData.stage)
	{
	case StageElement::Elec:
		meleeData.type = Type::Elec;
		rangeData.type = Type::Elec;
		imgMelee.Load(L"images\\battle\\sprite_beedrill.png", { 33,33 }, { 7,6 }, { 21,22 });
		imgMelee.ScaleImage(1.2f, 1.2f);
		imgRange.Load(L"images\\battle\\sprite_zapdos.png", { 58,58 }, { 12,12 }, { 36,46 });
		imgRangeBullet.Load(L"images\\battle\\bullet_zapdos.png", { 14,14 });
		imgRangeBullet.ScaleImage(0.9f, 0.9f);
		createDelay_Melee = 1500;
		createDelay_Range = 3000;
		createAmount_Melee = 5;
		createAmount_Range = 5;

		meleeData.hp = 6;
		meleeData.speed = 1.5f;
		meleeData.attackDelay = 1000;
		meleeData.damage = 3;

		meleeData.frameNum_Idle = 0;
		meleeData.frameNum_IdleMax = 2;
		meleeData.frameNum_Atk = 3;
		meleeData.frameNum_AtkMax = 4;

		rangeData.hp = 4.75f;
		rangeData.speed = 2;
		rangeData.attackDelay = 2000;
		rangeData.damage = 2.25f;

		rangeData.frameNum_Idle = 0;
		rangeData.frameNum_IdleMax = 2;
		rangeData.frameNum_Atk = 3;
		rangeData.frameNum_AtkMax = 4;
		rangeData.frameNum_AtkRev = 4;
		rangeData.bulletSpeed = 4;
		meleeData.frameNum_AtkRev = 3;
		break;
	case StageElement::Water:
		meleeData.type = Type::Water;
		rangeData.type = Type::Water;
		imgMelee.Load(L"images\\battle\\sprite_wingull.png", { 34,33 }, { 4,6 }, { 28,22 });
		imgMelee.ScaleImage(1.2f, 1.2f);
		imgRange.Load(L"images\\battle\\sprite_seadra.png", { 29,31 }, { 3,3 }, { 25,28 });
		imgRange.ScaleImage(1.2f, 1.2f);
		imgRangeBullet.Load(L"images\\battle\\bullet_seadra.png", { 14,14 });
		imgRangeBullet.ScaleImage(1.2f, 1.2f);

		createDelay_Melee = 1350;
		createDelay_Range = 3250;
		createAmount_Melee = 7;
		createAmount_Range = 5;

		meleeData.hp = 2.85f;
		meleeData.speed = 2;
		meleeData.attackDelay = 700;
		meleeData.damage = 2.5f;

		meleeData.frameNum_Idle = 0;
		meleeData.frameNum_IdleMax = 2;
		meleeData.frameNum_Atk = 3;
		meleeData.frameNum_AtkMax = 4;
		meleeData.frameNum_AtkRev = 3;

		rangeData.hp = 5;
		rangeData.speed = 0.7f;
		rangeData.attackDelay = 2000;
		rangeData.damage = 2.8f;

		rangeData.frameNum_Idle = 0;
		rangeData.frameNum_IdleMax = 2;
		rangeData.frameNum_Atk = 3;
		rangeData.frameNum_AtkMax = 6;
		rangeData.frameNum_AtkRev = 3;
		rangeData.bulletSpeed = 3;
		break;
	case StageElement::Fire:
		meleeData.type = Type::Fire;
		rangeData.type = Type::Fire;
		imgMelee.Load(L"images\\battle\\sprite_ledyba.png", { 37,37 }, { 6,6 }, { 27,27 });
		imgMelee.ScaleImage(1.2f, 1.2f);
		imgRange.Load(L"images\\battle\\sprite_latias.png", { 44,34 }, { 2,4 }, { 42,29 });
		imgRange.ScaleImage(1.3f, 1.3f);
		imgRangeBullet.Load(L"images\\battle\\bullet_latias.png", { 14,14 });
		imgRangeBullet.ScaleImage(0.8f, 0.8f);

		createDelay_Melee = 2000;
		createDelay_Range = 3000;
		createAmount_Melee = 5;
		createAmount_Range = 4;

		meleeData.hp = 6;
		meleeData.speed = 1.65f;
		meleeData.attackDelay = 1250;
		meleeData.damage = 2.0f;

		meleeData.frameNum_Idle = 0;
		meleeData.frameNum_IdleMax = 1;
		meleeData.frameNum_Atk = 2;
		meleeData.frameNum_AtkMax = 5;
		meleeData.frameNum_AtkRev = 5;

		rangeData.hp = 6.25f;
		rangeData.speed = 0.5f;
		rangeData.attackDelay = 1250;
		rangeData.damage = 3.0f;

		rangeData.frameNum_Idle = 0;
		rangeData.frameNum_IdleMax = 0;
		rangeData.frameNum_Atk = 1;
		rangeData.frameNum_AtkMax = 2;
		rangeData.frameNum_AtkRev = 2;
		rangeData.bulletSpeed = 3.5f;
		break;
	case StageElement::Dark:
		meleeData.type = Type::Dark;
		rangeData.type = Type::Dark;
		imgMelee.Load(L"images\\battle\\sprite_crobat.png", { 40,30 }, { 5,7 }, { 32,19 });
		imgMelee.ScaleImage(1.1f, 1.1f);
		imgRange.Load(L"images\\battle\\sprite_aerodactyl.png", { 40,40 }, { 6,9 }, { 30,27 });
		imgRange.ScaleImage(1.5f, 1.5f);
		imgRangeBullet.Load(L"images\\battle\\bullet_aerodactyl.png", { 10,10 });
		imgRangeBullet.ScaleImage(1.6f, 1.6f);

		createDelay_Melee = 2000;
		createDelay_Range = 3000;
		createAmount_Melee = 10;
		createAmount_Range = 4;

		meleeData.hp = 4.0f;
		meleeData.speed = 2.25f;
		meleeData.attackDelay = 850;
		meleeData.damage = 2.0f;

		meleeData.frameNum_Idle = 0;
		meleeData.frameNum_IdleMax = 2;
		meleeData.frameNum_Atk = 2;
		meleeData.frameNum_AtkMax = 4;
		meleeData.frameNum_AtkRev = 4;

		rangeData.hp = 8.5f;
		rangeData.speed = 0.75f;
		rangeData.attackDelay = 2000;
		rangeData.damage = 3.0f;

		rangeData.frameNum_Idle = 0;
		rangeData.frameNum_IdleMax = 2;
		rangeData.frameNum_Atk = 2;
		rangeData.frameNum_AtkMax = 4;
		rangeData.frameNum_AtkRev = 4;
		rangeData.bulletSpeed = 3.0f;
		break;
	default:
		assert(0);
		break;
	}

	if (phase.GetPhase() > 0)
	{
		++createAmount_Melee;
		++createAmount_Range;
	}

	const float randHP_Melee = (float)(rand() % 6) / 10;
	const float randHP_Range = (float)(rand() % 6) / 10;
	meleeData.hp += randHP_Melee;
	rangeData.hp += randHP_Range;

	bullets = new EnemyBullet(imgRangeBullet);
}
EnemyController::~EnemyController()
{
	for (Enemy* enemy : enemies)
	{
		delete enemy;
	}
	delete bullets;
}


void EnemyController::CreateCheckMelee()
{
	if (boss->IsCreated() == true)
	{
		return;
	}
	else if (gui->IsFieldEnd() == true)
	{
		return;
	}

	delay_Melee += ELAPSE_BATTLE_INVALIDATE;
	if (delay_Melee < createDelay_Melee)
	{
		return;
	}
	delay_Melee = 0;

	for (int i = 0; i < createAmount_Melee; ++i)
	{
		float xPos = rand() % WINDOWSIZE_X;
		float yPos = -(rand() % 100);

		Melee* enemy = new Melee(imgMelee, { xPos, yPos }, meleeData);
		enemies.emplace_back(enemy);
	}
}
void EnemyController::CreateCheckRange()
{
	if (boss->IsCreated() == true)
	{
		return;
	}
	else if (gui->IsFieldEnd() == true)
	{
		return;
	}

	delay_Range += ELAPSE_BATTLE_INVALIDATE;
	if (delay_Range < createDelay_Range)
	{
		return;
	}
	delay_Range = 0;

	for (int i = 0; i < createAmount_Range; ++i)
	{
		rangeData.maxYPos = (rand() % 100) + 50;
		const float xPos = (rand() % (WINDOWSIZE_X - 100)) + 50;
		const float yPos = -(rand() % 100);

		Range* enemy = new Range(imgRange, { xPos, yPos }, rangeData);
		enemies.emplace_back(enemy);
	}
}

void EnemyController::Paint(HDC hdc)
{
	for (Enemy* enemy : enemies)
	{
		enemy->Paint(hdc);
	}
	bullets->Paint(hdc);
}
void EnemyController::Move()
{
	for (Enemy* enemy : enemies)
	{
		enemy->Move();
	}
}
void EnemyController::Animate()
{
	for (Enemy* enemy : enemies)
	{
		enemy->Animate();
	}
}
bool EnemyController::CheckHit(const RECT& rectSrc, float damage, Type hitType, const POINT& effectPoint)
{
	for (size_t i = 0;i<enemies.size();++i)
	{
		if (enemies.at(i)->IsCollide(rectSrc) == true)
		{
			effects->CreateHitEffect(effectPoint, hitType);
			const float calDamage = CalculateDamage(damage, enemies.at(i)->GetType(), hitType);
			if (enemies.at(i)->Hit(damage) == true)
			{
				EnemyController::Pop(i);
			}
			return true;
		}
	}

	return false;
}
void EnemyController::CheckHitAll(const RECT& rectSrc, float damage, Type hitType)
{
	for (size_t i = 0; i < enemies.size(); ++i)
	{
		if (enemies.at(i)->IsCollide(rectSrc) == true)
		{
			POINT effectPoint = enemies.at(i)->GetPosCenter();
			GetRandEffectPoint(effectPoint);
			effects->CreateHitEffect(effectPoint, hitType);
			const float calDamage = CalculateDamage(damage, enemies.at(i)->GetType(), hitType);
			if (enemies.at(i)->Hit(calDamage) == true)
			{
				EnemyController::Pop(i);
			}
		}
	}
}

void EnemyController::CreateBullet(const POINT& center, const BulletData& data, const Vector2& unitVector)
{
	bullets->CreateBullet(center, data, unitVector);
}
void EnemyController::CreateBullet(const POINT& center, const BulletData& data, Dir dir)
{
	bullets->CreateBullet(center, data, dir);
}
void EnemyController::MoveBullets()
{
	bullets->Move();
}
void EnemyController::DestroyCollideBullet(const RECT& rect)
{
	bullets->DestroyCollideBullet(rect);
}