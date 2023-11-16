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
extern Player* player;
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

extern Cloud cloud[4];
extern Logo logo;
extern Menu menu;
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
		KillTimer(hWnd, TIMERID_CLOUD);
		KillTimer(hWnd, TIMERID_LOGO);
		KillTimer(hWnd, TIMERID_ANIMATION);
		KillTimer(hWnd, TIMERID_TWINKLEEMOTION);
		break;
	case Scene::Town:
		KillTimer(hWnd, TIMERID_TPANIMATION);
		KillTimer(hWnd, TIMERID_TPANIMATION_DIR);
		KillTimer(hWnd, TIMERID_NPCMOTION);
		break;
	case Scene::Stage:
		KillTimer(hWnd, TIMERID_TARGETMOVE);
		KillTimer(hWnd, TIMERID_SelectPokemonMove);
		KillTimer(hWnd, TIMERID_TWINKLEEMOTION);
		break;
	case Scene::PhaseManager:
		KillTimer(hWnd, TIMERID_ANIMATION);
		break;
	case Scene::Battle: // 배틀 씬에서는 타이머 삭제뿐만 아니라 생성된 모든 객체들을 제거한다.
		KillTimer(hWnd, TIMERID_BATTLE_INVALIDATE);
		KillTimer(hWnd, TIMERID_BATTLE_ANIMATION);
		KillTimer(hWnd, TIMERID_BATTLE_EFFECT);
		KillTimer(hWnd, TIMERID_BATTLE_GUI);
		KillTimer(hWnd, TIMERID_BATTLE_MOVE_PLAYER);
		KillTimer(hWnd, TIMERID_BATTLE_ANIMATION_BOSS);

		delete player;
		delete enemies;
		delete effects;
		delete boss;
		delete gui;

		player = nullptr;
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

