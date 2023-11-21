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

// 배틀화면에서 필요한 타이머
void CALLBACK T_Battle_Invalidate(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	InvalidateRect(hWnd, NULL, FALSE);
	mPlayer->CheckShot(); // 플레이어의 기본 공격에 쿨타임을 주는 함수이다
	enemies->CreateCheckMelee(); // 배틀 타이머당 생성되는 근거리 적 생성 함수
	enemies->CreateCheckRange(); // 배틀 타이머당 생성되는 원거리 적 생성 함수
	enemies->CheckAttackDelay(); // 적이 공격을 하고 나서 딜레이를 주는 함수
	boss->CheckActDelay(); // 보스의 움직임에 딜레이를 주는 함수
	boss->CheckAttackDelay(); // 보스의 공격에 딜레이를 주는 함수

	mPlayer->MoveBullets(); // 플레이어 탄막 이동 함수
	enemies->MoveBullets(); // 적 탄막 이동 함수
	enemies->Update(); // 적 이동 및 충돌 검사 함수
	boss->Update(); // 보스 이동 및 충돌 검사 함수
}

void CALLBACK T_Battle_Animate(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	mPlayer->Animate(hWnd); // 플레이어 스프라이트 이미지 애니메이션 함수
	enemies->Animate(); // 적 스프라이트 이미지 애니메이션 함수
	boss->AnimateSkill(); // 보스 스킬 스프라이트 이미지 애니메이션 함수
}

void CALLBACK T_Battle_AnimateBoss(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	boss->Animate(hWnd); // 보스 스프라이트 이미지 애니메이션 함수
}

void CALLBACK T_Battle_MovePlayer(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	mPlayer->Update(hWnd, TIMERID_BATTLE_MOVE_PLAYER); // 실제 플레이어의 이동을 수행하는 함수
}

