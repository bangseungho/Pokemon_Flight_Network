#include "stdafx.h"
#include "stage.h"
#include "intro.h"
#include "scene.h"
#include "sound.h"
#include "Network.h"
#include "interface.h"
#include "timer.h"

extern SceneManager* sceneManager;
extern SoundManager* soundManager;
extern GameData gameData;
extern Intro intro;

// ���������� �ʿ��� �̹��� ��� �ε�
Stage::Stage()
{
	mFingerCount = 0;
	target = new Target();
	_water.Load(L"images\\stage\\Water_phase.bmp");
	_fire.Load(L"images\\stage\\Fire_phase.bmp");
	_electronic.Load(L"images\\stage\\Electronic_phase.bmp");
	_dark.Load(L"images\\stage\\Dark_phase.bmp");
	target->_img.Load(L"images\\stage\\target.png");
	target->_select_img.Load(L"images\\stage\\select_target.png");
	_dialog_bar.Load(L"images\\dialog\\Dialog_bar.png");
	mGlowingBlack.Load(L"images\\loading\\Loading_Black_background.bmp");

	_select_Pikachu.Load(L"images\\stage\\_select_Pikachu.png");
	_select_Charmander.Load(L"images\\stage\\_select_Charmander.png");
	_select_Squirtle.Load(L"images\\stage\\_select_Squirtle.png");
	_select_Zapados.Load(L"images\\stage\\_select_Zapados.png");
	_select_Moltres.Load(L"images\\stage\\_select_HoOh.png");
	_select_Articuno.Load(L"images\\stage\\_select_Articuno.png");

	_ready_Pikachu[0].Load(L"images\\stage\\Pikachu_1.png");
	_ready_Pikachu[1].Load(L"images\\stage\\Pikachu_2.png");
	_ready_Charmander[0].Load(L"images\\stage\\Charmander_1.png");
	_ready_Charmander[1].Load(L"images\\stage\\Charmander_2.png");
	_ready_Squirtle[0].Load(L"images\\stage\\Squirtle_1.png");
	_ready_Squirtle[1].Load(L"images\\stage\\Squirtle_2.png");

	_ready_Zapados[0].Load(L"images\\stage\\Zapados_1.png");
	_ready_Zapados[1].Load(L"images\\stage\\Zapados_2.png");
	_ready_Moltres[0].Load(L"images\\stage\\Moltres_1.png");
	_ready_Moltres[1].Load(L"images\\stage\\Moltres_2.png");
	_ready_Articuno[0].Load(L"images\\stage\\Articuno_1.png");
	_ready_Articuno[1].Load(L"images\\stage\\Articuno_2.png");

	_clearStage = StageElement::Water;
}

// �� �������� �浹 �ڽ� ũ�� �� ��ġ ����
void Stage::Init()
{
	const RECT& rectWindow = sceneManager->GetRectWindow();
	target->_rectDraw = { (float)(rectWindow.right / 2 - 40), (float)(rectWindow.bottom / 2 - 40), (float)(rectWindow.right / 2 + 40),  (float)(rectWindow.bottom / 2 + 40) }; // �߰��� ��ġ Ÿ����
	target->_rectImage = { 0, 0, TARGET_IMAGESIZE_X, TARGET_IMAGESIZE_Y };
	mRectTarget = target->_rectDraw;

	rectStage[static_cast<int>(StageElement::Water)] = { 350, 570, 610, 720 };
	rectStage[static_cast<int>(StageElement::Fire)] = { -230, 570, 30, 720 };
	rectStage[static_cast<int>(StageElement::Elec)] = { 300, 20, 560, 160 };
	rectStage[static_cast<int>(StageElement::Dark)] = { -230, 100, 30, 250 };
	rectStage[4] = { 150, 200, 250, 260 };

	StageData stageData = { MY_INDEX, static_cast<uint32>(gameData.ClearRecord), 0, target->_rectDraw, false };
	GET_SINGLE(Network)->SendDataAndType(stageData);

	soundManager->StopBGMSound();
	if (sceneManager->GetIsEnding())
	{
		soundManager->PlayBGMSound(BGMSound::Ending, 1.0f, true);
	}
	else
	{
		soundManager->PlayBGMSound(BGMSound::Stage, 1.0f, true);
	}

	for (auto& member : GET_MEMBER_MAP) {
		member.second.mStageData.IsReady = false;
	}

	airPokemon = Type::Empty;
	landPokemon = Type::Empty;
}

