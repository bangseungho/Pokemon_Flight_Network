#include "stdafx.h"
#include "skill.h"
#include "player.h"
#include "enemy.h"
#include "effect.h"
#include "bullet.h"
#include "boss.h"
#include "interface.h"
#include "scene.h"
#include "sound.h"

#include "battle.h"

extern GameData gameData;
extern Player* mPlayer;
extern Boss* boss;
extern EnemyController* enemies;
extern EffectManager* effects;
extern SceneManager* sceneManager;
extern SoundManager* soundManager;
extern Battle battle;

// void BossSkillManager::Animate() 각 스킬 알고리즘보다 그냥 이 부분만 보면 됨
// 플레이어의 스킬은 Q뿐이이고 W와 E는 탄막이기 때문에 스킬 매니저에서 관리하지 않고 불릿 매니저에서 관리한다.
// 스킬 초기화 및 렌더링 함수

SkillManager::Effect::Effect(const EffectImage& imgSkill, Type type)
{
	this->imgSkill = &imgSkill;
	this->type = type;
}
void SkillManager::Effect::Paint(const HDC& hdc, const RECT& rectBody) const
{
	RECT rectImage;
	if (type == Type::Water)
	{
		rectImage = ISprite::GetRectImage(*imgSkill, 0);
	}
	else
	{
		rectImage = ISprite::GetRectImage(*imgSkill, frame);
	}
	imgSkill->Paint(hdc, rectBody, &rectImage);
}

// 스킬 애니메이션 함수
bool SkillManager::Effect::Animate()
{
	static int frameLoopCount = 4;

	// 프레임이 최대 프레임을 벗어난 경우 0으로 초기화 후 애니메이션 종료
	if (++frame >= imgSkill->GetMaxFrame())
	{
		frame = 0;
		frameLoopCount = 4;
		return false;
	}

	switch (type)
	{
	case Type::Elec:
	{
		if (frame >= 8)
		{
			if (frameLoopCount-- > 0)
			{
				frame = 4;
				return true;
			}
		}
	}
	break;
	default:
		break;
	}

	return true;
}





SkillManager::SkillManager()
{
	const Type type = mPlayer->GetType();
	switch (type)
	{
	case Type::Elec:
		imgSkill_Elec_Q.Load(_T("images\\battle\\sprite_skill_elec.png"), { 34,226 }, 10, 0xdf);
		skillEffect = new Effect(imgSkill_Elec_Q, type);
		break;
	case Type::Fire:
		imgSkill_Fire_Q.Load(_T("images\\battle\\sprite_skill_fire.png"), { 80,96 }, 50);
		skillEffect = new Effect(imgSkill_Fire_Q, type);
		break;
	case Type::Water:
		imgSkill_Water_Q.Load(_T("images\\battle\\skill_water.png"), { 273,843 }, 50, 0xaf);
		skillEffect = new Effect(imgSkill_Water_Q, type);
		break;
	default:
		assert(0);
		break;
	}
}

// 플레이어의 타입에 따라서 스킬의 범위 지정
RECT SkillManager::GetRectBody() const
{
	RECT rectBody = { 0, };
	switch (mPlayer->GetType())
	{
	case Type::Elec:
		rectBody = mPlayer->GetRectBody();
		rectBody.left -= 20;
		rectBody.right += 20;
		rectBody.bottom = rectBody.top;
		rectBody.top = rectBody.bottom - WINDOWSIZE_Y;
		break;
	case Type::Fire:
		rectBody = mPlayer->GetRectBody();
		rectBody.left -= 40;
		rectBody.right += 30;
		rectBody.bottom = rectBody.top + 10;
		rectBody.top = rectBody.bottom - 400;
		break;
	case Type::Water:
	{
		const int maxFrame = imgSkill_Water_Q.GetMaxFrame();
		const int frame = skillEffect->GetFrame();
		rectBody.left = 0;
		rectBody.right = WINDOWSIZE_X;
		rectBody.top = WINDOWSIZE_Y - (((float)frame / maxFrame) * WINDOWSIZE_Y * 2);
		rectBody.bottom = rectBody.top + WINDOWSIZE_Y;
	}
	break;
	default:
		assert(0);
		break;
	}

	return rectBody;
}

// W, E 일반 스킬 사용 함수
void SkillManager::UseSkill()
{
	if (IsUsingSkill() == false)
	{
		return;
	}

	switch (crntSkill)
	{
	case Skill::Sector:
		ShotBySector();
		break;
	case Skill::Circle:
		ShotByCircle();
		break;
	default:
		assert(0);
		break;
	}

	// 스킬이 모두 종료되었다면 현재 상태를 Empty로 변경
	if (--skillCount <= 0)
	{
		skillCount = 0;
		crntSkill = Skill::Empty;
	}
}

// 궁극기 사용시 스킬 렌더링
void SkillManager::Paint(const HDC& hdc) const
{
	if (isIdentity == false) 
	{
		return;
	}

	const RECT rectBody = SkillManager::GetRectBody();
	skillEffect->Paint(hdc, rectBody);
}

