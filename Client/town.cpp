#include "stdafx.h"
#include "town.h"
#include "scene.h"
#include "Network.h"
#include "gameTimer.h"
#include "sound.h"
#include "timer.h"

extern SceneManager* sceneManager;

// Ÿ�� ȭ�鿡 �ʿ��� �̹��� ��� �ε�
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

// Ÿ��� �ʿ��� �ʱ�ȭ �۾�
void Town::Init()
{
	mHwnd = sceneManager->GetHwnd();
	const RECT& rectWindow = sceneManager->GetRectWindow();
	mAdjValue = Vector2{ 0, 0 };

	// Ÿ� �����ϴ� �ǹ����� �浹 ó���� ���� ��s�� �ڽ� ��ġ ����
	for (int i = 0; i < TOWN_OBJECT_NUM; ++i) {
		mObjects[i] = std::make_shared<TownObject>();
	}

	_npcRect = { 125, 515, 194, 556 };
	_npc2Rect = { 406, 219, 194, 556 };
	_npc3Rect = { 581, 402, 605, 432 };
	_npc4Rect = { 550, 175, 387, 424 };

	_rectDraw = rectWindow;
	_rectImage = rectWindow;

	// Ÿ�� �÷��̾� ����
	if (mPlayer == nullptr)
		mPlayer = new Player();

	mPlayer->_rectImage = { 0, 0, TPLAYER_IMAGESIZE_X, TPLAYER_IMAGESIZE_Y };

	// ���� ���� ���� �÷��̾� ����� ��ġ ����
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

	StopPlayer();

	// ���� ��ǥ�� ����� ���� ����
	mPlayer->aboutMapPos = { mPlayer->_Pos.x, mPlayer->_Pos.y };

	TownData::TownPlayerData playerData{ mPlayer->aboutMapPos, mPlayer->_rectDraw, mPlayer->_rectImage };
	TownData sendData{ GET_SINGLE(Network)->GetClientIndex(), playerData, false };
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

	SetTimer(mHwnd, TIMERID_TPANIMATION_DIR, ELAPSE_TPANIMATION_DIR, T_TPAnimationDir); // �÷��̾� ���� Ÿ�̸�
	SetTimer(mHwnd, TIMERID_NPCMOTION, ELAPSE_NPCMOTION, T_NpcMotion); // NPC ������ Ÿ�̸�
}

