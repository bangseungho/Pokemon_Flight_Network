#include "stdafx.h"
#include "scene.h"
#include "interface.h"
#include "player.h"
#include "enemy.h"
#include "effect.h"
#include "boss.h"
#include "timer.h"
#include "sound.h"
#include "Network.h"

#include "intro.h"
#include "town.h"
#include "stage.h"
#include "phase.h"
#include "battle.h"
#include "gameTimer.h"

#define CLOUD_NUM 4
#define FIRSTCLOUD_X 125
#define FIRSTCLOUD_Y 187
#define SECONDCLOUD_X 187
#define SECONDCLOUD_Y 31
#define THIRDCLOUD_X -62
#define THIRDCLOUD_Y 93
#define FOURTHCLOUD_X 437
#define FOURTHCLOUD_Y 187

extern GameData gameData;
extern Player* mPlayer;
extern EnemyController* enemies;
extern EffectManager* effects;
extern GUIManager* gui;
extern Boss* boss;
extern SoundManager* soundManager;

extern Intro intro;
extern Loading loading;
extern Town town;
extern Stage stage;
extern PhaseManager phase;
extern Battle battle;

extern CImage glowing_black;

extern bool flag = false;

// �������� �ϱ� ���� �ʱ� �۾����� ���� �׸��� ���� ȣ���ؾ���
void SceneManager::StartPaint(const HWND& hWnd, PAINTSTRUCT& ps, HDC& hdc, HDC& memDC, HBITMAP& hBitmap, RECT& rectWindow) const
{
	hdc = BeginPaint(hWnd, &ps);
	memDC = CreateCompatibleDC(hdc);
	hBitmap = CreateCompatibleBitmap(hdc, rectWindow.right, rectWindow.bottom);
	SelectObject(memDC, hBitmap);
	SelectObject(memDC, GetStockObject(WHITE_BRUSH));
	Rectangle(memDC, 0, 0, rectWindow.right, rectWindow.bottom);
	SetStretchBltMode(hdc, COLORONCOLOR);
	SetStretchBltMode(memDC, COLORONCOLOR);
}

// �������� ��ġ�� ���� �Ĺ� �۾����� ���� ��� �׸��� ȣ���ؾ���
void SceneManager::FinishPaint(const HWND& hWnd, PAINTSTRUCT& ps, HDC& hdc, HDC& memDC, HBITMAP& hBitmap, RECT& rectWindow) const
{
	BitBlt(hdc, 0, 0, rectWindow.right, rectWindow.bottom, memDC, 0, 0, SRCCOPY);
	ValidateRect(hWnd, NULL);
	DeleteDC(memDC);
	DeleteObject(hBitmap);
	EndPaint(hWnd, &ps);
}

// ���� ������ �Ѿ�� ���� ������ Ÿ�̸Ӹ� �������ش�.
void SceneManager::DeleteScene(const HWND& hWnd)
{
	switch (crntScene)
	{
	case Scene::Intro:
		break;
	case Scene::Town:
		break;
	case Scene::Stage:
		break;
	case Scene::PhaseManager:
		break;
	case Scene::Battle: // ��Ʋ �������� Ÿ�̸� �����Ӹ� �ƴ϶� ������ ��� ��ü���� �����Ѵ�.
		delete mPlayer;
		delete enemies;
		delete effects;
		delete boss;
		delete gui;

		mPlayer = nullptr;
		enemies = nullptr;
		effects = nullptr;
		boss = nullptr;
		gui = nullptr;

		soundManager->StopEffectSound();
		soundManager->StopSkillSound();
		soundManager->StopBossSound();
		break;
	}
}

