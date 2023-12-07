#include "stdafx.h"
#include "Intro.h"
#include "scene.h"
#include "sound.h"
#include "Network.h"
#include "timer.h"

extern SceneManager* sceneManager;
extern SoundManager* soundManager;

// ============================================================= IntroGameObject
void MyGameObject::Init()
{
}
void MyGameObject::Init(const FRECT& draw, const wchar_t* imgfile)
{
	if (imgfile != nullptr)
		mCImage.Load(imgfile);

	mRectDraw = draw;
	ConvertToVector();
	mRectImage = { 0, 0, (LONG)mSize.x, (LONG)mSize.y };
}
void MyGameObject::Init(const Vector2& pos, const Vector2& size, const RECT& rectImage, const wchar_t* imgfile)
{
	if (imgfile != nullptr)
		mCImage.Load(imgfile);

	mPos = pos;
	mSize = size;
	mRectImage = rectImage;
	ConvertToFRECT();
}
void MyGameObject::Init(const Vector2& pos, const wchar_t* imgfile)
{
	if (imgfile != nullptr)
		mCImage.Load(imgfile);

	mSize.x = mCImage.GetWidth();
	mSize.y = mCImage.GetHeight();

	mPos = pos;
	mRectImage = { 0, 0, (LONG)mSize.x, (LONG)mSize.y };
}
void MyGameObject::Update(float elapsedTime)
{
}
void MyGameObject::Paint(HDC hdc)
{
	// 사이즈에 따른 위치 렉트값
	mRectDraw = { mPos.x, mPos.y, mSize.x + mPos.x, mSize.y + mPos.y };

	FRECT rect = { mPos.x, mPos.y, mSize.x, mSize.y };
	// 이미지 출력
	mCImage.TransparentBlt(hdc, rect, mRectImage, RGB(254, 254, 254));
}

// ============================================================= Cloud
void Cloud::Update(float elapsedTime)
{
	mPos.x += mMove.x * elapsedTime;
	mPos.y += mMove.y * elapsedTime;

	// 윈도우 벗어날 시 왼쪽의 일정 위치부터 시작
	if (mRectDraw.left > mRectWindow.right)
		mPos.x = -188;
}

// ============================================================= Logo
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

	mFlightStrPos.y += moveSpeed;
	mPokemonStrPos.y += moveSpeed;

	if (acc >= 10 or acc < 0)
		direction *= -1.f;
}
void Logo::Paint(HDC hdc)
{
	// 폰트 설정
	HFONT hFont = CreateFont(LOGO_SIZE, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, VARIABLE_PITCH | FF_ROMAN, TEXT("ChubbyChoo-SemiBold"));
	HFONT oldFont = (HFONT)SelectObject(hdc, hFont);

	// 글자 배경색 투명으로
	SetBkMode(hdc, TRANSPARENT);

	// 로고 그림자 출력
	SetTextColor(hdc, LOGO_SHADOW_COLOR);
	TextOut(hdc, mPokemonStrPos.x + 5.f, mPokemonStrPos.y + 5.f, L"POKEMON", 7);
	TextOut(hdc, mFlightStrPos.x + 5.f, mFlightStrPos.y + 5.f, L"FLIGHT", 6);

	// 로고 출력
	SetTextColor(hdc, LOGO_COLOR);
	TextOut(hdc, mPokemonStrPos.x, mPokemonStrPos.y, L"POKEMON", 7);
	TextOut(hdc, mFlightStrPos.x, mFlightStrPos.y, L"FLIGHT", 6);

	SelectObject(hdc, oldFont);
	DeleteObject(hFont);
}

