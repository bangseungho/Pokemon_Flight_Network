#include "stdafx.h"
#include "town.h"
#include "scene.h"
#include "Network.h"

extern SceneManager* sceneManager;

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
	_glowing_black.Load(L"images\\loading\\Loading_Black_background.bmp");
}

// 타운에서 필요한 초기화 작업
void Town::Init(const RECT& rectWindow, const HWND& hWnd)
{
	mHwnd = hWnd;
	mAdjValue = POINT{ 0, 0 };

	// 타운에 존재하는 건물들의 충돌 처리를 위한 충s돌 박스 위치 설정
	_object[0] = { 0, 0, 125, 190 };
	_object[1] = { 156, 0, 300, 100 };
	_object[2] = { 337, 0, 750, 175 };
	_object[3] = { 644, 175, 750, 300 };
	_object[4] = { 0, 253, 144, 750 };
	_object[5] = { 130, 556, 267, 750 };
	_object[6] = { 267, 594, 325, 750 };
	_object[7] = { 325, 619, 362, 750 };
	_object[8] = { 687, 487, 750, 750 };
	_object[9] = { 725, 300, 750, 372 };
	_object[10] = { 218, 290, 247, 312 };
	_object[11] = { 306, 290, 334, 312 };
	_object[12] = { 184, 394, 312, 481 };
	_object[13] = { 500, 312, 600, 406 };
	_object[14] = { 184, 325, 187, 481 };
	_object[15] = { 506, 500, 594, 594 };
	_object[16] = { 0, 710, 750, 750 };
	_object[17] = { 189, 80, 191, 100 };

	_npcRect = { 125, 515, 194, 556 };
	_npc2Rect = { 406, 219, 194, 556 };
	_npc3Rect = { 581, 402, 605, 432 };
	_npc4Rect = { 550, 175, 387, 424 };

	_rectDraw = rectWindow;
	_rectImage = rectWindow;

	// 타운 플레이어 생성
	if (mPlayer == nullptr)
		mPlayer = new Player();

	mPlayer->_rectImage = { 0, 0, TPLAYER_IMAGESIZE_X, TPLAYER_IMAGESIZE_Y };

	// 이전 씬에 따라서 플레이어 방향과 위치 설정
	if (sceneManager->GetPrevScene() == Scene::Intro)
	{
		mPlayer->_Pos.x = 60;
		mPlayer->_Pos.y = 232;
		mPlayer->_dir = Dir::Right;
	}
	else if (sceneManager->GetPrevScene() == Scene::Stage)
	{
		mPlayer->_Pos.x = rectWindow.right / 2;
		mPlayer->_Pos.y = rectWindow.bottom / 2;
		mPlayer->_dir = Dir::Down;
	}

	// 첫 정보 넘겨주기
	//TownPlayerData playerData{ mPlayer->_Pos, mPlayer->_rectDraw, mPlayer->_rectImage };
	//TownData townData{ 0, playerData, false };
	//GET_SINGLE(Network)->SendTownData(townData);

	StopPlayer();

	// 절대 좌표값 출력을 위한 변수
	mPlayer->aboutMapPos = { mPlayer->_Pos.x, mPlayer->_Pos.y };
}