// 스킬 애니메이션 진행
void SkillManager::Animate()
{
	if (isIdentity == false)
	{
		return;
	}
	else if (skillEffect->Animate() == false)
	{
		isIdentity = false;
		soundManager->StopSkillSound();
		return;
	}

	RECT rectBody = GetRectBody();
	rectBody.top += 20;
	const Type playerType = mPlayer->GetType();
	if (playerType == Type::Fire) // 플레이어 타입이 fire인 경우 프레임 업데이트
	{
		if (skillEffect->GetFrame() < 17)
		{
			rectBody.left += (rectBody.right - rectBody.left) / 2;
			rectBody.top += 100;
		}
		else if (skillEffect->GetFrame() > 27)
		{
			rectBody.right -= (rectBody.right - rectBody.left) / 2;
		}
	}

	const float damage = mPlayer->GetDamage_Q();
	for (int i = 0; i < 5; ++i) // 스킬은 보스를 5번씩 히트한다.
	{
		if (boss->CheckHit(rectBody, damage, playerType) == false)
		{
			break;
		}
	}
	enemies->CheckHitAll(rectBody, damage, playerType); // 스킬은 보스를 5번씩 히트하고 적 센터 기준 양옆으로 어느정도 랜덤한 위치에서 이펙트 실행
	if (playerType == Type::Fire || // Fire나 Elec은 적의 총알을 제거가능하다.
		playerType == Type::Elec)
	{
		enemies->DestroyCollideBullet(rectBody);
	}
}

// 스킬 사용 함수 플래그가 켜져 있다면 현재 스킬 상태를 업데이트
void SkillManager::ActiveSkill(Skill skill)
{
	if (skill == Skill::Identity && isIdentity == true) // 현재 궁극기 사용중이면 리턴(연속으로 사용 못하도록)
	{
		return;
	}
	else if (skill != Skill::Identity && IsUsingSkill() == true) // 현재 W, E 스킬 중 하나를 사용중이면 리턴(연속으로 사용 못하도록)
	{
		return;
	}

	// 스킬에 따라서 상태 업데이트
	// MP가 적다면 바로 리턴
	switch (skill)
	{
	case Skill::Sector:
		if (mPlayer->ReduceMP(15) == false)
		{
			return;
		}
		skillCount = 7;
		crntSkill = skill;
		break;
	case Skill::Circle:
		if (mPlayer->ReduceMP(10) == false)
		{
			return;
		}
		skillCount = 10;
		crntSkill = skill;
		break;
	case Skill::Identity:
		if (mPlayer->ReduceMP(30) == false)
		{
			return;
		}
		// 궁극기 사용시 맵을 흔든다.
		switch (mPlayer->GetType())
		{
		case Type::Elec:
			battle.ShakeMap(10);
			soundManager->PlaySkillSound(SkillSound::Elec);
			break;
		case Type::Fire:
			battle.ShakeMap(15);
			soundManager->PlaySkillSound(SkillSound::Fire);
			break;
		case Type::Water:
			battle.ShakeMap(20);
			soundManager->PlaySkillSound(SkillSound::Water);
			break;
		default:
			assert(0);
			break;
		}

		isIdentity = true; // 현재 궁극기 사용중임을 나타내는 플래그
		break;
	default:
		assert(0);
		break;
	}
}

// 반원 형태로 탄막을 발사함
void SkillManager::ShotBySector()
{
	constexpr int bulletCount = 12;

	BulletData bulletData;
	bulletData.bulletType = mPlayer->GetSubType();
	bulletData.damage = mPlayer->GetDamage_WE();
	bulletData.speed = 10;

	const RECT rectBody = mPlayer->GetRectBody();
	POINT bulletPos = { 0, };
	bulletPos.y = rectBody.top;
	bulletPos.x = rectBody.left + ((rectBody.right - rectBody.left) / 2);

	Vector2 unitVector = Vector2::Up();
	const float startDegree = 10 + (skillCount * 10);
	const float rotationDegree = -(startDegree * 2) / bulletCount;
	unitVector = Rotate(unitVector, startDegree);

	for (int i = 0; i < bulletCount + 1; ++i)
	{
		mPlayer->CreateSubBullet(bulletPos, bulletData, unitVector, true, true);
		unitVector = Rotate(unitVector, rotationDegree);
	}

	soundManager->PlayEffectSound(EffectSound::Shot_nLoop);
}

// 원 형태로 탄막을 발사함
void SkillManager::ShotByCircle()
{
	constexpr int bulletCount = 18;

	BulletData bulletData;
	bulletData.bulletType = mPlayer->GetSubType();
	bulletData.damage = mPlayer->GetDamage_WE();
	bulletData.speed = 10;

	const POINT bulletPos = mPlayer->GetPosCenter();

	Vector2 unitVector = Vector2::Up();
	constexpr int degree = 6;
	unitVector = Rotate(unitVector, degree * skillCount); // Make different degree each fire

	for (int i = 0; i < bulletCount; ++i)
	{
		mPlayer->CreateSubBullet(bulletPos, bulletData, unitVector, true, true);
		unitVector = Rotate(unitVector, 360 / bulletCount);
	}

	soundManager->PlayEffectSound(EffectSound::Shot_nLoop);
}

