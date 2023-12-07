#include "..\Utils.h"
#include "player.h"
#include "enemy.h"
#include "bullet.h"
#include "timer.h"
#include "skill.h"
#include "effect.h"
#include "battle.h"

Player::Player(NetworkPlayerData* recvData, shared_ptr<class EnemyController> enemyController)
{
	if (recvData != nullptr)
		mRecvData = recvData;

	constexpr int damagePerSec = (1000 / ELAPSE_BATTLE_ANIMATION);

	ObjectImage bulletImage;
	ObjectImage subBulletImage;

	playerData.type = recvData->mSceneData.AirPokemon;
	playerData.subType = recvData->mSceneData.LandPokemon;
	switch (playerData.type)
	{
	case Type::Elec:
		pokemon = Pokemon::Thunder;
		img_mainPokemon.Load(_T("images\\battle\\sprite_thunder.png"), { 53, 48 }, { 19, 10 }, { 17,24 });
		bulletImage.Load(_T("..\\Client\\images\\battle\\bullet_elec_main.png"), { 5,16 });

		playerData.maxhp = 40;
		playerData.maxmp = 80;
		playerData.mp = 100;
		playerData.speed = 4;
		playerData.damage = 1.0f;
		playerData.damage_Q = 10.5f / damagePerSec;
		playerData.bulletSpeed = 8;
		playerData.shotDelay = 90;
		break;
	case Type::Water:
		pokemon = Pokemon::Articuno;
		img_mainPokemon.Load(_T("..\\Client\\images\\battle\\sprite_articuno.png"), { 69, 69 }, { 29, 28 }, { 13,23 });
		img_mainPokemon.ScaleImage(1.2f, 1.2f);
		bulletImage.Load(_T("..\\Client\\images\\battle\\bullet_ice.png"), { 7,15 });
		bulletImage.ScaleImage(0.9f, 0.9f);

		playerData.maxhp = 65;
		playerData.maxmp = 120;
		playerData.mp = 120;
		playerData.speed = 2.5f;
		playerData.damage = 1.25f;
		playerData.damage_Q = 4.0f / damagePerSec;
		playerData.bulletSpeed = 6;
		playerData.shotDelay = 110;
		break;
	case Type::Fire:
		pokemon = Pokemon::Moltres;
		img_mainPokemon.Load(_T("..\\Client\\images\\battle\\sprite_moltres.png"), { 83, 75 }, { 35, 25 }, { 15,28 });
		bulletImage.Load(_T("..\\Client\\images\\battle\\bullet_fire.png"), { 11,16 });
		bulletImage.ScaleImage(0.9f, 0.9f);

		playerData.maxhp = 50;
		playerData.maxmp = 100;
		playerData.mp = 100;
		playerData.speed = 3;
		playerData.damage = 1.35f;
		playerData.damage_Q = 12.5f / damagePerSec;
		playerData.bulletSpeed = 7;
		playerData.shotDelay = 100;
		break;
	default:
		assert(0);
		break;
	}
	playerData.hp = playerData.maxhp;

	switch (playerData.subType)
	{
	case Type::Elec:
		subPokemon = SubPokemon::Pikachu;
		img_subPokemon.Load(L"..\\Client\\images\\battle\\sub_pikachu.png", { 23,25 });
		subBulletImage.Load(_T("..\\Client\\images\\battle\\bullet_elec.png"), { 11,30 });
		subBulletImage.ScaleImage(0.7f, 0.7f);
		playerData.subDamage = 1.0f;
		break;
	case Type::Water:
		subPokemon = SubPokemon::Squirtle;
		img_subPokemon.Load(L"..\\Client\\images\\battle\\sub_squirtle.png", { 17,24 });
		subBulletImage.Load(_T("..\\Client\\images\\battle\\bullet_water.png"), { 8,24 });
		subBulletImage.ScaleImage(0.8f, 0.7f);
		playerData.subDamage = 1.1f;
		break;
	case Type::Fire:
		subPokemon = SubPokemon::Charmander;
		img_subPokemon.Load(L"..\\Client\\images\\battle\\sub_charmander.png", { 18,23 });
		subBulletImage.Load(_T("..\\Client\\images\\battle\\bullet_flame.png"), { 8,16 });
		subBulletImage.ScaleImage(0.7f, 0.7f);
		playerData.subDamage = 1.2f;
		break;
	default:
		assert(0);
		break;
	}

	GameObject::Init(img_mainPokemon, { 250, 500 });

	bullets = new PlayerBullet(bulletImage, enemyController);
	subBullets = new PlayerBullet(subBulletImage, enemyController);
}

