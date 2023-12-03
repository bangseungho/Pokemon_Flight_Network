#pragma once
#define ELAPSE_BATTLE_INVALIDATE 10
#define ELAPSE_BATTLE_ANIMATION 50
#define ELAPSE_BATTLE_ANIMATION_BOSS 100
#define ELAPSE_BATTLE_MOVE_PLAYER 10
#define ELAPSE_BATTLE_EFFECT 50
#define ELAPSE_BATTLE_GUI 10

void T_Battle_Invalidate();
void T_Battle_Animate();
void T_Battle_AnimateBoss();
void T_Battle_MovePlayer();
void T_Battle_Effect();
void T_Battle_GUI();
void T_Battle_MapMove();

void T_Loading(float elapsedTime);
void T_Loadingbar(float elapsedTime);