// 씬을 로드하는 함수로 씬에서 필요한 애니메이션의 타이머를 작동한다.
void SceneManager::LoadScene(const HWND& hWnd)
{
	switch (crntScene)
	{
	case Scene::Intro:
		SetTimer(hWnd, TIMERID_CLOUD, ELAPSE_CLOUD, T_MoveCloud); // 구름 이동 타이머
		SetTimer(hWnd, TIMERID_LOGO, ELAPSE_LOGO, T_MoveLogo); // 로고 움직임 타이머
		SetTimer(hWnd, TIMERID_ANIMATION, ELAPSE_ANIMATION, T_Animation); // 인트로 화면 핑거 컨트롤러 타이머
		SetTimer(hWnd, TIMERID_TWINKLEEMOTION, ELAPSE_TWINKLEEMOTION, T_TwinkleEmotion); // 핑거 컨트롤러 깜빡거리는 효과 타이머

		soundManager->StopBGMSound();
		soundManager->PlayBGMSound(BGMSound::Intro, 1.0f, true);
		break;
	case Scene::Town:
	{
		town.Init(rectWindow);
		SetTimer(hWnd, TIMERID_TPANIMATION, ELAPSE_TPANIMATION, T_TPAnimation); // 플레이어 움직임 타이머
		SetTimer(hWnd, TIMERID_TPANIMATION_DIR, ELAPSE_TPANIMATION_DIR, T_TPAnimationDir); // 플레이어 방향 타이머
		SetTimer(hWnd, TIMERID_NPCMOTION, ELAPSE_NPCMOTION, T_NpcMotion); // NPC 움직임 타이머

		soundManager->StopBGMSound();
		const int randBgm = rand() % 2;
		if (randBgm == 0)
		{
			soundManager->PlayBGMSound(BGMSound::Town1, 1.0f, true);
		}
		else
		{
			soundManager->PlayBGMSound(BGMSound::Town2, 1.0f, true);
		}
	}
		break;
	case Scene::Stage:
		stage.Init(rectWindow);
		SetTimer(hWnd, TIMERID_TARGETMOVE, ELAPSE_TARGETMOVE, T_TargetMove); // 스테이지를 고르기 위한 타겟의 움직임 타이머
		SetTimer(hWnd, TIMERID_SelectPokemonMove, ELAPSE_SelectPokemonMove, T_SelectPokemonMove); // 포켓몬 선택창 타이머
		SetTimer(hWnd, TIMERID_TWINKLEEMOTION, ELAPSE_TWINKLEEMOTION, T_TwinkleEmotion); // 핑거 컨트롤러 깜빡이는 효과 타이머

		if (prevScene != Scene::PhaseManager)
		{
			soundManager->StopBGMSound();
			if (isEnding == true)
			{
				soundManager->PlayBGMSound(BGMSound::Ending, 1.0f, true);
			}
			else
			{
				soundManager->PlayBGMSound(BGMSound::Stage, 1.0f, true);
			}
		}
		break;
	case Scene::PhaseManager:
		SetTimer(hWnd, TIMERID_ANIMATION, ELAPSE_ANIMATION, T_Animation); // 페이즈 화면 핑거 컨트롤러 타이머
		phase.Init();
		
		if (prevScene == Scene::Battle)
		{
			soundManager->StopBGMSound();
			if (isEnding == true)
			{
				soundManager->PlayBGMSound(BGMSound::Ending, 1.0f, true);
			}
			else
			{
				soundManager->PlayBGMSound(BGMSound::Stage, 1.0f, true);
			}
		}
		break;
	case Scene::Battle: 
		gameData.stage = stage.GetStage();

		// 스테이지 씬(캐릭터 창)에서 고른 포켓몬 정보를 가져와서 해당 포켓몬으로 플레이어를 생성한다.
		player = new Player(stage.GetAirPokemon(), stage.GetLandPokemon());

		// 플레이어의 스킬 매니저를 생성한다.
		player->Init();

		// gameData의 스테이지에 따라 등장하는 적 포켓몬 데이터(hp, 범위, 공격력 등)를 각각 세팅한다.
		enemies = new EnemyController();

		// 게임에 등장하는 이펙트(폭발 효과, 피격 효과 등(스테이지에 따라 다름))를 관리하는 이펙트 매니저를 생성
		effects = new EffectManager();

		// gameData의 스테이지에 따라 등장하는 적 보스 포켓몬을 생성하며 스테이지에 따라 각각 세팅한다.
		boss = new Boss();

		// 게임에서 필요한 GUI(스킬, 맵 거리 등) 매니저를 생성한다.
		gui = new GUIManager(rectWindow);

		rectDisplay = gui->GetRectDisplay();

		SetTimer(hWnd, TIMERID_BATTLEMAPMOVE, ELAPSE_BATTLEMAPMOVE, T_Battle_MapMove); // 종스크롤 맵 움직임 타이머
		SetTimer(hWnd, TIMERID_BATTLE_INVALIDATE, ELAPSE_BATTLE_INVALIDATE, T_Battle_Invalidate); // 충돌처리 또는 위치 이동 타이머
		SetTimer(hWnd, TIMERID_BATTLE_ANIMATION, ELAPSE_BATTLE_ANIMATION, T_Battle_Animate); // 플레이어, 적, 보스 스킬 애니메이션 타이머
		SetTimer(hWnd, TIMERID_BATTLE_EFFECT, ELAPSE_BATTLE_EFFECT, T_Battle_Effect); // 전투 이펙트 효과 타이머
		SetTimer(hWnd, TIMERID_BATTLE_GUI, ELAPSE_BATTLE_GUI, T_Battle_GUI); // GUI 타이머
		SetTimer(hWnd, TIMERID_BATTLE_ANIMATION_BOSS, ELAPSE_BATTLE_ANIMATION_BOSS, T_Battle_AnimateBoss); // 보스 애니메이션 타이머

		soundManager->StopBGMSound();
		soundManager->PlayBGMSound(BGMSound::Battle, 1.0f, true);
		soundManager->PlayEffectSound(EffectSound::Shot, 0.5f, true);
		break;
	}
}