Player::~Player()
{
	delete bullets;
	delete subBullets;
	delete skillManager;
}

// 스킬 매니저 생성
void Player::Init()
{
	skillManager = new SkillManager(shared_from_this());
}

void Player::SetBulletsPlayer(shared_ptr<Player> player)
{
	bullets->SetPlayer(shared_from_this());
	subBullets->SetPlayer(shared_from_this());
}

// 플레이어가 죽었을 경우 저 멀리 보내고 사운드 처리
void Player::Death()
{
	GameObject::SetPos({ WINDOWSIZE_X / 2, 10000 });
	playerData.isDeath = true;
	playerData.hp = 0;
}

// 탄막 발사 함수
void Player::Shot()
{
	// 탄막의 데미지는 플레이어의 데이터에 따라서 달라진다.
	const RECT rectBody = GetRectBody();
	BulletData bulletData;
	bulletData.bulletType = playerData.type;
	bulletData.damage = playerData.damage;
	bulletData.speed = playerData.bulletSpeed;

	// 메인 포켓몬의 탄막을 생성하여 BulletController에 추가한다.
	POINT bulletPos = { 0, };
	bulletPos.y = rectBody.top;
	bulletPos.x = rectBody.left - 10;
	bullets->CreateBullet(bulletPos, bulletData, Dir::Up);
	bulletPos.x = rectBody.right + 10;
	bullets->CreateBullet(bulletPos, bulletData, Dir::Up);

	// 서브 포켓몬의 탄막을 생성하여 BulletController에 추가한다.
	bulletData.bulletType = playerData.subType;
	bulletData.damage = playerData.subDamage;
	bulletPos.x = rectBody.left + ((rectBody.right - rectBody.left) / 2);
	subBullets->CreateBullet(bulletPos, bulletData, Dir::Up);

	// 스킬도 사용시 스킬 매니저를 이용해서 스킬 업데이트
	skillManager->UseSkill();
}

// 플레이어의 기본 공격에 쿨타임을 주는 함수이다
void Player::CheckShot()
{
	if (playerData.isDeath == true)
	{
		return;
	}

	// 타이머를 통해서 crntShotDelay 값을 줄이며 만약 0보다 작아지면 그 때 탄막을 발사하도록 하고 다시 리셋한다.
	playerData.crntShotDelay -= ELAPSE_BATTLE_INVALIDATE;
	if (IsClearShotDelay() == true)
	{
		Shot();
		ResetShotDelay();
	}
}

// 서브 포켓몬의 탄막 생성 함수
void Player::CreateSubBullet(const POINT& center, const BulletData& data, Vector2 unitVector, bool isRotateImg, bool isSkillBullet)
{
	subBullets->CreateBullet(center, data, unitVector, isRotateImg, isSkillBullet);
}

// 플레이어가 살아 있다면 스킬 재생
void Player::ActiveSkill(Skill skill)
{
	if (playerData.isDeath == true)
	{
		return;
	}

	skillManager->ActiveSkill(skill);
}

// 플레이어의 총알 업데이트
void Player::MoveBullets()
{
	bullets->Update();
	subBullets->Update();
}

// 스킬 사용시 어떤 스킬을 사용했는지 변수 설정
bool Player::IsUsingSkill() const
{
	return skillManager->IsUsingSkill();
}