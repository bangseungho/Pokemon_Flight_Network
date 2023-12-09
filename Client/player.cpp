#include "stdafx.h"
#include "player.h"
#include "bullet.h"
#include "timer.h"
#include "skill.h"
#include "effect.h"
#include "interface.h"
#include "scene.h"
#include "sound.h"
#include "battle.h"
#include "enemy.h"
#include "Network.h"

extern GUIManager* gui;
extern EffectManager* effects;
extern SceneManager* sceneManager;
extern SoundManager* soundManager;
extern EnemyController* enemies;

extern Battle battle;

// �÷��̾��� Ÿ�Կ� ���� �÷��̾� �����͸� �����ϰ� �̹����� �ε��Ѵ�. ź���� ���� ź���� �����Ѵ�.
Player::Player(Type type, Type subType, uint8 id)
{
	constexpr int damagePerSec = (1000 / ELAPSE_BATTLE_ANIMATION);
	playerData.id = id;

	ObjectImage bulletImage;
	ObjectImage subBulletImage;

	playerData.type = type;
	playerData.subType = subType;
	switch (playerData.type)
	{
	case Type::Elec:
		pokemon = Pokemon::Thunder;
		img_mainPokemon.Load(_T("images\\battle\\sprite_thunder.png"), { 53, 48 }, { 19, 10 }, { 17,24 });
		bulletImage.Load(_T("images\\battle\\bullet_elec_main.png"), { 5,16 });

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
		img_mainPokemon.Load(_T("images\\battle\\sprite_articuno.png"), { 69, 69 }, { 29, 28 }, { 13,23 });
		img_mainPokemon.ScaleImage(1.2f, 1.2f);
		bulletImage.Load(_T("images\\battle\\bullet_ice.png"), { 7,15 });
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
		img_mainPokemon.Load(_T("images\\battle\\sprite_moltres.png"), { 83, 75 }, { 35, 25 }, { 15,28 });
		bulletImage.Load(_T("images\\battle\\bullet_fire.png"), { 11,16 });
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
		img_subPokemon.Load(L"images\\battle\\sub_pikachu.png", { 23,25 });
		subBulletImage.Load(_T("images\\battle\\bullet_elec.png"), { 11,30 });
		subBulletImage.ScaleImage(0.7f, 0.7f);
		playerData.subDamage = 1.0f;
		break;
	case Type::Water:
		subPokemon = SubPokemon::Squirtle;
		img_subPokemon.Load(L"images\\battle\\sub_squirtle.png", { 17,24 });
		subBulletImage.Load(_T("images\\battle\\bullet_water.png"), { 8,24 });
		subBulletImage.ScaleImage(0.8f, 0.7f);
		playerData.subDamage = 1.1f;
		break;
	case Type::Fire:
		subPokemon = SubPokemon::Charmander;
		img_subPokemon.Load(L"images\\battle\\sub_charmander.png", { 18,23 });
		subBulletImage.Load(_T("images\\battle\\bullet_flame.png"), { 8,16 });
		subBulletImage.ScaleImage(0.7f, 0.7f);
		playerData.subDamage = 1.2f;
		break;
	default:
		assert(0);
		break;
	}

	if (playerData.id != MY_INDEX) {
		bulletImage.SetAlpha(120);
		subBulletImage.SetAlpha(120);
		img_mainPokemon.SetAlpha(120);
		img_subPokemon.SetAlpha(120);
	}

	GameObject::Init(img_mainPokemon, { 250, 500 });
	bullets = new PlayerBullet(bulletImage);
	subBullets = new PlayerBullet(subBulletImage);
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
	skillManager = new SkillManager(this);
}

// �÷��̾� ������
void Player::Paint(HDC hdc)
{
	// ź�� ������
	bullets->Paint(hdc);
	subBullets->Paint(hdc);

	// ���� ���¶�� ��� ����
	if (playerData.isDeath == true)
	{
		return;
	}

	const RECT rectImage = ISprite::GetRectImage(GetImage(), frame);
	GameObject::Paint(hdc, &rectImage);

	const ObjectImage& image = GetImage();
	float scaleX, scaleY;
	image.GetScale(scaleX, scaleY);

	RECT rectDest = GetRectBody();
	switch (pokemon)
	{
	case Pokemon::Moltres:
		rectDest.top += 7 * scaleY;
		rectDest.bottom = rectDest.top + (22 * scaleY);
		break;
	case Pokemon::Articuno:
		rectDest.top += 3 * scaleY;
		rectDest.bottom = rectDest.top + (22 * scaleY);
		break;
	case Pokemon::Thunder:
		rectDest.top += 5 * scaleY;
		rectDest.bottom = rectDest.top + (20 * scaleY);
		break;
	default:
		assert(0);
		break;
	}

	img_subPokemon.Paint(rectDest, hdc);
}

