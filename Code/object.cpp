#include "stdafx.h"
#include "object.h"
#include "interface.h"

extern GameData gameData;

GameObject::GameObject(ObjectImage& image, const Vector2& pos)
{
	this->image = &image;
	bodySize = this->image->GetBodySize();

	SetPos(pos);
}
void GameObject::Init(ObjectImage& image, const Vector2& pos)
{
	this->image = &image;
	bodySize = this->image->GetBodySize();

	SetPos(pos);
}

void GameObject::SetPos(const Vector2& pos)
{
	posCenter.x = pos.x;
	posCenter.y = pos.y;

	rectBody.left = posCenter.x - ((float)bodySize.x / 2);
	rectBody.top = posCenter.y - ((float)bodySize.y / 2);
	rectBody.right = rectBody.left + bodySize.x;
	rectBody.bottom = rectBody.top + bodySize.y;
}

void GameObject::Paint(const HDC& hdc, const RECT* rectImage)
{
	image->Paint(hdc, rectBody, rectImage);
	
}

FRECT GameObject::GetRectBody(const Vector2& pos) const
{
	FRECT rectBody = { 0, };
	rectBody.left = pos.x - ((float)bodySize.x / 2);
	rectBody.right = rectBody.left + bodySize.x;
	rectBody.top = pos.y - ((float)bodySize.y / 2);
	rectBody.bottom = rectBody.top + bodySize.y;
	return rectBody;
}

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