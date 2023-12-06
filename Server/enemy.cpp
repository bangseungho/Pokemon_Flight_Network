#include "..\Utils.h"
#include <assert.h>
#include "enemy.h"

//#include "player.h"
//#include "bullet.h"
//#include "effect.h"
//#include "boss.h"

//extern GameData gameData;
//extern Player* mPlayer;
//extern Boss* boss;
//extern EffectManager* effects;
//extern EnemyController* enemies;
//extern SoundManager* soundManager;
//extern GUIManager* gui;
//extern PhaseManager phase;
extern unordered_map<uint8, NetworkPlayerData> sPlayerMap;

uint32 Enemy::sId = 0;

// 적 생성자 타입을 받아서 업데이트한다.
Enemy::Enemy(const Vector2& pos, const EnemyData& data) : GameObject(pos, data.bodySize)
{
	id = sId++;
	StartMove();
	this->data = data;
}

// 플레이어의 방향 벡터를 기준으로 적의 현재 방향을 구함
Dir Enemy::GetDir() const
{
	const float theta = atan2(unitVector.y, unitVector.x); // 플레이어를 향하는 방향 벡터의 라디안 값을 구한다.
	const float crntDegree = RADIAN_TO_DEGREE(theta); // 현재 라디안 값을 각도로 변환

	constexpr int unitDegree = 45;
	float degree = (float)unitDegree / 2;
	if (crntDegree > 0)
	{
		if (crntDegree < degree) // 0 ~ 22.5
		{
			return Dir::Left;
		}
		else if (crntDegree < (degree += unitDegree)) // 22.5 ~ 67.5
		{
			return Dir::LU;
		}
		else if (crntDegree < (degree += unitDegree)) // 67.5 ~ 112.5
		{
			return Dir::Up;
		}
		else if (crntDegree < (degree += unitDegree)) // 112.5 ~ 157.5
		{
			return Dir::RU;
		}
		else // 157.5 ~ 180
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

// 플레이어 방향 벡터를 구하고 근거리 적 몬스터의 위치를 플레이어쪽으로 이동하도록 한다.
void Melee::SetPosDest()
{
	if (IsMove() == false)
	{
		return;
	}

	const Vector2 posCenter = GetPosCenter();
	const Vector2 vectorToPlayer = posCenter - Vector2{ sPlayerMap[0].mBattleData.PosX, sPlayerMap[0].mBattleData.PosY };

	const float radius = GetRadius(vectorToPlayer.x, vectorToPlayer.y);

	unitVector = vectorToPlayer / radius;

	posDest = posCenter - (unitVector * data.speed);
}

// 원거리 적 몬스터의 이동으로 랜덤한 y축을 기준으로 아래로 내려가다 멈춘다.
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

// 최종적으로 위치를 이동
void Enemy::Update()
{
	if (IsMove() == false)
	{
		return;
	}

	SetPosDest();
	SetPos(posDest);

	// 클라이언트로 데이터 전송
}

Melee::Melee(const Vector2& pos, const EnemyData& data) : Enemy(pos, data)
{
	mSendData.AttackType = NetworkEnemyData::AttackType::MELEE;
	mSendData.Status = NetworkEnemyData::Status::CREATE;
	mSendData.ID = id;
	mSendData.Pos = GetPosCenter();
	mSendData.SpriteRow = GetSpriteRow();
}

// 최종적으로 근거리 적 몬스터 이동과 충돌 체크
void Melee::Update()
{
	if (IsMove() == false)
	{
		return;
	}
	//else if (CheckCollidePlayer() == true)
	//{
	//	//mPlayer->Hit(data.damage, GetType());
	//	//effects->CreateHitEffect(mPlayer->GetPosCenter(), GetType());
	//	return;
	//}

	SetPosDest();
	SetPos(posDest);

	mSendData.Status = NetworkEnemyData::Status::MOVE;
	mSendData.Pos = GetPosCenter();
	mSendData.SpriteRow = GetSpriteRow();
	for (const auto& player : sPlayerMap) {
		Data::SendDataAndType<NetworkEnemyData>(player.second.mSock, mSendData);
	}
}

Range::Range(const Vector2& pos, const EnemyData& data) : Enemy(pos, data)
{
	mSendData.AttackType = NetworkEnemyData::AttackType::RANGE;
	mSendData.Status = NetworkEnemyData::Status::CREATE;
	mSendData.ID = id;
	mSendData.Pos = GetPosCenter();
	mSendData.SpriteRow = GetSpriteRow();
}

// 최종적으로 원거리 적 몬스터 이동, 충돌 체크는 원거리 적 몬스터의 총알하고만 한다.
void Range::Update()
{
	if (IsMove() == false)
	{
		return;
	}

	SetPosDest();
	SetPos(posDest);

	mSendData.Status = NetworkEnemyData::Status::MOVE;
	mSendData.Pos = GetPosCenter();
	mSendData.SpriteRow = GetSpriteRow();
	for (const auto& player : sPlayerMap) {
		Data::SendDataAndType<NetworkEnemyData>(player.second.mSock, mSendData);
	}
}

// 적의 방향에따라서 스프라이트 이미지 인덱스를 구한다.
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

//// 근거리 적과 플레이어가 충돌했다면 잠깐 멈추고 공격 액션으로 바꾼다.
//bool Melee::CheckCollidePlayer()
//{
//	const RECT rectBody = GetRectBody();
//	if (mPlayer->IsCollide(rectBody) == true)
//	{
//		StopMove();
//		SetAction(Action::Attack, data.frameNum_Atk);
//		ResetAttackDelay();
//
//		return true;
//	}
//
//	return false;
//}
//
//// 적이 피격될 경우 데미지를 입힌다. 만약 피가 0이하일 경우 죽었음을 나타낸다.
//bool Enemy::Hit(float damage)
//{
//	if ((data.hp -= damage) <= 0)
//	{
//		return true;
//	}
//
//	return false;
//}
//
//// 적이 공격을 하고 나서 딜레이를 주는 함수
//void Melee::CheckAttackDelay()
//{
//	if (IsMove() == false)
//	{
//		data.crntAttackDelay -= ELAPSE_BATTLE_INVALIDATE;
//		if (IsClearAttackDelay() == true) // 공격 딜레이가 끝났다면 움직이기 시작
//		{
//			StartMove();
//		}
//	}
//}
//void Range::CheckAttackDelay()
//{
//	if (IsMove() == false)	
//	{
//		data.crntAttackDelay -= ELAPSE_BATTLE_INVALIDATE;
//		if (IsClearAttackDelay() == true)
//		{
//			Fire();
//			ResetAttackDelay(); // 근거리 적은 딜레이가 끝났다면 발사 시작
//		}
//	}
//}
//
//// 원거리 적 스킬 발사 함수
//void Range::Fire()
//{
//	SetAction(Action::Attack, data.frameNum_Atk);
//
//	RECT rectBody = GetRectBody();
//	POINT bulletPos = { 0, };
//	bulletPos.x = rectBody.left + ((rectBody.right - rectBody.left) / 2);
//	bulletPos.y = rectBody.bottom;
//
//	BulletData bulletData;
//	bulletData.bulletType = GetType();
//	bulletData.damage = data.damage;
//	bulletData.speed = data.bulletSpeed;
//
//	Vector2 unitVector = Vector2::Down();
//	int randDegree = (rand() % 10) - 5;
//
//	// 3 방향으로 탄막 발사
//	unitVector = Rotate(unitVector, randDegree);
//	enemies->CreateBullet(bulletPos, bulletData, unitVector);
//	unitVector = Rotate(unitVector, 20);
//	enemies->CreateBullet(bulletPos, bulletData, unitVector);
//	unitVector = Rotate(unitVector, -40);
//	enemies->CreateBullet(bulletPos, bulletData, unitVector);
//}
//
//// 적이 죽었을 경우 효과 사운드를 재생하고 적 객체 삭제
//void EnemyController::Pop(size_t& index)
//{
//	effects->CreateExplosionEffect(enemies.at(index)->GetPosCenter(), enemies.at(index)->GetType());
//	soundManager->PlayEffectSound(EffectSound::Explosion);
//	enemies[index--] = enemies.back();
//	enemies.pop_back();
//}

// 적 객체들을 관리하는 클래스로 스테이지 상태에 따라서 적 오브젝트 초기화
EnemyController::EnemyController()
{
	switch (sPlayerMap[0].mStageData.Stage)
	{
	case StageElement::Elec:
		meleeData.type = Type::Elec;
		meleeData.bodySize = { 33, 33 };
		rangeData.type = Type::Elec;
		rangeData.bodySize = { 58, 58 };
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
		meleeData.bodySize = { 34, 33 };
		rangeData.type = Type::Water;
		rangeData.bodySize = { 29, 31 };

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
		meleeData.bodySize = { 37, 37 };
		rangeData.type = Type::Fire;
		rangeData.bodySize = { 44, 34 };

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
		meleeData.bodySize = { 40, 30 };
		rangeData.type = Type::Dark;
		rangeData.bodySize = { 40, 40 };

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

	////if (phase.GetPhase() > 0) 
	////{
	////	++createAmount_Melee; // 배틀 타이머당 생성되는 근거리 적 몬스터의 개수
	////	++createAmount_Range; // 배틀 타이머당 생성되는 원거리 적 몬스터의 개수
	////}

	const float randHP_Melee = (float)(rand() % 6) / 10;
	const float randHP_Range = (float)(rand() % 6) / 10;
	meleeData.hp += randHP_Melee;
	rangeData.hp += randHP_Range;

	////bullets = new EnemyBullet();
}
EnemyController::~EnemyController()
{
	for (Enemy* enemy : enemies)
	{
		delete enemy;
	}
	//delete bullets;
}

// 배틀 타이머당 생성되는 근거리 적 생성 함수
void EnemyController::CreateCheckMelee()
{
	//// 보스가 나왔거나 게임이 끝났다면 적 생성을 그만한다.
	//if (boss->IsCreated() == true)
	//{
	//	return;
	//}
	// 
	bool isFieldEnd = std::any_of(sPlayerMap.begin(), sPlayerMap.end(), [](const auto& a) {
		return a.second.mBattleData.IsFieldEnd == true;
		});
	if (isFieldEnd == true)
	{
		return;
	}

	// 현재 적을 생성하고 난 다음 지난 시간이 적 생성 시간을 넘겼을 경우에만 새로운 적을 생성한다.
	delay_Melee += ELAPSE_BATTLE_INVALIDATE;
	if (delay_Melee < createDelay_Melee)
	{
		return;
	}
	delay_Melee = 0;

	// 최대 생성 적 개수에 따라서 적 객체를 생성하여 객체 자료구조에 넣는다.
	for (int i = 0; i < createAmount_Melee; ++i)
	{
		float xPos = rand() % WINDOWSIZE_X;
		float yPos = -(rand() % 100);

		Melee* enemy = new Melee({ xPos, yPos }, meleeData);
		enemies.emplace_back(enemy);

		for (const auto& player : sPlayerMap) {
			Data::SendDataAndType<NetworkEnemyData>(player.second.mSock, enemy->GetSendData());
		}
	}

#ifdef _DEBUG 
	ShowEnemyCount(); // 적 객체 개수 확인
#endif
}

// 배틀 타이머당 생성되는 원거리 적 생성 함수
void EnemyController::CreateCheckRange()
{
	//if (boss->IsCreated() == true)
	//{
	//	return;
	//}
	bool isFieldEnd = std::any_of(sPlayerMap.begin(), sPlayerMap.end(), [](const auto& a) {
		return a.second.mBattleData.IsFieldEnd == true;
		});
	if (isFieldEnd == true)
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

		Range* enemy = new Range({ xPos, yPos }, rangeData);
		enemies.emplace_back(enemy);

		for (const auto& player : sPlayerMap) {
			Data::SendDataAndType<NetworkEnemyData>(player.second.mSock, enemy->GetSendData());
		}
	}

#ifdef _DEBUG 
	ShowEnemyCount(); // 적 객체 개수 확인
#endif
}

void EnemyController::Update()
{
	for (Enemy* enemy : enemies)
	{
		enemy->Update();
	}
}

void EnemyController::ShowEnemyCount() const
{
	if (!enemies.empty())
		cout << "EnemyCount: " << enemies.size() << endl;
}

//// 플레이어 탄막과 적의 충돌 함수이다. 이펙트 위치를 탄막의 위치로 지정하여 죽었을 경우 자료구조에서 제거한다.
//bool EnemyController::CheckHit(const RECT& rectSrc, float damage, Type hitType, const POINT& effectPoint)
//{
//	for (size_t i = 0;i<enemies.size();++i)
//	{
//		if (enemies.at(i)->IsCollide(rectSrc) == true)
//		{
//			effects->CreateHitEffect(effectPoint, hitType);
//			const float calDamage = CalculateDamage(damage, enemies.at(i)->GetType(), hitType);
//			if (enemies.at(i)->Hit(damage) == true)
//			{
//				EnemyController::Pop(i);
//			}
//			return true;
//		}
//	}
//
//	return false;
//}
//
//// 플레이어 스킬과 적의 충돌 함수이다. 이펙트 위치를 랜덤으로 지정하여 죽었을 경우 자료구조에서 제거한다.
//void EnemyController::CheckHitAll(const RECT& rectSrc, float damage, Type hitType)
//{
//	for (size_t i = 0; i < enemies.size(); ++i)
//	{
//		if (enemies.at(i)->IsCollide(rectSrc) == true)
//		{
//			POINT effectPoint = enemies.at(i)->GetPosCenter();
//			GetRandEffectPoint(effectPoint);
//			effects->CreateHitEffect(effectPoint, hitType);
//			const float calDamage = CalculateDamage(damage, enemies.at(i)->GetType(), hitType);
//			if (enemies.at(i)->Hit(calDamage) == true)
//			{
//				EnemyController::Pop(i);
//			}
//		}
//	}
//}
//
//// 적 탄막 생성함수 및 이동 함수 및 삭제 함수
//void EnemyController::CreateBullet(const POINT& center, const BulletData& data, const Vector2& unitVector)
//{
//	bullets->CreateBullet(center, data, unitVector);
//}
//void EnemyController::CreateBullet(const POINT& center, const BulletData& data, Dir dir)
//{
//	bullets->CreateBullet(center, data, dir);
//}
//void EnemyController::MoveBullets()
//{
//	bullets->Update();
//}
//void EnemyController::DestroyCollideBullet(const RECT& rect)
//{
//	bullets->DestroyCollideBullet(rect);
//}