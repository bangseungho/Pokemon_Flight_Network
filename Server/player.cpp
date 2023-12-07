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

// ��ų �Ŵ��� ����
void Player::Init()
{
	skillManager = new SkillManager(shared_from_this());
}

void Player::SetBulletsPlayer(shared_ptr<Player> player)
{
	bullets->SetPlayer(shared_from_this());
	subBullets->SetPlayer(shared_from_this());
}

// �÷��̾ �׾��� ��� �� �ָ� ������ ���� ó��
void Player::Death()
{
	GameObject::SetPos({ WINDOWSIZE_X / 2, 10000 });
	playerData.isDeath = true;
	playerData.hp = 0;
}

// ź�� �߻� �Լ�
void Player::Shot()
{
	// ź���� �������� �÷��̾��� �����Ϳ� ���� �޶�����.
	const RECT rectBody = GetRectBody();
	BulletData bulletData;
	bulletData.bulletType = playerData.type;
	bulletData.damage = playerData.damage;
	bulletData.speed = playerData.bulletSpeed;

	// ���� ���ϸ��� ź���� �����Ͽ� BulletController�� �߰��Ѵ�.
	POINT bulletPos = { 0, };
	bulletPos.y = rectBody.top;
	bulletPos.x = rectBody.left - 10;
	bullets->CreateBullet(bulletPos, bulletData, Dir::Up);
	bulletPos.x = rectBody.right + 10;
	bullets->CreateBullet(bulletPos, bulletData, Dir::Up);

	// ���� ���ϸ��� ź���� �����Ͽ� BulletController�� �߰��Ѵ�.
	bulletData.bulletType = playerData.subType;
	bulletData.damage = playerData.subDamage;
	bulletPos.x = rectBody.left + ((rectBody.right - rectBody.left) / 2);
	subBullets->CreateBullet(bulletPos, bulletData, Dir::Up);

	// ��ų�� ���� ��ų �Ŵ����� �̿��ؼ� ��ų ������Ʈ
	skillManager->UseSkill();
}

// �÷��̾��� �⺻ ���ݿ� ��Ÿ���� �ִ� �Լ��̴�
void Player::CheckShot()
{
	if (playerData.isDeath == true)
	{
		return;
	}

	// Ÿ�̸Ӹ� ���ؼ� crntShotDelay ���� ���̸� ���� 0���� �۾����� �� �� ź���� �߻��ϵ��� �ϰ� �ٽ� �����Ѵ�.
	playerData.crntShotDelay -= ELAPSE_BATTLE_INVALIDATE;
	if (IsClearShotDelay() == true)
	{
		Shot();
		ResetShotDelay();
	}
}

// ���� ���ϸ��� ź�� ���� �Լ�
void Player::CreateSubBullet(const POINT& center, const BulletData& data, Vector2 unitVector, bool isRotateImg, bool isSkillBullet)
{
	subBullets->CreateBullet(center, data, unitVector, isRotateImg, isSkillBullet);
}

// �÷��̾ ��� �ִٸ� ��ų ���
void Player::ActiveSkill(Skill skill)
{
	if (playerData.isDeath == true)
	{
		return;
	}

	skillManager->ActiveSkill(skill);
}

// �÷��̾��� �Ѿ� ������Ʈ
void Player::MoveBullets()
{
	bullets->Update();
	subBullets->Update();
}

// ��ų ���� � ��ų�� ����ߴ��� ���� ����
bool Player::IsUsingSkill() const
{
	return skillManager->IsUsingSkill();
}