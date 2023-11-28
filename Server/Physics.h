#pragma once

class Physics
{
public:
	static void MoveTownPlayer(TownData& townData, float elapsedTime);
	static void MoveStagePlayer(StageData& townData, float elapsedTime);
};