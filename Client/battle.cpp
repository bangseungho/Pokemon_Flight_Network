#include "stdafx.h"
#include "battle.h"
#include "timer.h"
#include "sound.h"

extern SoundManager* soundManager;

// ��Ʋ ȭ�鿡 �ʿ��� �̹��� ��� �ε�
Battle::Battle()
{
	_water.Load(L"images\\battle\\Water_spstage.bmp");
	_fire.Load(L"images\\battle\\Fire_spstage.bmp");
	_electronic.Load(L"images\\battle\\Electronic_spstage.bmp");
	_dark.Load(L"images\\battle\\Dark_spstage.bmp");
}

void Battle::Init()
{
	_rectImage = { 0, 0, 500, 1500 };
	_rectDraw = { 0, -750, 500, 750 };
	_rectDraw2 = { 0, -2250, 500, -750 };
	_shakeCount = 0;
	_shakeWindow = 0;
	_mapSpeed = 5;

	soundManager->StopBGMSound();
	soundManager->PlayBGMSound(BGMSound::Battle, 1.0f, true);
	soundManager->PlayEffectSound(EffectSound::Shot, 0.5f, true);
}

void Battle::Update(float elapsedStime)
{
	static float invalidateAccTime = 0.f;
	static float animationAccTime = 0.f;
	static float animationBossAccTime = 0.f;
	static float effectAccTime = 0.f;
	invalidateAccTime += elapsedStime;
	animationAccTime += elapsedStime;
	animationBossAccTime += elapsedStime;
	effectAccTime += elapsedStime;

	if (invalidateAccTime > 0.016f) {
		T_Battle_MovePlayer();
		T_Battle_Invalidate();
		T_Battle_MapMove();
		T_Battle_GUI();
		invalidateAccTime = 0.f;
	}

	if (animationAccTime > 0.05f) {
		T_Battle_Animate();
		animationAccTime = 0.f;
	}

	if (animationBossAccTime > 0.16f) {
		T_Battle_AnimateBoss();
		animationBossAccTime = 0.f;
	}

	if (effectAccTime > 0.05f) {
		T_Battle_Effect();
		effectAccTime = 0.f;
	}
}

// ��Ʋ ȭ�� ������ �� ���� �̹����� ���η� �̾� �ٿ� �ϳ��� �̹����� �� ��ũ���� ���� ��� ��ũ�ѵǸ� �ٽ� �ٸ� �̹��� ���� �ö�
void Battle::Paint(HDC hdc, const RECT& rectWindow, StageElement _select_index)
{
	switch (_select_index)
	{
	case StageElement::Water:
		_water.Draw(hdc, _rectDraw, _rectImage);
		_water.Draw(hdc, _rectDraw2, _rectImage);
		break;
	case StageElement::Fire:
		_fire.Draw(hdc, _rectDraw, _rectImage);
		_fire.Draw(hdc, _rectDraw2, _rectImage);
		break;
	case StageElement::Elec:
		_electronic.Draw(hdc, _rectDraw, _rectImage);
		_electronic.Draw(hdc, _rectDraw2, _rectImage);
		break;
	case StageElement::Dark:
		_dark.Draw(hdc, _rectDraw, _rectImage);
		_dark.Draw(hdc, _rectDraw2, _rectImage);
		break;
	}

	if (_rectDraw.bottom > 3000)
	{
		_rectDraw.top = -1490;
		_rectDraw.bottom = 10;
	}
	if (_rectDraw2.bottom > 3000)
	{
		_rectDraw2.top = -1490;
		_rectDraw2.bottom = 10;
	}

	// ȭ�� ����
	if (isShake == true)
	{
		constexpr int shakeAmount = 5;
		if (_shakeWindow % 5 == 0 && _rectDraw.left == 0)
		{
			_rectDraw.left -= 5;
			_rectDraw.right -= 5;
			_rectDraw2.left -= 5;
			_rectDraw2.right -= 5;
		}
		if (_shakeWindow % 5 == 4 && _rectDraw.left == -5)
		{
			_rectDraw.left += 5;
			_rectDraw.right += 5;
			_rectDraw2.left += 5;
			_rectDraw2.right += 5;
		}

		++_shakeWindow;
		if (--_shakeCount <= 0)
		{
			_shakeWindow = 0;
			isShake = false;
		}
	}
	else
	{
		_rectDraw.left = 0;
		_rectDraw.right = 500;
		_rectDraw2.left = 0;
		_rectDraw2.right = 500;
	}
}