#include "stdafx.h"
#include "sound.h"


inline constexpr void CheckResult(const FMOD_RESULT& result)
{
	if (result != FMOD_OK)
	{
		assert(0);
	}
}
SoundManager::SoundManager()
{
	FMOD_System_Create(&soundSystem, FMOD_VERSION);
	FMOD_System_Init(soundSystem, 32, FMOD_INIT_NORMAL, NULL);

	FMOD_RESULT result = FMOD_OK;

	// bgmSound
	result = FMOD_System_CreateSound(soundSystem, "Sounds\\bgm_intro.wav", FMOD_LOOP_NORMAL, 0, &bgmSoundList[static_cast<int>(BGMSound::Intro)]);
	CheckResult(result);
	result = FMOD_System_CreateSound(soundSystem, "Sounds\\bgm_town1.wav", FMOD_LOOP_NORMAL, 0, &bgmSoundList[static_cast<int>(BGMSound::Town1)]);
	CheckResult(result);
	result = FMOD_System_CreateSound(soundSystem, "Sounds\\bgm_town2.mp3", FMOD_LOOP_NORMAL, 0, &bgmSoundList[static_cast<int>(BGMSound::Town2)]);
	CheckResult(result);
	result = FMOD_System_CreateSound(soundSystem, "Sounds\\bgm_stage.wav", FMOD_LOOP_NORMAL, 0, &bgmSoundList[static_cast<int>(BGMSound::Stage)]);
	CheckResult(result);
	result = FMOD_System_CreateSound(soundSystem, "Sounds\\bgm_battle_field.mp3", FMOD_LOOP_NORMAL, 0, &bgmSoundList[static_cast<int>(BGMSound::Battle)]);
	CheckResult(result);
	result = FMOD_System_CreateSound(soundSystem, "Sounds\\bgm_battle_boss.mp3", FMOD_LOOP_NORMAL, 0, &bgmSoundList[static_cast<int>(BGMSound::Battle_Boss)]);
	CheckResult(result);
	result = FMOD_System_CreateSound(soundSystem, "Sounds\\bgm_ending.wav", FMOD_LOOP_NORMAL, 0, &bgmSoundList[static_cast<int>(BGMSound::Ending)]);
	CheckResult(result);

	// effectSound
	result = FMOD_System_CreateSound(soundSystem, "Sounds\\loss.mp3", FMOD_DEFAULT, 0, &effectSoundList[static_cast<int>(EffectSound::Loss)]);
	CheckResult(result);
	result = FMOD_System_CreateSound(soundSystem, "Sounds\\win.wav", FMOD_DEFAULT, 0, &effectSoundList[static_cast<int>(EffectSound::Win)]);
	CheckResult(result);
	result = FMOD_System_CreateSound(soundSystem, "Sounds\\shot.mp3", FMOD_LOOP_NORMAL, 0, &effectSoundList[static_cast<int>(EffectSound::Shot)]);
	CheckResult(result);
	result = FMOD_System_CreateSound(soundSystem, "Sounds\\shot.mp3", FMOD_DEFAULT, 0, &effectSoundList[static_cast<int>(EffectSound::Shot_nLoop)]);
	CheckResult(result);
	result = FMOD_System_CreateSound(soundSystem, "Sounds\\explosion.mp3", FMOD_DEFAULT, 0, &effectSoundList[static_cast<int>(EffectSound::Explosion)]);
	CheckResult(result);

	// skillSound
	result = FMOD_System_CreateSound(soundSystem, "Sounds\\skill_elec.wav", FMOD_DEFAULT, 0, &skillSoundList[static_cast<int>(SkillSound::Elec)]);
	CheckResult(result);
	result = FMOD_System_CreateSound(soundSystem, "Sounds\\skill_fire.wav", FMOD_DEFAULT, 0, &skillSoundList[static_cast<int>(SkillSound::Fire)]);
	CheckResult(result);
	result = FMOD_System_CreateSound(soundSystem, "Sounds\\skill_water.wav", FMOD_DEFAULT, 0, &skillSoundList[static_cast<int>(SkillSound::Water)]);
	CheckResult(result);	

	// hitSound
	result = FMOD_System_CreateSound(soundSystem, "Sounds\\hit_elec.wav", FMOD_DEFAULT, 0, &hitSoundList[static_cast<int>(HitSound::Elec)]);
	CheckResult(result);
	result = FMOD_System_CreateSound(soundSystem, "Sounds\\hit_fire.wav", FMOD_DEFAULT, 0, &hitSoundList[static_cast<int>(HitSound::Fire)]);
	CheckResult(result);
	result = FMOD_System_CreateSound(soundSystem, "Sounds\\hit_water.mp3", FMOD_DEFAULT, 0, &hitSoundList[static_cast<int>(HitSound::Water)]);
	CheckResult(result);
	result = FMOD_System_CreateSound(soundSystem, "Sounds\\hit_dark.wav", FMOD_DEFAULT, 0, &hitSoundList[static_cast<int>(HitSound::Dark)]);
	CheckResult(result);

	// bossSound
	result = FMOD_System_CreateSound(soundSystem, "Sounds\\boss_elec_laser.wav", FMOD_DEFAULT, 0, &bossSoundList[static_cast<int>(BossSound::Elec_Laser)]);
	CheckResult(result);
	result = FMOD_System_CreateSound(soundSystem, "Sounds\\boss_fire_meteor.wav", FMOD_DEFAULT, 0, &bossSoundList[static_cast<int>(BossSound::Fire_Meteor)]);
	CheckResult(result);
	result = FMOD_System_CreateSound(soundSystem, "Sounds\\boss_fire_ball.wav", FMOD_DEFAULT, 0, &bossSoundList[static_cast<int>(BossSound::Fire_Ball)]);
	CheckResult(result);
	result = FMOD_System_CreateSound(soundSystem, "Sounds\\boss_water_tsunami.mp3", FMOD_DEFAULT, 0, &bossSoundList[static_cast<int>(BossSound::Water_Tsunami)]);
	CheckResult(result);
	result = FMOD_System_CreateSound(soundSystem, "Sounds\\boss_water_splash.wav", FMOD_DEFAULT, 0, &bossSoundList[static_cast<int>(BossSound::Water_Splash)]);
	CheckResult(result);
	result = FMOD_System_CreateSound(soundSystem, "Sounds\\boss_dark.wav", FMOD_DEFAULT, 0, &bossSoundList[static_cast<int>(BossSound::Dark)]);
	CheckResult(result);

	// selectSound
	result = FMOD_System_CreateSound(soundSystem, "Sounds\\pikachu1.wav", FMOD_DEFAULT, 0, &selectSoundList[static_cast<int>(SelectSound::Pikachu1)]);
	CheckResult(result);
	result = FMOD_System_CreateSound(soundSystem, "Sounds\\pikachu2.wav", FMOD_DEFAULT, 0, &selectSoundList[static_cast<int>(SelectSound::Pikachu2)]);
	CheckResult(result);
	result = FMOD_System_CreateSound(soundSystem, "Sounds\\squirtle.wav", FMOD_DEFAULT, 0, &selectSoundList[static_cast<int>(SelectSound::Squirtle)]);
	CheckResult(result);
	result = FMOD_System_CreateSound(soundSystem, "Sounds\\charmander.wav", FMOD_DEFAULT, 0, &selectSoundList[static_cast<int>(SelectSound::Charmander)]);
	CheckResult(result);

	for (int i = 0; i < SOUND_CHANNEL_LIST; ++i)
	{
		soundChannel[i] = 0;
	}
}

