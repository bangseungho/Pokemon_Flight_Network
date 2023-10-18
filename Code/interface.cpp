#include "stdafx.h"
#include "interface.h"
#include "player.h"
#include "enemy.h"
#include "image.h"
#include "timer.h"
#include "boss.h"
#include "scene.h"
#include "sound.h"

#include "town.h"
#include "stage.h"
#include "phase.h"

#define KEYDOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000 ? 1 : 0))
#define KEYUP(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000 ? 0 : 1))

#define MOVE_UP VK_UP
#define MOVE_LEFT VK_LEFT
#define MOVE_DOWN VK_DOWN
#define MOVE_RIGHT VK_RIGHT

extern GameData gameData;
extern Player* player;
extern EnemyController* enemies;
extern Boss* boss;
extern GUIManager* gui;
extern SceneManager* sceneManager;
extern SoundManager* soundManager;

extern Town town;
extern Stage stage;
extern Phase phase;

void CheckKeyDown(const HWND& hWnd, const WPARAM& wParam)
{
	if (sceneManager->GetScene() == Scene::Town || sceneManager->GetScene() == Scene::Stage)
	{
		if (wParam == 'h' || wParam == 'H')
		{
			town._allHide = !town._allHide;
			stage._allHide = !stage._allHide;
		}
	}
	else if (sceneManager->GetScene() == Scene::Phase)
	{
		if (sceneManager->IsLoading() == false && wParam == VK_BACK)
		{
			sceneManager->MoveScene(hWnd, Scene::Stage);
		}
		else if (wParam == VK_ADD)
		{
			phase.ClearPhase();
		}
	}
	else if (sceneManager->GetScene() == Scene::Battle)
	{
		switch (wParam)
		{
		case _T('H'):
			gameData.isShowHitbox = !gameData.isShowHitbox;
			break;
		case _T('J'):
			gameData.isShowDrawBox = !gameData.isShowDrawBox;
			break;
		case _T('O'):
			player->Heal();
			break;
		case _T('P'):
			player->FullMP();
			break;
		case _T('I'):
			player->InvincibleMode();
			break;
		case VK_SUBTRACT:
			boss->KillBoss();
			break;
		case VK_BACK:
			gui->SkipField();
			break;
		case _T('Q'):
			player->ActiveSkill(Skill::Identity);
			break;
		case _T('W'):
			player->ActiveSkill(Skill::Sector);
			break;
		case _T('E'):
			player->ActiveSkill(Skill::Circle);
			break;
		}

		bool isMove = false;
		if (KEYDOWN(MOVE_LEFT))
		{
			player->Stop(Dir::Right);
			player->SetDirection(Dir::Left);
			isMove = true;
		}
		if (KEYDOWN(MOVE_RIGHT))
		{
			if (wParam != MOVE_LEFT)
			{
				player->Stop(Dir::Left);
				player->SetDirection(Dir::Right);
				isMove = true;
			}
		}
		if (KEYDOWN(MOVE_UP))
		{
			player->Stop(Dir::Down);
			player->SetDirection(Dir::Up);
			isMove = true;
		}
		if (KEYDOWN(MOVE_DOWN))
		{
			if (wParam != MOVE_UP)
			{
				player->Stop(Dir::Up);
				player->SetDirection(Dir::Down);
				isMove = true;
			}
		}

		if (isMove == true)
		{
			player->SetMove(hWnd, TIMERID_BATTLE_MOVE_PLAYER, ELAPSE_BATTLE_MOVE_PLAYER, T_Battle_MovePlayer);
		}
	}
}
void CheckKeyUp(const HWND& hWnd, const WPARAM& wParam)
{
	if (sceneManager->GetScene() == Scene::Town)
	{
		if (wParam == VK_LEFT || wParam == VK_RIGHT || wParam == VK_UP || wParam == VK_DOWN)
		{
			town.StopPlayer();
		}
	}
	else if (sceneManager->GetScene() == Scene::Battle)
	{
		if (player->IsMove() == true)
		{
			switch (wParam)
			{
			case MOVE_LEFT:
				player->Stop(Dir::Left);
				if (KEYDOWN(MOVE_RIGHT))
				{
					player->SetDirection(Dir::Right);
				}
				break;
			case MOVE_RIGHT:
				player->Stop(Dir::Right);
				if (KEYDOWN(MOVE_LEFT))
				{
					player->SetDirection(Dir::Left);
				}
				break;
			case MOVE_UP:
				player->Stop(Dir::Up);
				if (KEYDOWN(MOVE_DOWN))
				{
					player->SetDirection(Dir::Down);
				}
				break;
			case MOVE_DOWN:
				player->Stop(Dir::Down);
				if (KEYDOWN(MOVE_UP))
				{
					player->SetDirection(Dir::Up);
				}
				break;
			}
		}
	}
}