// Ÿ�� ȭ�� ������
void Town::Paint(HDC hdc, const RECT& rectWindow)
{
	// Ÿ�� ���ȭ�� ������
	_backTown.Draw(hdc, _rectDraw, _rectImage);

	// NPC �� �÷��̾� ������ 
	mNpcs[0]->mCImage.TransparentBlt(hdc, mNpcs[0]->mRectDraw.left, mNpcs[0]->mRectDraw.top, 75, 40, _npc1Move.x, _npc1Move.y, 50, 27, RGB(255, 255, 255));
	mNpcs[1]->mCImage.Draw(hdc, mNpcs[1]->mRectDraw.left, mNpcs[1]->mRectDraw.top, 40, 40, _npc2Move.x, _npc2Move.y, 64, 64);
	mNpcs[2]->mCImage.TransparentBlt(hdc, mNpcs[2]->mRectDraw.left, mNpcs[2]->mRectDraw.top, 24, 30, 0, 0, 38, 48, RGB(255, 255, 255));
	mNpcs[3]->mCImage.TransparentBlt(hdc, mNpcs[3]->mRectDraw.left, mNpcs[3]->mRectDraw.top, 34, 36, 0, 0, 54, 58, RGB(255, 255, 255));

	// �׸����� ���� �� �׸���
	mPlayer->img.Draw(hdc,
		mPlayer->_Pos.x - 20, mPlayer->_Pos.y - 20, 40, 40,
		mPlayer->_rectImage.left,
		mPlayer->_rectImage.top,
		mPlayer->_rectImage.right,
		mPlayer->_rectImage.bottom);

	TCHAR ID[1000];
	wsprintf(ID, L"ID : %d", (uint32)GET_SINGLE(Network)->GetClientIndex());
	TextOut(hdc, mPlayer->_Pos.x - 25, mPlayer->_Pos.y + 30, ID, lstrlen(ID));

	// ���� ������� ������ ��������
	const auto& members = GET_MEMBER_MAP;
	for (const auto& member : members) {
		if (member.first == MY_INDEX)
			continue;

		// ���� ����� Ÿ�� ���� ���ٸ� �׸��� �ʴ´�.
		if (member.second.mSceneData.Scene != static_cast<uint8>(Scene::Town))
			continue;

		// ���� ��� ��ġ ����
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

	// Ÿ�� �ǹ� ������
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

	// ���� ������ true��� ���� ���� ȭ�� ������
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

void Town::Update(float elapedTime)
{
	if (sceneManager->IsLoading() == true)
		return;

	if (MEMBER_MAP(MY_INDEX).mTownData.IsReady == true)
	{
		sceneManager->StartLoading(sceneManager->GetHwnd());
		_nextFlow = Scene::Stage;
		mAdjValue = Vector2{ 0, 0 };
		mPlayer->mCanNextScene = false;
	}

	const RECT rectWindow = sceneManager->GetRectWindow();

	// �� ������ ���� ����
	RECT rect = mObjects[17]->mRectDraw;
	if (IntersectRect2(mPlayer->_rectDraw, rect))
		_exit = true;

	// ��� ������Ʈ�� �˻��Ͽ� �浹�� �ݴ�� 1�� �ø���.
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
				return;
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
		mPlayer->mCanNextScene = true;
		mPlayer->_Pos.x -= 1;
		mPlayer->aboutMapPos.x -= 1;
	}
	else if (mPlayer->_Pos.x - 20 <= rectWindow.left)
	{
		sceneManager->StartLoading(sceneManager->GetHwnd());
		_nextFlow = Scene::Intro;
		mAdjValue = Vector2{ 0, 0 };
	}

	if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
		_exit = false;
		mPlayer->mCanNextScene = false;
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
	else if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
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
	else if (GetAsyncKeyState(VK_UP) & 0x8000) {
		Vector2 interval = { 0, -TPLAYER_SPEED * elapedTime };
		mPlayer->mCanNextScene = false;
		mPlayer->aboutMapPos += interval;
		mPlayer->_dir = Dir::Up;
		mPlayer->_Pos += interval;
	}
	else if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
		_exit = false;
		mPlayer->mCanNextScene = false;
		Vector2 interval = { 0, TPLAYER_SPEED * elapedTime };
		mPlayer->aboutMapPos += interval;
		mPlayer->_dir = Dir::Down;
		mPlayer->_Pos += interval;
	}
	else if (GetAsyncKeyState(VK_RETURN) & 0x0001 && _exit) {
		GET_SINGLE(Network)->SendDataAndType(EndProcessing{ GET_SINGLE(Network)->GetClientIndex() });
		PostQuitMessage(0);
	}
	else {
		InvalidateRect(sceneManager->GetHwnd(), NULL, false);
		return;
	}

	// ����Ű�� �������ٸ� ��Ŷ �۽�
	TownData::TownPlayerData playerData{ mPlayer->aboutMapPos, mPlayer->_rectDraw, mPlayer->_rectImage };
	TownData townData{ GET_SINGLE(Network)->GetClientIndex(), playerData, mPlayer->mCanNextScene, 0 };
	GET_SINGLE(Network)->SendDataAndType(townData);

	mActive = false;
}

void Town::StopPlayer()
{
	mPlayer->_rectImage.left = 0; // ������ �����ڼ�
	mPlayer->_keepGoing = false; // Ű ���� ������ ��� Ÿ�̸� ����
}

int Town::GetCamSizeX()
{
	return CAMSIZE_X;
}
int Town::GetCamSizeY()
{
	return CAMSIZE_Y;
}