#include "..\Utils.h"
#include "effect.h"
#include "boss.h"

// ����Ʈ �Ŵ����� ���ӿ��� �����Ǵ� ��� ����Ʈ�� ���Ϳ� ��Ƽ� �����Ѵ�.
// ����Ʈ�� �����ߴٸ� �翬�� �ش� ���Ϳ��� �����Ѵ�.
EffectManager::EffectManager()
{

}
EffectManager::Effect::Effect(const EffectImage& effectImage, const POINT& pos)
{
	this->effectImage = &effectImage;
	this->pos = pos;
}

// �� �Ǵ� �÷��̾ �浹���� ��� �÷��̾��� Ÿ�Կ� ���� �ش��ϴ� �浹 ����Ʈ�� �߰��Ѵ�.
void EffectManager::CreateHitEffect(const POINT& pos, Type type)
{

}

// �� �Ǵ� �÷��̾ ������� ��� �÷��̾��� Ÿ�Կ� ���� �ش��ϴ� ��� ����Ʈ�� �߰��Ѵ�.
void EffectManager::CreateExplosionEffect(const POINT& pos, Type type)
{

}

// ������ ��ġ�� ��ȯ�޴´�.
void GetRandEffectPoint(POINT& effectPoint)
{
	constexpr int range = 20;
	effectPoint.x += (rand() % range) - (range / 2);
	effectPoint.y += (rand() % range) - (range / 2);
}

//// ���� ��� ����Ʈ�� ó���� ���� ������ �Ͼ�ٰ� CreateBossExplosionEffect �Լ����� ū ������ �� �� �Ͼ�� ������.
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