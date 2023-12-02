#pragma once
#pragma warning(disable:28251)
#pragma warning(disable:4244)

#ifdef _DEBUG
#ifdef UNICODE                                                                                      
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console") 
#else                                                                                                    
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")   
#endif                                                                                                   
#endif  

#include "..\Utils.h"
#include <Windows.h>
#include <time.h>
#include <gdiplus.h>
#include <atlImage.h>
#include <mmsystem.h>
#include <fmod.hpp>
#include <cmath>
#include <vector>
#include <cassert>
#include <algorithm>
#include <unordered_map>

#pragma region Window Define
#define MSEC						1000
#pragma endregion
#pragma region Intro Define
#define INSTRUCTION_BACKGROUND_X	500
#define INSTRUCTION_BACKGROUND_Y	750
#define LOGO_COLOR					RGB(255, 212, 0)
#define LOGO_SHADOW_COLOR			RGB(200, 100, 0)
#define LOGO_SIZE					106
#define MENU_SIZE					31
#define IALPHA						230
#define CLOUD_NUM					4
#define FIRSTCLOUD_X				125
#define FIRSTCLOUD_Y				187
#define SECONDCLOUD_X				187
#define SECONDCLOUD_Y				31
#define THIRDCLOUD_X				-62
#define THIRDCLOUD_Y				93
#define FOURTHCLOUD_X				437
#define FOURTHCLOUD_Y				187
#pragma endregion
#pragma region Town Define
#define TPLAYER_SPEED				4
#pragma endregion
#pragma region Loading Define
#define LOADING_BAR_X				182
#define LOADING_BAR_Y				397
#define TIMERID_LODING				5
#define HALF_RECTWINDOW_X			175 
#define HALF_RECTWINDOW_Y			280
#define LOADING_POKEMON_X			150
#define LOADING_POKEMON_Y			121
#define LOADING_POKEMON_MAX_X		2100
#define LOADING_POKEMON_MAX_Y		121
#pragma endregion
#pragma region Stage Define
#define TARGET_IMAGESIZE_X			512
#define TARGET_IMAGESIZE_Y			512
#define CAMSIZE_X					162 
#define CAMSIZE_Y					375
#define STAGE_NUM					5
#define MAPSCROLL_SPEED				4
#define SELECTPOKEMONSIZE			135
#pragma endregion
#pragma region Scene Define
#define SALPHA						220
#pragma endregion
#pragma region Timer Define
#define TPLAYER_IMAGESIZE_X			64
#define TPLAYER_MAX_IMAGESIZE_X		256
#define DIR_LEFT_IMAGE_Y			64
#define DIR_RIGHT_IMAGE_Y			128
#define DIR_UP_IMAGE_Y				192
#define DIR_DOWN_IMAGE_Y			0
#pragma endregion
#pragma region Interface Define
#define VK_R 0x52
#define Endl endl << endl
#define PI 3.141592
#define DEGREE_TO_RADIAN(degree) ((PI/180) * (degree))
#define RADIAN_TO_DEGREE(radian) ((180/PI) * (radian))
#pragma endregion
#pragma region Battle Define
#define ALPHA 150
#pragma endregion

#define	WIN32_LEAN_AND_MEAN
#undef WINVER
#define WINVER 0x6000

enum class Difficulty { Easy = 0, Normal, Hard };
enum class Scene : uint8 { Intro = 0, Town, Stage, PhaseManager, Battle };
enum class Action { Idle = 0, Attack, Hurt, Death };

enum class Pokemon { Null = 0, Moltres, Articuno, Thunder };
enum class SubPokemon { Null = 0, Pikachu = 0, Squirtle, Charmander };
enum class Type { Empty = 0, Fire, Elec, Water, Dark };
enum class Skill { Empty = 0, Identity, Sector, Circle };

enum class StageElement { Water = 0, Fire, Elec, Dark, Town, Null };

// 싱글턴 객체로 클래스 내부에서 사용
#define SINGLETON(type)								\
public:												\
	static type* GetInstance()						\
	{												\
		if (!instance) instance = new type();		\
		return instance;							\
	}												\
	static void Destroy()							\
	{												\
		if (instance) delete instance;				\
		instance = nullptr;							\
	}												\
													\
