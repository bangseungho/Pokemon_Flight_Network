#pragma once

struct IntroData
{
	int Id;
	int Password;
};

struct TownData
{
	float	PosX;
	float	PosY;
	bool	IsReady;
};

struct StageData
{
	int Record;
};

struct PhaseData
{
	bool IsReady;
};

struct BattleData
{
	float	PosX;
	float	PosY;
	bool	IsCollider;
};