GUIManager::GUIManager(const RECT& rectWindow)
{
	const int crntPhase = phase.GetPhase();
	int fieldLength = 0;
	switch (crntPhase)
	{
	case 0:
		fieldLength = 360;
		break;
	case 1:
		fieldLength = 480;
		break;
	case 2:
		fieldLength = 480;
		break;
	case 3:
		fieldLength = 600;
		break;
	default:
		assert(0);
		break;
	}

	constexpr int main_guiHeight = 80;

	constexpr int box_guiWidth = 92;
	constexpr int gague_guiWidth = 80;

	mainGUI = new GUIImage();
	gagueGUI_main = new GUIImage();
	gagueGUI_border = new GUIImage();
	gagueGUI_hp = new GUIImage();
	gagueGUI_mp = new GUIImage();
	
	icon_Q = new GUIImage();
	icon_W = new GUIImage();
	icon_E = new GUIImage();

	moveBarGUI = new GUIImage();
	icon_pokemon = new GUIImage();
	gaugeMoveBarGUI = new GUIImage();

	hurtGUI_Fire.gui = new GUIImage();
	hurtGUI_Water.gui = new GUIImage();
	hurtGUI_Elec.gui = new GUIImage();
	hurtGUI_Dark.gui = new GUIImage();

	switch (player->GetType())
	{
	case Type::Elec:
		icon_Q->Load(_T("images\\battle\\icon_elec_Q.png"), { 35, 35 });
		icon_pokemon->Load(_T("images\\battle\\icon_thunder.png"), { 480, 480 });
		gaugeMoveBarGUI->Load(_T("images\\battle\\gague_bar_elec.png"), { 14, 217 });
		break;
	case Type::Water:
		icon_Q->Load(_T("images\\battle\\icon_water_Q.png"), { 260, 260 });
		icon_pokemon->Load(_T("images\\battle\\icon_articuno.png"), { 480, 480 });
		gaugeMoveBarGUI->Load(_T("images\\battle\\gague_bar_water.png"), { 14, 217 });
		break;
	case Type::Fire:
		icon_Q->Load(_T("images\\battle\\icon_fire_Q.png"), { 130, 130 });
		icon_pokemon->Load(_T("images\\battle\\icon_moltres.png"), { 480, 480 });
		gaugeMoveBarGUI->Load(_T("images\\battle\\gague_bar_fire.png"), { 14, 217 });
		break;
	}
	switch (player->GetSubType())
	{
	case Type::Elec:
		icon_W->Load(_T("images\\battle\\icon_elec_W.png"), { 130, 130 });
		icon_E->Load(_T("images\\battle\\icon_elec_E.png"), { 130, 130 });
		break;
	case Type::Water:
		icon_W->Load(_T("images\\battle\\icon_water_W.png"), { 130, 130 });
		icon_E->Load(_T("images\\battle\\icon_water_E.png"), { 130, 130 });
		break;
	case Type::Fire:
		icon_W->Load(_T("images\\battle\\icon_fire_W.png"), { 130, 130 });
		icon_E->Load(_T("images\\battle\\icon_fire_E.png"), { 130, 130 });
		break;
	}

	this->rectWindow = &rectWindow;
	rectMain = rectWindow;
	rectMain.top = rectMain.bottom - main_guiHeight;

	constexpr POINT skillBoxSize = { 38,39 };
	constexpr POINT boxStartPoint = { 59, 21 };
	rectSkill_Q.left = rectMain.left + boxStartPoint.x;
	rectSkill_Q.top = rectMain.top + boxStartPoint.y;
	rectSkill_Q.right = rectSkill_Q.left + skillBoxSize.x;
	rectSkill_Q.bottom = rectSkill_Q.top + skillBoxSize.y;

	rectSkill_W = rectSkill_Q;
	rectSkill_W.left += box_guiWidth;
	rectSkill_W.right += box_guiWidth + 1;

	rectSkill_E = rectSkill_W;
	rectSkill_E.left += box_guiWidth;
	rectSkill_E.right += box_guiWidth;

	constexpr POINT gagueSize = { 60,60 };
	constexpr POINT gagueStartPoint = { 30, 10 };
	rectHP.left = rectSkill_E.right + gagueStartPoint.x;
	rectHP.right = rectHP.left + gagueSize.x;
	rectHP.top = rectMain.top + gagueStartPoint.y;
	rectHP.bottom = rectHP.top + gagueSize.y;

	rectMP = rectHP;
	rectMP.left += gague_guiWidth;
	rectMP.right += gague_guiWidth;

	constexpr POINT moveBarSize = { 20, 220 };
	constexpr POINT moveBarStartPoint = { WINDOWSIZE_X - 40, 220 };
	rectMoveBar.left = moveBarStartPoint.x;
	rectMoveBar.top = moveBarStartPoint.y;
	rectMoveBar.right = rectMoveBar.left + moveBarSize.x;
	rectMoveBar.bottom = rectMoveBar.top + moveBarSize.y;

	constexpr POINT gagueMoveBarSize = { 14,214 };
	constexpr POINT gagueMoveBarStartPoint = { 3, 4 };
	rectGaugeMoveBar.left = rectMoveBar.left + gagueMoveBarStartPoint.x;
	rectGaugeMoveBar.top = rectMoveBar.top + gagueMoveBarStartPoint.y;
	rectGaugeMoveBar.right = rectGaugeMoveBar.left + gagueMoveBarSize.x;
	rectGaugeMoveBar.bottom = rectGaugeMoveBar.top + gagueMoveBarSize.y;

	constexpr POINT pokemonIconSize = { 24, 24 };
	const int pokemonIconCenterX = rectMoveBar.left + ((rectMoveBar.right - rectMoveBar.left) / 2);
	rectPokemonIcon.left = pokemonIconCenterX - (pokemonIconSize.x / 2);
	rectPokemonIcon.right = rectPokemonIcon.left + pokemonIconSize.x;
	rectPokemonIcon.top = rectMoveBar.bottom - (pokemonIconSize.y / 2);
	rectPokemonIcon.bottom = rectPokemonIcon.top + pokemonIconSize.y;

	constexpr float moveBarHeight = moveBarSize.y - moveBarSize.x;
	constexpr int corrValue = 5;
	iconMoveMaxY = (rectMoveBar.top - (pokemonIconSize.y / 2)) + corrValue;
	iconMoveMinY = (rectMoveBar.bottom + (pokemonIconSize.y / 2));
	iconMoveAmount = (moveBarHeight / fieldLength) / ELAPSE_BATTLE_GUI;

	mainGUI->Load(_T("images\\battle\\battleGUI.png"), { 500, 80 });
	gagueGUI_main->Load(_T("images\\battle\\gauge.png"), { 130, 130 });
	gagueGUI_border->Load(_T("images\\battle\\gauge_border.png"), { 130, 130 });
	gagueGUI_hp->Load(_T("images\\battle\\gauge_hp.png"), { 130, 130 });
	gagueGUI_mp->Load(_T("images\\battle\\gauge_mp.png"), { 130, 130 });
	moveBarGUI->Load(_T("images\\battle\\moveBar.png"), { 20, 223 });

	hurtGUI_Fire.gui->Load(_T("images\\battle\\frame_hurt_fire.png"), { 700, 1000 }, 0x00);
	hurtGUI_Water.gui->Load(_T("images\\battle\\frame_hurt_water.png"), { 239, 371 }, 0x00);
	hurtGUI_Elec.gui->Load(_T("images\\battle\\frame_hurt_elec.png"), { 239, 371 }, 0x00);
	hurtGUI_Dark.gui->Load(_T("images\\battle\\frame_hurt_dark.png"), { 239, 371 }, 0x00);
}