// 씬 매니저 생성자에서는 사운드 매니저를 생성하고 현재 씬에서 필요한 이미지를 로드한다.
SceneManager::SceneManager()
{
	soundManager = new SoundManager();

	crntScene = Scene::Intro;
	cloud[0].Init(L"images\\intro\\Instruction_Cloud1.bmp", FIRSTCLOUD_X, FIRSTCLOUD_Y);
	cloud[1].Init(L"images\\intro\\Instruction_Cloud2.bmp", SECONDCLOUD_X, SECONDCLOUD_Y);
	cloud[2].Init(L"images\\intro\\Instruction_Cloud3.bmp", THIRDCLOUD_X, THIRDCLOUD_Y);
	cloud[3].Init(L"images\\intro\\Instruction_Cloud4.bmp", FOURTHCLOUD_X, FOURTHCLOUD_Y);
}

// 스크린 사이즈를 받아오고 crntScene(현재 씬)에 따라서 씬을 로드한다.
void SceneManager::Init(const HWND& hWnd)
{
	GetClientRect(hWnd, &rectWindow);
	rectDisplay = rectWindow;

	GET_SINGLE(Network)->Init("127.0.0.1");

	LoadScene(hWnd);
}

// 현재 씬을 렌더링한다.
void SceneManager::Paint(const HWND& hWnd)
{
	PAINTSTRUCT ps;
	HDC hdc, memDC;
	HBITMAP hBitmap;

	StartPaint(hWnd, ps, hdc, memDC, hBitmap, rectWindow);

	switch (crntScene)
	{
	case Scene::Intro:
		intro.Paint(memDC, rectWindow);

		for (int i = 0; i < CLOUD_NUM; i++)
		{
			cloud[i].Paint(memDC, rectWindow);
		}

		logo.Paint(memDC);

		menu.Paint(memDC, hWnd);
		break;
	case Scene::Town:
		town.Paint(memDC, rectWindow);
		break;
	case Scene::Stage:
		stage.Paint(memDC, rectWindow, menu);
		break;
	case Scene::PhaseManager:
		phase.Paint(memDC, rectWindow, stage.GetStage());
		break;
	case Scene::Battle:
		battle.Paint(memDC, rectWindow, stage.GetStage());
		boss->Paint(memDC);
		player->Paint(memDC);
		enemies->Paint(memDC);
		player->PaintSkill(memDC);
		effects->Paint(memDC);
		gui->Paint(memDC);
		break;
	}

	// 씬 매니저는 isLoading 값을 가지고 있으며 true이면 로딩 화면을 렌더링한다.
	if (IsLoading() == true)
	{
		loading.Paint(memDC, hWnd, rectWindow);
	}

	FinishPaint(hWnd, ps, hdc, memDC, hBitmap, rectWindow);
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
	SetTimer(hWnd, TIMERID_LOADING, ELAPSE_LOADING, T_Loading);
	SetTimer(hWnd, TIMERID_LOADINGBAR, ELAPSE_LOADINGBAR, T_Loadingbar);
}

// 로딩 화면을 끝내기 위한 함수로 로딩 화면에 필요한 애니메이션 타이머를 삭제한다.
void SceneManager::StopLoading(const HWND& hWnd)
{
	isLoading = false;
	KillTimer(hWnd, TIMERID_LOADING);
	KillTimer(hWnd, TIMERID_LOADINGBAR);
}

#define LOADING_POKEMON_X 150
#define LOADING_POKEMON_Y 121

#define HALF_RECTWINDOW_X 175 
#define HALF_RECTWINDOW_Y 280

#define LOADING_BAR_X 182
#define LOADING_BAR_Y 397

#define ALPHA 220

// 로딩 생성자에서는 로딩 화면에 필요한 이미지를 로드한다.
Loading::Loading()
{
	_loding_pokemon_rectImage = { 0, 0, LOADING_POKEMON_X, LOADING_POKEMON_Y };
	_loding_pokemon_rectDraw = { HALF_RECTWINDOW_X, HALF_RECTWINDOW_Y, LOADING_POKEMON_X, LOADING_POKEMON_Y };

	_glowing_black.Load(L"images\\loading\\Loading_Black_background.bmp");
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
	_glowing_black.AlphaBlend(hdc, rectWindow, rectWindow, ALPHA);
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