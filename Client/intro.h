#pragma once

class MainIntro
{
public:
	// 인트로 화면의 위치
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
	CImage _glowing_black; // 반투명한 검은색 화면으로 주로 로딩 화면 같은 곳에서 쓰인다.
	int _finger = 0; // 인트로 화면에서 깜빡이는 화살표의 상태 값이다. (start = 0, producer = 1, finish = 2)
	int _finger_twinkle_cnt = 0; // 화살표를 깜빡이게 하기 위한 변수이다.
	bool _producer = false; // 켜져 있다면 개발자 정보를 확인할 수 있다.
	POINT _fingerPos = { 0, };
public:
	Menu()
	{
		_glowing_black.Load(L"images\\loading\\Loading_Black_background.bmp");
	}
	void Paint(HDC hdc, HWND hWnd);

	// 인트로, 스테이지, 페이즈에서 필요하며 키보드 입력을 받는 마우스 같은 역할을 한다.
	// 인트로에서 핑거 컨트롤러는 Start, Producer, Finish 중 하나를 고를 수 있으며,
	// 스테이지에서 핑거 컨트롤러는 6개의 포켓몬 중 2개의 포켓몬을 고를 수 있으며,
	// 페이즈에서 핑거 컨트롤러는 단지 키 입력 값이 엔터 키라면 다음 씬으로 넘어간다.
	void fingerController(const HWND& hWnd); 
};