void CALLBACK T_Battle_Effect(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	effects->Animate(); // 게임 중에 생긴 모든 이펙트를 실행시키고 삭제하는 함쑤
}
void CALLBACK T_Battle_GUI(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	gui->Update(hWnd); // GUI 업데이트 함수
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

extern Cloud cloud[4];
extern Logo logo;
extern Menu menu;
extern CImage glowing_black;

// 인트로에서 구름 움직이는 타이머
void CALLBACK T_MoveCloud(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	const RECT rectWindow = sceneManager->GetRectDisplay();

	// 구름마다 속도 다르게 제어
	cloud[0].Update(4, 0, rectWindow);
	cloud[1].Update(2, 0, rectWindow);
	cloud[2].Update(1, 0, rectWindow);
	cloud[3].Update(4, 0, rectWindow);
}

// POKEMON FLIGHT의 흔들거리는 움직임
void CALLBACK T_MoveLogo(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	static bool thiscnt = false;

	// 로고가 얼마나 움직였는지를 확인한 후 값에 따라 다시 반대로 움직이기
	if (logo._logoMovingCnt > 10)
		thiscnt = true;
	else if (logo._logoMovingCnt == 0)
		thiscnt = false;

	if (thiscnt == true)
		logo._logoMovingCnt--;
	else
		logo._logoMovingCnt++;
}

// 씬 정보를 받아와서 인트로이거나 페이즈일 경우 핑거 컨트롤러를 애니메이션 하는 함수
void CALLBACK T_Animation(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	const Scene scene = sceneManager->GetScene();
	if (scene == Scene::Intro && sceneManager->IsLoading() == false)
		menu.fingerController(hWnd);
	else if (scene == Scene::PhaseManager && sceneManager->IsLoading() == false)
	{
		battle.Init();
		phase.fingerController(hWnd);
	}
	InvalidateRect(hWnd, NULL, false);
}

// 화살표가 반짝일 수 있도록 하는 함수
void CALLBACK T_TwinkleEmotion(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	menu._finger_twinkle_cnt++;
}

// sceneManager->StartLoading 함수만 부르면 현재 씬 정보를 가져와서 알아서 다음 씬을 정한다.
void CALLBACK T_Loading(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	const RECT rectWindow = sceneManager->GetRectWindow();
	const Scene scene = sceneManager->GetScene();

	// 로딩 모두 끝날 시 타이머 정지
	if (loading.IsLoaded() == true)
	{
		// 로딩 화면이 끝날때마다 타운 모두 초기화
		town.Init(rectWindow, hWnd);

		stage.SelectPokemonInit();

		if (scene == Scene::Intro)// 전의 게임 플로우 값이 메인화면이라면 다음 게임 플로우는 타운
		{
			sceneManager->MoveScene(hWnd, Scene::Town);
		}
		else if (scene == Scene::Town) // 전의 게임 플로우 값이 타운이라면 다음 게임 플로우는 스테이지
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
			if (stage.GetStage() == StageElement::Town) // 타운 오브젝트 타겟
				sceneManager->MoveScene(hWnd, Scene::Town);
			else
				sceneManager->MoveScene(hWnd, Scene::PhaseManager);

		}
		else if (scene == Scene::PhaseManager)
		{
			sceneManager->MoveScene(hWnd, Scene::Battle);
		}

		else if (scene == Scene::Battle) // 게임 배틀 끝나고 스테이지 넘어가기
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

// 로딩 화면 업데이트 함수
void CALLBACK T_Loadingbar(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	loading.Load(hWnd);
}

// 타운에 있는 플레이어 움직임 타이머
void CALLBACK T_TPAnimation(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	const RECT rectWindow = sceneManager->GetRectWindow();
	const Scene scene = sceneManager->GetScene();

	FRECT rectDraw = { town.mPlayer->_Pos.x - 20, town.mPlayer->_Pos.y - 20, town.mPlayer->_Pos.x + 20, town.mPlayer->_Pos.y + 20 };
	town.mPlayer->_rectDraw = rectDraw;

	// 게임 플로우가 현재 타운일때와 장면이 끝남을 확인하는 변수가 참이 아닐 때 움직일 수 있도록 설정
	if (scene == Scene::Town && sceneManager->IsLoading() == false)
	{
		town.Update(rectWindow);
		InvalidateRect(hWnd, NULL, false);
	}

	// 일정 범위 (렉트 윈도우의 오른 쪽) 넘어갈시 다음 스테이지 진행
	if (sceneManager->IsLoading() == false && town.mPlayer->_Pos.x + 20 >= rectWindow.right)
	{
		sceneManager->StartLoading(hWnd);
		town._nextFlow = Scene::Stage;
	}
	// 일정 범위 (렉트 윈도우의 왼 쪽) 넘어갈시 전 메인화면으로 진행
	else if (sceneManager->IsLoading() == false && town.mPlayer->_Pos.x - 20 <= rectWindow.left)
	{
		sceneManager->StartLoading(hWnd);
		town._nextFlow = Scene::Intro;
	}
}

// 타운에 있는 플레이어 움직임 방향
void CALLBACK T_TPAnimationDir(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	const Scene scene = sceneManager->GetScene();

	// 타운의 플레이어가 계속 가는중이면 이미지 장면 변경(애니메이션)
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

// npc 모션 
void CALLBACK T_NpcMotion(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	const Scene scene = sceneManager->GetScene();
	static int npc1cnt = 0;
	static int npc2cnt = 0;

	// npc2의 방향을 반대로 바꾸기 위한 변수
	static int negative_number = 1;

	if (scene == Scene::Town)
	{
		// npc1의 모션 주기
		npc1cnt++;
		// npc2의 모션 주기
		npc2cnt++;

		// npc1의 애니메이션
		if (town._npc1Move.x < 150)
			town._npc1Move.x += 50;
		if (npc1cnt == 100)
		{
			npc1cnt = 0;
			town._npc1Move.x = 0;
		}

		// npc2가 위치값이 1000이 넘을 경우 반대로 다시 걸어가기
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

		// warking npc2의 cnt가 5의 배수가 될때마다 앞으로 한걸음
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

// 스테이지에서 타겟을 움직이는 함수
void CALLBACK T_TargetMove(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	const RECT rectWindow = sceneManager->GetRectWindow();
	const Scene scene = sceneManager->GetScene();

	stage.fingerController(hWnd);
	if (sceneManager->IsLoading() == false)
	{
		stage.Update(hWnd, rectWindow);
		InvalidateRect(hWnd, NULL, false);
	}
}

// 배틀화면에서 종 스크롤 함수
void CALLBACK T_Battle_MapMove(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	battle._rectDraw.top += battle._mapSpeed;
	battle._rectDraw.bottom += battle._mapSpeed;
	battle._rectDraw2.top += battle._mapSpeed;
	battle._rectDraw2.bottom += battle._mapSpeed;
}

// 플레이어 포켓몬 선택 함수
void CALLBACK T_SelectPokemonMove(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	stage._select_pokemon_move++;
	if (stage._select_pokemon_move == 2)
		stage._select_pokemon_move = 0;
}