private:											\
	static type* instance;							\

#define GET_SINGLE(type)			type::GetInstance()
#define DESTROY_SINGLE(type)		type::Destroy()
#define DECLARE_SINGLE(type)		type* type::instance = nullptr;

enum class Dir { Empty = 0, Left, Right, Up, Down, LD, LU, RD, RU }; // 총 방향은 8개이다.

// Direction 연산자 오버로딩
// ex) Left - LD = LD - Left = Down
//     Left + Up = LU
constexpr Dir operator-(Dir lhs, Dir rhs)
{
	if (lhs == rhs)
	{
		return Dir::Empty;
	}
	else if (lhs < rhs)
	{
		Dir temp = lhs;
		lhs = rhs;
		rhs = temp;
	}

	Dir result = Dir::Empty;
	switch (lhs)
	{
	case Dir::LD:
		if (rhs == Dir::Left)
		{
			result = Dir::Down;
		}
		else if (rhs == Dir::Down)
		{
			result = Dir::Left;
		}
		break;
	case Dir::LU:
		if (rhs == Dir::Left)
		{
			result = Dir::Up;
		}
		else if (rhs == Dir::Up)
		{
			result = Dir::Left;
		}
		break;
	case Dir::RD:
		if (rhs == Dir::Right)
		{
			result = Dir::Down;
		}
		else if (rhs == Dir::Down)
		{
			result = Dir::Right;
		}
		break;
	case Dir::RU:
		if (rhs == Dir::Right)
		{
			result = Dir::Up;
		}
		else if (rhs == Dir::Up)
		{
			result = Dir::Right;
		}
		break;
	default:
		break;
	}

	return result;
}
constexpr Dir operator+(Dir lhs, Dir rhs)
{
	if (lhs == rhs)
	{
		return lhs;
	}
	else if (lhs < rhs)
	{
		Dir temp = lhs;
		lhs = rhs;
		rhs = temp;
	}

	switch (rhs)
	{
	case Dir::Left:
	{
		switch (lhs)
		{
		case Dir::Up:	return Dir::LU;
		case Dir::Down:	return Dir::LD;
		}
	}
	case Dir::Right:
	{
		switch (lhs)
		{
		case Dir::Up:	return Dir::RU;
		case Dir::Down:	return Dir::RD;
		}
	}
	case Dir::Up:
	{
		switch (lhs)
		{
		case Dir::Left:		return Dir::LU;
		case Dir::Right:	return Dir::RU;
		}
	}
	case Dir::Down:
	{
		switch (lhs)
		{
		case Dir::Left:		return Dir::LD;
		case Dir::Right:	return Dir::RD;
		}
	}
	}

	return Dir::Empty;
}

// POINT 연산자 오버로딩
// ex) POINT a,b;
// possible : a - b, a + b;
inline constexpr POINT operator-(const POINT& lhs, const POINT& rhs)
{
	return { lhs.x - rhs.x, lhs.y - rhs.y };
}
inline constexpr POINT operator+(const POINT& lhs, const POINT& rhs)
{
	return { lhs.x + rhs.x, lhs.y + rhs.y };
}

struct Vector2 {
	float x = 0;
	float y = 0;

	inline constexpr Vector2 operator+(const Vector2& rhs) const
	{
		return { x + rhs.x, y + rhs.y };
	}
	inline constexpr Vector2 operator-(const Vector2& rhs) const
	{
		return { x - rhs.x, y - rhs.y };
	}
	inline constexpr Vector2 operator*(const float& rhs) const
	{
		return { x * rhs, y * rhs };
	}
	inline constexpr Vector2 operator/(const float& rhs) const
	{
		return { x / rhs, y / rhs };
	}
	inline constexpr operator POINT () const
	{
		return { (LONG)x, (LONG)y };
	}
	inline constexpr Vector2 operator=(const POINT& rhs)
	{
		return { static_cast<float>(rhs.x), static_cast<float>(rhs.y) };
	}
	inline constexpr bool operator==(const Vector2& rhs) const
	{
		return (x == rhs.x && y == rhs.y) ? true : false;
	}
	inline constexpr bool operator!=(const Vector2& rhs) const
	{
		return (x != rhs.x || y != rhs.y) ? true : false;
	}

