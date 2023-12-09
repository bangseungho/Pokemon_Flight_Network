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
extern atomic<bool> isEndBattle;

extern CImage glowing_black;

extern bool flag = false;

// 렌더링을 하기 위한 초기 작업으로 씬을 그리기 전에 호출해야함
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

// 렌더링을 마치기 위한 후반 작업으로 씬을 모두 그리고 호출해야함
void SceneManager::FinishPaint(const HWND& hWnd, PAINTSTRUCT& ps, HDC& hdc, HDC& memDC, HBITMAP& hBitmap, RECT& rectWindow) const
{
	BitBlt(hdc, 0, 0, rectWindow.right, rectWindow.bottom, memDC, 0, 0, SRCCOPY);
	ValidateRect(hWnd, NULL);
	DeleteDC(memDC);
	DeleteObject(hBitmap);
	EndPaint(hWnd, &ps);
}

// 다음 씬으로 넘어가면 이전 씬들의 타이머를 삭제해준다.
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
	case Scene::Battle: // 배틀 씬에서는 타이머 삭제뿐만 아니라 생성된 모든 객체들을 제거한다.
		//for (auto& member : mMemberMap)
		//	delete member.second;

		mMemberMap.clear();
		delete enemies;
		delete effects;
		delete boss;
		delete gui;

		enemies = nullptr;
		effects = nullptr;
		boss = nullptr;
		gui = nullptr;

		soundManager->StopEffectSound();
		soundManager->StopSkillSound();
		soundManager->StopBossSound();

		isEndBattle.store(true);
		break;
	}
}

// 씬을 로드하는 함수로 씬에서 필요한 애니메이션의 타이머를 작동한다.
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

		// 모든 멤버 렌더링
		for (const auto& member : GET_MEMBER_MAP)
			mMemberMap[member.first] = new Player(member.second.mSceneData.AirPokemon, member.second.mSceneData.LandPokemon, member.first);

		// 나의 인덱스가 바로 mPlayer
		mPlayer = mMemberMap[MY_INDEX];

		// 모든 멤버 초기화
		for (const auto& member : GET_MEMBER_MAP)
			mMemberMap[member.first]->Init();

		// gameData의 스테이지에 따라 등장하는 적 포켓몬 데이터(hp, 범위, 공격력 등)를 각각 세팅한다.
		enemies = new EnemyController();

		// 게임에 등장하는 이펙트(폭발 효과, 피격 효과 등(스테이지에 따라 다름))를 관리하는 이펙트 매니저를 생성
		effects = new EffectManager();

		// gameData의 스테이지에 따라 등장하는 적 보스 포켓몬을 생성하며 스테이지에 따라 각각 세팅한다.
		boss = new Boss();

		// 게임에서 필요한 GUI(스킬, 맵 거리 등) 매니저를 생성한다.
		gui = new GUIManager(rectWindow);

		rectDisplay = gui->GetRectDisplay();
		break;
	}

	// 네트워크에 연결이 되어 있다면 씬 정보를 송신한다.
	if (GET_SINGLE(Network)->IsConnected()) {
		SceneData sendData{ GET_SINGLE(Network)->GetClientIndex(), 0, static_cast<uint8>(crntScene), gameData.ClearRecord, stage.GetAirPokemon(), stage.GetLandPokemon() };
		GET_SINGLE(Network)->SendDataAndType(sendData);
	}
	InvalidateRect(mHwnd, NULL, false);
}

// 씬 매니저 생성자에서는 사운드 매니저를 생성하고 현재 씬에서 필요한 이미지를 로드한다.
SceneManager::SceneManager()
{
	soundManager = new SoundManager();
	crntScene = Scene::Intro;
}

// 스크린 사이즈를 받아오고 crntScene(현재 씬)에 따라서 씬을 로드한다.
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

void SceneManager::Move()
{
	GET_SINGLE(GameTimer)->Move();

	switch (crntScene) {
	case Scene::Intro:
		intro.Move(DELTA_TIME);
		break;
	case Scene::Town:
		town.Move(DELTA_TIME);
		break;
	case Scene::Stage:
		stage.Move(DELTA_TIME);
		break;
	case Scene::PhaseManager:
		phase.Move(DELTA_TIME);
		break;
	case Scene::Battle:
		battle.Move(DELTA_TIME);
		break;
	}

	T_Loading(DELTA_TIME);
	T_Loadingbar(DELTA_TIME);

	Paint();
}

// 현재 씬을 렌더링한다.
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

		for (auto& member : mMemberMap)
			member.second->Paint(memDC);

		enemies->Paint(memDC);

		for (auto& member : mMemberMap)
			member.second->PaintSkill(memDC);

		effects->Paint(memDC);
		gui->Paint(memDC);
		break;
	}

	// 씬 매니저는 isLoading 값을 가지고 있으며 true이면 로딩 화면을 렌더링한다.
	if (IsLoading() == true)
	{
		loading.Paint(memDC, mHwnd, rectWindow);
	}

	FinishPaint(mHwnd, ps, hdc, memDC, hBitmap, rectWindow);
}

// 씬 인자값을 받아서 이전 씬과 현재 씬을 업데이트 하고 현재 씬을 로드한다.
void SceneManager::MoveScene(const HWND& hWnd, Scene scene)
{
	DeleteScene(hWnd);
	prevScene = crntScene;
	crntScene = scene;
	LoadScene(hWnd);
}

// 다음 씬으로 넘어갈 때 로딩 화면을 띄우기 위한 함수로 로딩 화면에 필요한 애니메이션 타이머를 작동한다.
void SceneManager::StartLoading(const HWND& hWnd)
{
	isLoading = true;
	loading.ResetLoading();
}

// 로딩 화면을 끝내기 위한 함수로 로딩 화면에 필요한 애니메이션 타이머를 삭제한다.
void SceneManager::StopLoading(const HWND& hWnd)
{
	isLoading = false;
}

unordered_map<uint8, class Player*>& SceneManager::GetMemberMap()
{
	return mMemberMap;
}


// 로딩 생성자에서는 로딩 화면에 필요한 이미지를 로드한다.
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

// 로딩 화면을 렌더링
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

// 로딩 바는 총 5개의 이미지 배열이며 타이머의 시간에 따라서 _loading_bar_cnt 값을 올리게 되면 해당 인덱스의 로드 바를 렌더링한다.
void Loading::Load(const HWND& hWnd)
{
	if (++_loading_bar_cnt > 4)
	{
		_loading_bar_cnt = 4;
	}
	InvalidateRect(hWnd, NULL, false);
}