// ���� �ε��ϴ� �Լ��� ������ �ʿ��� �ִϸ��̼��� Ÿ�̸Ӹ� �۵��Ѵ�.
void SceneManager::LoadScene(const HWND& hWnd)
{
	switch (crntScene)
	{
	case Scene::Intro:
		intro.Init();
		break;
	case Scene::Town:
		town.Init();
		break;
	case Scene::Stage:
		stage.Init();
		break;
	case Scene::PhaseManager:
		phase.Init();
		break;
	case Scene::Battle: 
		battle.Init();

		gameData.stage = stage.GetStage();

		// �������� ��(ĳ���� â)���� �� ���ϸ� ������ �����ͼ� �ش� ���ϸ����� �÷��̾ �����Ѵ�.
		mPlayer = new Player(stage.GetAirPokemon(), stage.GetLandPokemon());

		// �÷��̾��� ��ų �Ŵ����� �����Ѵ�.
		mPlayer->Init();

		// gameData�� ���������� ���� �����ϴ� �� ���ϸ� ������(hp, ����, ���ݷ� ��)�� ���� �����Ѵ�.
		enemies = new EnemyController();

		// ���ӿ� �����ϴ� ����Ʈ(���� ȿ��, �ǰ� ȿ�� ��(���������� ���� �ٸ�))�� �����ϴ� ����Ʈ �Ŵ����� ����
		effects = new EffectManager();

		// gameData�� ���������� ���� �����ϴ� �� ���� ���ϸ��� �����ϸ� ���������� ���� ���� �����Ѵ�.
		boss = new Boss();

		// ���ӿ��� �ʿ��� GUI(��ų, �� �Ÿ� ��) �Ŵ����� �����Ѵ�.
		gui = new GUIManager(rectWindow);

		rectDisplay = gui->GetRectDisplay();
		break;
	}

	// ��Ʈ��ũ�� ������ �Ǿ� �ִٸ� �� ������ �۽��Ѵ�.
	if (GET_SINGLE(Network)->IsConnected()) {
		SceneData sendData{ GET_SINGLE(Network)->GetClientIndex(), 0, static_cast<uint8>(crntScene), gameData.ClearRecord, stage.GetAirPokemon(), stage.GetLandPokemon() };
		GET_SINGLE(Network)->SendDataAndType(sendData);
	}
	InvalidateRect(mHwnd, NULL, false);
}

// �� �Ŵ��� �����ڿ����� ���� �Ŵ����� �����ϰ� ���� ������ �ʿ��� �̹����� �ε��Ѵ�.
SceneManager::SceneManager()
{
	soundManager = new SoundManager();
	crntScene = Scene::Intro;
}

// ��ũ�� ����� �޾ƿ��� crntScene(���� ��)�� ���� ���� �ε��Ѵ�.
void SceneManager::Init(const HWND& hWnd)
{
	GetClientRect(hWnd, &rectWindow);
	rectDisplay = rectWindow;
	mHwnd = hWnd;

	GET_SINGLE(GameTimer)->Init();
	GET_SINGLE(GameTimer)->Start();
	GET_SINGLE(Network)->Init("127.0.0.1");

	LoadScene(hWnd);
}

void SceneManager::Update()
{
	GET_SINGLE(GameTimer)->Update();

	switch (crntScene){
	case Scene::Intro:
		intro.Update(DELTA_TIME);
		break;
	case Scene::Town:
		town.Update(DELTA_TIME);
		break;
	case Scene::Stage:
		stage.Update(DELTA_TIME);
		break;
	case Scene::PhaseManager:
		phase.Update(DELTA_TIME);
		break;
	case Scene::Battle:
		battle.Update(DELTA_TIME);
		break;
	}

	T_Loading(DELTA_TIME);
	T_Loadingbar(DELTA_TIME);

	Paint();
}

// ���� ���� �������Ѵ�.
void SceneManager::Paint()
{
	PAINTSTRUCT ps;
	HDC hdc, memDC;
	HBITMAP hBitmap;

	StartPaint(mHwnd, ps, hdc, memDC, hBitmap, rectWindow);

	switch (crntScene)
	{
	case Scene::Intro:
		intro.Paint(memDC);
		break;
	case Scene::Town:
		town.Paint(memDC, rectWindow);
		break;
	case Scene::Stage:
		stage.Paint(memDC, rectWindow);
		break;
	case Scene::PhaseManager:
		phase.Paint(memDC, rectWindow, stage.GetStage());
		break;
	case Scene::Battle:
		battle.Paint(memDC, rectWindow, stage.GetStage());
		boss->Paint(memDC);
		mPlayer->Paint(memDC);
		enemies->Paint(memDC);
		mPlayer->PaintSkill(memDC);
		effects->Paint(memDC);
		gui->Paint(memDC);
		break;
	}

	// �� �Ŵ����� isLoading ���� ������ ������ true�̸� �ε� ȭ���� �������Ѵ�.
	if (IsLoading() == true)
	{
		loading.Paint(memDC, mHwnd, rectWindow);
	}

	FinishPaint(mHwnd, ps, hdc, memDC, hBitmap, rectWindow);
}

