#include "..\Utils.h"
#include "object.h"

// 생성자에서 오브젝트 이미지와 포지션을 받는다.
GameObject::GameObject(const Vector2& pos, const POINT& imgSize)
{
	bodySize = imgSize;

	SetPos(pos);
}
void GameObject::Init(ObjectImage& image, const Vector2& pos)
{
	this->image = &image;
	bodySize = this->image->GetBodySize();

	SetPos(pos);
}
void GameObject::Init(const Vector2& pos, const POINT& imgSize)
{
	bodySize = imgSize;

	SetPos(pos);
}

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

// 게임 오브젝트의 사이즈를 반환
FRECT GameObject::GetRectBody(const Vector2& pos) const
{
	FRECT rectBody = { 0, };
	rectBody.left = pos.x - ((float)bodySize.x / 2);
	rectBody.right = rectBody.left + bodySize.x;
	rectBody.top = pos.y - ((float)bodySize.y / 2);
	rectBody.bottom = rectBody.top + bodySize.y;
	return rectBody;
}

// 충돌 처리
bool GameObject::IsCollide(const RECT& rectSrc, RECT* lprcDst) const
{
	const RECT rect = rectBody;
	if (lprcDst == nullptr)
	{
		return IntersectRect2(rect, rectSrc);
	}
	else
	{
		return IntersectRect(lprcDst, &rect, &rectSrc);
	}
}