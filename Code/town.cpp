#include "stdafx.h"
#include "town.h"
#include "scene.h"

#define CAMSIZE_X 162
#define CAMSIZE_Y 375

#define TPLAYER_IMAGESIZE_X 64
#define TPLAYER_IMAGESIZE_Y 64
#define TPLAYER_MAX_IMAGESIZE_X 256

#define TPLAYER_SPEED 4;

#define DIR_LEFT_IMAGE_Y 64
#define DIR_RIGHT_IMAGE_Y 128
#define DIR_UP_IMAGE_Y 192
#define DIR_DOWN_IMAGE_Y 0

#define ALPHA 150

extern SceneManager* sceneManager;

Town::Town()
{
	player = new Player();
	_backTown.Load(L"images\\town\\Town.png");
	player->img.Load(L"images\\town\\TownPlayer.png");
	_npc.Load(L"images\\town\\npc1.bmp");
	_npc2.Load(L"images\\town\\npc2.png");
	_npc3.Load(L"images\\town\\npc3.bmp");
	_npc4.Load(L"images\\town\\npc4.bmp");
	_exits.Load(L"images\\dialog\\Exit.bmp");
	_glowing_black.Load(L"images\\loading\\Loading_Black_background.bmp");
}
void Town::Init(const RECT& rectWindow)
{
	// 왼쪽 상단부터
	_object[0] = { 0, 0, 125, 190 };
	_object[1] = { 156, 0, 300, 100 };
	_object[2] = { 337, 0, 750, 175 };
	_object[3] = { 644, 175, 750, 300 };
	_object[4] = { 0, 253, 144, 750 };
	_object[5] = { 130, 556, 267, 750 };
	_object[6] = { 267, 594, 325, 750 };
	_object[7] = { 325, 619, 362, 750 };
	_object[8] = { 687, 487, 750, 750 };
	_object[9] = { 725, 300, 750, 372 };
	_object[10] = { 218, 290, 247, 312 };
	_object[11] = { 306, 290, 334, 312 };
	_object[12] = { 184, 394, 312, 481 };
	_object[13] = { 500, 312, 600, 406 };
	_object[14] = { 184, 325, 187, 481 };
	_object[15] = { 506, 500, 594, 594 };
	_object[16] = { 0, 710, 750, 750 };
	_object[17] = { 189, 80, 191, 100 };

	_npcRect = { 125, 515, 194, 556 };
	_npc2Rect = { 406, 219, 194, 556 };
	_npc3Rect = { 581, 402, 605, 432 };
	_npc4Rect = { 550, 175, 387, 424 };

	_rectDraw = rectWindow;

	_rectImage = rectWindow;

	if (player == nullptr)
		player = new Player();

	player->_rectImage = { 0, 0, TPLAYER_IMAGESIZE_X, TPLAYER_IMAGESIZE_Y };

	if (sceneManager->GetPrevScene() == Scene::Intro)
	{
		player->_Pos.x = 60;
		player->_Pos.y = 232;
		player->_dir = Dir::Right;
	}
	else if (sceneManager->GetPrevScene() == Scene::Stage)
	{
		player->_Pos.x = rectWindow.right / 2;
		player->_Pos.y = rectWindow.bottom / 2;
		player->_dir = Dir::Down;
	}

	StopPlayer();

	// 절대 좌표값 출력을 위한 변수
	player->aboutMapPos = { player->_Pos.x, player->_Pos.y };
}

void Town::Paint(HDC hdc, const RECT& rectWindow)
{
	_backTown.Draw(hdc, _rectDraw, _rectImage);

	_npc2.Draw(hdc, _npc2Rect.left, _npc2Rect.top, 40, 40, _npc2Move.x, _npc2Move.y, 64, 64);

	_npc3.TransparentBlt(hdc, _npc3Rect.left, _npc3Rect.top, 24, 30, 0, 0, 38, 48, RGB(255, 255, 255));

	_npc4.TransparentBlt(hdc, _npc4Rect.left, _npc4Rect.top, 34, 36, 0, 0, 54, 58, RGB(255, 255, 255));

	player->img.Draw(hdc, player->_Pos.x - 20, player->_Pos.y - 20, 40, 40,
		player->_rectImage.left, player->_rectImage.top, player->_rectImage.right, player->_rectImage.bottom);

	_npc.TransparentBlt(hdc, _npcRect.left, _npcRect.top, 75, 40, _npc1Move.x, _npc1Move.y, 50, 27, RGB(255, 255, 255));

	if (_allHide == true)
	{
		// 건물 오브젝트1 박스
		TCHAR lpOut[TOWN_OBJECT_NUM];
		for (int i = 0; i < TOWN_OBJECT_NUM; i++)
		{
			wsprintf(lpOut, L"%d", i);
			Rectangle(hdc, _object[i].left, _object[i].top, _object[i].right, _object[i].bottom);
			TextOut(hdc, _object[i].left + 10, _object[i].top + 10, lpOut, lstrlen(lpOut));
		}


		HBRUSH hBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
		HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, hBrush);

		Rectangle(hdc, player->_rectDraw.left, player->_rectDraw.top, player->_rectDraw.right, player->_rectDraw.bottom);
		Rectangle(hdc, player->_cam.left, player->_cam.top, player->_cam.right, player->_cam.bottom);

		TCHAR Posx[1000];
		TCHAR Posy[1000];
		wsprintf(Posx, L"X : %d", player->aboutMapPos.x);
		wsprintf(Posy, L"Y : %d", player->aboutMapPos.y);
		TextOut(hdc, player->_Pos.x - 25, player->_Pos.y + 30, Posx, lstrlen(Posx));
		TextOut(hdc, player->_Pos.x - 25, player->_Pos.y + 50, Posy, lstrlen(Posy));

		SelectObject(hdc, oldBrush);
		DeleteObject(hBrush);
	}

	if (_exit == true)
	{
		_glowing_black.AlphaBlend(hdc, 0, 0, 500, 750, 0, 0, 500, 750, ALPHA);
		_exits.Draw(hdc, rectWindow.right / 2 - 100, rectWindow.bottom / 2 + 60, 200, 51, 0, 0, 810, 210);

		HFONT hFont = CreateFont(30, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, VARIABLE_PITCH | FF_ROMAN, TEXT("ChubbyChoo-SemiBold"));
		HFONT oldFont = (HFONT)SelectObject(hdc, hFont);
		SetBkMode(hdc, TRANSPARENT);
		SetTextColor(hdc, RGB(255, 255, 255));

		TextOut(hdc, 70, rectWindow.bottom / 2, L"PRESS ENTER KEY TO EXIT...", 25);

		SelectObject(hdc, oldFont);
		DeleteObject(hFont);
	}

}

