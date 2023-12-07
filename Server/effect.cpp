#include "..\Utils.h"
#include "effect.h"
#include "boss.h"

// 이펙트 매니저는 게임에서 생성되는 모든 이펙트를 벡터에 담아서 관리한다.
// 이펙트를 실행했다면 당연히 해당 벡터에서 제거한다.
EffectManager::EffectManager()
{

}
EffectManager::Effect::Effect(const EffectImage& effectImage, const POINT& pos)
{
	this->effectImage = &effectImage;
	this->pos = pos;
}

// 적 또는 플레이어가 충돌했을 경우 플레이어의 타입에 따라서 해당하는 충돌 이펙트를 추가한다.
void EffectManager::CreateHitEffect(const POINT& pos, Type type)
{

}

// 적 또는 플레이어가 사망했을 경우 플레이어의 타입에 따라서 해당하는 사망 이펙트를 추가한다.
void EffectManager::CreateExplosionEffect(const POINT& pos, Type type)
{

}

// 랜덤한 위치를 반환받는다.
void GetRandEffectPoint(POINT& effectPoint)
{
	constexpr int range = 20;
	effectPoint.x += (rand() % range) - (range / 2);
	effectPoint.y += (rand() % range) - (range / 2);
}

//// 보스 사망 이펙트로 처음에 작은 폭발이 일어나다가 CreateBossExplosionEffect 함수에서 큰 폭발이 한 번 일어나고 끝난다.
//void EffectManager::CreateBossDeathEffect(const Boss& boss)
//{
//	const EffectImage* effectImage = nullptr;
//	switch (boss.GetType())
//	{
//	case Type::Elec:
//		effectImage = &cloud_elec;
//		break;
//	case Type::Fire:
//		effectImage = &cloud_fire;
//		break;
//	case Type::Water:
//		effectImage = &cloud_water;
//		break;
//	case Type::Dark:
//		effectImage = &cloud_dark;
//		break;
//	default:
//		assert(0);
//		break;
//	}
//
//	const FRECT rectBoss = boss.GetRectBody();
//	const int width = boss.GetBodyWidth();
//	const int height = boss.GetBodyHeight();
//	for (int i = 0; i < 10; ++i)
//	{
//		const int randX = rectBoss.left + (rand() % width);
//		const int randY = rectBoss.top + (rand() % height);
//		const POINT pos = { randX, randY };
//		effects.emplace_back(*effectImage, pos);
//	}
//	soundManager->PlayEffectSound(EffectSound::Explosion);
//}
//void EffectManager::CreateBossExplosionEffect(const Boss& boss)
//{
//	static EffectImage* effectImage = nullptr;
//	if (effectImage != nullptr)
//	{
//		delete effectImage;
//	}
//	effectImage = new EffectImage();
//
//	switch (boss.GetType())
//	{
//	case Type::Elec:
//		*effectImage = cloud_elec;
//		break;
//	case Type::Fire:
//		*effectImage = cloud_fire;
//		break;
//	case Type::Water:
//		*effectImage = cloud_water;
//		break;
//	case Type::Dark:
//		*effectImage = cloud_dark;
//		break;
//	default:
//		assert(0);
//		break;
//	}
//
//	effectImage->ScaleImage(10.0f, 10.0f);
//	effects.emplace_back(*effectImage, boss.GetPosCenter());
//	soundManager->PlayEffectSound(EffectSound::Explosion);
//}