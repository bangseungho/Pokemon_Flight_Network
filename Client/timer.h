#pragma once

#define TIMERID_BATTLE_INVALIDATE 900
#define ELAPSE_BATTLE_INVALIDATE 10

#define TIMERID_BATTLE_ANIMATION 901
#define ELAPSE_BATTLE_ANIMATION 50

#define TIMERID_BATTLE_ANIMATION_BOSS 902
#define ELAPSE_BATTLE_ANIMATION_BOSS 100

#define TIMERID_BATTLE_MOVE_PLAYER 903
#define ELAPSE_BATTLE_MOVE_PLAYER 10

#define TIMERID_BATTLE_EFFECT 904
#define ELAPSE_BATTLE_EFFECT 50

#define TIMERID_BATTLE_GUI 905
#define ELAPSE_BATTLE_GUI 10

void CALLBACK T_Battle_Invalidate(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime);
void CALLBACK T_Battle_Animate(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime);
void CALLBACK T_Battle_AnimateBoss(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime);
void CALLBACK T_Battle_MovePlayer(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime);
void CALLBACK T_Battle_Effect(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime);
void CALLBACK T_Battle_GUI(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime);


#define TIMERID_CLOUD 1
#define ELAPSE_CLOUD 100

#define TIMERID_LOGO 2
#define ELAPSE_LOGO 100

#define TIMERID_ANIMATION 3
#define ELAPSE_ANIMATION 10

#define TIMERID_TWINKLEEMOTION 4
#define ELAPSE_TWINKLEEMOTION 500

#define TIMERID_LOADING 5
#define ELAPSE_LOADING 80
#define FINISH_LOADING 4 

#define TIMERID_TPANIMATION 6
#define ELAPSE_TPANIMATION 10

#define TIMERID_TPANIMATION_DIR 7
#define ELAPSE_TPANIMATION_DIR 100

#define TIMERID_NPCMOTION 8
#define ELAPSE_NPCMOTION 100

#define TIMERID_TARGETMOVE 9 
#define ELAPSE_TARGETMOVE 10

#define TIMERID_BATTLEMAPMOVE 10
#define ELAPSE_BATTLEMAPMOVE 10

#define TIMERID_LOADINGBAR 11
#define ELAPSE_LOADINGBAR 450

#define TIMERID_SelectPokemonMove 12
#define ELAPSE_SelectPokemonMove 300

void CALLBACK T_MoveCloud(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime);
void CALLBACK T_MoveLogo(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime);
void CALLBACK T_Animation(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime);
void CALLBACK T_TwinkleEmotion(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime);
void CALLBACK T_Loading(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime);
void CALLBACK T_TPAnimation(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime);
void CALLBACK T_TPAnimationDir(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime);
void CALLBACK T_NpcMotion(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime);
void CALLBACK T_TargetMove(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime);
void CALLBACK T_Battle_MapMove(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime);
void CALLBACK T_Loadingbar(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime);
void CALLBACK T_SelectPokemonMove(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime);