#pragma once

#define TOWN_OBJECT_NUM 18
#define NPC_COUNT 4

class TownObject : public MyGameObject
{
};

class NpcObject : public MyGameObject
{
};

class Town
{
private:
	std::array<std::shared_ptr<TownObject>, TOWN_OBJECT_NUM> mObjects;
	std::array<std::shared_ptr<NpcObject>, NPC_COUNT> mNpcs;

	RECT _object[TOWN_OBJECT_NUM];
	CImage _backTown;
	RECT _rectDraw = { 0, };
	CImage _npc; // fishing npc
	RECT _npcRect;
	CImage _npc2; // warking npc
	CImage _npc3; // npc3
	RECT _npc3Rect;
	CImage _npc4; // npc4
	RECT _npc4Rect;

	Vector2 mAdjValue; // �÷��̾��� ī�޶� �� ���� �������� ��� �ٸ� ������Ʈ���� �ݴ�� ����ϱ� ���� ��

	HWND mHwnd;
	
	CImage _exits;
	CImage mGlowingBlack;
	bool _exit = false;

public:
	class Player
	{
	public:
		Vector2 aboutMapPos;
		CImage img;
		Vector2 _Pos = { 0, };
		RECT _rectDraw = { 0, };
		RECT _rectImage = { 0, };
		FRECT _cam = { 0, };
		Dir _dir = Dir::Down;
		bool mIsReady = false;
		bool _keepGoing = false;
	};

	bool _allHide = false;
	bool mActive = true;

	Scene _nextFlow = Scene::Town;
	Player* mPlayer;
	FRECT _rectImage = { 0, };
	RECT _npc2Rect;
	POINT _npc1Move = { 0, };
	POINT _npc2Move = { 0, };

	Town();
	void Init();
	void Paint(HDC hdc, const RECT& rectWindow);
	void Animate(float elpasedTime);
	void AnimateNPC(float elpasedTime);
	void Update(float elapedTime);
	void StopPlayer();
	int GetCamSizeX();
	int GetCamSizeY();
};
