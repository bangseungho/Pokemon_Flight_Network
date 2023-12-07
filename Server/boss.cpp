#include "..\Utils.h"
#include "boss.h"
#include "player.h"
#include "bullet.h"
#include "timer.h"
#include "effect.h"
#include "skill.h"

extern unordered_map<uint8, NetworkPlayerData> sPlayerMap;

// 플레이어부터 보스로 향하는 방향 벡터를 업데이트하고 움직임을 true로 업데이트
void Boss::SetMove(const Vector2& unitVector)
{
	this->unitVector = unitVector;
	StartMove();
}

// 보스가 죽었을 경우 플레이어를 무적으로 
void Boss::Death()
{
	bossData.hp = 0;
	bossData.isDeath = true;

	//soundManager->StopEffectSound();
	//soundManager->StopBossSound();
}

// 보스 액션에 따른 공격 설정 함수
void Boss::StartAttack()
{
	if (bossData.type != Type::Dark)
	{
		SetAction(Action::Attack, bossData.frameNum_Atk);
	}

	if (++normalSkillCount < 3) // 일반 공격은 연속으로 3번 랜덤하게 5개 일반 공격 중 하나로 진행
	{
		act = static_cast<BossAct>(rand() % 5);
	}
	else // 스킬은 2개 중 하나로 진행
	{
		normalSkillCount = 0;
		act = static_cast<BossAct>((rand() % 2) + 5);
	}

	switch (act)
	{
	case BossAct::Skill1:
	case BossAct::Skill2:
		skill->UseSkill();
		return;
	case BossAct::Line: // 양 옆으로 왔다갔다 함
	{
		int random = rand() % 2;
		if (random != 0)
		{
			SetMove(Vector2::Left());
		}
		else
		{
			SetMove(Vector2::Right());
		}
	}
	break;
	}

	// 현재 공격에 따라서 최대 공격 개수를 받아온다.
	skillCount = maxSkillCount[static_cast<int>(act)];
}

// 보스 액션에 따라서 공격 함수 호출
void Boss::Shot()
{
	switch (act)
	{
	case BossAct::Line:
		ShotByLine();
		break;
	case BossAct::Sector:
		ShotBySector();
		break;
	case BossAct::Circle:
		ShotByCircle();
		break;
	case BossAct::Spiral:
		ShotBySpiral();
		break;
	case BossAct::Spread:
		ShotBySpread();
		break;
	default:
		return;
	}

	// 스킬 모두 사용시 액션을 Idle로 지정
	if (--skillCount <= 0)
	{
		skillCount = 0;
		act = BossAct::Idle;
	}
}

// 현재 액션에 따른 총알 정보 Get함수
BulletData Boss::GetBulletData()
{
	BulletData bulletData;
	const int index = static_cast<int>(act);
	if (index >= BOSS_BULLET_LIST)
	{
		assert(0);
		return bulletData;
	}

	bulletData.bulletType = bossData.type;
	bulletData.damage = bossData.damage;
	bulletData.speed = bossData.bulletSpeed[index];

	return bulletData;
}

// 공격을 한 뒤 딜레이를 위해 쿨타임을 다시 설정하는 함수
void Boss::ResetAttackDelay()
{
	const int index = static_cast<int>(act);
	if (index >= BOSS_BULLET_LIST)
	{
		return;
	}
	bossData.crntAttackDelay = bossData.attackDelay[index];
}