// ���� �ʱ�ȭ
void Stage::SelectPokemonInit()
{
	mFingerCount = 0;
	_select_pokemon = false;
	_ready_Air_pokemon = false;
	_ready_Land_pokemon = false;
	_enter_select = false;
}

// �������� ������
void Stage::Paint(HDC hdc, const RECT& rectWindow)
{
	// ���� �رݵ� ���������� ���� �������� ������
	switch (_clearStage)
	{
	case StageElement::Water:
		_water.Draw(hdc, 0, 0, 800, 1200, moveX, 0, 800, 1200);
		break;
	case StageElement::Fire:
		_fire.Draw(hdc, 0, 0, 800, 1200, moveX, 0, 800, 1200);
		break;
	case StageElement::Elec:
		_electronic.Draw(hdc, 0, 0, 800, 1200, moveX, 0, 800, 1200);
		break;
	case StageElement::Dark:
		_dark.Draw(hdc, 0, 0, 800, 1200, moveX, 0, 800, 1200);
		break;
	}

	// �浹 �ڽ� ������
	if (_allHide == true)
	{
		for (int i = 0; i < STAGE_NUM; i++)
		{
			Rectangle(hdc, rectStage[i].left, rectStage[i].top, rectStage[i].right, rectStage[i].bottom);
		}

		HBRUSH hBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
		HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, hBrush);

		Rectangle(hdc, target->_cam.left, target->_cam.top, target->_cam.right, target->_cam.bottom);

		SelectObject(hdc, oldBrush);
		DeleteObject(hBrush);

	}

	// Ÿ���� �ش� �������� �浹 �ڽ��� �浹�� ������ Ÿ�� �̹����� ����
	if (target->_select == false)
	{
		target->_img.Draw(hdc, target->_rectDraw, target->_rectImage);
	}
	else
	{
		target->_select_img.Draw(hdc, target->_rectDraw, target->_rectImage);
	}

	// ���� �رݵ��� ���� �������� ���ý� ��� ����
	if (_dialogflag == true)
	{
		HFONT hFont = CreateFont(35, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, VARIABLE_PITCH | FF_ROMAN, TEXT("ChubbyChoo-SemiBold"));
		HFONT oldFont = (HFONT)SelectObject(hdc, hFont);
		SetBkMode(hdc, TRANSPARENT);
		_dialog_bar.AlphaBlend(hdc, 20, 550, 444, 250, 0, 0, 1024, 576, 200);
		TextOut(hdc, 85, 620, L"THIS STAGE IS LOCKED!", 21);

		SelectObject(hdc, oldFont);
		DeleteObject(hFont);
	}

	// ���������� �����Ͽ� ���ϸ� ����â�� ���� ��� ���ϸ� ����â�� ������
	if (_select_pokemon)
	{
		mGlowingBlack.AlphaBlend(hdc, 0, 0, 500, 750, 0, 0, 500, 750, ALPHA);

		_select_Zapados.TransparentBlt(hdc, 28, 359, SELECTPOKEMONSIZE, SELECTPOKEMONSIZE, 0, 0, 200, 200, RGB(10, 10, 10));
		_select_Moltres.TransparentBlt(hdc, 175, 360, SELECTPOKEMONSIZE, SELECTPOKEMONSIZE, 0, 0, 200, 200, RGB(10, 10, 10));
		_select_Articuno.TransparentBlt(hdc, 320, 360, SELECTPOKEMONSIZE, SELECTPOKEMONSIZE, 0, 0, 200, 200, RGB(10, 10, 10));

		_select_Pikachu.TransparentBlt(hdc, 28, 539, SELECTPOKEMONSIZE, SELECTPOKEMONSIZE, 0, 0, 200, 200, RGB(10, 10, 10));
		_select_Charmander.TransparentBlt(hdc, 178, 540, SELECTPOKEMONSIZE, SELECTPOKEMONSIZE, 0, 0, 200, 200, RGB(10, 10, 10));
		_select_Squirtle.TransparentBlt(hdc, 320, 540, SELECTPOKEMONSIZE, SELECTPOKEMONSIZE, 0, 0, 200, 200, RGB(10, 10, 10));

		HFONT hFont = CreateFont(18, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, VARIABLE_PITCH | FF_ROMAN, TEXT("ARCADECLASSIC"));
		HFONT oldFont = (HFONT)SelectObject(hdc, hFont);

		SetBkMode(hdc, TRANSPARENT);
		SetTextColor(hdc, RGB(255, 255, 255));

		TextOut(hdc, 62, 350, L"Zapados", 7);
		TextOut(hdc, 208, 350, L"Moltres", 7);
		TextOut(hdc, 347, 350, L"Articuno", 8);

		TextOut(hdc, 64, 530, L"Pikachu", 7);
		TextOut(hdc, 195, 530, L"Charmander", 10);
		TextOut(hdc, 349, 530, L"Squirtle", 8);

		switch (mFingerCount)
		{
		case 0:
			_fingerPos.x = 88;
			_fingerPos.y = 490;
			break;
		case 1:
			_fingerPos.x = 233;
			_fingerPos.y = 490;
			break;
		case 2:
			_fingerPos.x = 380;
			_fingerPos.y = 490;
			break;
		case 3:
			_fingerPos.x = 88;
			_fingerPos.y = 670;
			break;
		case 4:
			_fingerPos.x = 233;
			_fingerPos.y = 670;
			break;
		case 5:
			_fingerPos.x = 380;
			_fingerPos.y = 670;
			break;
		}

		if (_ready_Air_pokemon)
		{
			switch (_play_Air_pokemon)
			{
			case 0:
				_ready_Zapados[(int)_select_pokemon_move].Draw(hdc, 80, 130, 150, 150, 0, 0, 150, 150);
				break;
			case 1:
				_ready_Moltres[(int)_select_pokemon_move].Draw(hdc, 80, 130, 150, 150, 0, 0, 150, 150);
				break;
			case 2:
				_ready_Articuno[(int)_select_pokemon_move].Draw(hdc, 80, 130, 150, 150, 0, 0, 150, 150);
				break;
			}
		}

		if (_ready_Land_pokemon)
		{
			switch (_play_Land_pokemon)
			{
			case 3:
				_ready_Pikachu[(int)_select_pokemon_move].Draw(hdc, 290, 160, 100, 100, 0, 0, 150, 150);
				break;
			case 4:
				_ready_Charmander[(int)_select_pokemon_move].Draw(hdc, 290, 160, 100, 100, 0, 0, 150, 150);
				break;
			case 5:
				_ready_Squirtle[(int)_select_pokemon_move].Draw(hdc, 290, 160, 100, 100, 0, 0, 150, 150);
				break;
			}
		}

		HFONT hFont2 = CreateFont(30, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, VARIABLE_PITCH | FF_ROMAN, TEXT("ARCADECLASSIC"));
		HFONT oldFont2 = (HFONT)SelectObject(hdc, hFont2);

		if (!_ready_Land_pokemon && (int)mTwinkleCnt % 3 != 0)
			TextOut(hdc, _fingerPos.x, _fingerPos.y, L"��", 1);

		HFONT hFont3 = CreateFont(30, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, VARIABLE_PITCH | FF_ROMAN, TEXT("ChubbyChoo-SemiBold"));
		HFONT oldFont3 = (HFONT)SelectObject(hdc, hFont3);

		if (_ready_Air_pokemon && _ready_Land_pokemon && (int)mTwinkleCnt % 2 != 0)
			TextOut(hdc, 35, 50, L"PRESS ENTER KEY TO CONTINUE", 28);

		SelectObject(hdc, oldFont);
		SelectObject(hdc, oldFont2);
		SelectObject(hdc, oldFont3);
		DeleteObject(hFont);
		DeleteObject(hFont2);
		DeleteObject(hFont3);
	}

}

