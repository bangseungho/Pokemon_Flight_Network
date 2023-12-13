#include "..\Utils.h"
#include "object.h"

// 오브젝트의 위치를 설정하는 함수
void GameObject::SetPos(const Vector2& pos)
{
	posCenter.x = pos.x;
	posCenter.y = pos.y;

	rectBody.left = posCenter.x - ((float)bodySize.x / 2);
	rectBody.top = posCenter.y - ((float)bodySize.y / 2);
	rectBody.right = rectBody.left + bodySize.x;
	rectBody.bottom = rectBody.top + bodySize.y;
}