// 보스 스킬 초기화 
RECT BossSkillManager::Effect::GetRectBody() const
{
	const POINT size = imgSkill.GetDrawSize();
	RECT rectBody = { 0, };
	rectBody.left = posCenter.x - ((float)size.x / 2);
	rectBody.top = posCenter.y - ((float)size.y / 2);
	rectBody.right = rectBody.left + size.x;
	rectBody.bottom = rectBody.top + size.y;

	return rectBody;
}
BossSkillManager::Effect::Effect(const EffectImage& imgSkill, const FRECT rectDraw, const SkillData& skillData)
{
	this->imgSkill = imgSkill;
	this->rectDraw = rectDraw;
	this->skillData = skillData;
}
BossSkillManager::Effect::Effect(const EffectImage& imgSkill, const Vector2& pos, const SkillData& skillData)
{
	this->imgSkill = imgSkill;
	this->posCenter = pos;
	this->skillData = skillData;
}
BossSkillManager::Effect::Effect(const EffectImage& imgSkill, const Vector2& pos, float rotationDegree, const SkillData& skillData) : Effect(imgSkill, pos, skillData)
{
	if (rotationDegree != 0)
	{
		this->skillData.isRotated = true;
		unitVector_imgRotation = ::Rotate(Vector2::Down(), rotationDegree);
	}
}
BossSkillManager::Effect::Effect(const EffectImage& imgSkill, const Vector2& pos, const Vector2& unitVector_imgRotation, const SkillData& skillData) : Effect(imgSkill, pos, skillData)
{
	if (unitVector_imgRotation != Vector2::Down())
	{
		this->skillData.isRotated = true;
	}
	else
	{
		this->skillData.isRotated = false;
	}

	this->unitVector_imgRotation = unitVector_imgRotation;
}
BossSkillManager::Effect::Effect(const EffectImage& imgSkill, const Vector2& pos, const Vector2& unitVector_imgRotation, const Vector2& unitVector_direction, const SkillData& skillData) : Effect(imgSkill, pos, unitVector_imgRotation, skillData)
{
	this->unitVector_direction = unitVector_direction;
}
BossSkillManager::Effect::Effect(const EffectImage& imgSkill, const SkillData& skillData, const DarkSkillData& darkSkillData)
{
	this->imgSkill = imgSkill;
	this->skillData = skillData;
	this->darkSkillData = darkSkillData;

	posCenter = darkSkillData.GetPosCenter();
}


void BossSkillManager::Effect::Paint(HDC hdc) const
{
	const RECT rectBody = GetRectBody();
	RECT rectImage = { 0, };
	if (skillData.damage == 0)
	{
		rectImage = imgSkill.GetRectImage();
	}
	else
	{
		rectImage = ISprite::GetRectImage(imgSkill, frame);

	}

	if (rectDraw.left != -1)
	{
		imgSkill.Paint(hdc, rectDraw, &rectImage);
	}
	else if (skillData.isRotated == false)
	{
		imgSkill.Paint(hdc, rectBody, &rectImage);
	}
	else
	{
		Vector2 vPoints[4];
		if (unitVector_direction == Vector2::Zero())
		{
			GetRotationPos(rectBody, unitVector_imgRotation, Vector2::Up(), vPoints);
		}
		else
		{
			GetRotationPos(rectBody, unitVector_direction, Vector2::Up(), vPoints);
		}
		imgSkill.PaintRotation(hdc, vPoints, &rectImage);
		
		if (gameData.isShowHitbox == true)
		{
			MoveToEx(hdc, posCenter.x, posCenter.y, NULL);
			LineTo(hdc, posCenter.x + (unitVector_direction.x*100), posCenter.y + (unitVector_direction.y * 100));
		}
	}
}
bool BossSkillManager::Effect::Animate()
{
	const int maxFrame = imgSkill.GetMaxFrame();
	if (maxFrame > 0 && ++frame >= maxFrame)
	{
		return false;
	}
	else if (skillData.damage > 0 && skillData.isActiveDamage == true)
	{
		RECT rectBody = GetRectBody();

		if (skillData.isRotated == true)
		{
			Vector2 vPoints[4];
			GetRotationPos(rectBody, unitVector_imgRotation, Vector2::Up(), vPoints);
			if (SATIntersect(mPlayer->GetRectBody(), vPoints) == true)
			{
				mPlayer->Hit(skillData.damage, boss->GetType());
				if (skillData.isHitOnce == true)
				{
					return false;
				}
			}
		}
		else
		{
			if (rectDraw.left != -1)
			{
				rectBody = rectDraw;
			}

			const RECT rectPlayer = mPlayer->GetRectBody();
			if (IntersectRect2(rectBody, rectPlayer) == true)
			{
				mPlayer->Hit(skillData.damage, boss->GetType());
				if (skillData.isHitOnce == true)
				{
					return false;
				}
			}
		}
		
	}

	return Move();
}
bool BossSkillManager::Effect::Move()
{
	if (unitVector_direction == Vector2::Zero())
	{
		if (darkSkillData.posOrigin != nullptr)
		{
			constexpr float rotationDegree = 3.0f;
			if (*darkSkillData.posOrigin == Vector2::Zero())
			{
				return false;
			}
			posCenter = darkSkillData.GetRotatePos(rotationDegree);
		}
		
		return true;
	}

	posCenter = posCenter + (unitVector_direction * skillData.speed);
	const RECT rectDisplay = sceneManager->GetRectDisplay();
	const RECT rectBody = GetRectBody();
		
	if (boss->GetType() == Type::Dark)
	{
		if (skillData.damage > 0)
		{
			if (OutOfRange(rectBody, rectDisplay) == true)
			{
				return false;
			}
		}
	}
	else if (rectBody.top > rectDisplay.bottom)
	{
		return false;
	}

	return true;
}
bool BossSkillManager::Effect::RotateToPlayer(float t)
{
	constexpr int maxRotation = 35;
	constexpr int maxMove = 80;
	skillData.isRotated = true;

	if (skillData.rotationCount++ < maxRotation)
	{
		::Rotate(posCenter, mPlayer->GetPosCenter(), unitVector_direction, t);
	}
	else if (skillData.rotationCount > maxMove)
	{
		return false;
	}

	return true;
}

