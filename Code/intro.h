#pragma once

class MainIntro
{
public:
	POINT _Pos = { 0, };
	RECT _rectDraw = { 0, };

	void Init(const wchar_t* imgfile, int _PosX, int _PosY);
	virtual void Paint(HDC hdc, const RECT& rectWindow);	

private:
	CImage _img;
	POINT _Size = { 0, };
	RECT _rectImage = { 0, };
};

class Intro : public MainIntro
{
public:
	Intro()
	{
		Init(L"images\\intro\\Instruction_Background2.png", 0, 0);
	}
};

class Cloud : public MainIntro
{
public:
	void Move(int MoveX, int MoveY, const RECT& rectWindow);
};

class Logo : public MainIntro
{
public:
	int _logoMovingCnt = 0;
public:
	void Paint(HDC hdc);
};

class Menu : public MainIntro
{
public:
	CImage _glowing_black;
	int _finger = 0;
	int _finger_twinkle_cnt = 0;
	bool _producer = false;
	POINT _fingerPos = { 0, };
public:
	Menu()
	{
		_glowing_black.Load(L"images\\loading\\Loading_Black_background.bmp");
	}
	void Paint(HDC hdc, HWND hWnd);
	void fingerController(const HWND& hWnd);
};