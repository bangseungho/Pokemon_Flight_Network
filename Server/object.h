#pragma once
#include "image.h"

class GameObject abstract {
private:
	ObjectImage* image;

	Vector2 posCenter;
	POINT bodySize = { 0, };
	FRECT rectBody = { 0, };

protected:
	Dir direction = Dir::Empty;

	GameObject() {};
	GameObject(const Vector2& pos, const POINT& imgSize);
	void Init(ObjectImage& image, const Vector2& pos);
	void Init(const Vector2& pos, const POINT& imgSize);
	void SetPos(const Vector2& pos);

public:
	FRECT GetRectBody(const Vector2& pos) const;

	bool IsCollide(const RECT& rectSrc, RECT* lprcDst = nullptr) const;

	inline constexpr FRECT GetRectBody() const
	{
		return rectBody;
	}
	inline constexpr Vector2 GetPosCenter() const
	{
		return posCenter;
	}
	inline float GetBodyWidth() const
	{
		return rectBody.right - rectBody.left;
	}
	inline float GetBodyHeight() const
	{
		return rectBody.bottom - rectBody.top;
	}
};

inline float GetRadius(float x, float y)
{
	return sqrtf((x * x) + (y * y));
}

// ���� �÷��̾ ������ �� �ִ� �߻�ȭ Ŭ������ �� Ŭ������ ��ӹ��� ��ü�� ���� ������ �� �ִ�.
class IControllable abstract {
private:
	bool isMove = false;
public:
	inline void StartMove()
	{
		isMove = true;
	}
	inline void StopMove()
	{
		isMove = false;
	}
	inline bool IsMove() const
	{
		return isMove;
	}
};

// �ڵ����� �����̴� �߻�ȭ Ŭ������ �� Ŭ������ ��ӹ��� ��ü�� AI�� ���� �����ȴ�.
class IMovable abstract {
private:
	virtual void SetPosDest() abstract;
	bool isMove = false;
public:
	virtual void Update() abstract;

	inline void StartMove()
	{
		isMove = true;
	}
	inline void StopMove()
	{
		isMove = false;
	}
	inline bool IsMove() const
	{
		return isMove;
	}
};