void SoundManager::PlayEffectSound(const EffectSound& effectSound, const float& volume, bool isNewChannel)
{
	FMOD_System_Update(soundSystem);
	if (isNewChannel == true)
	{
		FMOD_System_PlaySound(soundSystem, effectSoundList[static_cast<int>(effectSound)], 0, false, &soundChannel[static_cast<int>(SoundChannel::Effect)]);
		FMOD_Channel_SetVolume(soundChannel[static_cast<int>(SoundChannel::Effect)], volume);
	}
	else
	{
		FMOD_System_PlaySound(soundSystem, effectSoundList[static_cast<int>(effectSound)], 0, false, NULL);
	}
}
void SoundManager::StopEffectSound()
{
	FMOD_Channel_Stop(soundChannel[static_cast<int>(SoundChannel::Effect)]);
}

void SoundManager::PlayBGMSound(const BGMSound& bgmSound, const float& volume, bool isNewChannel)
{
	FMOD_System_Update(soundSystem);
	if (isNewChannel == true)
	{
		FMOD_System_PlaySound(soundSystem, bgmSoundList[static_cast<int>(bgmSound)], 0, false, &soundChannel[static_cast<int>(SoundChannel::Bgm)]);
		FMOD_Channel_SetVolume(soundChannel[static_cast<int>(SoundChannel::Bgm)], volume);
	}
}
void SoundManager::StopBGMSound()
{
	FMOD_Channel_Stop(soundChannel[static_cast<int>(SoundChannel::Bgm)]);
}

void SoundManager::PlaySkillSound(SkillSound skillSound, const float& volume)
{
	FMOD_System_Update(soundSystem);
	FMOD_System_PlaySound(soundSystem, skillSoundList[static_cast<int>(skillSound)], 0, false, &soundChannel[static_cast<int>(SoundChannel::Skill)]);
	FMOD_Channel_SetVolume(soundChannel[static_cast<int>(SoundChannel::Skill)], volume);
}
void SoundManager::StopSkillSound()
{
	FMOD_Channel_Stop(soundChannel[static_cast<int>(SoundChannel::Skill)]);
}

void SoundManager::PlayHitSound(HitSound hitSound)
{
	FMOD_System_Update(soundSystem);
	FMOD_System_PlaySound(soundSystem, hitSoundList[static_cast<int>(hitSound)], 0, false, NULL);
}

void SoundManager::PlayBossSound(BossSound bossSound)
{
	FMOD_System_Update(soundSystem);
	FMOD_System_PlaySound(soundSystem, bossSoundList[static_cast<int>(bossSound)], 0, false, &soundChannel[static_cast<int>(SoundChannel::Boss)]);
}
void SoundManager::StopBossSound()
{
	FMOD_Channel_Stop(soundChannel[static_cast<int>(SoundChannel::Boss)]);
}

void SoundManager::PlaySelectSound(SelectSound selectSound)
{
	FMOD_System_Update(soundSystem);
	FMOD_System_PlaySound(soundSystem, selectSoundList[static_cast<int>(selectSound)], 0, false, NULL);
}