// �������������� Update �Լ�
void Stage::Update(float elapsedTime)
{
	if (sceneManager->IsLoading())
		return;

	if(_select_pokemon)
		mTwinkleCnt += elapsedTime * 3.f;

	RECT rect;
	auto rectWindow = sceneManager->GetRectWindow();

	int inputKey = 0;
	if (!_select_pokemon) {
		target->_select = false;

		// Ÿ���� �� ������Ʈ ���� �ö� ���� ��� ���� flag��  true�� ����
		for (int i = 0; i < STAGE_NUM; i++)
		{
			if (IntersectRect2(rectStage[i], target->_rectDraw))
			{
				target->_select = true;
				target->_select_index = static_cast<StageElement>(i); // Ÿ���� �����ִ� ��ġ�� ���� �ε����� �ٲ� ex) ǥ���� Dark�� index ���� 3
				break;
			}
			target->_select_index = StageElement::Null;
		}

		// Ÿ�� �̵�
		if (GetAsyncKeyState(VK_LEFT) & 0x8000 && target->_rectDraw.left > rectWindow.left)
		{
			inputKey = VK_LEFT;

			if (!_select_pokemon) {
				mRectTarget.left -= 200 * elapsedTime;
				mRectTarget.right -= 200 * elapsedTime;
			}
		}
		// Ÿ�� �̵�
		else if (GetAsyncKeyState(VK_RIGHT) & 0x8000 && target->_rectDraw.right < rectWindow.right)
		{
			inputKey = VK_RIGHT;

			if (!_select_pokemon) {
				mRectTarget.left += 200 * elapsedTime;
				mRectTarget.right += 200 * elapsedTime;
			}
		}
		// Ÿ�� �̵�
		else if (GetAsyncKeyState(VK_UP) & 0x8000 && target->_rectDraw.top > rectWindow.top)
		{
			inputKey = VK_UP;

			if (!_select_pokemon) {
				mRectTarget.top -= 200 * elapsedTime;
				mRectTarget.bottom -= 200 * elapsedTime;
			}
		}
		// Ÿ�� �̵�
		else if (GetAsyncKeyState(VK_DOWN) & 0x8000 && target->_rectDraw.bottom < rectWindow.bottom)
		{
			inputKey = VK_DOWN;

			if (!_select_pokemon) {
				mRectTarget.top += 200 * elapsedTime;
				mRectTarget.bottom += 200 * elapsedTime;
			}
		}
		else if (GetAsyncKeyState(VK_RETURN) & 0x0001)
		{
			inputKey = VK_RETURN;
		}
	}

	if (inputKey != 0) {
		if (MY_INDEX == MP_INDEX) {
			StageData sendData{ MY_INDEX, gameData.ClearRecord, inputKey, mRectTarget };
			GET_SINGLE(Network)->SendDataAndType(sendData);
		}
	}

	auto& recvData = MEMBER_MAP(MP_INDEX).mStageData;
	if (recvData.InputKey != 0) {
		target->_rectDraw = recvData.RectDraw;
		_dialogflag = false;
	}

	fingerController(elapsedTime);

	if (recvData.InputKey == VK_RETURN && _ready_Air_pokemon && _ready_Land_pokemon)
	{
		if (recvData.CanGoNextScene == true) {
			moveX = 300;
			sceneManager->StartLoading(sceneManager->GetHwnd());
		}
	}

	// ��ȿ�� ���������� Ÿ���� �浹�Ͽ��� �� ���� Ű�� ������ ���� ������ �̵��Ѵ�.
	if (recvData.InputKey == VK_RETURN && target->_select == true)
	{
		_enter_select = true;

		if (target->_select_index == StageElement::Town)
		{
			moveX = 300;
			sceneManager->StartLoading(sceneManager->GetHwnd());
			return;
		}

		switch (_clearStage)
		{
		case StageElement::Water:
		{
			if (target->_select_index == StageElement::Water)
			{
				_select_pokemon = true;
			}
			else
			{
				_dialogflag = true;
			}
		}
		break;
		case StageElement::Fire:
		{
			if (target->_select_index == StageElement::Water ||
				target->_select_index == StageElement::Fire)
			{
				_select_pokemon = true;
			}
			else
			{
				_dialogflag = true;
			}
		}
		break;
		case StageElement::Elec:
		{
			if (target->_select_index == StageElement::Water ||
				target->_select_index == StageElement::Fire ||
				target->_select_index == StageElement::Elec)
			{
				_select_pokemon = true;
			}
			else
			{
				_dialogflag = true;
			}
		}
		break;
		case StageElement::Dark:
		{
			if (target->_select_index == StageElement::Water ||
				target->_select_index == StageElement::Fire ||
				target->_select_index == StageElement::Elec ||
				target->_select_index == StageElement::Dark)
			{
				_select_pokemon = true;
			}
			else
			{
				_dialogflag = true;
			}
		}
		break;

		}
	}
	if (GetAsyncKeyState(VK_R) & 0x0001)
	{
		_ready_Air_pokemon = false;
		_ready_Land_pokemon = false;
		_enter_select = true;
		mFingerCount = 0;
	}


	recvData.InputKey = 0;
	target->_cam = { target->_rectDraw.left - CAMSIZE_X, (float)rectWindow.top, target->_rectDraw.right + CAMSIZE_X, (float)rectWindow.bottom };
	InvalidateRect(sceneManager->GetHwnd(), NULL, false);
}