void GUIManager::Paint(const HDC& hdc)
{
	mainGUI->Paint(hdc, rectMain);

	gagueGUI_main->Paint(hdc, rectHP);
	gagueGUI_hp->PaintGauge(hdc, rectHP, player->GetHP(), player->GetMaxHP());
	gagueGUI_border->Paint(hdc, rectHP);

	gagueGUI_main->Paint(hdc, rectMP);
	gagueGUI_mp->PaintGauge(hdc, rectMP, player->GetMP(), player->GetMaxMP());
	gagueGUI_border->Paint(hdc, rectMP);

	icon_Q->Paint(hdc, rectSkill_Q);
	icon_W->Paint(hdc, rectSkill_W);
	icon_E->Paint(hdc, rectSkill_E);

	if (isIconStop == false)
	{
		moveBarGUI->Paint(hdc, rectMoveBar);
		gaugeMoveBarGUI->PaintGauge(hdc, rectGaugeMoveBar, iconMoveMinY - rectPokemonIcon.bottom, iconMoveMaxY);
		icon_pokemon->Paint(hdc, rectPokemonIcon);
	}

	hurtGUI_Fire.gui->Paint(hdc, *rectWindow);
	hurtGUI_Water.gui->Paint(hdc, *rectWindow);
	hurtGUI_Elec.gui->Paint(hdc, *rectWindow);
	hurtGUI_Dark.gui->Paint(hdc, *rectWindow);
}