// ============================================================= Menu
void Menu::Init()
{
	mGlowingBlack.Load(L"images\\loading\\Loading_Black_background.bmp");
}
void Menu::Paint(HDC hdc)
{
	HFONT hFont = CreateFont(MENU_SIZE, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, VARIABLE_PITCH | FF_ROMAN, TEXT("ARCADECLASSIC"));
	HFONT oldFont = (HFONT)SelectObject(hdc, hFont);

	SetBkMode(hdc, TRANSPARENT);
	SetTextColor(hdc, RGB(0, 0, 0));

	switch (mFingerCount)
	{
	case MI_Menu::start:
		mPos.x = 175;
		mPos.y = 525;
		break;
	case MI_Menu::producer:
		mPos.x = 147;
		mPos.y = 575;
		break;
	case MI_Menu::finish:
		mPos.x = 165;
		mPos.y = 625;
		break;
	}

	TextOut(hdc, 200, 525, L"START", 5);
	TextOut(hdc, 174, 575, L"PRODUCER", 8);
	TextOut(hdc, 192, 625, L"FINISH", 6);

	// 이 변수가 켜져 있으면 개발자 이름을 화면 위에 렌더링한다.
	if (mProducer)
	{
		mGlowingBlack.AlphaBlend(hdc, 0, 0, 800, 1200, 0, 0, 800, 1200, IALPHA);

		HFONT hFont = CreateFont(31, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, VARIABLE_PITCH | FF_ROMAN, TEXT("ChubbyChoo-SemiBold"));
		HFONT oldFont = (HFONT)SelectObject(hdc, hFont);

		// 글자 배경색 투명으로
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

	// 화살표의 깜빡거리는 부분 설정으로 1.5초에 한번씩 깜빡거림
	if ((int)mTwinkleCnt % 3 != 0 && !mProducer)
		TextOut(hdc, mPos.x, mPos.y, L"▶", 1);

	SelectObject(hdc, oldFont);
	DeleteObject(hFont);
}
void Menu::Update(float elapsedTime)
{
	mTwinkleCnt += elapsedTime * 3.f;

	if(!IsWindowActive(sceneManager->GetHwnd()))
		return;

	if (GetAsyncKeyState(VK_UP) & 0x0001 && mFingerCount > 0)
	{
		mProducer = false;
		mFingerCount -= 1;
	}
	else if (GetAsyncKeyState(VK_DOWN) & 0x0001 && mFingerCount < 2)
	{
		mProducer = false;
		mFingerCount += 1;
	}
	else if (GetAsyncKeyState(VK_RETURN) & 0x0001)
	{
		switch (mFingerCount)
		{
		case MI_Menu::start:
		{
			GET_SINGLE(Network)->Connect();
			sceneManager->StartLoading(sceneManager->GetHwnd());
		}
			break;
		case MI_Menu::producer:
			mProducer = true;
			break;
		case MI_Menu::finish:
			if (GET_SINGLE(Network)->IsConnected())
				GET_SINGLE(Network)->SendDataAndType(EndProcessing{ GET_SINGLE(Network)->GetClientIndex() });
			PostQuitMessage(0);
			break;
		}
	}

}

// ============================================================= Intro
void Intro::Init()
{
	mBackground = make_shared<MyGameObject>();
	mBackground->Init(Vector2{ 0.f, 0.f }, L"images\\intro\\Instruction_Background2.png");

	mMenu = make_shared<Menu>();
	mMenu->Init();

	mLogo = make_shared<Logo>();
	mLogo->Init();

	{
		const RECT rectWindow = sceneManager->GetRectDisplay();

		std::shared_ptr<Cloud> cloud1 = make_shared<Cloud>();
		cloud1->SetMove(Vector2{ 40, 0 });
		cloud1->SetRectWindow(rectWindow);
		cloud1->Init(Vector2{ FIRSTCLOUD_X, FIRSTCLOUD_Y }, L"images\\intro\\Instruction_Cloud1.bmp");
		mClouds.emplace_back(cloud1);

		std::shared_ptr<Cloud> cloud2 = make_shared<Cloud>();
		cloud2->Init(Vector2{ SECONDCLOUD_X, SECONDCLOUD_Y }, L"images\\intro\\Instruction_Cloud2.bmp");
		cloud2->SetMove(Vector2{ 20, 0 });
		cloud2->SetRectWindow(rectWindow);
		mClouds.emplace_back(cloud2);

		std::shared_ptr<Cloud> cloud3 = make_shared<Cloud>();
		cloud3->Init(Vector2{ THIRDCLOUD_X, THIRDCLOUD_Y }, L"images\\intro\\Instruction_Cloud3.bmp");
		cloud3->SetMove(Vector2{ 10, 0 });
		cloud3->SetRectWindow(rectWindow);
		mClouds.emplace_back(cloud3);

		std::shared_ptr<Cloud> cloud4 = make_shared<Cloud>();
		cloud4->Init(Vector2{ FOURTHCLOUD_X, FOURTHCLOUD_Y }, L"images\\intro\\Instruction_Cloud4.bmp");
		cloud4->SetMove(Vector2{ 40, 0 });
		cloud4->SetRectWindow(rectWindow);
		mClouds.emplace_back(cloud4);
	}

	soundManager->StopBGMSound();
	soundManager->PlayBGMSound(BGMSound::Intro, 1.0f, true);
}
void Intro::Update(float elapsedTime)
{
	const RECT rectWindow = sceneManager->GetRectDisplay();

	mClouds[0]->Update(elapsedTime);
	mClouds[1]->Update(elapsedTime);
	mClouds[2]->Update(elapsedTime);
	mClouds[3]->Update(elapsedTime);
	mLogo->Update(elapsedTime);
	mMenu->Update(elapsedTime);

	InvalidateRect(sceneManager->GetHwnd(), NULL, false);
}

void Intro::Paint(HDC hdc)
{
	mBackground->Paint(hdc);
	for (auto& cloud : mClouds)
		cloud->Paint(hdc);
	mLogo->Paint(hdc);
	mMenu->Paint(hdc);
}