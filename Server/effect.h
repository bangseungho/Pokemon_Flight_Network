#pragma once
#include "image.h"

enum class EffectType { Empty = 0, Explode_Fire, Explode_Water, Explode_Elec, Cloud_Fire, Cloud_Water, Cloud_Elec };
class Boss;

class EffectManager {
private:
	class Effect : public ISprite {
	private:
		const EffectImage* effectImage;
		POINT pos = { 0, };
	public:
		Effect(const EffectImage& effectImage, const POINT& pos);
		bool Animate();
	};

	// Ω««‡«ÿæﬂ «“ √— ¿Ã∆Â∆Æ∏¶ ¥„¥¬ ∫§≈Õ
	std::vector<Effect> effects;
public:
	EffectManager();
	void CreateHitEffect(const POINT& pos, Type type);
	void CreateExplosionEffect(const POINT& pos, Type type);

	//void CreateBossDeathEffect(const Boss& boss);
	//void CreateBossExplosionEffect(const Boss& boss);
};

void GetRandEffectPoint(POINT& effectPoint);