void GUIManager::Update(const HWND& hWnd)
{
	hurtGUI_Fire.ReduceAlpha();
	hurtGUI_Water.ReduceAlpha();
	hurtGUI_Elec.ReduceAlpha();
	hurtGUI_Dark.ReduceAlpha();

	int crntPhase = phase.GetPhase();
	if (player->IsDeath() == true)
	{
		return;
	}
	else if (isIconStop == true)
	{
		if (crntPhase < 2 && isClearPhase == false && enemies->IsEmenyClear() == true)
		{
			isClearPhase = true;

			player->InvincibleMode();
			phase.ClearPhase();
			soundManager->StopEffectSound();
			soundManager->StopBGMSound();
			soundManager->PlayEffectSound(EffectSound::Win);
			sceneManager->StartLoading(hWnd);
		}
		return;
	}
	rectPokemonIcon.top -= iconMoveAmount;
	rectPokemonIcon.bottom -= iconMoveAmount;
	if (rectPokemonIcon.top <= iconMoveMaxY)
	{
		const int corrValue = rectPokemonIcon.top - iconMoveMaxY;
		rectPokemonIcon.top += corrValue;
		rectPokemonIcon.bottom += corrValue;
		isIconStop = true;
	
		if (crntPhase >= 2)
		{
			boss->Create();
		}
	}
}
RECT GUIManager::GetRectDisplay() const
{
	RECT rectDisplay = *rectWindow;
	rectDisplay.bottom = rectMain.top;
	return rectDisplay;	
}

void GUIManager::DisplayHurtFrame(Type type)
{
	switch (type)
	{
	case Type::Fire:
		hurtGUI_Fire.alpha = hurtGUI_alpha;
		break;
	case Type::Water:
		hurtGUI_Water.alpha = hurtGUI_alpha;
		break;
	case Type::Elec:
		hurtGUI_Elec.alpha = hurtGUI_alpha;
		break;
	case Type::Dark:
		CheckOverflowAdd(hurtGUI_Dark.alpha, 0x50);
		break;
	default:
		assert(0);
		break;
	}
}




void GUIManager::HurtGUI::ReduceAlpha()
{
	if (alpha == 0x00)
	{
		return;
	}
	gui->SetAlpha(alpha--);
}