// 게임 스테이지에 따라서 이미지 로드
Boss::Boss()
{
	const RECT rectDisplay = { (float)(RECT_WINDOW_WIDTH / 2 - 40), (float)(RECT_WINDOW_HEIGHT / 2 - 40), (float)(RECT_WINDOW_WIDTH / 2 + 40), (float)(RECT_WINDOW_HEIGHT / 2 + 40) };

	ObjectImage imgBullet;
	image = new ObjectImage();
	
	switch (sPlayerMap.begin()->second.mStageData.Stage)
	{
	case StageElement::Elec:
		image->Load(_T("..\\Client\\images\\battle\\sprite_boss_elec.png"), { 73,68 }, { 3,7 }, { 69,50 });
		image->ScaleImage(4, 4);
		imgBullet.Load(_T("..\\Client\\images\\battle\\bullet_boss_elec.png"), { 400,400 });
		imgBullet.ScaleImage(0.05f, 0.05f);
		break;
	case StageElement::Water:
		image->Load(_T("..\\Client\\images\\battle\\sprite_boss_water.png"), { 65,41 }, { 2,3 }, { 63,36 });
		image->ScaleImage(4, 4);
		imgBullet.Load(_T("..\\Client\\images\\battle\\bullet_boss_water.png"), { 400,400 });
		imgBullet.ScaleImage(0.05f, 0.05f);
		break;
	case StageElement::Fire:
		image->Load(_T("..\\Client\\images\\battle\\sprite_boss_fire.png"), { 54,44 }, { 6,12 }, { 44,29 });
		image->ScaleImage(6, 6);
		imgBullet.Load(_T("..\\Client\\images\\battle\\bullet_boss_fire.png"), { 400,400 });
		imgBullet.ScaleImage(0.05f, 0.05f);
		break;
	case StageElement::Dark:
		image->Load(_T("..\\Client\\images\\battle\\sprite_boss_dark.png"), { 110,110 }, { 20,30 }, { 67,50 });
		image->ScaleImage(5, 5);
		imgBullet.Load(_T("..\\Client\\images\\battle\\bullet_boss_dark.png"), { 700,700 });
		imgBullet.ScaleImage(0.03f, 0.03f);
		break;
	default:
		assert(0);
		break;
	}

	// 탄막 생성 및 최대 스킬 개수 지정
	bullets = new EnemyBullet(imgBullet);
	maxSkillCount[static_cast<unsigned int>(BossAct::Line)] = INT_MAX;
	maxSkillCount[static_cast<unsigned int>(BossAct::Circle)] = 10;
	maxSkillCount[static_cast<unsigned int>(BossAct::Spiral)] = 360;
	maxSkillCount[static_cast<unsigned int>(BossAct::Sector)] = 15;
	maxSkillCount[static_cast<unsigned int>(BossAct::Spread)] = 720;
}
Boss::~Boss()
{
	if (skill != nullptr)
	{
		delete skill;
	}
	delete image;
	delete bullets;
}

// 보스 생성 및 초기화 함수
void Boss::Create()
{
	bossData = GetBossData();
	bossData.isCreated = true;
	skill = new BossSkillManager();

	Vector2 posCenter = { WINDOWSIZE_X / 2 , -300 };
	GameObject::Init(*image, posCenter);
	SetMove(Vector2::Down());

	//soundManager->StopBGMSound();
	//soundManager->PlayBGMSound(BGMSound::Battle_Boss, 1.0f, true);
}

