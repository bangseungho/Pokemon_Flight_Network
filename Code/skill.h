#pragma once
#include "image.h"

class SkillManager {
private:
	class Effect : public ISprite {
	private:
		const EffectImage* imgSkill;
		Type type;
	public:
		Effect(const EffectImage& imgSkill, Type type);
		void Paint(const HDC& hdc, const RECT& rectBody) const;
		bool Animate();

		inline int GetFrame() const
		{
			return frame;
		}
	};

	Effect* skillEffect = nullptr;
	Skill crntSkill = Skill::Empty;
	bool isIdentity = false;

	int skillCount = 0;

	EffectImage imgSkill_Elec_Q;
	EffectImage imgSkill_Fire_Q;
	EffectImage imgSkill_Water_Q;

	void ShotBySector();
	void ShotByCircle();

	RECT GetRectBody() const;
public:
	SkillManager();

	void UseSkill();
	void Paint(const HDC& hdc) const;
	void Animate();

	void ActiveSkill(Skill skill);
	
	inline bool IsUsingSkill() const
	{
		return (crntSkill == Skill::Sector || crntSkill == Skill::Circle) ? true : false;
	}
};



class BossSkillManager {
private:
	typedef struct SkillData {
		int rotationCount = 0;
		float speed = 1;
		float damage = 0;
		bool isActiveDamage = true;
		bool isRotated = false;
		bool isHitOnce = false;

	}SkillData;
	typedef struct DarkSkillData {
		const Vector2* posOrigin = nullptr;
		float rotationDegree = 0.0f;
		float radius = 0.0f;

		inline constexpr Vector2 GetPosCenter() const
		{
			Vector2 vector = ::Rotate(Vector2::Down(), rotationDegree) * radius;
			return *posOrigin + vector;
		}
		inline constexpr Vector2 GetRotatePos(float degree)
		{
			rotationDegree += degree;
			return GetPosCenter();
		}
	}DarkSkillData;

	class Effect : public ISprite {
	private:
		EffectImage imgSkill;
		FRECT rectDraw = { -1, };
		Vector2 posCenter = { 0, };

		Vector2 unitVector_imgRotation = Vector2::Down();
		Vector2 unitVector_direction = Vector2::Zero();
		
		SkillData skillData;
		DarkSkillData darkSkillData;

		bool Move();
	public:
		Effect(const EffectImage& imgSkill, const FRECT rectDraw, const SkillData& skillData = {});
		Effect(const EffectImage& imgSkill, const Vector2& pos, const SkillData& skillData = {});
		Effect(const EffectImage& imgSkill, const Vector2& pos, float rotationDegree, const SkillData& skillData = {});
		Effect(const EffectImage& imgSkill, const Vector2& pos, const Vector2& unitVector_imgRotation, const SkillData& skillData);
		Effect(const EffectImage& imgSkill, const Vector2& pos, const Vector2& unitVector_imgRotation, const Vector2& unitVector_direction, const SkillData& skillData);
		Effect(const EffectImage& imgSkill, const SkillData& skillData, const DarkSkillData& darkSkillData);
		void Paint(HDC hdc) const;
		bool Animate();

		RECT GetRectBody() const;	
		bool RotateToPlayer(float t);
		inline void IncreaseAlpha(BYTE alpha)
		{
			imgSkill.IncreaseAlpha(alpha);
		}
		inline void ReduceAlpha(BYTE alpha)
		{
			imgSkill.ReduceAlpha(alpha);
		}
		inline void Rotate(float degree)
		{
			skillData.isRotated = true;
			unitVector_imgRotation = ::Rotate(unitVector_imgRotation, degree);
		}
		inline constexpr Vector2 GetUnitVector_ImgRotation() const
		{
			return unitVector_imgRotation;
		}
		inline constexpr Vector2 GetPosCenter() const
		{
			return posCenter;
		}
		inline constexpr float GetDamage() const
		{
			return skillData.damage;
		}
		inline constexpr FRECT GetRectDraw() const
		{
			return rectDraw;
		}
		inline constexpr int GetFrame() const
		{
			return frame;
		}
		inline constexpr int GetRotationCount() const
		{
			return skillData.rotationCount;
		}
		inline constexpr void ActiveDamage(bool active)
		{
			this->skillData.isActiveDamage = active;
		}
		inline constexpr void SetPosCenter(const Vector2& pos)
		{
			this->posCenter = pos;
		}
	};

	bool isWarning = false;
	float rotationDegree = 0.0f;

	EffectImage imgSkill1;
	EffectImage imgSkill1_Warning;
	EffectImage imgSkill2;
	EffectImage imgSkill2_Warning;

	const int dark_Skill1Count = 5;
	Vector2* posOrigins = nullptr;

	std::vector<Effect>warningEffects;
	std::vector<Effect>skillEffects;
	void Skill1_Elec_Create();
	void Skill1_Elec();
	void Skill2_Elec_Create();
	void Skill2_Elec();

	void Skill1_Water_Create();
	void Skill1_Water();
	void Skill2_Water_Create();
	void Skill2_Water();

	void Skill1_Fire_Create();
	void Skill1_Fire();
	void Skill2_Fire_Create();
	void Skill2_Fire();

	void Skill1_Dark_Create();
	void Skill1_Dark();
	void Skill2_Dark_Create();
	void Skill2_Dark();
public:
	BossSkillManager();
	~BossSkillManager();

	void Paint(const HDC& hdc);
	void Animate();

	void UseSkill();
};