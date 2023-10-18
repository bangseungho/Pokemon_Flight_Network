#include "stdafx.h"
#include "Intro.h"
#include "scene.h"

#define INSTRUCTION_BACKGROUND_X 500
#define INSTRUCTION_BACKGROUND_Y 750

#define LOGO_COLOR RGB(255, 212, 0)
#define LOGO_SHADOW_COLOR RGB(200, 100, 0)

#define LOGO_SIZE 106

#define MENU_SIZE 31

#define TIMERID_LODING 5
#define ELAPSE_LOADING 100

#define ALPHA 230

extern SceneManager* sceneManager;

enum MI_Menu { start = 0, producer, finish };

void MainIntro::Init(const wchar_t* imgfile, int _PosX, int _PosY)
{
	_img.Load(imgfile);

	_Size.x = _img.GetWidth();
	_Size.y = _img.GetHeight();

	// �̹����� ��ġ��
	_Pos.x = _PosX;
	_Pos.y = _PosY;

	// ���� �̹��� ��Ʈ��
	_rectImage = { 0, 0, _Size.x, _Size.y };
}

void MainIntro::Paint(HDC hdc, const RECT& rectWindow)
{
	// ����� ���� ��ġ ��Ʈ��
	_rectDraw = { _Pos.x, _Pos.y, _Size.x + _Pos.x, _Size.y + _Pos.y };

	// �̹��� ���
	_img.TransparentBlt(hdc, _rectDraw, _rectImage, RGB(254, 254, 254));
}

// ���� ȭ�� ���� ������
void Cloud::Move(int MoveX, int MoveY, const RECT& rectWindow)
{
	_Pos.x += MoveX;
	_Pos.y += MoveY;

	// ������ ��� �� ������ ���� ��ġ���� ����
	if (_rectDraw.left > rectWindow.right)
		_Pos.x = -188;
}

void Logo::Paint(HDC hdc)
{
	// ��Ʈ ����
	HFONT hFont = CreateFont(LOGO_SIZE, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, VARIABLE_PITCH | FF_ROMAN, TEXT("ChubbyChoo-SemiBold"));
	HFONT oldFont = (HFONT)SelectObject(hdc, hFont);

	// ���� ���� ��������
	SetBkMode(hdc, TRANSPARENT);

	// �ΰ� �׸��� ���
	SetTextColor(hdc, LOGO_SHADOW_COLOR);
	TextOut(hdc, 45, 99 + _logoMovingCnt, L"POKEMON", 7);
	TextOut(hdc, 99, 177 + _logoMovingCnt, L"FLIGHT", 6);

	// �ΰ� ���
	SetTextColor(hdc, LOGO_COLOR);
	TextOut(hdc, 40, 94 + _logoMovingCnt, L"POKEMON", 7);
	TextOut(hdc, 94, 172 + _logoMovingCnt, L"FLIGHT", 6);

	SelectObject(hdc, oldFont);
	DeleteObject(hFont);
}

// ó�� ���� ȭ�� start, finish, finish�� ���� �޴� ����
void Menu::Paint(HDC hdc, HWND hWnd)
{
	HFONT hFont = CreateFont(MENU_SIZE, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, VARIABLE_PITCH | FF_ROMAN, TEXT("ARCADECLASSIC"));
	HFONT oldFont = (HFONT)SelectObject(hdc, hFont);

	SetBkMode(hdc, TRANSPARENT);
	SetTextColor(hdc, RGB(0, 0, 0));

	switch (_finger)
	{
	case MI_Menu::start:
		_fingerPos.x = 175;
		_fingerPos.y = 525;
		break;
	case MI_Menu::producer:
		_fingerPos.x = 147;
		_fingerPos.y = 575;
		break;
	case MI_Menu::finish:
		_fingerPos.x = 165;
		_fingerPos.y = 625;
		break;
	}

	TextOut(hdc, 200, 525, L"START", 5);
	TextOut(hdc, 174, 575, L"PRODUCER", 8);
	TextOut(hdc, 192, 625, L"FINISH", 6);

	if (_producer)
	{
		_glowing_black.AlphaBlend(hdc, 0, 0, 800, 1200, 0, 0, 800, 1200, ALPHA);

		HFONT hFont = CreateFont(31, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, VARIABLE_PITCH | FF_ROMAN, TEXT("ChubbyChoo-SemiBold"));
		HFONT oldFont = (HFONT)SelectObject(hdc, hFont);

		// ���� ���� ��������
		SetBkMode(hdc, TRANSPARENT);
		SetTextColor(hdc, RGB(255, 255, 255));


		TextOut(hdc, 50, 40, L"WINDOW PROGRAMMING", 19);
		TextOut(hdc, 50, 70, L"TERM PROJECT", 13);
		TextOut(hdc, 50, 160, L"PRODUCER", 9);
		TextOut(hdc, 50, 250, L"MIN DONG HYEON", 15);
		TextOut(hdc, 50, 340, L"BAEK SEUNG HO", 14);

		SetTextColor(hdc, RGB(255, 0, 0));
		TextOut(hdc, 50, 220, L"GAME PLAY", 10);
		SetTextColor(hdc, RGB(0, 255, 0));
		TextOut(hdc, 50, 310, L"GAME FLOW", 10);

		SelectObject(hdc, oldFont);
		DeleteObject(hFont);

	}

	// ȭ��ǥ�� �����Ÿ��� �κ� �������� 1.5�ʿ� �ѹ��� �����Ÿ�
	if (_finger_twinkle_cnt % 3 != 0 && !_producer)
		TextOut(hdc, _fingerPos.x, _fingerPos.y, L"��", 1);

	SelectObject(hdc, oldFont);
	DeleteObject(hFont);
}

void Menu::fingerController(const HWND& hWnd)
{
	if (GetAsyncKeyState(VK_UP) & 0x0001 && _finger > 0)
	{
		_producer = false;
		_finger -= 1;
	}
	else if (GetAsyncKeyState(VK_DOWN) & 0x0001 && _finger < 2)
	{
		_producer = false;
		_finger += 1;
	}
	else if (GetAsyncKeyState(VK_RETURN) & 0x0001)
	{
		switch (_finger)
		{
		case MI_Menu::start:
			sceneManager->StartLoading(hWnd);
			break;
		case MI_Menu::producer:
			_producer = true;
			break;
		case MI_Menu::finish:
			PostQuitMessage(0);
			break;
		}
	}

}
