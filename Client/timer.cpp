#include "stdafx.h"
#include "interface.h"
#include "enemy.h"
#include "player.h"
#include "timer.h"
#include "effect.h"
#include "boss.h"
#include "scene.h"
#include "Network.h"
#include "intro.h"
#include "town.h"
#include "stage.h"
#include "phase.h"
#include "battle.h"

extern Player* mPlayer;
extern EnemyController* enemies;
extern Boss* boss;
extern EffectManager* effects;
extern GUIManager* gui;
extern SceneManager* sceneManager;

extern Intro intro;
extern Loading loading;
extern Town town;
extern Stage stage;
extern PhaseManager phase;
extern Battle battle;

extern CImage glowing_black;

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

void T_Battle_Animate()
{
	mPlayer->Animate(sceneManager->GetHwnd()); // �÷��̾� ��������Ʈ �̹��� �ִϸ��̼� �Լ�
	enemies->Animate(); // �� ��������Ʈ �̹��� �ִϸ��̼� �Լ�
	boss->AnimateSkill(); // ���� ��ų ��������Ʈ �̹��� �ִϸ��̼� �Լ�
}

void T_Battle_AnimateBoss()
{
	boss->Animate(sceneManager->GetHwnd()); // ���� ��������Ʈ �̹��� �ִϸ��̼� �Լ�
}

void T_Battle_MovePlayer()
{
	mPlayer->Update(sceneManager->GetHwnd(), 0); // ���� �÷��̾��� �̵��� �����ϴ� �Լ�
}

void T_Battle_Effect()
{
	effects->Animate(); // ���� �߿� ���� ��� ����Ʈ�� �����Ű�� �����ϴ� �Ծ�
}
void T_Battle_GUI()
{
	gui->Update(sceneManager->GetHwnd()); // GUI ������Ʈ �Լ�
}

void T_Battle_MapMove()
{
	battle._rectDraw.top += battle._mapSpeed;
	battle._rectDraw.bottom += battle._mapSpeed;
	battle._rectDraw2.top += battle._mapSpeed;
	battle._rectDraw2.bottom += battle._mapSpeed;
}


// sceneManager->StartLoading �Լ��� �θ��� ���� �� ������ �����ͼ� �˾Ƽ� ���� ���� ���Ѵ�.
void T_Loading(float elapsedTime)
{
	if (sceneManager->IsLoading() == false)
		return;

	static float accTime = 0.f;
	accTime += elapsedTime;

	if (accTime >= 0.08f) {
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
				sceneManager->MoveScene(sceneManager->GetHwnd(), Scene::Stage);
			}
			else if (scene == Scene::Town) // ���� ���� �÷ο� ���� Ÿ���̶�� ���� ���� �÷ο�� ��������
			{
				town.mPlayer->_Pos.x = rectWindow.right / 2;
				town.mPlayer->_Pos.y = rectWindow.bottom / 2;
				town.mPlayer->_cam = { town.mPlayer->_Pos.x - town.GetCamSizeX(), (float)rectWindow.top, town.mPlayer->_Pos.x + town.GetCamSizeX(), (float)rectWindow.bottom };
				town._rectImage = rectWindow;

				if (town._nextFlow == Scene::Stage)
				{
					sceneManager->MoveScene(sceneManager->GetHwnd(), Scene::Stage);
				}
				if (town._nextFlow == Scene::Intro)
				{
					sceneManager->MoveScene(sceneManager->GetHwnd(), Scene::Intro);
				}
			}
			else if (scene == Scene::Stage)
			{
				if (stage.GetStage() == StageElement::Town) // Ÿ�� ������Ʈ Ÿ��
					sceneManager->MoveScene(sceneManager->GetHwnd(), Scene::Town);
				else
					sceneManager->MoveScene(sceneManager->GetHwnd(), Scene::PhaseManager);

			}
			else if (scene == Scene::PhaseManager)
			{
				sceneManager->MoveScene(sceneManager->GetHwnd(), Scene::Battle);
			}

			else if (scene == Scene::Battle) // ���� ��Ʋ ������ �������� �Ѿ��
			{
				sceneManager->MoveScene(sceneManager->GetHwnd(), Scene::PhaseManager);
			}

			sceneManager->StopLoading(sceneManager->GetHwnd());
		}

		loading._loding_pokemon_rectImage.left += LOADING_POKEMON_X;

		if (loading._loding_pokemon_rectImage.left == LOADING_POKEMON_MAX_X)
		{
			loading._loding_pokemon_rectImage.left = 0;
			loading._loading_pokemon_cnt++;
		}
		InvalidateRect(sceneManager->GetHwnd(), NULL, false);

		accTime = 0.f;
	}
}

// �ε� ȭ�� ������Ʈ �Լ�
void T_Loadingbar(float elapsedTime)
{
	if (sceneManager->IsLoading() == false)
		return;

	static float accTime = 0.f;
	accTime += elapsedTime;

	if (accTime >= 0.45f) {
		loading.Load(sceneManager->GetHwnd());
		accTime = 0.f;
	}
}