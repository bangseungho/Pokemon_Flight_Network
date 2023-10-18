#include "stdafx.h"
#include "interface.h"
#include "enemy.h"
#include "player.h"
#include "timer.h"
#include "effect.h"
#include "boss.h"
#include "scene.h"

extern Player* player;
extern EnemyController* enemies;
extern Boss* boss;
extern EffectManager* effects;
extern GUIManager* gui;
extern SceneManager* sceneManager;

void CALLBACK T_Battle_Invalidate(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	InvalidateRect(hWnd, NULL, FALSE);
	player->CheckShot();
	enemies->CreateCheckMelee();
	enemies->CreateCheckRange();
	enemies->CheckAttackDelay();
	boss->CheckActDelay();
	boss->CheckAttackDelay();

	player->MoveBullets();
	enemies->MoveBullets();
	enemies->Move();
	boss->Move();
}

void CALLBACK T_Battle_Animate(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	player->Animate(hWnd);
	enemies->Animate();
	boss->AnimateSkill();
}

void CALLBACK T_Battle_AnimateBoss(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	boss->Animate(hWnd);
}

void CALLBACK T_Battle_MovePlayer(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	player->Move(hWnd, TIMERID_BATTLE_MOVE_PLAYER);
}

void CALLBACK T_Battle_Effect(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	effects->Animate();
}
void CALLBACK T_Battle_GUI(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	gui->Update(hWnd);
}

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

extern Intro intro;
extern Loading loading;
extern Town town;
extern Stage stage;
extern Phase phase;
extern Battle battle;

extern Cloud cloud[4];
extern Logo logo;
extern Menu menu;
extern CImage glowing_black;

void CALLBACK T_MoveCloud(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	const RECT rectWindow = sceneManager->GetRectDisplay();

	// �������� �ӵ� �ٸ��� ����
	cloud[0].Move(4, 0, rectWindow);
	cloud[1].Move(2, 0, rectWindow);
	cloud[2].Move(1, 0, rectWindow);
	cloud[3].Move(4, 0, rectWindow);
}

// POKEMON FLIGHT�� ���Ÿ��� ������
void CALLBACK T_MoveLogo(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	static bool thiscnt = false;

	// �ΰ� �󸶳� ������������ Ȯ���� �� ���� ���� �ٽ� �ݴ�� �����̱�
	if (logo._logoMovingCnt > 10)
		thiscnt = true;
	else if (logo._logoMovingCnt == 0)
		thiscnt = false;

	if (thiscnt == true)
		logo._logoMovingCnt--;
	else
		logo._logoMovingCnt++;
}

void CALLBACK T_Animation(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	const Scene scene = sceneManager->GetScene();
	if (scene == Scene::Intro && sceneManager->IsLoading() == false)
		menu.fingerController(hWnd);
	else if (scene == Scene::Phase && sceneManager->IsLoading() == false)
	{
		battle.Init();
		phase.fingerController(hWnd);
	}
	InvalidateRect(hWnd, NULL, false);
}

void CALLBACK T_TwinkleEmotion(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	menu._finger_twinkle_cnt++;
}

#define LOADING_POKEMON_X 150
#define LOADING_POKEMON_Y 121
#define LOADING_POKEMON_MAX_X 2100
#define LOADING_POKEMON_MAX_Y 121

void CALLBACK T_Loading(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	const RECT rectWindow = sceneManager->GetRectWindow();
	const Scene scene = sceneManager->GetScene();

	// �ε� ��� ���� �� Ÿ�̸� ����
	if (loading.IsLoaded() == true)
	{
		// �ε� ȭ���� ���������� Ÿ�� ��� �ʱ�ȭ
		town.Init(rectWindow);

		stage.SelectPokemonInit();

		if (scene == Scene::Intro)// ���� ���� �÷ο� ���� ����ȭ���̶�� ���� ���� �÷ο�� Ÿ��
		{
			sceneManager->MoveScene(hWnd, Scene::Town);
		}
		else if (scene == Scene::Town) // ���� ���� �÷ο� ���� Ÿ���̶�� ���� ���� �÷ο�� ��������
		{
			town.player->_Pos.x = rectWindow.right / 2;
			town.player->_Pos.y = rectWindow.bottom / 2;
			town.player->_cam = { town.player->_Pos.x - town.GetCamSizeX(), rectWindow.top, town.player->_Pos.x + town.GetCamSizeX(), rectWindow.bottom };
			town._rectImage = rectWindow;

			if (town._nextFlow == Scene::Stage)
			{
				sceneManager->MoveScene(hWnd, Scene::Stage);
			}
			if (town._nextFlow == Scene::Intro)
			{
				sceneManager->MoveScene(hWnd, Scene::Intro);
			}
		}
		else if (scene == Scene::Stage)
		{
			if (stage.GetStage() == StageElement::Town) // Ÿ�� ������Ʈ Ÿ��
				sceneManager->MoveScene(hWnd, Scene::Town);
			else
				sceneManager->MoveScene(hWnd, Scene::Phase);

		}
		else if (scene == Scene::Phase)
		{
			sceneManager->MoveScene(hWnd, Scene::Battle);
		}

		else if (scene == Scene::Battle) // ���� ��Ʋ ������ �������� �Ѿ��
		{
			sceneManager->MoveScene(hWnd, Scene::Phase);
		}

		sceneManager->StopLoading(hWnd);
	}

	loading._loding_pokemon_rectImage.left += LOADING_POKEMON_X;

	if (loading._loding_pokemon_rectImage.left == LOADING_POKEMON_MAX_X)
	{
		loading._loding_pokemon_rectImage.left = 0;
		loading._loading_pokemon_cnt++;
	}
	InvalidateRect(hWnd, NULL, false);
}

