#include "stdafx.h"
#include "effect.h"
#include "boss.h"
#include "sound.h"

extern SoundManager* soundManager;

// 이펙트 매니저는 게임에서 생성되는 모든 이펙트를 벡터에 담아서 관리한다.
// 이펙트를 실행했다면 당연히 해당 벡터에서 제거한다.
EffectManager::EffectManager()
{
	explode_fire.Load(_T("images\\battle\\explode_fire.png"), { 56, 56 }, 8);
	explode_fire.ScaleImage(0.7f, 0.7f);
	explode_elec.Load(_T("images\\battle\\explode_elec.png"), { 81, 73 }, 8);
	explode_elec.ScaleImage(0.6f, 0.6f);
	explode_water.Load(_T("images\\battle\\explode_water.png"), { 72, 64 }, 9);
	explode_water.ScaleImage(0.7f, 0.7f);
	explode_dark.Load(_T("images\\battle\\explode_dark.png"), { 40, 40 }, 12);

	cloud_fire.Load(_T("images\\battle\\sprite_cloud_fire.png"), { 56, 64 }, 11);
	cloud_fire.ScaleImage(0.8f, 0.8f);
	cloud_elec.Load(_T("images\\battle\\sprite_cloud_elec.png"), { 56, 64 }, 11);
	cloud_elec.ScaleImage(0.8f, 0.8f);
	cloud_water.Load(_T("images\\battle\\sprite_cloud_water.png"), { 48, 56 }, 15);
	cloud_water.ScaleImage(0.9f, 0.9f);
	cloud_dark.Load(_T("images\\battle\\sprite_cloud_dark.png"), { 56, 64 }, 11);
}
EffectManager::Effect::Effect(const EffectImage& effectImage, const POINT& pos)
{
	this->effectImage = &effectImage;
	this->pos = pos;
}

// 이펙트 렌더링
void EffectManager::Paint(HDC hdc) const
{
	for (const Effect& effect : effects)
	{
		effect.Paint(hdc);
	}
}

// 적 또는 플레이어가 충돌했을 경우 플레이어의 타입에 따라서 해당하는 충돌 이펙트를 추가한다.
void EffectManager::CreateHitEffect(const POINT& pos, Type type)
{
	switch (type)
	{
	case Type::Fire:
		effects.emplace_back(explode_fire, pos);
		break;
	case Type::Elec:
		effects.emplace_back(explode_elec, pos);
		break;
	case Type::Water:
		effects.emplace_back(explode_water, pos);
		break;
	case Type::Dark:
		effects.emplace_back(explode_dark, pos);
		break;
	default:
		assert(0);
		break;
	}
}

// 적 또는 플레이어가 사망했을 경우 플레이어의 타입에 따라서 해당하는 사망 이펙트를 추가한다.
void EffectManager::CreateExplosionEffect(const POINT& pos, Type type)
{
	switch (type)
	{
	case Type::Fire:
		effects.emplace_back(cloud_fire, pos);
		break;
	case Type::Elec:
		effects.emplace_back(cloud_elec, pos);
		break;
	case Type::Water:
		effects.emplace_back(cloud_water, pos);
		break;
	case Type::Dark:
		effects.emplace_back(cloud_dark, pos);
		break;
	default:
		assert(0);
		break;
	}
}

// 게임 중에 생긴 모든 이펙트를 실행시키고 삭제한다.
void EffectManager::Animate()
{
	for (size_t i = 0; i < effects.size(); ++i)
	{
		if (effects.at(i).Animate() == false)
		{
			effects[i--] = effects.back();
			effects.pop_back();
		}
	}
}

// 이펙트를 애니메이션하고 렌더링하는 함수들
void EffectManager::Effect::Paint(HDC hdc) const
{
	if (frame < -9999)
		return;
	
	const RECT rectImage = ISprite::GetRectImage(*effectImage, frame);
	effectImage->Paint(hdc, pos, &rectImage);
}
bool EffectManager::Effect::Animate()
{
	if (++frame >= effectImage->GetMaxFrame())
	{
		return false;
	}

	return true;
}

// 랜덤한 위치를 반환받는다.
void GetRandEffectPoint(POINT& effectPoint)
{
	constexpr int range = 20;
	effectPoint.x += (rand() % range) - (range / 2);
	effectPoint.y += (rand() % range) - (range / 2);
}

// 보스 사망 이펙트로 처음에 작은 폭발이 일어나다가 CreateBossExplosionEffect 함수에서 큰 폭발이 한 번 일어나고 끝난다.
void EffectManager::CreateBossDeathEffect(const Boss& boss)
{
	const EffectImage* effectImage = nullptr;
	switch (boss.GetType())
	{
	case Type::Elec:
		effectImage = &cloud_elec;
		break;
	case Type::Fire:
		effectImage = &cloud_fire;
		break;
	case Type::Water:
		effectImage = &cloud_water;
		break;
	case Type::Dark:
		effectImage = &cloud_dark;
		break;
	default:
		assert(0);
		break;
	}

	const FRECT rectBoss = boss.GetRectBody();
	const int width = boss.GetBodyWidth();
	const int height = boss.GetBodyHeight();
	for (int i = 0; i < 10; ++i)
	{
		const int randX = rectBoss.left + (rand() % width);
		const int randY = rectBoss.top + (rand() % height);
		const POINT pos = { randX, randY };
		effects.emplace_back(*effectImage, pos);
	}
	soundManager->PlayEffectSound(EffectSound::Explosion);
}
void EffectManager::CreateBossExplosionEffect(const Boss& boss)
{
	static EffectImage* effectImage = nullptr;
	if (effectImage != nullptr)
	{
		delete effectImage;
	}
	effectImage = new EffectImage();

	switch (boss.GetType())
	{
	case Type::Elec:
		*effectImage = cloud_elec;
		break;
	case Type::Fire:
		*effectImage = cloud_fire;
		break;
	case Type::Water:
		*effectImage = cloud_water;
		break;
	case Type::Dark:
		*effectImage = cloud_dark;
		break;
	default:
		assert(0);
		break;
	}

	effectImage->ScaleImage(10.0f, 10.0f);
	effects.emplace_back(*effectImage, boss.GetPosCenter());
	soundManager->PlayEffectSound(EffectSound::Explosion);
}