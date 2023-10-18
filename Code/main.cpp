#include "stdafx.h"
#include "resource.h"

#include "interface.h"
#include "enemy.h"
#include "player.h"
#include "timer.h"
#include "effect.h"
#include "boss.h"
#include "scene.h"
#include "sound.h"

#include "intro.h"
#include "town.h"
#include "stage.h"
#include "phase.h"
#include "battle.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow){
	srand((unsigned int)time(NULL));

	HWND hWnd;
	MSG Message;
	WNDCLASSEX WndClass{};

	LPCTSTR lpszClass = L"Window Class Name";
	LPCTSTR lpszWindowName = L"Pokemon Flight";

	static Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	static ULONG_PTR gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	WndClass.cbSize = sizeof(WndClass);
	WndClass.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	WndClass.lpfnWndProc = (WNDPROC)WndProc;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hInstance = hInstance;
	WndClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	WndClass.lpszMenuName = NULL;
	WndClass.lpszClassName = lpszClass;
	WndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	RegisterClassEx(&WndClass);

	hWnd = CreateWindow(lpszClass,
		lpszWindowName,
		WS_OVERLAPPED,
		300,
		300,
		WINDOWSIZE_X,
		WINDOWSIZE_Y,
		NULL,
		(HMENU)NULL,
		hInstance,
		NULL);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	while (GetMessage(&Message, 0, 0, 0))
	{
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}

	Gdiplus::GdiplusShutdown(gdiplusToken);
	return Message.wParam;
}


Intro intro;
Loading loading;
Town town;
Stage stage;
Phase phase;
Battle battle;

Cloud cloud[4];
Logo logo;
Menu menu;
CImage glowing_black;

GameData gameData;
Player* player = nullptr;
EnemyController* enemies = nullptr;
EffectManager* effects = nullptr;
GUIManager* gui = nullptr;
Boss* boss = nullptr;
SceneManager* sceneManager = nullptr;
SoundManager* soundManager = nullptr;

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
	{
		sceneManager = new SceneManager();
		sceneManager->Init(hWnd);
	}
	break;
	case WM_ERASEBKGND:
		return FALSE;
	case WM_PAINT:
	{
		sceneManager->Paint(hWnd);
	}
	break;
	case WM_KEYDOWN:
		CheckKeyDown(hWnd, wParam);
		break;
	case WM_KEYUP:
		CheckKeyUp(hWnd, wParam);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}