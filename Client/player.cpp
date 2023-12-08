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
#include "Network.h"

extern GUIManager* gui;
extern EffectManager* effects;
extern SceneManager* sceneManager;
extern SoundManager* soundManager;

extern Battle battle;

// 플레이어의 타입에 따라서 플레이어 데이터를 설정하고 이미지도 로드한다. 탄막과 서브 탄막도 생성한다.
Player::Player(Type type, Type subType)
{
	constexpr int damagePerSec = (1000 / ELAPSE_BATTLE_ANIMATION);

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

// 스킬 매니저 생성
void Player::Init()
{
	skillManager = new SkillManager();
}

// 플레이어 렌더링
void Player::Paint(HDC hdc)
{
	// 탄막 렌더링
	bullets->Paint(hdc);
	subBullets->Paint(hdc);

	// 죽은 상태라면 즉시 리턴
	if (playerData.isDeath == true)
	{
		return;
	}

	const RECT rectImage =ISprite::GetRectImage(GetImage(), frame);
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

// 스킬 매니저를 통해서 스킬 렌더링
void Player::PaintSkill(HDC hdc)
{
	if (playerData.isDeath == true)
	{
		return;
	}

	skillManager->Paint(hdc);
}

// 플레이어가 죽었을 경우 저 멀리 보내고 사운드 처리
void Player::Death()
{
	GameObject::SetPos({ WINDOWSIZE_X / 2, 10000 });
	playerData.isDeath = true;
	playerData.hp = 0;

	soundManager->StopEffectSound();
	soundManager->PlayEffectSound(EffectSound::Loss);
	soundManager->StopBGMSound();
}

// [IControllable의 가상함수] 플레이어 데이터의 속도에 따라서 이동해야할 위치를 설정(바로 이동하지 않는다.)
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
	
	BattleData sendData{MY_INDEX, GetPosCenter(), GetRectBody(), gui->IsFieldEnd() };
	GET_SINGLE(Network)->SendDataAndType(sendData);
}

// 인자에 따라서 플레이어의 방향을 설정한다.
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

// [IControllable의 가상함수] 플레이어가 죽지 않았고 alpha 값이 0이라면 플레이어를 업데이트한다.
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

// 실제 플레이어의 이동을 수행한다.
void Player::Move(const HWND& hWnd, int timerID)
{
	// 파트너 이동 관련 코드
	auto& members = sceneManager->GetMemberMap();
	for (const auto& member : GET_MEMBER_MAP) {
		if (member.first == MY_INDEX)
			continue;

		if (members.find(member.first) != members.end())
			members[member.first]->SetPos(member.second.mBattleData.PosCenter);
	}


	if (playerData.isCanGo == false)
		return;

	Vector2 posCenter = GetPosCenter();

	// 선형 보간을 통해 부드럽게 다음 이동할 위치의 좌표를 얻는다.
	Vector2 posNext = Vector2::Lerp(posCenter, posDest, alpha);

	// 윈도우 화면을 다음 이동할 위치의 좌표가 넘는지 검사한다.
	CheckCollideWindow(posNext);

	// 넘지 않았다면 다음 좌표로 플레이어의 중심 좌표를 이동한다
	SetPos(posNext);
	posCenter = GetPosCenter();
	
	// 벡터의 뺄셈 계산을 통해서 현재 어느 방향으로 가는지를 구할 수 있다.
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

	// direction이 inputDir과 같은 경우에만 Dir::Empty를 넣어준다.
	direction = direction - inputDir;
}

// 플레이어가 윈도우를 넘어갔는지 검사한다.
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

	// 넘어갔다면 다음 이동할 위치 좌표에 나간 만큼 +를 하여 다시 돌아가게 만든다.
	pos.x += corrValue.x;
	pos.y += corrValue.y;
}

// 플레이어의 애니메이션 함수이다.
void Player::Animate(const HWND& hWnd)
{
	// 플레이어가 죽었다면
	if (playerData.isDeath == true)
	{
		if (--deathFrame == 0)
		{
			// 씬 매니저를 통해 다음 씬으로 이동(현재 씬이 배틀이므로 페이즈로 넘어간다.)
			sceneManager->StartLoading(hWnd);
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

	// 현재 액션 값을 받아와서 애니메이션 한다.
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

	// 스킬 매니저도 애니메이션한다.
	skillManager->Animate();
}

// 2023/12/08 : 네트워크 탄막 발사 함수 
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
	//skillManager->UseSkill();
}

void Player::BulletPop(size_t& bulletIndex)
{
	bullets->Pop(bulletIndex);
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
	//if (IsClearShotDelay() == true)
	//{
		Shot();
		//ResetShotDelay();
	//}
}

// 서브 포켓몬의 탄막 생성 함수
void Player::CreateSubBullet(const POINT& center, const BulletData& data, Vector2 unitVector, bool isRotateImg, bool isSkillBullet)
{
	subBullets->CreateBullet(center, data, unitVector, isRotateImg, isSkillBullet);
}

// 플레이어 피격 함수로 이펙트는 생성되어야 한다면 이펙트 매니저의 자료구조에 해당 이펙트를 추가한다.
void Player::Hit(float damage, Type hitType, POINT effectPoint, uint8 memberIndex)
{
	if (playerData.isDeath == true)
	{
		return;
	}
	else if (effectPoint.x == -1) // 만약 총알을 맞은 부분이 -1이라면 랜덤한 곳에서 이펙트 생성
	{
		effectPoint = GetPosCenter();
		GetRandEffectPoint(effectPoint);
	}
	effects->CreateHitEffect(effectPoint, hitType); // 피격 효과를 이펙트 매니저에 추가한다.

	if (MY_INDEX == memberIndex) {
		gui->DisplayHurtFrame(hitType); // 피격시 화면에 생성되는 프레임
		battle.ShakeMap(); // 맵 흔들기

		if (playerData.isInvincible == true)
		{
			return;
		}

		damage = CalculateDamage(damage, playerData.type, hitType); // 데미지 계산
		if ((playerData.hp -= damage) <= 0) // 계산된 데미지를 통해서 플레이어 hp 감소시 0보다 작다면 폭발 효과 이펙트 매니저에 추가하고 플레이어 사망 함수 호출
		{
			effects->CreateExplosionEffect(GetPosCenter(), playerData.type);
			Player::Death();
		}
	}

	switch (hitType) // 맞은 탄막의 속성에 따라 사운드 재생
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
	bullets->Move();
	subBullets->Move();
}

// 스킬 사용시 어떤 스킬을 사용했는지 변수 설정
bool Player::IsUsingSkill() const
{
	return skillManager->IsUsingSkill();
}