#pragma once

#define TOWN_OBJECT_NUM 18

class Town
{
private:
	class Player
	{
	public:
		POINT aboutMapPos;
		CImage img;
		POINT _Pos = { 0, };
		RECT _rectDraw = { 0, };
		RECT _rectImage = { 0, };
		RECT _cam = { 0, };
		Dir _dir = Dir::Down;
		bool _keepGoing = false;
	};

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
	
	CImage _exits;
	CImage _glowing_black;
	bool _exit = false;

public:
	bool _allHide = false;
	Scene _nextFlow = Scene::Town;
	Player* player;
	RECT _rectImage = { 0, };

	RECT _npc2Rect;
	POINT _npc1Move = { 0, };
	POINT _npc2Move = { 0, };

	Town();
	void Init(const RECT& rectWindow);
	void Paint(HDC hdc, const RECT& rectWindow);
	void Move(const RECT& rectWindow);
	void StopPlayer();
	int GetCamSizeX();
	int GetCamSizeY();
};