// 타운 화면 렌더링
void Town::Paint(HDC hdc, const RECT& rectWindow)
{
	// 타운 배경화면 렌더링
	_backTown.Draw(hdc, _rectDraw, _rectImage);

	// NPC 및 플레이어 렌더링
	_npc2.Draw(hdc, _npc2Rect.left, _npc2Rect.top, 40, 40, _npc2Move.x, _npc2Move.y, 64, 64);
	_npc3.TransparentBlt(hdc, _npc3Rect.left, _npc3Rect.top, 24, 30, 0, 0, 38, 48, RGB(255, 255, 255));
	_npc4.TransparentBlt(hdc, _npc4Rect.left, _npc4Rect.top, 34, 36, 0, 0, 54, 58, RGB(255, 255, 255));
	_npc.TransparentBlt(hdc, _npcRect.left, _npcRect.top, 75, 40, _npc1Move.x, _npc1Move.y, 50, 27, RGB(255, 255, 255));

	// 그릴때만 여러 번 그리기
	mPlayer->img.Draw(hdc, 
		mPlayer->_Pos.x - 20, mPlayer->_Pos.y - 20, 40, 40,
		mPlayer->_rectImage.left, 
		mPlayer->_rectImage.top, 
		mPlayer->_rectImage.right, 
		mPlayer->_rectImage.bottom);

	// 현재 멤버들의 정보를 가져오기
	const auto& members = GET_SINGLE(Network)->GetMemberMap();

	for (const auto& member : members) {
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
	};

	// 타운 건물 렌더링
	if (_allHide == true)
	{
		// 건물 오브젝트1 박스
		TCHAR lpOut[TOWN_OBJECT_NUM];
		for (int i = 0; i < TOWN_OBJECT_NUM; i++)
		{
			wsprintf(lpOut, L"%d", i);
			Rectangle(hdc, _object[i].left, _object[i].top, _object[i].right, _object[i].bottom);
			TextOut(hdc, _object[i].left + 10, _object[i].top + 10, lpOut, lstrlen(lpOut));
		}


		HBRUSH hBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
		HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, hBrush);

		Rectangle(hdc, mPlayer->_rectDraw.left, mPlayer->_rectDraw.top, mPlayer->_rectDraw.right, mPlayer->_rectDraw.bottom);
		Rectangle(hdc, mPlayer->_cam.left, mPlayer->_cam.top, mPlayer->_cam.right, mPlayer->_cam.bottom);

		TCHAR Posx[1000];
		TCHAR Posy[1000];
		wsprintf(Posx, L"X : %d", mPlayer->aboutMapPos.x);
		wsprintf(Posy, L"Y : %d", mPlayer->aboutMapPos.y);
		TextOut(hdc, mPlayer->_Pos.x - 25, mPlayer->_Pos.y + 30, Posx, lstrlen(Posx));
		TextOut(hdc, mPlayer->_Pos.x - 25, mPlayer->_Pos.y + 50, Posy, lstrlen(Posy));

		for (const auto& member : members) {
			POINT newPos = member.second.mTownData.PlayerData.Pos;
			wsprintf(Posx, L"X : %d", newPos.x);
			wsprintf(Posy, L"Y : %d", newPos.y);

			newPos = newPos + mAdjValue;

			TextOut(hdc, newPos.x - 25, newPos.y + 30, Posx, lstrlen(Posx));
			TextOut(hdc, newPos.x - 25, newPos.y + 50, Posy, lstrlen(Posy));
		};

		SelectObject(hdc, oldBrush);
		DeleteObject(hBrush);
	}

	// 종료 변수가 true라면 종료 여부 화면 렌더링
	if (_exit == true)
	{
		_glowing_black.AlphaBlend(hdc, 0, 0, 500, 750, 0, 0, 500, 750, ALPHA);
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

void Town::Update(const RECT& rectWindow)
{
	//if (GetForegroundWindow() != mHwnd)
	//	return;

	RECT temp;
	mPlayer->_cam = { mPlayer->_Pos.x - CAMSIZE_X, rectWindow.top, mPlayer->_Pos.x + CAMSIZE_X, rectWindow.bottom };

	// 문 도착시 게임 종료
	if (IntersectRect(&temp, &mPlayer->_rectDraw, &_object[17]))
		_exit = true;

	// 모든 오브젝트를 검사하여 충돌시 반대로 1씩 올린다.
	for (int i = 0; i < TOWN_OBJECT_NUM; i++)
	{
		if (IntersectRect(&temp, &mPlayer->_rectDraw, &_object[i]))
		{
			switch (mPlayer->_dir)
			{
			case Dir::Left:
				mPlayer->aboutMapPos.x += 1;
				mPlayer->_Pos.x += 1;
				return;
			case Dir::Right:
				mPlayer->aboutMapPos.x -= 1;
				mPlayer->_Pos.x -= 1;
				return;
			case Dir::Up:
				mPlayer->aboutMapPos.y += 1;
				mPlayer->_Pos.y += 1;
				return;
			case Dir::Down:
				mPlayer->aboutMapPos.y -= 1;
				mPlayer->_Pos.y -= 1;
				return;
			}
		}
	}

	// _exit 변수가 true일 경우 엔터키를 누르면 게임 종료
	if (GetAsyncKeyState(VK_RETURN) & 0x8000 && _exit == true)
	{
		GET_SINGLE(Network)->SendDataToTemplate(EndProcessing{ GET_SINGLE(Network)->GetClientIndex() });
		PostQuitMessage(0);
	}

	// 플레이어의 위치 이동
	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
	{
		mActive = true;
		mPlayer->_keepGoing = true;
		mPlayer->_dir = Dir::Left;
		mPlayer->aboutMapPos.x -= TPLAYER_SPEED;
		_exit = false;

		// 만약 플레이어를 찍고 있는 카메라 왼쪽 위치 값이 윈도우 화면의 왼쪽에 닿으면 오브젝트는 반대로 이동시킨다.
		if (mPlayer->_cam.left < rectWindow.left && _rectImage.left > 0)
		{
			mAdjValue.x += TPLAYER_SPEED;

			_rectImage.right -= TPLAYER_SPEED;
			_rectImage.left -= TPLAYER_SPEED;

			// 모든 건물들을 자리 그대로 놓기 -> 안하면 플레이어 이동시 오브젝트가 같이 움직임
			for (int i = 0; i < TOWN_OBJECT_NUM; i++)
			{
				_object[i].left += TPLAYER_SPEED;
				_object[i].right += TPLAYER_SPEED;
			}

			_npcRect.left += TPLAYER_SPEED;
			_npc2Rect.left += TPLAYER_SPEED;
			_npc3Rect.left += TPLAYER_SPEED;
			_npc4Rect.left += TPLAYER_SPEED;
		}
		else
			mPlayer->_Pos.x -= TPLAYER_SPEED;
	}
	else if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
	{
		mActive = true;
		mPlayer->_keepGoing = true;
		mPlayer->_dir = Dir::Right;
		mPlayer->aboutMapPos.x += TPLAYER_SPEED;
		_exit = false;

		if (mPlayer->_cam.right > rectWindow.right && _rectImage.right < 748)
		{
			mAdjValue.x -= TPLAYER_SPEED;

			_rectImage.right += TPLAYER_SPEED;
			_rectImage.left += TPLAYER_SPEED;

			for (int i = 0; i < TOWN_OBJECT_NUM; i++)
			{
				_object[i].left -= TPLAYER_SPEED;
				_object[i].right -= TPLAYER_SPEED;
			}

			_npcRect.left -= TPLAYER_SPEED;
			_npc2Rect.left -= TPLAYER_SPEED;
			_npc3Rect.left -= TPLAYER_SPEED;
			_npc4Rect.left -= TPLAYER_SPEED;
		}
		else
			mPlayer->_Pos.x += TPLAYER_SPEED;
	}
	else if (GetAsyncKeyState(VK_UP) & 0x8000)
	{
		mActive = true;
		mPlayer->aboutMapPos.y -= TPLAYER_SPEED;
		mPlayer->_keepGoing = true;
		mPlayer->_dir = Dir::Up;
		mPlayer->_Pos.y -= TPLAYER_SPEED;
	}
	else if (GetAsyncKeyState(VK_DOWN) & 0x8000)
	{
		mActive = true;
		_exit = false;
		mPlayer->aboutMapPos.y += TPLAYER_SPEED;
		mPlayer->_keepGoing = true;
		mPlayer->_dir = Dir::Down;
		mPlayer->_Pos.y += TPLAYER_SPEED;
	}

	// 키가 눌린 경우에만 패킷을 송신한다.
	if (true == mPlayer->_keepGoing && mActive) {
		TownData::TownPlayerData playerData{ mPlayer->aboutMapPos, mPlayer->_rectDraw, mPlayer->_rectImage };
		TownData townData{ GET_SINGLE(Network)->GetClientIndex(), playerData, false};
		GET_SINGLE(Network)->SendTownData(townData);
	}

	mActive = false;
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