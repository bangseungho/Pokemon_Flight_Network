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
	case Scene::Battle: // ��Ʋ �������� Ÿ�̸� �����Ӹ� �ƴ϶� ������ ��� ��ü���� �����Ѵ�.
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

// ���� �ε��ϴ� �Լ��� ������ �ʿ��� �ִϸ��̼��� Ÿ�̸Ӹ� �۵��Ѵ�.
void SceneManager::LoadScene(const HWND& hWnd)
{
	switch (crntScene)
	{
	case Scene::Intro:
		SetTimer(hWnd, TIMERID_CLOUD, ELAPSE_CLOUD, T_MoveCloud); // ���� �̵� Ÿ�̸�
		SetTimer(hWnd, TIMERID_LOGO, ELAPSE_LOGO, T_MoveLogo); // �ΰ� ������ Ÿ�̸�
		SetTimer(hWnd, TIMERID_ANIMATION, ELAPSE_ANIMATION, T_Animation); // ��Ʈ�� ȭ�� �ΰ� ��Ʈ�ѷ� Ÿ�̸�
		SetTimer(hWnd, TIMERID_TWINKLEEMOTION, ELAPSE_TWINKLEEMOTION, T_TwinkleEmotion); // �ΰ� ��Ʈ�ѷ� �����Ÿ��� ȿ�� Ÿ�̸�

		soundManager->StopBGMSound();
		soundManager->PlayBGMSound(BGMSound::Intro, 1.0f, true);
		break;
	case Scene::Town:
	{
		town.Init(rectWindow);
		SetTimer(hWnd, TIMERID_TPANIMATION, ELAPSE_TPANIMATION, T_TPAnimation); // �÷��̾� ������ Ÿ�̸�
		SetTimer(hWnd, TIMERID_TPANIMATION_DIR, ELAPSE_TPANIMATION_DIR, T_TPAnimationDir); // �÷��̾� ���� Ÿ�̸�
		SetTimer(hWnd, TIMERID_NPCMOTION, ELAPSE_NPCMOTION, T_NpcMotion); // NPC ������ Ÿ�̸�

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
		SetTimer(hWnd, TIMERID_TARGETMOVE, ELAPSE_TARGETMOVE, T_TargetMove); // ���������� ���� ���� Ÿ���� ������ Ÿ�̸�
		SetTimer(hWnd, TIMERID_SelectPokemonMove, ELAPSE_SelectPokemonMove, T_SelectPokemonMove); // ���ϸ� ����â Ÿ�̸�
		SetTimer(hWnd, TIMERID_TWINKLEEMOTION, ELAPSE_TWINKLEEMOTION, T_TwinkleEmotion); // �ΰ� ��Ʈ�ѷ� �����̴� ȿ�� Ÿ�̸�

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
		SetTimer(hWnd, TIMERID_ANIMATION, ELAPSE_ANIMATION, T_Animation); // ������ ȭ�� �ΰ� ��Ʈ�ѷ� Ÿ�̸�
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

		// �������� ��(ĳ���� â)���� �� ���ϸ� ������ �����ͼ� �ش� ���ϸ����� �÷��̾ �����Ѵ�.
		player = new Player(stage.GetAirPokemon(), stage.GetLandPokemon());

		// �÷��̾��� ��ų �Ŵ����� �����Ѵ�.
		player->Init();

		// gameData�� ���������� ���� �����ϴ� �� ���ϸ� ������(hp, ����, ���ݷ� ��)�� ���� �����Ѵ�.
		enemies = new EnemyController();

		// ���ӿ� �����ϴ� ����Ʈ(���� ȿ��, �ǰ� ȿ�� ��(���������� ���� �ٸ�))�� �����ϴ� ����Ʈ �Ŵ����� ����
		effects = new EffectManager();

		// gameData�� ���������� ���� �����ϴ� �� ���� ���ϸ��� �����ϸ� ���������� ���� ���� �����Ѵ�.
		boss = new Boss();

		// ���ӿ��� �ʿ��� GUI(��ų, �� �Ÿ� ��) �Ŵ����� �����Ѵ�.
		gui = new GUIManager(rectWindow);

		rectDisplay = gui->GetRectDisplay();

		SetTimer(hWnd, TIMERID_BATTLEMAPMOVE, ELAPSE_BATTLEMAPMOVE, T_Battle_MapMove); // ����ũ�� �� ������ Ÿ�̸�
		SetTimer(hWnd, TIMERID_BATTLE_INVALIDATE, ELAPSE_BATTLE_INVALIDATE, T_Battle_Invalidate); // �浹ó�� �Ǵ� ��ġ �̵� Ÿ�̸�
		SetTimer(hWnd, TIMERID_BATTLE_ANIMATION, ELAPSE_BATTLE_ANIMATION, T_Battle_Animate); // �÷��̾�, ��, ���� ��ų �ִϸ��̼� Ÿ�̸�
		SetTimer(hWnd, TIMERID_BATTLE_EFFECT, ELAPSE_BATTLE_EFFECT, T_Battle_Effect); // ���� ����Ʈ ȿ�� Ÿ�̸�
		SetTimer(hWnd, TIMERID_BATTLE_GUI, ELAPSE_BATTLE_GUI, T_Battle_GUI); // GUI Ÿ�̸�
		SetTimer(hWnd, TIMERID_BATTLE_ANIMATION_BOSS, ELAPSE_BATTLE_ANIMATION_BOSS, T_Battle_AnimateBoss); // ���� �ִϸ��̼� Ÿ�̸�

		soundManager->StopBGMSound();
		soundManager->PlayBGMSound(BGMSound::Battle, 1.0f, true);
		soundManager->PlayEffectSound(EffectSound::Shot, 0.5f, true);
		break;
	}
}

// �� �Ŵ��� �����ڿ����� ���� �Ŵ����� �����ϰ� ���� ������ �ʿ��� �̹����� �ε��Ѵ�.
SceneManager::SceneManager()
{
	soundManager = new SoundManager();

	crntScene = Scene::Intro;
	cloud[0].Init(L"images\\intro\\Instruction_Cloud1.bmp", FIRSTCLOUD_X, FIRSTCLOUD_Y);
	cloud[1].Init(L"images\\intro\\Instruction_Cloud2.bmp", SECONDCLOUD_X, SECONDCLOUD_Y);
	cloud[2].Init(L"images\\intro\\Instruction_Cloud3.bmp", THIRDCLOUD_X, THIRDCLOUD_Y);
	cloud[3].Init(L"images\\intro\\Instruction_Cloud4.bmp", FOURTHCLOUD_X, FOURTHCLOUD_Y);
}

// ��ũ�� ����� �޾ƿ��� crntScene(���� ��)�� ���� ���� �ε��Ѵ�.
void SceneManager::Init(const HWND& hWnd)
{
	GetClientRect(hWnd, &rectWindow);
	rectDisplay = rectWindow;

	GET_SINGLE(Network)->Init("127.0.0.1");

	LoadScene(hWnd);
}

// ���� ���� �������Ѵ�.
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

	// �� �Ŵ����� isLoading ���� ������ ������ true�̸� �ε� ȭ���� �������Ѵ�.
	if (IsLoading() == true)
	{
		loading.Paint(memDC, hWnd, rectWindow);
	}

	FinishPaint(hWnd, ps, hdc, memDC, hBitmap, rectWindow);
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
	SetTimer(hWnd, TIMERID_LOADING, ELAPSE_LOADING, T_Loading);
	SetTimer(hWnd, TIMERID_LOADINGBAR, ELAPSE_LOADINGBAR, T_Loadingbar);
}

// �ε� ȭ���� ������ ���� �Լ��� �ε� ȭ�鿡 �ʿ��� �ִϸ��̼� Ÿ�̸Ӹ� �����Ѵ�.
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

// �ε� �����ڿ����� �ε� ȭ�鿡 �ʿ��� �̹����� �ε��Ѵ�.
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

// �ε� ȭ���� ������
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

// �ε� �ٴ� �� 5���� �̹��� �迭�̸� Ÿ�̸��� �ð��� ���� _loading_bar_cnt ���� �ø��� �Ǹ� �ش� �ε����� �ε� �ٸ� �������Ѵ�.
void Loading::Load(const HWND& hWnd)
{
	if (++_loading_bar_cnt > 4)
	{
		_loading_bar_cnt = 4;
	}
	InvalidateRect(hWnd, NULL, false);
}