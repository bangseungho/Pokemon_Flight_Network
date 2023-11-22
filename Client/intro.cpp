#include "stdafx.h"
#include "Intro.h"
#include "scene.h"
#include "Network.h"
#include "timer.h"

extern SceneManager* sceneManager;
extern Cloud cloud[4];
extern Logo logo;
extern Menu menu;

// ��Ʈ�� ����
enum MI_Menu { start = 0, producer, finish };

// ��Ʈ�� ȭ�� �ʱ�ȭ
void MainIntro::Init()
{
	logo.Init();
}

// ��Ʈ�� ȭ�鿡 �ʿ��� �̹����� �ε�(�߻�ȭ �Լ�)
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

// ��Ʈ�� ȭ�� ������
void MainIntro::Paint(HDC hdc, const RECT& rectWindow)
{
	// ����� ���� ��ġ ��Ʈ��
	_rectDraw = { _Pos.x, _Pos.y, _Size.x + _Pos.x, _Size.y + _Pos.y };

	// �̹��� ���
	_img.TransparentBlt(hdc, _rectDraw, _rectImage, RGB(254, 254, 254));
}

void Intro::Update(float elapsedTime)
{
	const RECT rectWindow = sceneManager->GetRectDisplay();

	cloud[0].Update(40, 0, rectWindow, elapsedTime);
	cloud[1].Update(20, 0, rectWindow, elapsedTime);
	cloud[2].Update(10, 0, rectWindow, elapsedTime);
	cloud[3].Update(40, 0, rectWindow, elapsedTime);

	logo.Update(elapsedTime);
	menu.Update(elapsedTime);

	InvalidateRect(sceneManager->GetHwnd(), NULL, false);

//else if (scene == Scene::PhaseManager && sceneManager->IsLoading() == false)
//{
//	battle.Init();
//	phase.fingerController(sceneManager->GetHwnd());
//}
}

// ��Ʈ�� ȭ�鿡 �ʿ��� ���� �̵�, �������� �� �Ŵ������� ���� Paint �Լ� ȣ��
void Cloud::Update(float MoveX, int MoveY, const RECT& rectWindow, float elapsedTime)
{
	_Pos.x += MoveX * elapsedTime;
	_Pos.y += MoveY * elapsedTime;

	// ������ ��� �� ������ ���� ��ġ���� ����
	if (_rectDraw.left > rectWindow.right)
		_Pos.x = -188;
}

void Logo::Init()
{
	mPokemonStrPos = { 40.f, 94.f };
	mFlightStrPos = { 94.f, 172.f };
}

void Logo::Update(float elapsedTime)
{
	static float direction = 1.f;
	float moveSpeed = direction * 10.f * elapsedTime;

	static float acc = 0.f;
	acc += moveSpeed;

	logo.mFlightStrPos.y += moveSpeed;
	logo.mPokemonStrPos.y += moveSpeed;

	if (acc >= 10 or acc < 0)
		direction *= -1.f;
}

// ��Ʈ�� ȭ�鿡 �ʿ��� �ΰ� ������
void Logo::Paint(HDC hdc)
{
	// ��Ʈ ����
	HFONT hFont = CreateFont(LOGO_SIZE, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, VARIABLE_PITCH | FF_ROMAN, TEXT("ChubbyChoo-SemiBold"));
	HFONT oldFont = (HFONT)SelectObject(hdc, hFont);

	// ���� ���� ��������
	SetBkMode(hdc, TRANSPARENT);

	// �ΰ� �׸��� ���
	SetTextColor(hdc, LOGO_SHADOW_COLOR);
	TextOut(hdc, mPokemonStrPos.x + 5.f, mPokemonStrPos.y + 5.f, L"POKEMON", 7);
	TextOut(hdc, mFlightStrPos.x + 5.f, mFlightStrPos.y + 5.f, L"FLIGHT", 6);

	// �ΰ� ���
	SetTextColor(hdc, LOGO_COLOR);
	TextOut(hdc, mPokemonStrPos.x, mPokemonStrPos.y, L"POKEMON", 7);
	TextOut(hdc, mFlightStrPos.x, mFlightStrPos.y, L"FLIGHT", 6);

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

	// �� ������ ���� ������ ������ �̸��� ȭ�� ���� �������Ѵ�.
	if (_producer)
	{
		_glowing_black.AlphaBlend(hdc, 0, 0, 800, 1200, 0, 0, 800, 1200, IALPHA);

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
	if ((int)_finger_twinkle_cnt % 3 != 0 && !_producer)
		TextOut(hdc, _fingerPos.x, _fingerPos.y, L"��", 1);

	SelectObject(hdc, oldFont);
	DeleteObject(hFont);
}

// ���� ��Ʈ�� ȭ���� ���¸� ����Ű�� ȭ��ǥ�̴�. ȭ��ǥ�� ����Ų ���¿��� ����Ű�� ������ �׿� �´� �Լ� ȣ��
void Menu::Update(float elapsedTime)
{
	_finger_twinkle_cnt += elapsedTime * 3.f;

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
		{
			GET_SINGLE(Network)->Connect();
			sceneManager->StartLoading(sceneManager->GetHwnd());
		}
			break;
		case MI_Menu::producer:
			_producer = true;
			break;
		case MI_Menu::finish:
			if (GET_SINGLE(Network)->IsConnected())
				GET_SINGLE(Network)->SendDataAndType(EndProcessing{ GET_SINGLE(Network)->GetClientIndex() });
			PostQuitMessage(0);
			break;
		}
	}

}