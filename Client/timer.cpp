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

// 배틀화면에서 필요한 타이머
void T_Battle_Invalidate()
{
	InvalidateRect(sceneManager->GetHwnd(), NULL, FALSE);
	//mPlayer->CheckShot(); // 플레이어의 기본 공격에 쿨타임을 주는 함수이다
	//ㅇ enemies->CreateRecvMelee(); // 배틀 타이머당 생성되는 근거리 적 생성 함수
	//ㅇ enemies->CreateCheckRange(); // 배틀 타이머당 생성되는 원거리 적 생성 함수
	enemies->CheckAttackDelay(); // 적이 공격을 하고 나서 딜레이를 주는 함수
	boss->CheckActDelay(); // 보스의 움직임에 딜레이를 주는 함수
	boss->CheckAttackDelay(); // 보스의 공격에 딜레이를 주는 함수

	for (auto& member : sceneManager->GetMemberMap())
		member.second->MoveBullets();

	//mPlayer->MoveBullets(); // 플레이어 탄막 이동 함수
	enemies->MoveBullets(); // 적 탄막 이동 함수
	enemies->Move();
	boss->Move(); // 보스 이동 및 충돌 검사 함수
}

void T_Battle_Animate()
{
	for (auto& member : sceneManager->GetMemberMap())
		member.second->Animate(sceneManager->GetHwnd());

	enemies->Animate(); // 적 스프라이트 이미지 애니메이션 함수
	boss->AnimateSkill(); // 보스 스킬 스프라이트 이미지 애니메이션 함수
}

void T_Battle_AnimateBoss()
{
	boss->Animate(sceneManager->GetHwnd()); // 보스 스프라이트 이미지 애니메이션 함수
}

void T_Battle_MovePlayer()
{
	mPlayer->Move(sceneManager->GetHwnd(), 0); // 실제 플레이어의 이동을 수행하는 함수
}

void T_Battle_Effect()
{
	effects->Animate(); // 게임 중에 생긴 모든 이펙트를 실행시키고 삭제하는 함쑤
}
void T_Battle_GUI()
{
	gui->Move(sceneManager->GetHwnd()); // GUI 업데이트 함수
}

void T_Battle_MapMove()
{
	battle._rectDraw.top += battle._mapSpeed;
	battle._rectDraw.bottom += battle._mapSpeed;
	battle._rectDraw2.top += battle._mapSpeed;
	battle._rectDraw2.bottom += battle._mapSpeed;
}


// sceneManager->StartLoading 함수만 부르면 현재 씬 정보를 가져와서 알아서 다음 씬을 정한다.
void T_Loading(float elapsedTime)
{
	if (sceneManager->IsLoading() == false)
		return;

	static float accTime = 0.f;
	accTime += elapsedTime;

	if (accTime >= 0.08f) {
		const RECT rectWindow = sceneManager->GetRectWindow();
		const Scene scene = sceneManager->GetScene();

		// 로딩 모두 끝날 시 타이머 정지
		if (loading.IsLoaded() == true)
		{
			// 로딩 화면이 끝날때마다 모두 초기화
			stage.SelectPokemonInit();

			if (scene == Scene::Intro)// 전의 게임 플로우 값이 메인화면이라면 다음 게임 플로우는 타운
			{
				//sceneManager->MoveScene(hWnd, Scene::Town);
				sceneManager->MoveScene(sceneManager->GetHwnd(), Scene::Stage);
			}
			else if (scene == Scene::Town) // 전의 게임 플로우 값이 타운이라면 다음 게임 플로우는 스테이지
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
				if (stage.GetStage() == StageElement::Town) // 타운 오브젝트 타겟
					sceneManager->MoveScene(sceneManager->GetHwnd(), Scene::Town);
				else
					sceneManager->MoveScene(sceneManager->GetHwnd(), Scene::PhaseManager);

			}
			else if (scene == Scene::PhaseManager)
			{
				sceneManager->MoveScene(sceneManager->GetHwnd(), Scene::Battle);
			}

			else if (scene == Scene::Battle) // 게임 배틀 끝나고 스테이지 넘어가기
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

// 로딩 화면 업데이트 함수
void T_Loadingbar(float elapsedTime)
{
	if (sceneManager->IsLoading() == false)
		return;

	//static float accTime = 0.f;
	//accTime += elapsedTime;

	//if (accTime >= 0.45f) {
		loading.Load(sceneManager->GetHwnd());
		//accTime = 0.f;
	//}
}