// ���ϸ� ����â�� ���� ��쿡 ���ϸ��� �����ϴ� �ΰ� ��Ʈ�ѷ�
void Stage::fingerController(float elpasedTime)
{
	_select_pokemon_move += elpasedTime * 2.f;
	if ((int)_select_pokemon_move == 2)
		_select_pokemon_move = 0;

	if (_select_pokemon && sceneManager->IsLoading() == false)
	{
		if (GetAsyncKeyState(VK_RETURN) & 0x0001 && _enter_select)
		{
			if (!_ready_Air_pokemon)
			{
				_play_Air_pokemon = mFingerCount;
				mFingerCount = 3;
				_ready_Air_pokemon = true;

				switch (_play_Air_pokemon)
				{
				case 0:
					airPokemon = Type::Elec;
					soundManager->PlayHitSound(HitSound::Elec);
					break;
				case 1:
					airPokemon = Type::Fire;
					soundManager->PlayHitSound(HitSound::Fire);
					break;
				case 2:
					airPokemon = Type::Water;
					soundManager->PlayHitSound(HitSound::Water);
					break;
				default:
					assert(0);
					break;
				}
			}
			else if (!_ready_Land_pokemon)
			{
				_play_Land_pokemon = mFingerCount;
				_ready_Land_pokemon = true;

				switch (_play_Land_pokemon)
				{
				case 3:
				{
					landPokemon = Type::Elec;

					const int randSound = rand() % 2;
					if (randSound != 0)
					{
						soundManager->PlaySelectSound(SelectSound::Pikachu1);
					}
					else
					{
						soundManager->PlaySelectSound(SelectSound::Pikachu2);
					}
				}
				break;
				case 4:
					landPokemon = Type::Fire;
					soundManager->PlaySelectSound(SelectSound::Charmander);
					break;
				case 5:
					landPokemon = Type::Water;
					soundManager->PlaySelectSound(SelectSound::Squirtle);
					break;
				default:
					assert(0);
					break;
				}
			}
			else if (_ready_Air_pokemon && _ready_Land_pokemon)
			{
				StageData sendData = { MY_INDEX, gameData.ClearRecord, VK_RETURN, target->_rectDraw, true, false };
				GET_SINGLE(Network)->SendDataAndType<StageData>(sendData);
			}
		}

		if (!_ready_Air_pokemon)
		{
			if (GetAsyncKeyState(VK_LEFT) & 0x0001 && mFingerCount > 0)
				mFingerCount -= 1;
			if (GetAsyncKeyState(VK_RIGHT) & 0x0001 && mFingerCount < 2)
				mFingerCount += 1;
		}
		else if (!_ready_Land_pokemon)
		{
			if (GetAsyncKeyState(VK_LEFT) & 0x0001 && mFingerCount > 3)
				mFingerCount -= 1;
			if (GetAsyncKeyState(VK_RIGHT) & 0x0001 && mFingerCount < 5)
				mFingerCount += 1;
		}
		if (GetAsyncKeyState(VK_BACK) & 0x8000)
			SelectPokemonInit();
	}
}

// ���������� Ŭ���� ��� Ŭ���� �������� ��� ������Ʈ
void Stage::ClearStage()
{
	switch (_clearStage)
	{
	case StageElement::Water:
		_clearStage = StageElement::Fire;
		break;
	case StageElement::Fire:
		_clearStage = StageElement::Elec;
		break;
	case StageElement::Elec:
		_clearStage = StageElement::Dark;
		break;
	case StageElement::Dark: // ���� ������������ Ŭ�������� ��� isEnding ������ true�� ����
		sceneManager->GameClear();
		break;
	}
}