// 보스 움직임을 설정하는 함수
void Boss::SetPosDest()
{
	if (IsMove() == false)
	{
		return;
	}

	const RECT rectDisplay = { (float)(RECT_WINDOW_WIDTH / 2 - 40), (float)(RECT_WINDOW_HEIGHT / 2 - 40), (float)(RECT_WINDOW_WIDTH / 2 + 40), (float)(RECT_WINDOW_HEIGHT / 2 + 40) };

	posDest = Vector2::GetDest(GetPosCenter(), unitVector, bossData.speed);
	if (act == BossAct::Idle) // Idle시 maxYPos 위치까지 이동 후 정지
	{
		const RECT rectBody = GetRectBody();
		const int maxYPos = (rectBody.bottom - rectBody.top) / 2;

		Vector2 unitVector = Vector2::Down();
		
		if (posDest.y > maxYPos)
		{
			StopMove();
			bossData.speed = 5;
		}
	}
	else if (act == BossAct::Line) // Line시 좌우측으로 이동하다가 어느정도 움직이면 다시 가운데에서 정지
	{
		constexpr int moveCount = 6;
		static int crntMoveCount = moveCount;
		static bool isReturn = false;

		const RECT rectBody = GetRectBody();
		const int minXPos = bullets->GetBulletSize().x * 4;
		const int maxXPos = rectDisplay.right - minXPos;
		const Vector2 posCenter = GetPosCenter();

		if (posCenter.x < minXPos)
		{
			if (unitVector == Vector2::Left() && --crntMoveCount <= 0)
			{
				isReturn = true;
			}
			SetMove(Vector2::Right());
		}
		else if (posCenter.x > maxXPos)
		{
			if (unitVector == Vector2::Right() && --crntMoveCount <= 0)
			{
				isReturn = true;
			}
			SetMove(Vector2::Left());
		}
		else if (isReturn == true)
		{
			crntMoveCount = moveCount;

			constexpr int centerX = WINDOWSIZE_X / 2;
			if ((unitVector == Vector2::Left() && posCenter.x <= centerX) ||
				(unitVector == Vector2::Right() && posCenter.x >= centerX))
			{
				isReturn = false;
				act = BossAct::Idle;
				StopMove();
			}
		}
	}
}

// 보스를 업데이트 하는 함수 충돌 처리와 이동 함수를 호출한다.
void Boss::Update()
{
	if (bossData.isCreated == false)
	{
		return;
	}

	// 탄막 업데이트
	bullets->Update();

	if (bossData.isDeath == true)
	{
		return;
	}

	const RECT rectBody = GetRectBody();
	//if (mPlayer->IsCollide(rectBody) == true)
	//{
	//	//mPlayer->Hit(bossData.damage, GetType());
	//}

	if (IsMove() == false)
	{
		return;
	}

	SetPosDest();
	SetPos(posDest);
}

// 보스 일반 공격 쿨타임을 체크하는 함수 
void Boss::CheckAttackDelay()
{
	if (bossData.isCreated == false)
	{
		return;
	}
	else if (bossData.isDeath == true)
	{
		return;
	}
	else if (act != BossAct::Idle)
	{
		// 쿨타임이 끝났다면 공격을 실행하고 다시 쿨타임을 리셋
		bossData.crntAttackDelay -= ELAPSE_BATTLE_INVALIDATE;
		if (IsClearAttackDelay() == true)
		{
			Shot();
			ResetAttackDelay();
		}
	}
}

// 보스 전체 움직임 쿨타임을 체크하는 함수 
void Boss::CheckActDelay()
{
	if (bossData.isCreated == false)
	{
		return;
	}
	else if (bossData.isDeath == true)
	{
		return;
	}
	else if (IsMove() == false && act == BossAct::Idle)
	{
		bossData.crntActDelay -= ELAPSE_BATTLE_INVALIDATE;
		if (IsClearActDelay() == true)
		{
			StartAttack();
			ResetActDelay();
		}
	}
}

// 보스 충돌 검사 함수
bool Boss::CheckHit(const RECT& rectSrc, float damage, Type hitType, POINT effectPoint)
{
	if (bossData.isCreated == false)
	{
		return false;
	}
	else if (bossData.isDeath == true)
	{
		return false;
	}

	const RECT rectBody = GameObject::GetRectBody();
	RECT rectInter = { 0, };
	if (rectSrc.top > rectBody.bottom) // 만약 탄막의 top이 보스의 bottom보다 크다면 충돌 처리 할 일이 없으니깐 리턴
	{
		return false;
	}
	else if (GameObject::IsCollide(rectSrc, &rectInter) == true) // 충돌 처리를 통해서 rectInter을 받아온다.
	{
		if (effectPoint.x == -1) // effecPoint.x 값이 -1이라면 플레이어의 스킬 공격을 맞은 경우임
		{
			const int interWidth = rectInter.right - rectInter.left;
			const int interHeight = rectInter.bottom - rectInter.top;
			const int randX = rectInter.left + (rand() % interWidth);
			const int randY = rectInter.top + (rand() % interHeight);

			effectPoint = { randX, randY };
		}
		else // -1이 아니라면 플레이어의 탄막을 맞은 경우임
		{
			const int maxYPos = rectBody.top + 30;
			if (maxYPos < rectSrc.top)
			{
				const int randHit = rand() % 12;
				if (randHit != 0)
				{
					return false;
				}
			}
		}

		// 충돌 검사를 통과했을 경우 이펙트를 생성하고 보스의 hp를 감소시킴
		//effects->CreateHitEffect(effectPoint, hitType);
		const float calDamage = CalculateDamage(damage, GetType(), hitType);
		if ((bossData.hp -= calDamage) <= 0)
		{
			Death();
		}
		return true;
	}

	return false;
}

