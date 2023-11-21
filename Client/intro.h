#pragma once

class MainIntro
{
public:
	// ��Ʈ�� ȭ���� ��ġ
	POINT _Pos = { 0, };
	RECT _rectDraw = { 0, };

	void Init();
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
	
	void Update();
};

class Cloud : public MainIntro
{
public:
	void Update(float MoveX, int MoveY, const RECT& rectWindow);
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
	CImage _glowing_black; // �������� ������ ȭ������ �ַ� �ε� ȭ�� ���� ������ ���δ�.
	int _finger = 0; // ��Ʈ�� ȭ�鿡�� �����̴� ȭ��ǥ�� ���� ���̴�. (start = 0, producer = 1, finish = 2)
	int _finger_twinkle_cnt = 0; // ȭ��ǥ�� �����̰� �ϱ� ���� �����̴�.
	bool _producer = false; // ���� �ִٸ� ������ ������ Ȯ���� �� �ִ�.
	POINT _fingerPos = { 0, };
public:
	Menu()
	{
		_glowing_black.Load(L"images\\loading\\Loading_Black_background.bmp");
	}
	void Paint(HDC hdc, HWND hWnd);

	// ��Ʈ��, ��������, ������� �ʿ��ϸ� Ű���� �Է��� �޴� ���콺 ���� ������ �Ѵ�.
	// ��Ʈ�ο��� �ΰ� ��Ʈ�ѷ��� Start, Producer, Finish �� �ϳ��� �� �� ������,
	// ������������ �ΰ� ��Ʈ�ѷ��� 6���� ���ϸ� �� 2���� ���ϸ��� �� �� ������,
	// ������� �ΰ� ��Ʈ�ѷ��� ���� Ű �Է� ���� ���� Ű��� ���� ������ �Ѿ��.
	void fingerController(const HWND& hWnd); 
};