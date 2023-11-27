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
		stageData.RectDraw.left -= 0.2f;
		stageData.RectDraw.right -= 0.2f;
		break;
	case VK_RIGHT:
		stageData.RectDraw.left += 0.2f;
		stageData.RectDraw.right += 0.2f;
		break;
	case VK_UP:
		stageData.RectDraw.top -= 0.2f;
		stageData.RectDraw.bottom -= 0.2f;
		break;
	case VK_DOWN:
		stageData.RectDraw.top += 0.2f;
		stageData.RectDraw.bottom += 0.2f;
		break;
	default:
		break;
	}
}
