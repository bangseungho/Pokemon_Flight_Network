#pragma once

struct IntroData
{
	DataType Type;
	int Id;
	int Password;
};

struct TownData
{
	DataType Type;
	float	PosX;
	float	PosY;
	bool	IsReady;
};

struct StageData
{
	DataType Type;
	int Record;
};

struct PhaseData
{
	DataType Type;
	bool IsReady;
};

struct BattleData
{
	DataType Type;
	float	PosX;
	float	PosY;
	bool	IsCollider;
};