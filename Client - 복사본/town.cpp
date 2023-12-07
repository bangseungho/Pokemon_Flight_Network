#include "stdafx.h"
#include "town.h"
#include "scene.h"
#include "Network.h"
#include "gameTimer.h"
#include "sound.h"
#include "timer.h"

extern SceneManager* sceneManager;
extern SoundManager* soundManager;

// 타운 화면에 필요한 이미지 모두 로드
Town::Town()
{
	mPlayer = new Player();
	_backTown.Load(L"images\\town\\Town.png");
	mPlayer->img.Load(L"images\\town\\TownPlayer.png");
	_npc.Load(L"images\\town\\npc1.bmp");
	_npc2.Load(L"images\\town\\npc2.png");
	_npc3.Load(L"images\\town\\npc3.bmp");
	_npc4.Load(L"images\\town\\npc4.bmp");
	_exits.Load(L"images\\dialog\\Exit.bmp");
	mGlowingBlack.Load(L"images\\loading\\Loading_Black_background.bmp");
}

// 타운에서 필요한 초기화 작업
void Town::Init()
{
	mHwnd = sceneManager->GetHwnd();
	const RECT& rectWindow = sceneManager->GetRectWindow();
	mAdjValue = Vector2{ 0, 0 };

	// 타운에 존재하는 건물들의 충돌 처리를 위한 충s돌 박스 위치 설정
	for (int i = 0; i < TOWN_OBJECT_NUM; ++i) {
		mObjects[i] = std::make_shared<TownObject>();
	}

	for (int i = 0; i < NPC_COUNT; ++i) {
		mNpcs[i] = std::make_shared<NpcObject>();
	}

	mObjects[0]->Init(FRECT{ 0, 0, 125, 190 });
	mObjects[1]->Init(FRECT{ 156, 0, 300, 100 });
	mObjects[2]->Init(FRECT{ 337, 0, 750, 175 });
	mObjects[3]->Init(FRECT{ 644, 175, 750, 300 });
	mObjects[4]->Init(FRECT{ 0, 253, 144, 750 });
	mObjects[5]->Init(FRECT{ 130, 556, 267, 750 });
	mObjects[6]->Init(FRECT{ 267, 594, 325, 750 });
	mObjects[7]->Init(FRECT{ 325, 619, 362, 750 });
	mObjects[8]->Init(FRECT{ 687, 487, 750, 750 });
	mObjects[9]->Init(FRECT{ 725, 300, 750, 372 });
	mObjects[10]->Init(FRECT{ 218, 290, 247, 312 });
	mObjects[11]->Init(FRECT{ 306, 290, 334, 312 });
	mObjects[12]->Init(FRECT{ 184, 394, 312, 481 });
	mObjects[13]->Init(FRECT{ 500, 312, 600, 406 });
	mObjects[14]->Init(FRECT{ 184, 325, 187, 481 });
	mObjects[15]->Init(FRECT{ 506, 500, 594, 594 });
	mObjects[16]->Init(FRECT{ 0, 710, 750, 750 });
	mObjects[17]->Init(FRECT{ 189, 80, 191, 100 });

	mNpcs[0]->Init(FRECT{ 125, 515, 194, 556 }, L"images\\town\\npc1.bmp");
	mNpcs[1]->Init(FRECT{ 406, 219, 194, 556 }, L"images\\town\\npc2.png");
	mNpcs[2]->Init(FRECT{ 581, 402, 605, 432 }, L"images\\town\\npc3.bmp");
	mNpcs[3]->Init(FRECT{ 550, 175, 387, 424 }, L"images\\town\\npc4.bmp");

	_npcRect = { 125, 515, 194, 556 };
	_npc2Rect = { 406, 219, 194, 556 };
	_npc3Rect = { 581, 402, 605, 432 };
	_npc4Rect = { 550, 175, 387, 424 };

	_rectDraw = rectWindow;
	_rectImage = FRECT{ (float)rectWindow.left, (float)rectWindow.top, (float)rectWindow.right, (float)rectWindow.bottom };

	// 타운 플레이어 생성
	if (mPlayer == nullptr)
		mPlayer = new Player();

	mPlayer->_rectImage = { 0, 0, TPLAYER_IMAGESIZE_X, TPLAYER_IMAGESIZE_Y };

	// 이전 씬에 따라서 플레이어 방향과 위치 설정
	if (sceneManager->GetPrevScene() == Scene::Intro)
	{
		mPlayer->_Pos.x = 100;
		mPlayer->_Pos.y = 232;
		mPlayer->_dir = Dir::Right;
	}
	else if (sceneManager->GetPrevScene() == Scene::Stage)
	{
		mPlayer->_Pos.x = 100;
		mPlayer->_Pos.y = 232;
		mPlayer->_dir = Dir::Down;
	}

	StopPlayer();

	// 절대 좌표값 출력을 위한 변수
	mPlayer->aboutMapPos = { mPlayer->_Pos.x, mPlayer->_Pos.y };
	mPlayer->mIsReady = false;
	TownData::TownPlayerData playerData{ mPlayer->aboutMapPos, mPlayer->_rectDraw, mPlayer->_rectImage };
	TownData sendData{ GET_SINGLE(Network)->GetClientIndex(), playerData, mPlayer->mIsReady, 0 };
	GET_SINGLE(Network)->SendDataAndType(sendData);

	soundManager->StopBGMSound();
	const int randBgm = rand() % 2;
	if (randBgm == 0)
	{
		soundManager->PlayBGMSound(BGMSound::Town1, 1.0f, true);
	}
	else
	{
		soundManager->PlayBGMSound(BGMSound::Town2, 1.0f, true);
	}
}

