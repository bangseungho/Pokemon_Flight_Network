#include "..\Utils.h"
#include "Physics.h"
#include "Timer.h"
#include <iomanip>

void Physics::MoveTownPlayer(TownData& townData, float elapsedTime)
{
	switch (townData.InputKey)
	{
	case VK_LEFT:
		townData.PlayerData.Pos.x -= 10.f * elapsedTime;
		break;
	case VK_RIGHT:
		townData.PlayerData.Pos.x += 10.f * elapsedTime;
		break;
	case VK_UP:
		townData.PlayerData.Pos.y -= 10.f * elapsedTime;
		break;
	case VK_DOWN:
		townData.PlayerData.Pos.y += 10.f * elapsedTime;
		break;
	default:
		break;
	}
}

void Physics::MoveStagePlayer(StageData& stageData, float elapsedTime)
{
	switch (stageData.InputKey)
	{
	case VK_LEFT:
		stageData.RectDraw.left -= 10.f * elapsedTime;
		stageData.RectDraw.right -= 10.f * elapsedTime;
		break;
	case VK_RIGHT:
		stageData.RectDraw.left += 10.f * elapsedTime;
		stageData.RectDraw.right += 10.f * elapsedTime;
		break;
	case VK_UP:
		stageData.RectDraw.top -= 10.f * elapsedTime;
		stageData.RectDraw.bottom -= 10.f * elapsedTime;
		break;
	case VK_DOWN:
		stageData.RectDraw.top += 10.f * elapsedTime;
		stageData.RectDraw.bottom += 10.f * elapsedTime;
		break;
	default:
		break;
	}
}