// 이 부분은 굳이 보지 않아도 되는 부분 스킬 알고리즘임
BossSkillManager::BossSkillManager()
{
	switch (boss->GetType())
	{
	case Type::Elec:
		imgSkill1.Load(_T("images\\battle\\sprite_boss_skill1_elec.png"), { 32,224 }, 9);
		imgSkill1.ScaleImage(1.0f, 6.0f);
		imgSkill1_Warning.Load(_T("images\\battle\\boss_skill1_elec_warning.png"), { 31,223 }, 16, 0x10);
		imgSkill1_Warning.ScaleImage(1.0f, 6.0f);
		imgSkill2.Load(_T("images\\battle\\sprite_boss_skill2_elec.png"), { 80,80 }, 15);
		imgSkill2.ScaleImage(0.5f, 0.5f);
		imgSkill2_Warning.Load(_T("images\\battle\\boss_warning_circle.png"), { 79,79 }, 15, 0x00);
		imgSkill2_Warning.ScaleImage(0.5f, 0.5f);
		break;
	case Type::Water:
		imgSkill1.Load(_T("images\\battle\\boss_skill1_water.png"), { 273,712 });
		imgSkill1.ScaleImage(0.25f, 0.25f);
		imgSkill2.Load(_T("images\\battle\\sprite_boss_skill2_water.png"), { 56,56 }, 14);
		imgSkill2.ScaleImage(0.5f, 0.5f);
		imgSkill2_Warning.Load(_T("images\\battle\\boss_warning_circle.png"), { 79,79 }, 15, 0x00);
		imgSkill2_Warning.ScaleImage(0.5f, 0.5f);
		break;
	case Type::Fire:
		imgSkill1.Load(_T("images\\battle\\boss_skill1_fire.png"), { 17,30 });
		imgSkill1.ScaleImage(2.0f, 2.0f);
		imgSkill2.Load(_T("images\\battle\\boss_skill2_fire.png"), { 173,230 });
		imgSkill2.ScaleImage(1.5f, 1.5f);
		imgSkill2_Warning.Load(_T("images\\battle\\boss_skill2_fire_warning.png"), { 11,223 }, 8, 0x00);
		imgSkill2_Warning.ScaleImage(1.0f, 6.0f);
		break;
	case Type::Dark:
		posOrigins = new Vector2[dark_Skill1Count];
		imgSkill1.Load(_T("images\\battle\\bullet_boss_dark.png"), { 699,699 });
		imgSkill1.ScaleImage(0.05f, 0.05f);
		imgSkill2.Load(_T("images\\battle\\boss_skill2_dark.png"), { 11,33 });
		imgSkill2.ScaleImage(1.5f, 1.5f);
		imgSkill2_Warning.Load(_T("images\\battle\\boss_skill2_dark_warning.png"), { 100,100 }, 200, 0x50);
		imgSkill2_Warning.ScaleImage(3.0f, 3.0f);
		break;	
	default:
		assert(0);
		break;
	}
}
BossSkillManager::~BossSkillManager()
{
	if (boss->GetType() == Type::Dark)
	{
		delete[] posOrigins;
	}
}