// 타운 화면 렌더링
void Town::Paint(HDC hdc, const RECT& rectWindow)
{
	// 타운 배경화면 렌더링
	_backTown.Draw(hdc, _rectDraw, _rectImage);

	// NPC 및 플레이어 렌더링 
	mNpcs[0]->mCImage.TransparentBlt(hdc, mNpcs[0]->mRectDraw.left, mNpcs[0]->mRectDraw.top, 75, 40, _npc1Move.x, _npc1Move.y, 50, 27, RGB(255, 255, 255));
	mNpcs[1]->mCImage.Draw(hdc, mNpcs[1]->mRectDraw.left, mNpcs[1]->mRectDraw.top, 40, 40, _npc2Move.x, _npc2Move.y, 64, 64);
	mNpcs[2]->mCImage.TransparentBlt(hdc, mNpcs[2]->mRectDraw.left, mNpcs[2]->mRectDraw.top, 24, 30, 0, 0, 38, 48, RGB(255, 255, 255));
	mNpcs[3]->mCImage.TransparentBlt(hdc, mNpcs[3]->mRectDraw.left, mNpcs[3]->mRectDraw.top, 34, 36, 0, 0, 54, 58, RGB(255, 255, 255));

	// 그릴때만 여러 번 그리기
	mPlayer->img.Draw(hdc,
		mPlayer->_Pos.x - 20, mPlayer->_Pos.y - 20, 40, 40,
		mPlayer->_rectImage.left,
		mPlayer->_rectImage.top,
		mPlayer->_rectImage.right,
		mPlayer->_rectImage.bottom);

	TCHAR ID[1000];
	wsprintf(ID, L"ID : %d", (uint32)GET_SINGLE(Network)->GetClientIndex());
	TextOut(hdc, mPlayer->_Pos.x - 25, mPlayer->_Pos.y + 30, ID, lstrlen(ID));

	// 현재 멤버들의 정보를 가져오기
	const auto& members = GET_MEMBER_MAP;
	for (const auto& member : members) {
		if (member.first == MY_INDEX)
			continue;

		// 현재 멤버가 타운 씬에 없다면 그리지 않는다.
		if (member.second.mSceneData.Scene != static_cast<uint8>(Scene::Town))
			continue;

		// 현재 멤버 위치 보간
		POINT newPos = member.second.mTownData.PlayerData.Pos + mAdjValue;

		mPlayer->img.Draw(hdc,
			newPos.x - 20, newPos.y - 20, 40, 40,
			member.second.mTownData.PlayerData.RectImage.left,
			member.second.mTownData.PlayerData.RectImage.top,
			member.second.mTownData.PlayerData.RectImage.right,
			member.second.mTownData.PlayerData.RectImage.bottom);

		wsprintf(ID, L"ID : %d", (uint32)member.first);
		TextOut(hdc, newPos.x - 25, newPos.y + 30, ID, lstrlen(ID));
	};

	// 타운 건물 렌더링
	if (_allHide == true)
	{
		TCHAR lpOut[TOWN_OBJECT_NUM];
		for (int i = 0; i < TOWN_OBJECT_NUM; i++)
		{
			wsprintf(lpOut, L"%d", i);
			Rectangle(hdc, mObjects[i]->mRectDraw.left, mObjects[i]->mRectDraw.top, mObjects[i]->mRectDraw.right, mObjects[i]->mRectDraw.bottom);
			TextOut(hdc, mObjects[i]->mRectDraw.left + 10, mObjects[i]->mRectDraw.top + 10, lpOut, lstrlen(lpOut));
		}

		HBRUSH hBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
		HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, hBrush);

		Rectangle(hdc, mPlayer->_rectDraw.left, mPlayer->_rectDraw.top, mPlayer->_rectDraw.right, mPlayer->_rectDraw.bottom);
		Rectangle(hdc, mPlayer->_cam.left, mPlayer->_cam.top, mPlayer->_cam.right, mPlayer->_cam.bottom);

		TCHAR Posx[1000];
		TCHAR Posy[1000];
		wsprintf(Posx, L"X : %d", (int)mPlayer->aboutMapPos.x);
		wsprintf(Posy, L"Y : %d", (int)mPlayer->aboutMapPos.y);
		TextOut(hdc, mPlayer->_Pos.x - 25, mPlayer->_Pos.y + 30, Posx, lstrlen(Posx));
		TextOut(hdc, mPlayer->_Pos.x - 25, mPlayer->_Pos.y + 50, Posy, lstrlen(Posy));

		for (const auto& member : members) {
			if (member.first == MY_INDEX)
				continue;

			POINT newPos = member.second.mTownData.PlayerData.Pos;
			wsprintf(Posx, L"X : %d", newPos.x);
			wsprintf(Posy, L"Y : %d", newPos.y);

			TextOut(hdc, newPos.x - 25 + mAdjValue.x, newPos.y + 30, Posx, lstrlen(Posx));
			TextOut(hdc, newPos.x - 25 + mAdjValue.x, newPos.y + 50, Posy, lstrlen(Posy));
		};

		SelectObject(hdc, oldBrush);
		DeleteObject(hBrush);
	}

	// 종료 변수가 true라면 종료 여부 화면 렌더링
	if (_exit == true)
	{
		mGlowingBlack.AlphaBlend(hdc, 0, 0, 500, 750, 0, 0, 500, 750, ALPHA);
		_exits.Draw(hdc, rectWindow.right / 2 - 100, rectWindow.bottom / 2 + 60, 200, 51, 0, 0, 810, 210);

		HFONT hFont = CreateFont(30, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, VARIABLE_PITCH | FF_ROMAN, TEXT("ChubbyChoo-SemiBold"));
		HFONT oldFont = (HFONT)SelectObject(hdc, hFont);
		SetBkMode(hdc, TRANSPARENT);
		SetTextColor(hdc, RGB(255, 255, 255));

		TextOut(hdc, 70, rectWindow.bottom / 2, L"PRESS ENTER KEY TO EXIT...", 25);

		SelectObject(hdc, oldFont);
		DeleteObject(hFont);
	}
}

