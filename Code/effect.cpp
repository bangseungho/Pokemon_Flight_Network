#include "stdafx.h"
#include "effect.h"
#include "boss.h"
#include "sound.h"

extern SoundManager* soundManager;

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

void EffectManager::Paint(HDC hdc) const
{
	for (const Effect& effect : effects)
	{
		effect.Paint(hdc);
	}
}

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


void EffectManager::Effect::Paint(HDC hdc) const
{
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


void GetRandEffectPoint(POINT& effectPoint)
{
	constexpr int range = 20;
	effectPoint.x += (rand() % range) - (range / 2);
	effectPoint.y += (rand() % range) - (range / 2);
}
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