// �� ���ڰ��� �޾Ƽ� ���� ���� ���� ���� ������Ʈ �ϰ� ���� ���� �ε��Ѵ�.
void SceneManager::MoveScene(const HWND& hWnd, Scene scene)
{
	DeleteScene(hWnd);
	prevScene = crntScene;
	crntScene = scene;
	LoadScene(hWnd);
}

// ���� ������ �Ѿ �� �ε� ȭ���� ���� ���� �Լ��� �ε� ȭ�鿡 �ʿ��� �ִϸ��̼� Ÿ�̸Ӹ� �۵��Ѵ�.
void SceneManager::StartLoading(const HWND& hWnd)
{
	isLoading = true;
	loading.ResetLoading();
}

// �ε� ȭ���� ������ ���� �Լ��� �ε� ȭ�鿡 �ʿ��� �ִϸ��̼� Ÿ�̸Ӹ� �����Ѵ�.
void SceneManager::StopLoading(const HWND& hWnd)
{
	isLoading = false;
}


// �ε� �����ڿ����� �ε� ȭ�鿡 �ʿ��� �̹����� �ε��Ѵ�.
Loading::Loading()
{
	_loding_pokemon_rectImage = { 0, 0, LOADING_POKEMON_X, LOADING_POKEMON_Y };
	_loding_pokemon_rectDraw = { HALF_RECTWINDOW_X, HALF_RECTWINDOW_Y, LOADING_POKEMON_X, LOADING_POKEMON_Y };

	mGlowingBlack.Load(L"images\\loading\\Loading_Black_background.bmp");
	_loading_pokemon.Load(L"images\\loading\\Loading_pokemon.png");
	_loading_bar[0].Load(L"images\\loading\\Loading_bar1.bmp");
	_loading_bar[1].Load(L"images\\loading\\Loading_bar2.bmp");
	_loading_bar[2].Load(L"images\\loading\\Loading_bar3.bmp");
	_loading_bar[3].Load(L"images\\loading\\Loading_bar4.bmp");
	_loading_bar[4].Load(L"images\\loading\\Loading_bar5.bmp");
}

// �ε� ȭ���� ������
void Loading::Paint(HDC hdc, HWND hWnd, const RECT& rectWindow)
{
	mGlowingBlack.AlphaBlend(hdc, rectWindow, rectWindow, SALPHA);
	_loading_pokemon.Draw(hdc, HALF_RECTWINDOW_X, HALF_RECTWINDOW_Y - 19, LOADING_POKEMON_X, LOADING_POKEMON_Y,
		_loding_pokemon_rectImage.left, _loding_pokemon_rectImage.top, _loding_pokemon_rectImage.right, _loding_pokemon_rectImage.bottom);

	_loading_bar[_loading_bar_cnt].TransparentBlt(hdc, LOADING_BAR_X, LOADING_BAR_Y, 125, 20, 0, 0, 1731, 286, RGB(0, 0, 0));

	HFONT hFont = CreateFont(31, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, VARIABLE_PITCH | FF_ROMAN, TEXT("ChubbyChoo-SemiBold"));
	HFONT oldFont = (HFONT)SelectObject(hdc, hFont);

	SetBkMode(hdc, TRANSPARENT);
	SetTextColor(hdc, RGB(255, 255, 255));
	TextOut(hdc, LOADING_BAR_X, LOADING_BAR_Y + 12, L"LOADING...", 10);

	SelectObject(hdc, oldFont);
	DeleteObject(hFont);
}

// �ε� �ٴ� �� 5���� �̹��� �迭�̸� Ÿ�̸��� �ð��� ���� _loading_bar_cnt ���� �ø��� �Ǹ� �ش� �ε����� �ε� �ٸ� �������Ѵ�.
void Loading::Load(const HWND& hWnd)
{
	if (++_loading_bar_cnt > 4)
	{
		_loading_bar_cnt = 4;
	}
	InvalidateRect(hWnd, NULL, false);
}