void Town::Move(const RECT& rectWindow)
{
	RECT temp;

	player->_cam = { player->_Pos.x - CAMSIZE_X, rectWindow.top, player->_Pos.x + CAMSIZE_X, rectWindow.bottom };

	// 문 도착시 게임 종료
	if (IntersectRect(&temp, &player->_rectDraw, &_object[17]))
		_exit = true;

	// 모든 오브젝트를 검사하여 충돌시 반대로 1씩 올린다.
	for (int i = 0; i < TOWN_OBJECT_NUM; i++)
	{
		if (IntersectRect(&temp, &player->_rectDraw, &_object[i]))
		{
			switch (player->_dir)
			{
			case Dir::Left:
				player->aboutMapPos.x += 1;
				player->_Pos.x += 1;
				return;
			case Dir::Right:
				player->aboutMapPos.x -= 1;
				player->_Pos.x -= 1;
				return;
			case Dir::Up:
				player->aboutMapPos.y += 1;
				player->_Pos.y += 1;
				return;
			case Dir::Down:
				player->aboutMapPos.y -= 1;
				player->_Pos.y -= 1;
				return;
			}
		}
	}

	if (GetAsyncKeyState(VK_RETURN) & 0x8000 && _exit == true)
	{
		PostQuitMessage(0);
	}

	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
	{
		player->_keepGoing = true;
		player->_dir = Dir::Left;
		player->aboutMapPos.x -= TPLAYER_SPEED;
		_exit = false;

		if (player->_cam.left < rectWindow.left && _rectImage.left > 0)
		{
			_rectImage.right -= TPLAYER_SPEED;
			_rectImage.left -= TPLAYER_SPEED;

			// 모든 건물들을 자리 그대로 놓기 -> 안하면 플레이어 이동시 오브젝트가 같이 움직임
			for (int i = 0; i < TOWN_OBJECT_NUM; i++)
			{
				_object[i].left += TPLAYER_SPEED;
				_object[i].right += TPLAYER_SPEED;
			}

			_npcRect.left += TPLAYER_SPEED;
			_npc2Rect.left += TPLAYER_SPEED;
			_npc3Rect.left += TPLAYER_SPEED;
			_npc4Rect.left += TPLAYER_SPEED;
		}
		else
			player->_Pos.x -= TPLAYER_SPEED;
	}
	else if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
	{
		player->_keepGoing = true;
		player->_dir = Dir::Right;
		player->aboutMapPos.x += TPLAYER_SPEED;
		_exit = false;

		if (player->_cam.right > rectWindow.right && _rectImage.right < 748)
		{
			_rectImage.right += TPLAYER_SPEED;
			_rectImage.left += TPLAYER_SPEED;

			for (int i = 0; i < TOWN_OBJECT_NUM; i++)
			{
				_object[i].left -= TPLAYER_SPEED;
				_object[i].right -= TPLAYER_SPEED;
			}

			_npcRect.left -= TPLAYER_SPEED;
			_npc2Rect.left -= TPLAYER_SPEED;
			_npc3Rect.left -= TPLAYER_SPEED;
			_npc4Rect.left -= TPLAYER_SPEED;
		}
		else
			player->_Pos.x += TPLAYER_SPEED;
	}
	else if (GetAsyncKeyState(VK_UP) & 0x8000)
	{
		player->aboutMapPos.y -= TPLAYER_SPEED;
		player->_keepGoing = true;
		player->_dir = Dir::Up;
		player->_Pos.y -= TPLAYER_SPEED;
	}
	else if (GetAsyncKeyState(VK_DOWN) & 0x8000)
	{
		_exit = false;
		player->aboutMapPos.y += TPLAYER_SPEED;
		player->_keepGoing = true;
		player->_dir = Dir::Down;
		player->_Pos.y += TPLAYER_SPEED;
	}
}

void Town::StopPlayer()
{
	player->_rectImage.left = 0; // 정지시 차렷자세
	player->_keepGoing = false; // 키 업시 움직임 모션 타이머 정지
}

int Town::GetCamSizeX()
{
	return CAMSIZE_X;
}
int Town::GetCamSizeY()
{
	return CAMSIZE_Y;
}