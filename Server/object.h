#pragma once
//#include "image.h"
//
class GameObject abstract {
private:
	//ObjectImage* image;

	Vector2 posCenter;
	POINT bodySize = { 0, };
	FRECT rectBody = { 0, };

protected:
	Dir direction = Dir::Empty;

	GameObject() {};
	GameObject(const Vector2& pos, const POINT& imgSize);
	void Init(const Vector2& pos, const POINT& imgSize);
	void SetPos(const Vector2& pos);

	//inline const ObjectImage& GetImage()
	//{
	//	return *image;
	//}

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

//// 직접 플레이어가 조종할 수 있는 추상화 클래스로 이 클래스를 상속받은 객체는 직접 조종할 수 있다.
//class IControllable abstract {
//private:
//	virtual void SetPosDest() abstract;
//	bool isMove = false;
//public:
//	virtual void SetMove(const HWND& hWnd, int timerID, int elpase, const TIMERPROC& timerProc) abstract;
//	virtual void Update(const HWND& hWnd, int timerID) abstract;
//	virtual void Stop(Dir dir) abstract;
//
//	inline void StartMove()
//	{
//		isMove = true;
//	}
//	inline void StopMove()
//	{
//		isMove = false;
//	}
//	inline bool IsMove() const
//	{
//		return isMove;
//	}
//};
//
//// 자동으로 움직이는 추상화 클래스로 이 클래스를 상속받은 객체는 AI에 따라서 조종된다.
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