	inline Vector2 Normalized() const
	{
		return *this / GetNorm(*this);
	}
	inline float Norm() const
	{
		return sqrtf(x * x + y * y);
	}
	static inline float GetNorm(Vector2 v)
	{
		return sqrtf(v.x * v.x + v.y * v.y);
	}
	static inline constexpr float Dot(const Vector2& lhs, const Vector2& rhs)
	{
		return (lhs.x * rhs.x) + (lhs.y * rhs.y);
	}
	static inline constexpr float GetZAxis(const Vector2& lhs, const Vector2& rhs)
	{
		return (lhs.x * rhs.y) - (rhs.x - lhs.y);
	}
	static inline float GetTheta(const Vector2& lhs, const Vector2& rhs)
	{
		float dot = Vector2::Dot(lhs, rhs);
		return acos(dot / (Vector2::GetNorm(lhs) * Vector2::GetNorm(rhs)));
	}

	static inline constexpr Vector2 Up()
	{
		return { 0, -1 };
	}
	static inline constexpr Vector2 Down()
	{
		return { 0, 1 };
	}
	static inline constexpr Vector2 Left()
	{
		return { -1, 0 };
	}
	static inline constexpr Vector2 Right()
	{
		return { 1, 0 };
	}
	static inline constexpr Vector2 Zero()
	{
		return { 0, 0 };
	}
	static inline constexpr Vector2 GetDest(Vector2 posCenter, Vector2 vector, float speed = 1)
	{
		return posCenter + (vector * speed);
	}
	static Vector2 Lerp(const Vector2& src, const Vector2& dst, float alpha)
	{
		Vector2 transform;
		transform.x = (src.x * (1 - alpha)) + (dst.x * alpha);
		transform.y = (src.y * (1 - alpha)) + (dst.y * alpha);
		return transform;
	}
	// Linear transform to find the orthogonal vector of the edge
	static Vector2 Get_Normalized_Proj_Axis(const Vector2& crntPoint, const Vector2& nextPoint)
	{
		const float axisX = -(nextPoint.y - crntPoint.y);
		const float axisY = nextPoint.x - crntPoint.x;
		const float magnitude = hypot(axisX, axisY);

		Vector2 axisNormalized = { 0, };
		axisNormalized.x = axisX / magnitude;
		axisNormalized.y = axisY / magnitude;

		return axisNormalized;
	}
};

typedef struct FRECT {
	float left = 0;
	float top = 0;
	float right = 0;
	float bottom = 0;

	inline constexpr operator RECT () const
	{
		return { (LONG)left, (LONG)top, (LONG)right, (LONG)bottom };
	}
	inline constexpr FRECT operator=(const RECT& rhs)
	{
		return { (float)rhs.left, (float)rhs.right, (float)rhs.top, (float)rhs.bottom };
	}
}FRECT;

Vector2 Rotate(Vector2 vector, float degree);
void Rotate(const Vector2& vSrc, const Vector2& vDst, Vector2& vector, float t);
bool OutOfRange(const RECT& rect, const RECT& rectRange);
void GetRotationPos(const RECT& rect, const Vector2& unitVector, Vector2 basisVector, Vector2 vPoints[4]);
RECT GetRotatedBody(const Vector2 vPoints[4]);
void PaintHitbox(HDC hdc, RECT rectBody);
void SetRectByWindow(RECT& rect);
void CheckOverflowAdd(BYTE& lhs, const BYTE& rhs);
void CheckOverflowSub(BYTE& lhs, const BYTE& rhs);
bool SATIntersect(const FRECT& rectSrc, const Vector2 vSrc[4]);
void ScaleRect(FRECT& rect, float scaleX, float scaleY);
FRECT GetRect(const Vector2& posCenter, float radius);
bool IntersectRect2(const RECT& rect1, const RECT& rect2);
float CalculateDamage(float damage, Type destType, Type srcType);

template <typename T>
inline constexpr int GetSign(T num)
{
	return num / abs(num);
}