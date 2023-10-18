#pragma once

enum class EffectSound { Win = 0, Loss, Shot, Shot_nLoop, Explosion };
enum class BGMSound { Intro = 0, Town1, Town2, Stage, Battle, Battle_Boss, Ending };
enum class SkillSound { Elec = 0, Fire, Water };
enum class HitSound { Elec = 0, Fire, Water, Dark };
enum class BossSound { Elec_Laser, Fire_Meteor, Fire_Ball, Water_Tsunami, Water_Splash, Dark };
enum class SelectSound { Pikachu1, Pikachu2, Squirtle, Charmander };
enum class SoundChannel { Bgm = 0, Effect, Skill, Boss };

// has Channel
#define SOUND_LIST_BGM 7
#define SOUND_LIST_Effect 10
#define SOUND_LIST_Skill 3
#define SOUND_LIST_BOSS 6
#define SOUND_LIST_SELECT 4
#define SOUND_CHANNEL_LIST 4

// has no Channel
#define SOUND_LIST_Hit 4

// Stop은 Play후에 사용할 것
class SoundManager {
private:
	FMOD_SYSTEM* soundSystem = nullptr;
	FMOD_SOUND* bgmSoundList[SOUND_LIST_BGM];
	FMOD_SOUND* effectSoundList[SOUND_LIST_Effect];
	FMOD_SOUND* skillSoundList[SOUND_LIST_Skill];
	FMOD_SOUND* hitSoundList[SOUND_LIST_Hit];
	FMOD_SOUND* bossSoundList[SOUND_LIST_BOSS];
	FMOD_SOUND* selectSoundList[SOUND_LIST_SELECT];
	FMOD_CHANNEL* soundChannel[SOUND_CHANNEL_LIST];


public:
	SoundManager();

	void PlayEffectSound(const EffectSound& effectSound, const float& volume = 1.0f, bool isNewChannel = false);
	void StopEffectSound();

	void PlayBGMSound(const BGMSound& bgmSound, const float& volume = 1.0f, bool isNewChannel = false);
	void StopBGMSound();

	void PlaySkillSound(SkillSound skillSound, const float& volume = 1.0f);
	void StopSkillSound();

	void PlayHitSound(HitSound hitSound);

	void PlayBossSound(BossSound bossSound);
	void StopBossSound();

	void PlaySelectSound(SelectSound selectSound);
};