#pragma once

extern bool finishScene;

class Phase
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

// rectImage : rectSrc
// rectDraw : rectDest
class PhaseManager
{
private:
	Phase* _water;
	Phase* _fire;
	Phase* _elec;
	Phase* _dark;

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
	PhaseManager();
	void Init();
	void Move(float elapsedTime);
	void Paint(HDC hdc, const RECT& rectWindow, StageElement _select_index);

	void ClearPhase();
	int GetPhase();
};

