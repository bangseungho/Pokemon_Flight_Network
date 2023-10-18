#include "stdafx.h"
#include "stage.h"
#include "intro.h"
#include "scene.h"
#include "sound.h"

#define TARGET_IMAGESIZE_X 512
#define TARGET_IMAGESIZE_Y 512
#define CAMSIZE_X 162

#define STAGE_NUM 5

#define MAPSCROLL_SPEED 4

#define ALPHA 220

#define SELECTPOKEMONSIZE 135

#define VK_R 0x52

extern SceneManager* sceneManager;
extern SoundManager* soundManager;

Stage::Stage()
{
	target = new Target();
	_water.Load(L"images\\stage\\Water_phase.bmp");
	_fire.Load(L"images\\stage\\Fire_phase.bmp");
	_electronic.Load(L"images\\stage\\Electronic_phase.bmp");
	_dark.Load(L"images\\stage\\Dark_phase.bmp");
	target->_img.Load(L"images\\stage\\target.png");
	target->_select_img.Load(L"images\\stage\\select_target.png");
	_dialog_bar.Load(L"images\\dialog\\Dialog_bar.png");
	_glowing_black.Load(L"images\\loading\\Loading_Black_background.bmp");

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

	_phase = StageElement::Water;
}

void Stage::Init(const RECT& rectWindow)
{
	target->_rectDraw = { (rectWindow.right / 2 - 40), (rectWindow.bottom / 2 - 40), rectWindow.right / 2 + 40,  (rectWindow.bottom / 2 + 40) }; // 중간에 위치 타겟을
	target->_rectImage = { 0, 0, TARGET_IMAGESIZE_X, TARGET_IMAGESIZE_Y };

	rectStage[static_cast<int>(StageElement::Water)] = { 350, 570, 610, 720 };
	rectStage[static_cast<int>(StageElement::Fire)] = { -230, 570, 30, 720 };
	rectStage[static_cast<int>(StageElement::Elec)] = { 300, 20, 560, 160 };
	rectStage[static_cast<int>(StageElement::Dark)] = { -230, 100, 30, 250 };
	rectStage[4] = { 150, 200, 250, 260 };
}

void Stage::SelectPokemonInit()
{
	_select_pokemon = false;
	_ready_Air_pokemon = false;
	_ready_Land_pokemon = false;
	_enter_select = false;
	_finger = 0;
}

void Stage::Paint(HDC hdc, const RECT& rectWindow, Menu menu)
{
	switch (_phase)
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

	if (target->_select == false)
	{
		target->_img.Draw(hdc, target->_rectDraw, target->_rectImage);
	}
	else
	{
		target->_select_img.Draw(hdc, target->_rectDraw, target->_rectImage);
	}

	// 경고 문구
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

	if (_select_pokemon)
	{
		_glowing_black.AlphaBlend(hdc, 0, 0, 500, 750, 0, 0, 500, 750, ALPHA);

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

		switch (_finger)
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
				_ready_Zapados[_select_pokemon_move].Draw(hdc, 80, 130, 150, 150, 0, 0, 150, 150);
				break;
			case 1:
				_ready_Moltres[_select_pokemon_move].Draw(hdc, 80, 130, 150, 150, 0, 0, 150, 150);
				break;
			case 2:
				_ready_Articuno[_select_pokemon_move].Draw(hdc, 80, 130, 150, 150, 0, 0, 150, 150);
				break;
			}
		}

		if (_ready_Land_pokemon)
		{
			switch (_play_Land_pokemon)
			{
			case 3:
				_ready_Pikachu[_select_pokemon_move].Draw(hdc, 290, 160, 100, 100, 0, 0, 150, 150);
				break;
			case 4:
				_ready_Charmander[_select_pokemon_move].Draw(hdc, 290, 160, 100, 100, 0, 0, 150, 150);
				break;
			case 5:
				_ready_Squirtle[_select_pokemon_move].Draw(hdc, 290, 160, 100, 100, 0, 0, 150, 150);
				break;
			}
		}

		HFONT hFont2 = CreateFont(30, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, VARIABLE_PITCH | FF_ROMAN, TEXT("ARCADECLASSIC"));
		HFONT oldFont2 = (HFONT)SelectObject(hdc, hFont2);

		if (!_ready_Land_pokemon && menu._finger_twinkle_cnt % 3 != 0)
			TextOut(hdc, _fingerPos.x, _fingerPos.y, L"▲", 1);

		HFONT hFont3 = CreateFont(30, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, VARIABLE_PITCH | FF_ROMAN, TEXT("ChubbyChoo-SemiBold"));
		HFONT oldFont3 = (HFONT)SelectObject(hdc, hFont3);

		if (_ready_Air_pokemon && _ready_Land_pokemon && menu._finger_twinkle_cnt % 2 != 0)
			TextOut(hdc, 35, 50, L"PRESS ENTER KEY TO CONTINUE", 28);

		SelectObject(hdc, oldFont);
		SelectObject(hdc, oldFont2);
		SelectObject(hdc, oldFont3);
		DeleteObject(hFont);
		DeleteObject(hFont2);
		DeleteObject(hFont3);
	}

}