// ��ų �Ŵ����� ���ؼ� ��ų ������
void Player::PaintSkill(HDC hdc)
{
	if (playerData.isDeath == true)
	{
		return;
	}

	skillManager->Paint(hdc);
}

// �÷��̾ �׾��� ��� �� �ָ� ������ ���� ó��
void Player::Death()
{
	GameObject::SetPos({ WINDOWSIZE_X / 2, 10000 });
	playerData.isDeath = true;
	playerData.hp = 0;

	soundManager->StopEffectSound();
	soundManager->PlayEffectSound(EffectSound::Loss);
	soundManager->StopBGMSound();

	BattleData sendData{ MY_INDEX, GetPosCenter(), GetRectBody(), gui->IsFieldEnd(), IsDeath(), enemies->IsEmenyClear() };
	GET_SINGLE(Network)->SendDataAndType(sendData);
}

// [IControllable�� �����Լ�] �÷��̾� �������� �ӵ��� ���� �̵��ؾ��� ��ġ�� ����(�ٷ� �̵����� �ʴ´�.)
void Player::SetPosDest()
{
	const int movementAmount = playerData.speed * 2;
	switch (direction)
	{
	case Dir::Left:
		vectorMove.x = -movementAmount;
		break;
	case Dir::Right:
		vectorMove.x = movementAmount;
		break;
	case Dir::Up:
		vectorMove.y = -movementAmount;
		break;
	case Dir::Down:
		vectorMove.y = movementAmount;
		break;
	case Dir::LD:
		vectorMove.x = -movementAmount;
		vectorMove.y = movementAmount;
		break;
	case Dir::LU:
		vectorMove.x = -movementAmount;
		vectorMove.y = -movementAmount;
		break;
	case Dir::RD:
		vectorMove.x = movementAmount;
		vectorMove.y = movementAmount;
		break;
	case Dir::RU:
		vectorMove.x = movementAmount;
		vectorMove.y = -movementAmount;
		break;
	default:
		break;
	}

	posDest = Vector2::GetDest(GetPosCenter(), vectorMove);
	
	BattleData sendData{ MY_INDEX, GetPosCenter(), GetRectBody(), gui->IsFieldEnd(), IsDeath(), enemies->IsEmenyClear() };
	GET_SINGLE(Network)->SendDataAndType(sendData);
}

// ���ڿ� ���� �÷��̾��� ������ �����Ѵ�.
void Player::SetDirection(Dir inputDir)
{
	if (direction == inputDir || direction == Dir::Empty)
	{
		direction = inputDir;
		return;
	}

	switch (direction)
	{
	case Dir::Left:
		if (inputDir == Dir::Up)		direction = Dir::LU;
		else if (inputDir == Dir::Down)	direction = Dir::LD;
		break;
	case Dir::Right:
		if (inputDir == Dir::Up)		direction = Dir::RU;
		else if (inputDir == Dir::Down)	direction = Dir::RD;
		break;
	case Dir::Up:
		if (inputDir == Dir::Left)		direction = Dir::LU;
		else if (inputDir == Dir::Right)direction = Dir::RU;
		break;
	case Dir::Down:
		if (inputDir == Dir::Left)		direction = Dir::LD;
		else if (inputDir == Dir::Right)direction = Dir::RD;
		break;
	default:
		return;
	}
	StopMove();
}

// [IControllable�� �����Լ�] �÷��̾ ���� �ʾҰ� alpha ���� 0�̶�� �÷��̾ ������Ʈ�Ѵ�.
void Player::SetMove(const HWND& hWnd, int timerID, int elpase, const TIMERPROC& timerProc)
{
	if (playerData.isDeath == true)
	{
		return;
	}
	else if (IsMove() == true && alpha > 0)
	{
		return;
	}

	SetPosDest();

	if (IsMove() == false && alpha == 0)
	{
		playerData.isCanGo = true;
	}
	else if (alpha > 0.5f)
	{
		alpha = 0.5f;
	}
	StartMove();
}