// 7개의 탄막이 1열로 y축 방향으로 나아감
void Boss::ShotByLine()
{
	constexpr int bulletCount = 7;

	const BulletData bulletData = GetBulletData();

	const RECT rectBody = GetRectBody();
	const int bulletSizeX = bullets->GetBulletSize().x;
	POINT bulletPos = { 0, };
	bulletPos.y = rectBody.bottom;
	bulletPos.x = rectBody.left + ((rectBody.right - rectBody.left) / 2) + (((bulletCount / 2) * bulletSizeX) / 2);

	const int bulletMoveAmount = bulletSizeX / 2;
	const Vector2 unitVector = Vector2::Down();
	for (int i = 0; i < bulletCount; ++i)
	{
		bullets->CreateBullet(bulletPos, bulletData, unitVector); // createBullet을 실행하면 unitVector에 따라서 Update함
		bulletPos.x -= bulletMoveAmount;
	}
}

// 중심에서 36개의 탄막이 원의 형태로 나아감
void Boss::ShotByCircle()
{
	constexpr int bulletCount = 36;

	const BulletData bulletData = GetBulletData();

	const POINT bulletPos = GetPosCenter();

	Vector2 unitVector = Vector2::Up();
	unitVector = Rotate(unitVector, skillCount * 6); // Make different degree each fire
	for (int i = 0; i < bulletCount; ++i)
	{
		bullets->CreateBullet(bulletPos, bulletData, unitVector);
		unitVector = Rotate(unitVector, 360 / bulletCount);
	}
}

// 중심에서 1개의 탄막이 원을 그리며 나아감(동글뱅이)
void Boss::ShotBySpiral()
{
	const BulletData bulletData = GetBulletData();

	const RECT rectBody = GetRectBody();
	POINT bulletPos = { 0, };
	bulletPos.y = rectBody.bottom;
	bulletPos.x = rectBody.left + ((rectBody.right - rectBody.left) / 2);

	static int rotation = 8;
	static Vector2 unitVector = Vector2::Down();
	if (skillCount == 360)
	{
		unitVector = Vector2::Down();
	}
	if (abs(skillCount % (180 / rotation)) == 0)
	{
		rotation = ((rand() % 3) + 4) * 2;
	}
	unitVector = Rotate(unitVector, rotation);
	bullets->CreateBullet(bulletPos, bulletData, unitVector);
}

// 13개의 탄막이 반원 형태로 중심에서 나아감
void Boss::ShotBySector()
{
	constexpr int bulletCount = 12;

	const BulletData bulletData = GetBulletData();

	const RECT rectBody = GetRectBody();
	POINT bulletPos = { 0, };
	bulletPos.y = rectBody.bottom;
	bulletPos.x = rectBody.left + ((rectBody.right - rectBody.left) / 2);

	Vector2 unitVector = Vector2::Down();
	const float startDegree = 50 + (((skillCount % 4) * 16));
	const int rotationDegree = -(startDegree * 2) / bulletCount;
	unitVector = Rotate(unitVector, startDegree);
	for (int i = 0; i < bulletCount + 1; ++i)
	{
		bullets->CreateBullet(bulletPos, bulletData, unitVector);
		unitVector = Rotate(unitVector, rotationDegree);
	}
}