void Town::Animate(float elpasedTime)
{
	static float accTime = 0.f;
	accTime += elpasedTime;

	if (accTime >= 0.1f) {
		if (mPlayer->_keepGoing == true)
		{
			mPlayer->_rectImage.left += TPLAYER_IMAGESIZE_X;

			if (mPlayer->_rectImage.left == TPLAYER_MAX_IMAGESIZE_X)
				mPlayer->_rectImage.left = 0;

			switch (mPlayer->_dir)
			{
			case Dir::Left:
				mPlayer->_rectImage.top = DIR_LEFT_IMAGE_Y;
				break;
			case Dir::Up:
				mPlayer->_rectImage.top = DIR_UP_IMAGE_Y;
				break;
			case Dir::Right:
				mPlayer->_rectImage.top = DIR_RIGHT_IMAGE_Y;
				break;
			case Dir::Down:
				mPlayer->_rectImage.top = DIR_DOWN_IMAGE_Y;
				break;
			}
		}
		accTime = 0.f;
	}
}

void Town::AnimateNPC(float elpasedTime)
{
	static float accTime = 0.f;
	accTime += elpasedTime;

	static int npc1cnt = 0;
	static int npc2cnt = 0;

	// npc2의 방향을 반대로 바꾸기 위한 변수
	static int negative_number = 1;

	if (accTime >= 0.1f) {

		// npc1의 모션 주기
		npc1cnt++;
		// npc2의 모션 주기
		npc2cnt++;

		// npc1의 애니메이션
		if (_npc1Move.x < 150) {
			_npc1Move.x += 50;
		}
		if (npc1cnt == 100)
		{
			npc1cnt = 0;
			_npc1Move.x = 0;
		}

		// npc2가 위치값이 1000이 넘을 경우 반대로 다시 걸어가기
		if (_npc2Rect.top > 625)
		{
			negative_number = -1;
			_npc2Move.y = 192;
		}
		if (_npc2Rect.top < 219)
		{
			negative_number = 1;
			_npc2Move.y = 0;
		}

		// warking npc2의 cnt가 5의 배수가 될때마다 앞으로 한걸음
		if (npc2cnt % 5 == 0)
		{
			mNpcs[1]->mPos.y += 5.f * negative_number;
			mNpcs[1]->ConvertToFRECT();

			_npc2Move.x += 64;
			_npc2Rect.top += 6 * negative_number;
		}
		if (npc2cnt == 20 && _npc2Move.x > 192)
		{
			npc2cnt = 0;
			_npc2Move.x = 0;
		}
		accTime = 0.f;
	}
}