void CALLBACK T_Loadingbar(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	loading.Load(hWnd);
}

// Ÿ� �ִ� �÷��̾� ������ Ÿ�̸�
void CALLBACK T_TPAnimation(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	const RECT rectWindow = sceneManager->GetRectWindow();
	const Scene scene = sceneManager->GetScene();

	town.player->_rectDraw = { town.player->_Pos.x - 20, town.player->_Pos.y - 20, town.player->_Pos.x + 20, town.player->_Pos.y + 20 };
	// ���� �÷ο찡 ���� Ÿ���϶��� ����� ������ Ȯ���ϴ� ������ ���� �ƴ� �� ������ �� �ֵ��� ����
	if (scene == Scene::Town && sceneManager->IsLoading() == false)
	{
		town.Move(rectWindow);
		InvalidateRect(hWnd, NULL, false);
	}

	// ���� ���� (��Ʈ �������� ���� ��) �Ѿ�� ���� �������� ����
	if (sceneManager->IsLoading() == false && town.player->_Pos.x + 20 >= rectWindow.right)
	{
		sceneManager->StartLoading(hWnd);
		town._nextFlow = Scene::Stage;
	}
	// ���� ���� (��Ʈ �������� �� ��) �Ѿ�� �� ����ȭ������ ����
	else if (sceneManager->IsLoading() == false && town.player->_Pos.x - 20 <= rectWindow.left)
	{
		sceneManager->StartLoading(hWnd);
		town._nextFlow = Scene::Intro;
	}
}

#define TPLAYER_IMAGESIZE_X 64
#define TPLAYER_MAX_IMAGESIZE_X 256

#define DIR_LEFT_IMAGE_Y 64
#define DIR_RIGHT_IMAGE_Y 128
#define DIR_UP_IMAGE_Y 192
#define DIR_DOWN_IMAGE_Y 0

// Ÿ� �ִ� �÷��̾� ������ ����
void CALLBACK T_TPAnimationDir(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	const Scene scene = sceneManager->GetScene();

	// Ÿ���� �÷��̾ ��� �������̸� �̹��� ��� ����(�ִϸ��̼�)
	if (scene == Scene::Town && town.player->_keepGoing == true)
	{
		town.player->_rectImage.left += TPLAYER_IMAGESIZE_X;

		if (town.player->_rectImage.left == TPLAYER_MAX_IMAGESIZE_X)
			town.player->_rectImage.left = 0;

		switch (town.player->_dir)
		{
		case Dir::Left:
			town.player->_rectImage.top = DIR_LEFT_IMAGE_Y;
			break;
		case Dir::Up:
			town.player->_rectImage.top = DIR_UP_IMAGE_Y;
			break;
		case Dir::Right:
			town.player->_rectImage.top = DIR_RIGHT_IMAGE_Y;
			break;
		case Dir::Down:
			town.player->_rectImage.top = DIR_DOWN_IMAGE_Y;
			break;
		}
	}
}

// npc ��� 
void CALLBACK T_NpcMotion(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	const Scene scene = sceneManager->GetScene();
	static int npc1cnt = 0;
	static int npc2cnt = 0;

	// npc2�� ������ �ݴ�� �ٲٱ� ���� ����
	static int negative_number = 1;

	if (scene == Scene::Town)
	{
		// npc1�� ��� �ֱ�
		npc1cnt++;
		// npc2�� ��� �ֱ�
		npc2cnt++;

		// npc1�� �ִϸ��̼�
		if (town._npc1Move.x < 150)
			town._npc1Move.x += 50;
		if (npc1cnt == 100)
		{
			npc1cnt = 0;
			town._npc1Move.x = 0;
		}

		// npc2�� ��ġ���� 1000�� ���� ��� �ݴ�� �ٽ� �ɾ��
		if (town._npc2Rect.top > 625)
		{
			negative_number = -1;
			town._npc2Move.y = 192;
		}
		if (town._npc2Rect.top < 219)
		{
			negative_number = 1;
			town._npc2Move.y = 0;
		}

		// warking npc2�� cnt�� 5�� ����� �ɶ����� ������ �Ѱ���
		if (npc2cnt % 5 == 0)
		{
			town._npc2Move.x += 64;
			town._npc2Rect.top += 6 * negative_number;
		}
		if (npc2cnt == 20 && town._npc2Move.x > 192)
		{
			npc2cnt = 0;
			town._npc2Move.x = 0;
		}
	}

}

void CALLBACK T_TargetMove(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	const RECT rectWindow = sceneManager->GetRectWindow();
	const Scene scene = sceneManager->GetScene();

	stage.fingerController(hWnd);
	if (sceneManager->IsLoading() == false)
	{
		stage.Move(hWnd, rectWindow);
		InvalidateRect(hWnd, NULL, false);
	}
}

void CALLBACK T_Battle_MapMove(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	battle._rectDraw.top += battle._mapSpeed;
	battle._rectDraw.bottom += battle._mapSpeed;
	battle._rectDraw2.top += battle._mapSpeed;
	battle._rectDraw2.bottom += battle._mapSpeed;
}

void CALLBACK T_SelectPokemonMove(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	stage._select_pokemon_move++;
	if (stage._select_pokemon_move == 2)
		stage._select_pokemon_move = 0;
}