// ���� �÷��̾��� �̵��� �����Ѵ�.
void Player::Move(const HWND& hWnd, int timerID)
{
	// ��Ʈ�� �̵� ���� �ڵ�
	auto& members = sceneManager->GetMemberMap();
	for (const auto& member : GET_MEMBER_MAP) {
		if (member.first == MY_INDEX)
			continue;

		if (members.find(member.first) != members.end()) {
			members[member.first]->SetPos(member.second.mBattleData.PosCenter);
			members[member.first]->SetDeath(member.second.mBattleData.IsDeath);
		}
	}

	if (playerData.isDeath == true)
		return;

	if (playerData.isCanGo == false)
		return;

	Vector2 posCenter = GetPosCenter();

	// ���� ������ ���� �ε巴�� ���� �̵��� ��ġ�� ��ǥ�� ��´�.
	Vector2 posNext = Vector2::Lerp(posCenter, posDest, alpha);

	// ������ ȭ���� ���� �̵��� ��ġ�� ��ǥ�� �Ѵ��� �˻��Ѵ�.
	CheckCollideWindow(posNext);

	// ���� �ʾҴٸ� ���� ��ǥ�� �÷��̾��� �߽� ��ǥ�� �̵��Ѵ�
	SetPos(posNext);
	posCenter = GetPosCenter();

	// ������ ���� ����� ���ؼ� ���� ��� �������� �������� ���� �� �ִ�.
	vectorMove = posDest - posCenter;

	alpha += 0.1f;
	if (alpha > 0.5f)
	{
		if (direction != Dir::Empty)
		{
			SetPosDest();

			alpha = 0.5f;
		}
		else if (alpha > 1)
		{
			vectorMove = { 0.0f, };
			StopMove();
			alpha = 0;
			playerData.isCanGo = true;
		}
	}
}

void Player::Stop(Dir inputDir)
{
	switch (direction)
	{
	case Dir::Left:
		if (inputDir != Dir::Left) return;
		break;
	case Dir::Right:
		if (inputDir != Dir::Right) return;
		break;
	case Dir::Up:
		if (inputDir != Dir::Up) return;
		break;
	case Dir::Down:
		if (inputDir != Dir::Down) return;
		break;
	case Dir::LU:
		if (inputDir != Dir::Left && inputDir != Dir::Up) return;
		break;
	case Dir::LD:
		if (inputDir != Dir::Left && inputDir != Dir::Down) return;
		break;
	case Dir::RU:
		if (inputDir != Dir::Right && inputDir != Dir::Up) return;
		break;
	case Dir::RD:
		if (inputDir != Dir::Right && inputDir != Dir::Down) return;
		break;
	default:
		break;
	}

	// direction�� inputDir�� ���� ��쿡�� Dir::Empty�� �־��ش�.
	direction = direction - inputDir;
}

// �÷��̾ �����츦 �Ѿ���� �˻��Ѵ�.
void Player::CheckCollideWindow(Vector2& pos) const
{
	const RECT rectDisplay = sceneManager->GetRectDisplay();
	const RECT rectBody = GetRectBody(pos);

	POINT corrValue = { 0, };
	if (rectBody.left < 0)
	{
		corrValue.x = -rectBody.left;
	}
	else if (rectBody.right > rectDisplay.right)
	{
		corrValue.x = rectDisplay.right - rectBody.right;
	}
	if (rectBody.top < 0)
	{
		corrValue.y = -rectBody.top;
	}
	else if (rectBody.bottom > rectDisplay.bottom)
	{
		corrValue.y = rectDisplay.bottom - rectBody.bottom;
	}

	// �Ѿ�ٸ� ���� �̵��� ��ġ ��ǥ�� ���� ��ŭ +�� �Ͽ� �ٽ� ���ư��� �����.
	pos.x += corrValue.x;
	pos.y += corrValue.y;
}

// �÷��̾��� �ִϸ��̼� �Լ��̴�.
void Player::Animate(const HWND& hWnd)
{
	// �÷��̾ �׾��ٸ�
	if (playerData.isDeath == true)
	{
		if (--deathFrame == 0)
		{
			if (MY_INDEX == playerData.id) {
				NetworkGameData sendData{ true };
				GET_SINGLE(Network)->SendDataAndType(sendData);
			}
		}
		return;
	}
	else if (isRevFrame == true)
	{
		--frame;
	}
	else
	{
		++frame;
	}

	// ���� �׼� ���� �޾ƿͼ� �ִϸ��̼� �Ѵ�.
	switch (GetAction())
	{
	case Action::Idle:
		if (frame > 2)
		{
			isRevFrame = true;
			--frame;
		}
		else if (frame < 0)
		{
			isRevFrame = false;
			++frame;
		}
		break;
	default:
		assert(0);
		break;
	}

	// ��ų �Ŵ����� �ִϸ��̼��Ѵ�.
	skillManager->Animate();
}