void Stage::Move(const HWND& hWnd, const RECT& rectWindow)
{
	RECT rect;
	target->_cam = { target->_rectDraw.left - CAMSIZE_X, rectWindow.top, target->_rectDraw.right + CAMSIZE_X, rectWindow.bottom };

	target->_select = false;
	target->_select_index = StageElement::Null;

	// 타겟이 맵 오브젝트 위에 올라가 있을 경우 선택 flag를  true로 설정
	for (int i = 0; i < STAGE_NUM; i++)
	{
		if (IntersectRect(&rect, &rectStage[i], &target->_rectDraw))
		{
			target->_select = true;
			target->_select_index = static_cast<StageElement>(i); // 타겟이 놓여있는 위치에 따라 인덱스를 바꿈 ex) 표적이 Dark면 index 값은 3
		}
	}

	if (GetAsyncKeyState(VK_LEFT) & 0x8000 && target->_rectDraw.left > rectWindow.left && !_select_pokemon)
	{
		target->_rectDraw.left -= MAPSCROLL_SPEED;
		target->_rectDraw.right -= MAPSCROLL_SPEED;
		_dialogflag = false;

		if (moveX > 0)
		{
			moveX -= 10;

			for (int i = 0; i < STAGE_NUM; i++)
			{
				rectStage[i].left += 10;
				rectStage[i].right += 10;
			}
		}
	}
	if (GetAsyncKeyState(VK_RIGHT) & 0x8000 && target->_rectDraw.right < rectWindow.right && !_select_pokemon)
	{
		target->_rectDraw.left += MAPSCROLL_SPEED;
		target->_rectDraw.right += MAPSCROLL_SPEED;
		_dialogflag = false;

		if (moveX < 450)
		{
			moveX += 10;

			for (int i = 0; i < STAGE_NUM; i++)
			{
				rectStage[i].left -= 10;
				rectStage[i].right -= 10;
			}
		}

	}
	if (GetAsyncKeyState(VK_UP) & 0x8000 && target->_rectDraw.top > rectWindow.top && !_select_pokemon)
	{
		target->_rectDraw.top -= MAPSCROLL_SPEED * 2;
		target->_rectDraw.bottom -= MAPSCROLL_SPEED * 2;
		_dialogflag = false;
	}
	if (GetAsyncKeyState(VK_DOWN) & 0x8000 && target->_rectDraw.bottom < rectWindow.bottom && !_select_pokemon)
	{
		target->_rectDraw.top += MAPSCROLL_SPEED * 2;
		target->_rectDraw.bottom += MAPSCROLL_SPEED * 2;
		_dialogflag = false;
	}

	if (GetAsyncKeyState(VK_RETURN) & 0x0001 && target->_select == true)
	{
		_enter_select = true;

		if (target->_select_index == StageElement::Town)
		{
			moveX = 300;
			sceneManager->StartLoading(hWnd);
			return;
		}

		switch (_phase)
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

	if (sceneManager->IsLoading() == false && GetAsyncKeyState(VK_BACK) & 0x0001)
	{
		_select_pokemon = false;
		_ready_Air_pokemon = false;
		_ready_Land_pokemon = false;
		_enter_select = false;
		_finger = 0;
	}
	if (GetAsyncKeyState(VK_R) & 0x0001)
	{
		_ready_Air_pokemon = false;
		_ready_Land_pokemon = false;
		_enter_select = true;
		_finger = 0;
	}

}

void Stage::fingerController(const HWND& hWnd)
{
	if (_select_pokemon && sceneManager->IsLoading() == false)
	{
		if (GetAsyncKeyState(VK_RETURN) & 0x0001 && _enter_select)
		{
			if (!_ready_Air_pokemon)
			{
				_play_Air_pokemon = _finger;
				_finger = 3;
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
				_play_Land_pokemon = _finger;
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
				moveX = 300;
				sceneManager->StartLoading(hWnd);
			}
		}

		if (!_ready_Air_pokemon)
		{
			if (GetAsyncKeyState(VK_LEFT) & 0x0001 && _finger > 0)
				_finger -= 1;
			if (GetAsyncKeyState(VK_RIGHT) & 0x0001 && _finger < 2)
				_finger += 1;
		}
		else if (!_ready_Land_pokemon)
		{
			if (GetAsyncKeyState(VK_LEFT) & 0x0001 && _finger > 3)
				_finger -= 1;
			if (GetAsyncKeyState(VK_RIGHT) & 0x0001 && _finger < 5)
				_finger += 1;
		}
	}
}

void Stage::ClearStage()
{
	switch (_phase)
	{
	case StageElement::Water:
		_phase = StageElement::Fire;
		break;
	case StageElement::Fire:
		_phase = StageElement::Elec;
		break;
	case StageElement::Elec:
		_phase = StageElement::Dark;
		break;
	case StageElement::Dark:
		sceneManager->GameClear();
		break;
	}
}