// 1개의 탄막이 랜덤한 방향으로 나아감
void Boss::ShotBySpread()
{
	const BulletData bulletData = GetBulletData();

	const RECT rectBody = GetRectBody();
	POINT bulletPos = { 0, };
	bulletPos.y = rectBody.bottom;
	bulletPos.x = rectBody.left + ((rectBody.right - rectBody.left) / 2);

	const int rotation = 30 + (rand() % 300);

	Vector2 unitVector = Rotate(Vector2::Up(), rotation);
	bullets->CreateBullet(bulletPos, bulletData, unitVector);
}

BossData Boss::GetBossData()
{
	BossData bossData;

	bossData.speed = 1;

	// 탄막(일반 공격)의 속도와 쿨타임을 설정
	bossData.bulletSpeed[static_cast<int>(BossAct::Line)] = 6;
	bossData.bulletSpeed[static_cast<int>(BossAct::Sector)] = 3;
	bossData.bulletSpeed[static_cast<int>(BossAct::Circle)] = 4;
	bossData.bulletSpeed[static_cast<int>(BossAct::Spiral)] = 5;
	bossData.bulletSpeed[static_cast<int>(BossAct::Spread)] = 6;

	bossData.attackDelay[static_cast<int>(BossAct::Line)] = 40;
	bossData.attackDelay[static_cast<int>(BossAct::Sector)] = 250;
	bossData.attackDelay[static_cast<int>(BossAct::Circle)] = 200;
	bossData.attackDelay[static_cast<int>(BossAct::Spiral)] = 10;
	bossData.attackDelay[static_cast<int>(BossAct::Spread)] = 10;

	bossData.frameNum_Idle = 0;

	// 보스의 타입에 따라서 초기화
	switch (sPlayerMap.begin()->second.mStageData.Stage)
	{
	case StageElement::Elec:
		bossData.type = Type::Elec;

		bossData.hp = 6000;
		bossData.damage = 3.0f;
		bossData.damage_skill1 = 15.5f;
		bossData.damage_skill2 = 2.5f;
		bossData.actDelay = 1250;

		bossData.frameNum_IdleMax = 2;
		bossData.frameNum_Atk = 1;
		bossData.frameNum_AtkMax = 5;
		bossData.frameNum_AtkRev = 5;
		break;
	case StageElement::Water:
		bossData.type = Type::Water;

		bossData.hp = 4000;
		bossData.damage = 2.0f;
		bossData.damage_skill1 = 4.5f;
		bossData.damage_skill2 = 2.5f;
		bossData.actDelay = 1750;

		bossData.frameNum_IdleMax = 2;
		bossData.frameNum_Atk = 2;
		bossData.frameNum_AtkMax = 3;
		bossData.frameNum_AtkRev = 3;
		break;
	case StageElement::Fire:
		bossData.type = Type::Fire;

		bossData.hp = 5000;
		bossData.damage = 2.5f;
		bossData.damage_skill1 = 7.5f;
		bossData.damage_skill2 = 15.0f;
		bossData.actDelay = 1500;

		bossData.frameNum_IdleMax = 1;
		bossData.frameNum_Atk = 1;
		bossData.frameNum_AtkMax = 6;
		bossData.frameNum_AtkRev = 6;
		break;
	case StageElement::Dark:
		bossData.type = Type::Dark;

		bossData.hp = 7000;
		bossData.damage = 5.0f;
		bossData.damage_skill1 = 6.5f;
		bossData.damage_skill2 = 7.5f;
		bossData.actDelay = 1150;

		bossData.frameNum_IdleMax = 14;
		bossData.frameNum_Atk = 0;
		bossData.frameNum_AtkMax = 0;
		bossData.frameNum_AtkRev = 0;
		break;
	}
	bossData.crntActDelay = bossData.actDelay;

	// 3페이즈(또도가스)인 경우 hp을 더 높게 설정
	//if (phase.GetPhase() == 3)
	//{
	//	bossData.hp += 1000;
	//}

	return bossData;
}