// 2023/12/08 : ��Ʈ��ũ ź�� �߻� �Լ� 
void Player::Shot(NetworkBulletData& recvData)
{
	//BulletData bulletData;
	//bulletData.bulletType = playerData.type;
	//bulletData.damage = playerData.damage;
	//bulletData.speed = playerData.bulletSpeed;

	//bullets->CreateBullet(recvData.StartPos, bulletData, Dir::Up);
}

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

void Player::BulletPop(size_t& bulletIndex)
{
	bullets->Pop(bulletIndex);
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
	//if (IsClearShotDelay() == true)
	//{
	Shot();
	//ResetShotDelay();
//}
}

// ���� ���ϸ��� ź�� ���� �Լ�
void Player::CreateSubBullet(const POINT& center, const BulletData& data, Vector2 unitVector, bool isRotateImg, bool isSkillBullet)
{
	subBullets->CreateBullet(center, data, unitVector, isRotateImg, isSkillBullet);
}

// �÷��̾� �ǰ� �Լ��� ����Ʈ�� �����Ǿ�� �Ѵٸ� ����Ʈ �Ŵ����� �ڷᱸ���� �ش� ����Ʈ�� �߰��Ѵ�.
void Player::Hit(float damage, Type hitType, uint8 memberIndex, POINT effectPoint)
{
	if (playerData.isDeath == true)
	{
		return;
	}
	else if (effectPoint.x == -1) // ���� �Ѿ��� ���� �κ��� -1�̶�� ������ ������ ����Ʈ ����
	{
		effectPoint = GetPosCenter();
		GetRandEffectPoint(effectPoint);
	}
	effects->CreateHitEffect(effectPoint, hitType); // �ǰ� ȿ���� ����Ʈ �Ŵ����� �߰��Ѵ�.

	if (MY_INDEX == memberIndex) {
		gui->DisplayHurtFrame(hitType); // �ǰݽ� ȭ�鿡 �����Ǵ� ������
		battle.ShakeMap(); // �� ����

		if (playerData.isInvincible == true)
		{
			return;
		}

		damage = CalculateDamage(damage, playerData.type, hitType); // ������ ���
		if ((playerData.hp -= damage) <= 0) // ���� �������� ���ؼ� �÷��̾� hp ���ҽ� 0���� �۴ٸ� ���� ȿ�� ����Ʈ �Ŵ����� �߰��ϰ� �÷��̾� ��� �Լ� ȣ��
		{
			effects->CreateExplosionEffect(GetPosCenter(), playerData.type);
			Player::Death();
		}
	}

	switch (hitType) // ���� ź���� �Ӽ��� ���� ���� ���
	{
	case Type::Elec:
		soundManager->PlayHitSound(HitSound::Elec);
		break;
	case Type::Fire:
		soundManager->PlayHitSound(HitSound::Fire);
		break;
	case Type::Water:
		soundManager->PlayHitSound(HitSound::Water);
		break;
	case Type::Dark:
		soundManager->PlayHitSound(HitSound::Dark);
		break;
	}
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
	bullets->Move();
	subBullets->Move();
}

// ��ų ���� � ��ų�� ����ߴ��� ���� ����
bool Player::IsUsingSkill() const
{
	return skillManager->IsUsingSkill();
}

bool Player::IsIdentity() const
{
	return skillManager->IsIdentity();
}

bool Player::ReduceMP(float amount, Skill skill)
{
	if (skill == Skill::Identity && skillManager->IsIdentity() == true) // ���� �ñر� ������̸� ����(�������� ��� ���ϵ���)
	{
		return false;
	}
	else if (skill != Skill::Identity && IsUsingSkill() == true) // ���� W, E ��ų �� �ϳ��� ������̸� ����(�������� ��� ���ϵ���)
	{
		return false;
	}

	if ((playerData.mp - amount) < 0)
	{
		return false;
	}
	playerData.mp -= amount;
	return true;
}
