#pragma once

extern bool finishScene;

class TotalStage
{
public:
	CImage _background;
	CImage _melee;
	CImage _range;
	CImage _boss;
	CImage _melee_mask;
	CImage _range_mask;
	CImage _boss_mask;
	RECT rectDraw_melee_mask = { 0, };
	RECT rectDraw_range_mask = { 0, };
	RECT rectDraw_boss_mask = { 0, };
};

class Water_stage : public TotalStage
{
public:
};

class Fire_stage : public TotalStage
{
public:
};

class Elec_stage : public TotalStage
{
public:
};

class Dark_stage : public TotalStage
{
public:
};

class Phase
{
private:
	Water_stage* _water;
	Fire_stage* _fire;
	Elec_stage* _elec;
	Dark_stage* _dark;

	RECT boss_rectImage = { 0, };
	RECT rectImage = { 0, };
	RECT rectDraw_range = { 0, };
	RECT rectDraw_melee = { 0, };
	RECT rectDraw_boss = { 0, };

	int phaseWater = 0;
	int phaseFire = 0;
	int phaseElec = 0;
	int phaseDark = 0;

public:
	Phase();
	void Init();
	void Paint(HDC hdc, const RECT& rectWindow, StageElement _select_index);
	void fingerController(const HWND& hWnd);

	void ClearPhase();
	int GetPhase();
};