Vector2 abs(const Vector2& point)
{
	Vector2 ret;
	ret.x = abs(point.x);
	ret.y = abs(point.y);

	return ret;
}

void Town::Update(float elapedTime)
{
	AnimateNPC(elapedTime);

	if (sceneManager->IsLoading() == true)
		return;

	bool allReady = all_of(GET_MEMBER_MAP.begin(), GET_MEMBER_MAP.end(), [](const auto& a) {
		return a.second.mTownData.IsReady == 1;
		});

	if (allReady) {
		Sleep(10);
		sceneManager->StartLoading(sceneManager->GetHwnd());
		_nextFlow = Scene::Stage;
		mAdjValue = Vector2{ 0, 0 };
	}


	const RECT rectWindow = sceneManager->GetRectWindow();

	// 문 도착시 게임 종료
	RECT rect = mObjects[17]->mRectDraw;
	if (IntersectRect2(mPlayer->_rectDraw, rect))
		_exit = true;

	// 모든 오브젝트를 검사하여 충돌시 반대로 1씩 올린다.
	for (int i = 0; i < TOWN_OBJECT_NUM; i++)
	{
		RECT rect = mObjects[i]->mRectDraw;
		if (IntersectRect2(mPlayer->_rectDraw, rect))
		{
			switch (mPlayer->_dir)
			{
			case Dir::Left:
				mPlayer->aboutMapPos.x += 1;
				mPlayer->_Pos.x += 1;
				break;
			case Dir::Right:
				mPlayer->aboutMapPos.x -= 1;
				mPlayer->_Pos.x -= 1;
				break;
			case Dir::Up:
				mPlayer->aboutMapPos.y += 1;
				mPlayer->_Pos.y += 1;
				break;
			case Dir::Down:
				mPlayer->aboutMapPos.y -= 1;
				mPlayer->_Pos.y -= 1;
				break;
			}
			mPlayer->_rectDraw = FRECT{ mPlayer->_Pos.x - 20, mPlayer->_Pos.y - 20, mPlayer->_Pos.x + 20, mPlayer->_Pos.y + 20 };
			return;
		}
	}

	if (mPlayer->_Pos.x + 20 >= rectWindow.right)
	{
		mPlayer->mIsReady = true;
		mPlayer->_Pos.x -= 1;
		mPlayer->aboutMapPos.x -= 1;
	}
	else if (mPlayer->_Pos.x - 20 <= rectWindow.left)
	{
		sceneManager->StartLoading(sceneManager->GetHwnd());
		_nextFlow = Scene::Intro;
		mAdjValue = Vector2{ 0, 0 };
	}

	if (GetAsyncKeyState('A') & 0x8000) {
		_exit = false;
		mPlayer->mIsReady = false;
		mPlayer->_dir = Dir::Left;
		Vector2 interval = { -TPLAYER_SPEED * elapedTime, 0 };
		mPlayer->aboutMapPos += interval;

		if (mPlayer->_cam.left < rectWindow.left && _rectImage.left > 10)
		{
			_rectImage.right += interval.x;
			_rectImage.left += interval.x;
			mAdjValue -= interval;

			for (auto& townObject : mObjects) {
				townObject->mPos -= interval;
				townObject->ConvertToFRECT();
			}

			for (auto& npcObject : mNpcs) {
				npcObject->mPos -= interval;
				npcObject->ConvertToFRECT();
			}
		}
		else {
			mPlayer->_Pos += interval;
		}
	}
	else if (GetAsyncKeyState('D') & 0x8000) {
		_exit = false;
		mPlayer->_dir = Dir::Right;
		Vector2 interval = { TPLAYER_SPEED * elapedTime, 0 };
		mPlayer->aboutMapPos += interval;

		if (mPlayer->_cam.right > rectWindow.right && _rectImage.right < 748)
		{
			_rectImage.right += interval.x;
			_rectImage.left += interval.x;
			mAdjValue -= interval;

			for (auto& townObject : mObjects) {
				townObject->mPos -= interval;
				townObject->ConvertToFRECT();
			}

			for (auto& npcObject : mNpcs) {
				npcObject->mPos -= interval;
				npcObject->ConvertToFRECT();
			}
		}
		else
			mPlayer->_Pos += interval;
	}
	else if (GetAsyncKeyState('W') & 0x8000) {
		Vector2 interval = { 0, -TPLAYER_SPEED * elapedTime };
		mPlayer->aboutMapPos += interval;
		mPlayer->_dir = Dir::Up;
		mPlayer->_Pos += interval;
	}
	else if (GetAsyncKeyState('S') & 0x8000) {
		_exit = false;
		Vector2 interval = { 0, TPLAYER_SPEED * elapedTime };
		mPlayer->aboutMapPos += interval;
		mPlayer->_dir = Dir::Down;
		mPlayer->_Pos += interval;
	}
	else if (GetAsyncKeyState(VK_INSERT) & 0x0001 && _exit) {
		GET_SINGLE(Network)->SendDataAndType(EndProcessing{ GET_SINGLE(Network)->GetClientIndex() });
		PostQuitMessage(0);
	}
	else {
		InvalidateRect(sceneManager->GetHwnd(), NULL, false);
		return;
	}

	// 방향키가 눌러졌다면 패킷 송신
	TownData::TownPlayerData playerData{ mPlayer->aboutMapPos, mPlayer->_rectDraw, mPlayer->_rectImage };
	TownData townData{ GET_SINGLE(Network)->GetClientIndex(), playerData, mPlayer->mIsReady, 0 };
	GET_SINGLE(Network)->SendDataAndType(townData);

	Animate(elapedTime);

	mPlayer->_cam = FRECT{ mPlayer->_Pos.x - CAMSIZE_X, 0, mPlayer->_Pos.x + CAMSIZE_X, (float)rectWindow.bottom };
	mPlayer->_rectDraw = FRECT{ mPlayer->_Pos.x - 20, mPlayer->_Pos.y - 20, mPlayer->_Pos.x + 20, mPlayer->_Pos.y + 20 };
	mPlayer->_keepGoing = true;
	InvalidateRect(sceneManager->GetHwnd(), NULL, false);
}

void Town::StopPlayer()
{
	mPlayer->_rectImage.left = 0; // 정지시 차렷자세
	mPlayer->_keepGoing = false; // 키 업시 움직임 모션 타이머 정지
}

int Town::GetCamSizeX()
{
	return CAMSIZE_X;
}
int Town::GetCamSizeY()
{
	return CAMSIZE_Y;
}