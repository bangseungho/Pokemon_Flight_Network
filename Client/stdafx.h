#pragma once
#pragma warning(disable:28251)
#pragma warning(disable:4244)

#ifdef _DEBUG
#ifdef UNICODE                                                                                      
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console") 
#else                                                                                                    
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")   
#endif                                                                                                   
#endif  

#include "..\Utils.h"
#include <Windows.h>
#include <time.h>
#include <gdiplus.h>
#include <atlImage.h>
#include <mmsystem.h>
#include <fmod.hpp>
#include <cmath>
#include <vector>
#include <cassert>
#include <algorithm>
#include <unordered_map>
#include <array>
#include <memory>

#pragma region Window Define
#define WINDOWSIZE_X				500
#define WINDOWSIZE_Y				750
#define MSEC						1000
#pragma endregion
#pragma region Intro Define
#define INSTRUCTION_BACKGROUND_X	500
#define INSTRUCTION_BACKGROUND_Y	750
#define LOGO_COLOR					RGB(255, 212, 0)
#define LOGO_SHADOW_COLOR			RGB(200, 100, 0)
#define LOGO_SIZE					106
#define MENU_SIZE					31
#define IALPHA						230
#define CLOUD_NUM					4
#define FIRSTCLOUD_X				125
#define FIRSTCLOUD_Y				187
#define SECONDCLOUD_X				187
#define SECONDCLOUD_Y				31
#define THIRDCLOUD_X				-62
#define THIRDCLOUD_Y				93
#define FOURTHCLOUD_X				437
#define FOURTHCLOUD_Y				187
#pragma endregion
#pragma region Town Define
#define TPLAYER_SPEED				4
#pragma endregion
#pragma region Loading Define
#define LOADING_BAR_X				182
#define LOADING_BAR_Y				397
#define TIMERID_LODING				5
#define HALF_RECTWINDOW_X			175 
#define HALF_RECTWINDOW_Y			280
#define LOADING_POKEMON_X			150
#define LOADING_POKEMON_Y			121
#define LOADING_POKEMON_MAX_X		2100
#define LOADING_POKEMON_MAX_Y		121
#pragma endregion
#pragma region Stage Define
#define TARGET_IMAGESIZE_X			512
#define TARGET_IMAGESIZE_Y			512
#define CAMSIZE_X					162 
#define CAMSIZE_Y					375
#define STAGE_NUM					5
#define MAPSCROLL_SPEED				4
#define SELECTPOKEMONSIZE			135
#pragma endregion
#pragma region Scene Define
#define SALPHA						220
#pragma endregion
#pragma region Timer Define
#define TPLAYER_IMAGESIZE_X			64
#define TPLAYER_MAX_IMAGESIZE_X		256
#define DIR_LEFT_IMAGE_Y			64
#define DIR_RIGHT_IMAGE_Y			128
#define DIR_UP_IMAGE_Y				192
#define DIR_DOWN_IMAGE_Y			0
#pragma endregion
#pragma region Interface Define
#define VK_R 0x52
#define Endl endl << endl
#define PI 3.141592
#define DEGREE_TO_RADIAN(degree) ((PI/180) * (degree))
#define RADIAN_TO_DEGREE(radian) ((180/PI) * (radian))
#pragma endregion
#pragma region Battle Define
#define ALPHA 150
#pragma endregion

#define	WIN32_LEAN_AND_MEAN
#undef WINVER
#define WINVER 0x6000

class MyGameObject
{
public:
	virtual void Init();
	virtual void Init(const FRECT& draw, const wchar_t* imgfile = nullptr);
	virtual void Init(const Vector2& pos, const Vector2& size, const RECT& rectImage = RECT{}, const wchar_t* imgfile = nullptr);
	virtual void Init(const Vector2& pos, const wchar_t* imgfile = nullptr);
	virtual void Update(float elapsedTime);
	virtual void Paint(HDC hdc);

public:
	void ConvertToVector() {
		mPos = Vector2{ (mRectDraw.right + mRectDraw.left) / 2.f, (mRectDraw.bottom + mRectDraw.top) / 2.f };
		mSize = Vector2{ (mRectDraw.right - mRectDraw.left) / 2.f, (mRectDraw.bottom - mRectDraw.top) / 2.f };
	}

	void ConvertToFRECT()
	{
		mRectDraw = FRECT{ mPos.x - mSize.x, mPos.y - mSize.y, mPos.x + mSize.x, mPos.y + mSize.y };
	}

public:
	CImage		mCImage;
	Vector2		mPos;
	Vector2		mSize = { 0, };
	FRECT		mRectDraw = { 0, };
	RECT		mRectImage = { 0, };
};


