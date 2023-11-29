#include "stdafx.h"
#include "interface.h"
#include "enemy.h"
#include "player.h"
#include "timer.h"
#include "effect.h"
#include "boss.h"
#include "scene.h"
#include "Network.h"

extern Player* mPlayer;
extern EnemyController* enemies;
extern Boss* boss;
extern EffectManager* effects;
extern GUIManager* gui;
extern SceneManager* sceneManager;

// ��Ʋȭ�鿡�� �ʿ��� Ÿ�̸�
void T_Battle_Invalidate()
{
	InvalidateRect(sceneManager->GetHwnd(), NULL, FALSE);
	mPlayer->CheckShot(); // �÷��̾��� �⺻ ���ݿ� ��Ÿ���� �ִ� �Լ��̴�
	enemies->CreateCheckMelee(); // ��Ʋ Ÿ�̸Ӵ� �����Ǵ� �ٰŸ� �� ���� �Լ�
	enemies->CreateCheckRange(); // ��Ʋ Ÿ�̸Ӵ� �����Ǵ� ���Ÿ� �� ���� �Լ�
	enemies->CheckAttackDelay(); // ���� ������ �ϰ� ���� �����̸� �ִ� �Լ�
	boss->CheckActDelay(); // ������ �����ӿ� �����̸� �ִ� �Լ�
	boss->CheckAttackDelay(); // ������ ���ݿ� �����̸� �ִ� �Լ�

	mPlayer->MoveBullets(); // �÷��̾� ź�� �̵� �Լ�
	enemies->MoveBullets(); // �� ź�� �̵� �Լ�
	enemies->Update(); // �� �̵� �� �浹 �˻� �Լ�
	boss->Update(); // ���� �̵� �� �浹 �˻� �Լ�
}

void CALLBACK T_Battle_Animate(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	mPlayer->Animate(hWnd); // �÷��̾� ��������Ʈ �̹��� �ִϸ��̼� �Լ�
	enemies->Animate(); // �� ��������Ʈ �̹��� �ִϸ��̼� �Լ�
	boss->AnimateSkill(); // ���� ��ų ��������Ʈ �̹��� �ִϸ��̼� �Լ�
}

void CALLBACK T_Battle_AnimateBoss(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	boss->Animate(hWnd); // ���� ��������Ʈ �̹��� �ִϸ��̼� �Լ�
}

void CALLBACK T_Battle_MovePlayer(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	mPlayer->Update(hWnd, TIMERID_BATTLE_MOVE_PLAYER); // ���� �÷��̾��� �̵��� �����ϴ� �Լ�
}

void CALLBACK T_Battle_Effect(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	effects->Animate(); // ���� �߿� ���� ��� ����Ʈ�� �����Ű�� �����ϴ� �Ծ�
}
void CALLBACK T_Battle_GUI(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	gui->Update(hWnd); // GUI ������Ʈ �Լ�
}

#include "intro.h"
#include "town.h"
#include "stage.h"
#include "phase.h"
#include "battle.h"

extern Intro intro;
extern Loading loading;
extern Town town;
extern Stage stage;
extern PhaseManager phase;
extern Battle battle;

extern CImage glowing_black;

// sceneManager->StartLoading �Լ��� �θ��� ���� �� ������ �����ͼ� �˾Ƽ� ���� ���� ���Ѵ�.
void CALLBACK T_Loading(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	const RECT rectWindow = sceneManager->GetRectWindow();
	const Scene scene = sceneManager->GetScene();

	// �ε� ��� ���� �� Ÿ�̸� ����
	if (loading.IsLoaded() == true)
	{
		// �ε� ȭ���� ���������� ��� �ʱ�ȭ
		stage.SelectPokemonInit();

		if (scene == Scene::Intro)// ���� ���� �÷ο� ���� ����ȭ���̶�� ���� ���� �÷ο�� Ÿ��
		{
			//sceneManager->MoveScene(hWnd, Scene::Town);
			sceneManager->MoveScene(hWnd, Scene::Stage);
		}
		else if (scene == Scene::Town) // ���� ���� �÷ο� ���� Ÿ���̶�� ���� ���� �÷ο�� ��������
		{
			town.mPlayer->_Pos.x = rectWindow.right / 2;
			town.mPlayer->_Pos.y = rectWindow.bottom / 2;
			town.mPlayer->_cam = { town.mPlayer->_Pos.x - town.GetCamSizeX(), (float)rectWindow.top, town.mPlayer->_Pos.x + town.GetCamSizeX(), (float)rectWindow.bottom };
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
				sceneManager->MoveScene(hWnd, Scene::PhaseManager);

		}
		else if (scene == Scene::PhaseManager)
		{
			sceneManager->MoveScene(hWnd, Scene::Battle);
		}

		else if (scene == Scene::Battle) // ���� ��Ʋ ������ �������� �Ѿ��
		{
			sceneManager->MoveScene(hWnd, Scene::PhaseManager);
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

// �ε� ȭ�� ������Ʈ �Լ�
void CALLBACK T_Loadingbar(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	loading.Load(hWnd);
}

// Ÿ� �ִ� �÷��̾� ������ ����
void CALLBACK T_TPAnimationDir(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	const Scene scene = sceneManager->GetScene();

	// Ÿ���� �÷��̾ ��� �������̸� �̹��� ��� ����(�ִϸ��̼�)
	if (scene == Scene::Town && town.mPlayer->_keepGoing == true)
	{
		town.mPlayer->_rectImage.left += TPLAYER_IMAGESIZE_X;

		if (town.mPlayer->_rectImage.left == TPLAYER_MAX_IMAGESIZE_X)
			town.mPlayer->_rectImage.left = 0;

		switch (town.mPlayer->_dir)
		{
		case Dir::Left:
			town.mPlayer->_rectImage.top = DIR_LEFT_IMAGE_Y;
			break;
		case Dir::Up:
			town.mPlayer->_rectImage.top = DIR_UP_IMAGE_Y;
			break;
		case Dir::Right:
			town.mPlayer->_rectImage.top = DIR_RIGHT_IMAGE_Y;
			break;
		case Dir::Down:
			town.mPlayer->_rectImage.top = DIR_DOWN_IMAGE_Y;
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

// ������������ Ÿ���� �����̴� �Լ�
void CALLBACK T_TargetMove(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	stage.fingerController(hWnd);
	if (sceneManager->IsLoading() == false)
	{
		InvalidateRect(hWnd, NULL, false);
	}
}

// ��Ʋȭ�鿡�� �� ��ũ�� �Լ�
void CALLBACK T_Battle_MapMove(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	battle._rectDraw.top += battle._mapSpeed;
	battle._rectDraw.bottom += battle._mapSpeed;
	battle._rectDraw2.top += battle._mapSpeed;
	battle._rectDraw2.bottom += battle._mapSpeed;
}

// �÷��̾� ���ϸ� ���� �Լ�
void CALLBACK T_SelectPokemonMove(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	stage._select_pokemon_move++;
	if (stage._select_pokemon_move == 2)
		stage._select_pokemon_move = 0;
}