void BossSkillManager::Paint(const HDC& hdc)
{
	for (const Effect& effect : warningEffects)
	{
		effect.Paint(hdc);
	}
	for (const Effect& effect : skillEffects)
	{
		effect.Paint(hdc);
	}
}
void BossSkillManager::UseSkill()
{
	switch (boss->GetType())
	{
	case Type::Elec:
		if (boss->GetAct() == BossAct::Skill1)
		{
			Skill1_Elec_Create();
		}
		else if (boss->GetAct() == BossAct::Skill2)
		{
			Skill2_Elec_Create();
		}
		break;
	case Type::Water:
		if (boss->GetAct() == BossAct::Skill1)
		{
			Skill1_Water_Create();
		}
		else if (boss->GetAct() == BossAct::Skill2)
		{
			Skill2_Water_Create();
		}
		break;
	case Type::Fire:
		if (boss->GetAct() == BossAct::Skill1)
		{
			Skill1_Fire_Create();
		}
		else if (boss->GetAct() == BossAct::Skill2)
		{
			Skill2_Fire_Create();
		}
		break;
	case Type::Dark:
		if (boss->GetAct() == BossAct::Skill1)
		{
			Skill1_Dark_Create();
		}
		else if (boss->GetAct() == BossAct::Skill2)
		{
			Skill2_Dark_Create();
		}
		break;
	default:
		assert(0);
		break;
	}
}
void BossSkillManager::Skill1_Elec_Create()
{
	isWarning = true;

	constexpr int lineCount = 9;
	const float rotationDegreePerAnimation = 3.0f + (((rand() % 10) + 1) * 0.1f);

	int sign = rand() % 2;
	if (sign == 0)
	{
		sign = -1;
	}
	rotationDegree = rotationDegreePerAnimation * sign;

	const float startDegree = (rand() % 180);
	for (int i = 0; i < lineCount; ++i)
	{
		skillEffects.emplace_back(imgSkill1_Warning, boss->GetPosCenter(), startDegree + (20.0f * i));
	}
}
void BossSkillManager::Skill1_Elec()
{
	bool isWarningEnd = false;
	bool isSkillEnd = false;

	std::vector<Vector2>unitVectors;
	bool isSoundPlayed = false;
	for (size_t i = 0; i < skillEffects.size(); ++i)
	{
		if (skillEffects.at(i).Animate() == false)
		{
			unitVectors.emplace_back(skillEffects.at(i).GetUnitVector_ImgRotation());
			skillEffects[i--] = skillEffects.back();
			skillEffects.pop_back();
			if (isWarning == true)
			{
				isWarningEnd = true;

				if (isSoundPlayed == false)
				{
					isSoundPlayed = true;
					battle.ShakeMap();
					soundManager->PlayBossSound(BossSound::Elec_Laser);
				}
			}
			else
			{
				isSkillEnd = true;
			}
		}
		else if (isWarning == true)
		{
			rotationDegree -= GetSign(rotationDegree) * 0.02f;
			skillEffects.at(i).Rotate(rotationDegree);
			skillEffects.at(i).IncreaseAlpha(0x0d);
		}
	}

	if (isWarningEnd == true)
	{
		isWarning = false;

		SkillData skillData;
		skillData.damage = boss->GetDamage_Skill1();
		for (const Vector2& unitVector : unitVectors)
		{
			skillEffects.emplace_back(imgSkill1, boss->GetPosCenter(), unitVector, skillData);
		}
	}
	else if (isSkillEnd == true)
	{
		boss->ReSetBossAct();
	}
}

void BossSkillManager::Skill2_Elec_Create()
{
	isWarning = true;

	constexpr int creationMaxCount = 120;

	const RECT rectDisplay = sceneManager->GetRectDisplay();
	for (int i = 0; i < creationMaxCount; ++i)
	{
		const float randX = rand() % (rectDisplay.left + (rectDisplay.right - rectDisplay.left));
		const float randY = rand() % (rectDisplay.top + (rectDisplay.bottom - rectDisplay.top));

		const Vector2 pos = { randX, randY };
		warningEffects.emplace_back(imgSkill2_Warning, pos);
	}
}
void BossSkillManager::Skill2_Elec()
{
	static size_t showCount = 0;
	bool isWarningEnd = false;

	showCount += 5;
	if (showCount > warningEffects.size())
	{
		showCount = warningEffects.size();
	}
	if (showCount == 0)
	{
		isWarningEnd = true;
	}

	std::vector<Vector2>positions;
	bool isSoundPlayed = false;
	for (size_t i = 0; i < showCount; ++i)
	{
		if (warningEffects.at(i).Animate() == false)
		{
			positions.emplace_back(warningEffects.at(i).GetPosCenter());
			warningEffects[i--] = warningEffects.back();
			warningEffects.pop_back();
			--showCount;

			if (isSoundPlayed == false)
			{
				isSoundPlayed = true;
				battle.ShakeMap();
				soundManager->PlayEffectSound(EffectSound::Explosion);
			}
		}
		else
		{
			warningEffects.at(i).IncreaseAlpha(0x10);
		}
	}

	for (size_t i = 0; i < skillEffects.size(); ++i)
	{
		if (skillEffects.at(i).Animate() == false)
		{
			skillEffects[i--] = skillEffects.back();
			skillEffects.pop_back();
		}
	}

	SkillData skillData;
	skillData.damage = boss->GetDamage_Skill2();
	for (const Vector2& position : positions)
	{
		skillEffects.emplace_back(imgSkill2, position, skillData);
	}

	if (isWarningEnd == true && skillEffects.empty() == true)
	{
		showCount = 0;
		boss->ReSetBossAct();
	}
}


