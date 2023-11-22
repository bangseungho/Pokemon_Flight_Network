#include "stdafx.h"
#include "Intro.h"
#include "scene.h"
#include "Network.h"
#include "timer.h"

extern SceneManager* sceneManager;

// ============================================================= IntroGameObject
void IntroGameObject::Init()
{
}
void IntroGameObject::Init(const wchar_t* imgfile, Vector2 pos)
{
	mCImage.Load(imgfile);

	mSize.x = mCImage.GetWidth();
	mSize.y = mCImage.GetHeight();

	mPos = pos;
	mRectImage = { 0, 0, mSize.x, mSize.y };
}
void IntroGameObject::Update(float elapsedTime)
{
}
void IntroGameObject::Paint(HDC hdc)
{
	// ����� ���� ��ġ ��Ʈ��
	mRectDraw = { mPos.x, mPos.y, mSize.x + mPos.x, mSize.y + mPos.y };

	// �̹��� ���
	mCImage.TransparentBlt(hdc, mRectDraw, mRectImage, RGB(254, 254, 254));
}

// ============================================================= Cloud
void Cloud::Update(float elapsedTime)
{
	mPos.x += mMove.x * elapsedTime;
	mPos.y += mMove.y * elapsedTime;

	// ������ ��� �� ������ ���� ��ġ���� ����
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

	// �� ������ ���� ������ ������ �̸��� ȭ�� ���� �������Ѵ�.
	if (mProducer)
	{
		mGlowingBlack.AlphaBlend(hdc, 0, 0, 800, 1200, 0, 0, 800, 1200, IALPHA);

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
	if ((int)mTwinkleCnt % 3 != 0 && !mProducer)
		TextOut(hdc, mPos.x, mPos.y, L"��", 1);

	SelectObject(hdc, oldFont);
	DeleteObject(hFont);
}
void Menu::Update(float elapsedTime)
{
	mTwinkleCnt += elapsedTime * 3.f;

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
	mBackground = make_shared<IntroGameObject>();
	mBackground->Init(L"images\\intro\\Instruction_Background2.png", Vector2{ 0.f, 0.f });

	mMenu = make_shared<Menu>();
	mMenu->Init();

	mLogo = make_shared<Logo>();
	mLogo->Init();

	{
		const RECT rectWindow = sceneManager->GetRectDisplay();

		std::shared_ptr<Cloud> cloud1 = make_shared<Cloud>();
		cloud1->SetMove(Vector2{ 40, 0 });
		cloud1->SetRectWindow(rectWindow);
		cloud1->Init(L"images\\intro\\Instruction_Cloud1.bmp", Vector2{ FIRSTCLOUD_X, FIRSTCLOUD_Y });
		mClouds.emplace_back(cloud1);

		std::shared_ptr<Cloud> cloud2 = make_shared<Cloud>();
		cloud2->Init(L"images\\intro\\Instruction_Cloud2.bmp", Vector2{ SECONDCLOUD_X, SECONDCLOUD_Y });
		cloud2->SetMove(Vector2{ 20, 0 });
		cloud2->SetRectWindow(rectWindow);
		mClouds.emplace_back(cloud2);

		std::shared_ptr<Cloud> cloud3 = make_shared<Cloud>();
		cloud3->Init(L"images\\intro\\Instruction_Cloud3.bmp", Vector2{ THIRDCLOUD_X, THIRDCLOUD_Y });
		cloud3->SetMove(Vector2{ 10, 0 });
		cloud3->SetRectWindow(rectWindow);
		mClouds.emplace_back(cloud3);

		std::shared_ptr<Cloud> cloud4 = make_shared<Cloud>();
		cloud4->Init(L"images\\intro\\Instruction_Cloud4.bmp", Vector2{ FOURTHCLOUD_X, FOURTHCLOUD_Y });
		cloud4->SetMove(Vector2{ 40, 0 });
		cloud4->SetRectWindow(rectWindow);
		mClouds.emplace_back(cloud4);
	}
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

	//else if (scene == Scene::PhaseManager && sceneManager->IsLoading() == false)
	//{
	//	battle.Init();
	//	phase.fingerController(sceneManager->GetHwnd());
	//}
}

void Intro::Paint(HDC hdc)
{
	mBackground->Paint(hdc);
	for (auto& cloud : mClouds)
		cloud->Paint(hdc);
	mLogo->Paint(hdc);
	mMenu->Paint(hdc);
}