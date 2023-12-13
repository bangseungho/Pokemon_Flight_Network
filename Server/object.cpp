#include "..\Utils.h"
#include "object.h"

// ������Ʈ�� ��ġ�� �����ϴ� �Լ�
void GameObject::SetPos(const Vector2& pos)
{
	posCenter.x = pos.x;
	posCenter.y = pos.y;

	rectBody.left = posCenter.x - ((float)bodySize.x / 2);
	rectBody.top = posCenter.y - ((float)bodySize.y / 2);
	rectBody.right = rectBody.left + bodySize.x;
	rectBody.bottom = rectBody.top + bodySize.y;
}