void BossSkillManager::Skill1_Water_Create()
{
	constexpr int creationMaxCount = 30;

	const RECT rectDisplay = sceneManager->GetRectDisplay();
	for (int i = 0; i < creationMaxCount; ++i)
	{
		const float randX = rand() % (rectDisplay.left + (rectDisplay.right - rectDisplay.left));
		const float randY = -100;

		const Vector2 pos = { randX, randY };
		SkillData skillData;
		skillData.damage = boss->GetDamage_Skill1();
		skillData.speed = 50.0f;
		skillEffects.emplace_back(imgSkill1, pos, Vector2::Down(), Vector2::Down(), skillData);
	}
}
void BossSkillManager::Skill1_Water()
{
	static float showCount = 0.0f;
	static int prevShowCount = 0;

	showCount += 0.15f;
	const size_t size = skillEffects.size();
	if (showCount > size)
	{
		showCount = size;
	}
	else if (static_cast<int>(showCount) != prevShowCount)
	{
		battle.ShakeMap();
		soundManager->PlayBossSound(BossSound::Water_Tsunami);
	}

	for (size_t i = 0; i < showCount; ++i)
	{
		if (skillEffects.at(i).Animate() == false)
		{
			skillEffects.erase(skillEffects.begin() + i--);
			--showCount;
		}
	}

	if (skillEffects.empty() == true)
	{
		showCount = 0;
		boss->ReSetBossAct();
	}
	prevShowCount = static_cast<int>(showCount);
}
void BossSkillManager::Skill2_Water_Create()
{
	constexpr int lines = 16;
	constexpr int creationCountByLine = 9;

	const RECT rectDisplay = sceneManager->GetRectDisplay();
	const Vector2 mainPosCenter = { (float)(rectDisplay.left + (rectDisplay.right / 2)), (float)(rectDisplay.top + (rectDisplay.bottom / 2)) };

	float radius = 20;
	FRECT rectDraw = GetRect(mainPosCenter, radius);

	warningEffects.emplace_back(imgSkill2_Warning, rectDraw);

	float rotationPerCount = 360.0f / lines;
	float degree = (rand() % 45);
	for (int i = 0; i < creationCountByLine; ++i)
	{
		Vector2 rotationVector = Vector2::Down() * (radius * (i + 1));
		rotationVector = Rotate(rotationVector, degree);
		for (int lineIndex = 0; lineIndex < lines; ++lineIndex)
		{
			const Vector2 posCenter = mainPosCenter + rotationVector;
			rectDraw = GetRect(posCenter, radius);
			warningEffects.emplace_back(imgSkill2_Warning, rectDraw);
			rotationVector = Rotate(rotationVector, rotationPerCount);
		}
		degree += 5;
		radius += 5;
	}
}
void BossSkillManager::Skill2_Water()
{
	constexpr int lines = 16;
	static float showCount = 1.0f;
	static float showLine = 0.0f;

	showLine += 0.5f;
	if (showLine >= 1)
	{
		showLine = 0;
		showCount += lines;
	}
	
	const size_t size = warningEffects.size();
	if (showCount > size)
	{
		showCount = size;
	}

	SkillData skillData;
	skillData.damage = boss->GetDamage_Skill2();

	bool isSoundPlayed = false;
	for (size_t i = 0; i < showCount; ++i)
	{
		if (warningEffects.at(i).Animate() == false)
		{
			if (isSoundPlayed == false)
			{
				isSoundPlayed = true;
				battle.ShakeMap();
				soundManager->PlayBossSound(BossSound::Water_Splash);
			}
			skillEffects.emplace_back(imgSkill2, warningEffects.at(i).GetRectDraw(), skillData);
			warningEffects.erase(warningEffects.begin() + i--);
			--showCount;
		}
		else
		{
			warningEffects.at(i).IncreaseAlpha(0x10);
		}
	}

	for (size_t i = 0; i < skillEffects.size(); ++i)
	{
		if (skillEffects.at(i).Animate() == false)
		{
			skillEffects[i--] = skillEffects.back();
			skillEffects.pop_back();
		}
		else
		{
			if (skillEffects.at(i).GetFrame() < 2 || skillEffects.at(i).GetFrame() > 11)
			{
				skillEffects.at(i).ActiveDamage(false);
			}
			else
			{
				skillEffects.at(i).ActiveDamage(true);
			}
		}
	}

	if (warningEffects.empty() == true && skillEffects.empty() == true)
	{
		showCount = 1.0f;
		showLine = 0.0f;
		boss->ReSetBossAct();
	}
}
void BossSkillManager::Skill1_Fire_Create()
{
	//do nothing
}
void BossSkillManager::Skill1_Fire()
{
	constexpr int creationMaxCount = 10;
	constexpr int skillDelay = 17;
	static int crntSkillDelay = 0;
	static int creationCount = 0;
	static int skillCount = 0;

	if (creationCount < creationMaxCount && crntSkillDelay <= 0)
	{
		constexpr int ballCount = 2;

		skillCount += ballCount;
		++creationCount;
		crntSkillDelay = skillDelay;

		const FRECT rectBoss = boss->GetRectBody();
		const int width = boss->GetBodyWidth();
		const int height = boss->GetBodyHeight();

		Vector2 posCenter = { 0, };
		posCenter.x = rectBoss.left + (rand() % width);
		posCenter.y = rectBoss.top + (rand() % height);
		Vector2 vToPlayer = (mPlayer->GetPosCenter() - posCenter).Normalized();

		SkillData skillData;
		skillData.speed = 20.0f;
		skillData.damage = boss->GetDamage_Skill1();
		skillData.isHitOnce = true;
		skillEffects.emplace_back(imgSkill1, posCenter, Vector2::Down(), vToPlayer, skillData);
		
		posCenter.x = rectBoss.left + (rand() % width);
		posCenter.y = rectBoss.top + (rand() % height);
		vToPlayer = (mPlayer->GetPosCenter() - posCenter).Normalized();
		skillEffects.emplace_back(imgSkill1, posCenter, Vector2::Down(), vToPlayer, skillData);

		battle.ShakeMap();
		soundManager->PlayBossSound(BossSound::Fire_Ball);
	}
	--crntSkillDelay;

	for (int i = 0; i < skillCount; ++i)
	{
		if (skillEffects.at(i).Animate() == false || skillEffects.at(i).RotateToPlayer(0.225f) == false)
		{
			effects->CreateExplosionEffect(skillEffects.at(i).GetPosCenter(), Type::Fire);
			skillEffects.erase(skillEffects.begin() + i--);
			--skillCount;
		}
	}

	if(skillEffects.empty() == true)
	{
		skillCount = 0;
		creationCount = 0;
		crntSkillDelay = 0;
		boss->ReSetBossAct();
	}
}
void BossSkillManager::Skill2_Fire_Create()
{
	const Vector2 posCenter = mPlayer->GetPosCenter();
	warningEffects.emplace_back(imgSkill2_Warning, posCenter);
}
void BossSkillManager::Skill2_Fire()
{
	SkillData skillData;
	skillData.damage = boss->GetDamage_Skill2();
	skillData.speed = 70.0f;
	if (warningEffects.empty() == false)
	{
		if (warningEffects.front().Animate() == false)
		{
			Vector2 posCenter = warningEffects.front().GetPosCenter();
			posCenter.y = -500;
			skillEffects.emplace_back(imgSkill2, posCenter, Vector2::Down(), Vector2::Down(), skillData);
			warningEffects.clear();

			battle.ShakeMap(10);
			soundManager->PlayBossSound(BossSound::Fire_Meteor);
		}
		else
		{
			warningEffects.front().IncreaseAlpha(0x20);
			const Vector2 vToPlayer = (mPlayer->GetPosCenter() - warningEffects.front().GetPosCenter()) * 0.25f;
			const Vector2 posCenter = warningEffects.front().GetPosCenter() + vToPlayer;
			warningEffects.front().SetPosCenter(posCenter);
		}
	}

	if(skillEffects.empty() == false)
	{
		if (skillEffects.front().Animate() == false)
		{
			skillEffects.clear();
		}
	}

	if (skillEffects.empty() == true && warningEffects.empty() == true)
	{
		boss->ReSetBossAct();
	}
}
void BossSkillManager::Skill1_Dark_Create()
{
	skillEffects.reserve(dark_Skill1Count * dark_Skill1Count);

	soundManager->PlayBossSound(BossSound::Dark);
}
void BossSkillManager::Skill1_Dark()
{
	const int creationMaxCount = dark_Skill1Count;
	constexpr int skillDelay = 60;
	static int crntSkillDelay = 0;
	static int creationCount = 0;

	DarkSkillData darkSkilldata;
	darkSkilldata.radius = 140.0f;
	darkSkilldata.posOrigin = &posOrigins[creationCount];
	darkSkilldata.rotationDegree = rand() % 45;

	if (creationCount < creationMaxCount && crntSkillDelay <= 0)
	{
		constexpr int ballCount = 8;
		crntSkillDelay = skillDelay;

		SkillData skillData;
		skillData.isHitOnce = true;
		skillData.damage = boss->GetDamage_Skill1();

		const RECT rectDisplay(sceneManager->GetRectDisplay());
		const int displayWidth = rectDisplay.right - rectDisplay.left;;

		const FRECT rectBoss(boss->GetRectBody());
		const float bossHeight = boss->GetBodyHeight();

		Vector2 posCenter = { 0, };
		posCenter.x = rectDisplay.left + (rand() % displayWidth);
		posCenter.y = rectBoss.bottom - bossHeight / 2;

		posOrigins[creationCount] = posCenter;
		for (int i = 0; i < ballCount; ++i)
		{
			skillEffects.emplace_back(imgSkill1, skillData, darkSkilldata);
			darkSkilldata.rotationDegree += 360 / ballCount;
		}

		EffectImage imgSkill1_Sub(imgSkill1);
		imgSkill1_Sub.ScaleImage(3.0f, 3.0f);
		darkSkilldata.radius = 20.0f;
		for (int i = 0; i < ballCount; ++i)
		{
			skillEffects.emplace_back(imgSkill1_Sub, skillData, darkSkilldata);
			darkSkilldata.rotationDegree += 360 / ballCount;
		}
		darkSkilldata.radius = 140.0f;

		++creationCount;
	}
	--crntSkillDelay;

	for (size_t i = 0; i < skillEffects.size(); ++i)
	{
		if (skillEffects.at(i).Animate() == false)
		{
			skillEffects[i--] = skillEffects.back();
			skillEffects.pop_back();
		}
	}

	for (int i = 0; i < creationCount; ++i)
	{
		if (posOrigins[i] != Vector2::Zero())
		{
			constexpr float speed = 5.0f;
			posOrigins[i].y += speed;
			if (posOrigins[i].y > (sceneManager->GetRectDisplay().bottom + (darkSkilldata.radius * 2.5f)))
			{
				posOrigins[i] = Vector2::Zero();
			}
		}
	}

	if (skillEffects.empty() == true)
	{
		crntSkillDelay = 0;
		creationCount = 0;
		boss->ReSetBossAct();
		soundManager->StopBossSound();
	}
}
void BossSkillManager::Skill2_Dark_Create()
{
	const RECT rectDisplay(sceneManager->GetRectDisplay());
	Vector2 posCenter = { 0, };
	posCenter.x = (float)rectDisplay.left + ((float)(rectDisplay.right - rectDisplay.left) / 2);
	posCenter.y = (float)rectDisplay.top + ((float)(rectDisplay.bottom - rectDisplay.top) / 2);

	warningEffects.emplace_back(imgSkill2_Warning, posCenter);

	soundManager->PlayBossSound(BossSound::Dark);
}
void BossSkillManager::Skill2_Dark()
{
	if (warningEffects.empty() == false)
	{
		if (warningEffects.back().Animate() == false)
		{
			warningEffects.clear();
		}
		else
		{
			if (warningEffects.back().GetFrame() < 170)
			{
				warningEffects.back().Rotate(5.0f);
				warningEffects.back().IncreaseAlpha(0x05);
			}
			else
			{
				warningEffects.back().Rotate(2.5f);
				warningEffects.back().ReduceAlpha(0x05);
			}

			if (warningEffects.back().GetFrame() > 30)
			{
				SkillData skillData;
				skillData.damage = boss->GetDamage_Skill2();
				skillData.speed = 20.0f;
				skillData.isHitOnce = true;

				const Vector2 posCenter = warningEffects.back().GetPosCenter();
				float degree = rand() % 360;
				Vector2 unitVector = ::Rotate(Vector2::Up(), degree);

				skillEffects.emplace_back(imgSkill2, posCenter, unitVector, unitVector, skillData);

				degree = rand() % 360;
				unitVector = ::Rotate(Vector2::Up(), degree);
				skillEffects.emplace_back(imgSkill2, posCenter, unitVector, unitVector, skillData);
			}
		}
	}

	for (size_t i = 0; i < skillEffects.size(); ++i)
	{
		if (skillEffects.at(i).Animate() == false)
		{
			skillEffects[i--] = skillEffects.back();
			skillEffects.pop_back();
		}
	}
	
	if(warningEffects.empty() == true && skillEffects.empty() == true)
	{
		boss->ReSetBossAct();
		soundManager->StopBossSound();
	}
}

// 보스 타입에 따라서 각 스킬 사용(여기만 보면 됨)
void BossSkillManager::Animate()
{
	switch (boss->GetType())
	{
	case Type::Elec:
		if (boss->GetAct() == BossAct::Skill1)
		{
			Skill1_Elec();
		}
		else if (boss->GetAct() == BossAct::Skill2)
		{
			Skill2_Elec();
		}
		break;
	case Type::Water:
		if (boss->GetAct() == BossAct::Skill1)
		{
			Skill1_Water();
		}
		else if (boss->GetAct() == BossAct::Skill2)
		{
			Skill2_Water();
		}
		break;
	case Type::Fire:
		if (boss->GetAct() == BossAct::Skill1)
		{
			Skill1_Fire();
		}
		else if (boss->GetAct() == BossAct::Skill2)
		{
			Skill2_Fire();
		}
		break;
	case Type::Dark:
		if (boss->GetAct() == BossAct::Skill1)
		{
			Skill1_Dark();
		}
		else if (boss->GetAct() == BossAct::Skill2)
		{
			Skill2_Dark();
		}
		break;
	default:
		assert(0);
		break;
	}
}