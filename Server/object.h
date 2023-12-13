#pragma once
class GameObject abstract {
private:
	Vector2 posCenter;
	POINT bodySize = { 0, };
	FRECT rectBody = { 0, };

protected:
	void SetPos(const Vector2& pos);

public:
	inline constexpr Vector2 GetPosCenter() const
	{
		return posCenter;
	}
};
