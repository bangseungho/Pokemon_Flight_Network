#define _CRT_SECURE_NO_WARNINGS 
#define _WINSOCK_DEPRECATED_NO_WARNINGS 

#define SERVERPORT		9000

#define TPLAYER_IMAGESIZE_X 64
#define TPLAYER_IMAGESIZE_Y 64

#define WINDOWSIZE_X 500
#define WINDOWSIZE_Y 750

#define RECT_WINDOW_WIDTH	484
#define RECT_WINDOW_HEIGHT	711

#define ELAPSE_BATTLE_INVALIDATE 10
#define ELAPSE_BATTLE_ANIMATION 50
#define ELAPSE_BATTLE_ANIMATION_BOSS 100
#define ELAPSE_BATTLE_MOVE_PLAYER 10
#define ELAPSE_BATTLE_EFFECT 50
#define ELAPSE_BATTLE_GUI 10

#define PI 3.141592
#define DEGREE_TO_RADIAN(degree) ((PI/180) * (degree))
#define RADIAN_TO_DEGREE(radian) ((180/PI) * (radian))

#include <winsock2.h> 
#include <ws2tcpip.h> 
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <algorithm>

#pragma comment(lib, "ws2_32") 
using namespace std;

using int8 = __int8;
using int16 = __int16;
using int32 = __int32;
using int64 = __int64;
using uint8 = unsigned __int8;
using uint16 = unsigned __int16;
using uint32 = unsigned __int32;
using uint64 = unsigned __int64;

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

enum class DataType : int
{
	NONE_DATA,
	TIMER_DATA,
	INTRO_DATA,
	TOWN_DATA,
	STAGE_DATA,
	PHASE_DATA,
	BATTLE_DATA,
	ENEMY_OBJECT_DATA,
	SCENE_DATA,
	END_PROCESSING,
};

enum class Difficulty { Easy = 0, Normal, Hard };
enum class Scene : uint8 { Intro = 0, Town, Stage, PhaseManager, Battle };
enum class Action { Idle = 0, Attack, Hurt, Death };

enum class Pokemon { Null = 0, Moltres, Articuno, Thunder };
enum class SubPokemon { Null = 0, Pikachu = 0, Squirtle, Charmander };
enum class Type { Empty = 0, Fire, Elec, Water, Dark };
enum class Skill { Empty = 0, Identity, Sector, Circle };

enum class StageElement { Water = 0, Fire, Elec, Dark, Town, Null };

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
	inline constexpr Vector2& operator+=(const Vector2& rhs)
	{
		x += rhs.x;
		y += rhs.y;
		return *this;
	}
	inline constexpr Vector2& operator-=(const Vector2& rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
		return *this;
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
	inline FRECT operator+(const FRECT& rhs)
	{
		FRECT res;
		res.left = left + rhs.left;
		res.top = top + rhs.top;
		res.right = right + rhs.right;
		res.bottom = bottom + rhs.bottom;
		return res;
	}
	inline FRECT& operator+=(const FRECT& rhs)
	{
		left = left + rhs.left;
		top = top + rhs.top;
		right = right + rhs.right;
		bottom = bottom + rhs.bottom;
		return *this;
	}
	inline FRECT operator-(const FRECT& rhs)
	{
		FRECT res;
		res.left = left - rhs.left;
		res.top = top - rhs.top;
		res.right = right - rhs.right;
		res.bottom = bottom - rhs.bottom;
		return res;
	}
	inline FRECT& operator-=(const FRECT& rhs)
	{
		left = left - rhs.left;
		top = top - rhs.top;
		right = right - rhs.right;
		bottom = bottom - rhs.bottom;
		return *this;
	}

}FRECT;

// 게임 오브젝트의 사이즈를 반환
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

struct ThreadSocket
{
	SOCKET	Sock;
	uint8	Id;
};

struct IntroData
{
	uint8	PlayerIndex = 0;
};

struct TownData
{
	struct TownPlayerData
	{
		Vector2	Pos;
		RECT	RectDraw;
		RECT	RectImage = { 0, 0, TPLAYER_IMAGESIZE_X, TPLAYER_IMAGESIZE_Y };
	};

	uint8			PlayerIndex = 0;
	TownPlayerData	PlayerData;
	bool			IsReady;
	int				InputKey;
	bool			CanGoNextScene = false;
};

struct StageData
{
	uint8	PlayerIndex = 0;
	int		Record;
	int		InputKey;
	FRECT	RectDraw = { (float)(RECT_WINDOW_WIDTH / 2 - 40), (float)(RECT_WINDOW_HEIGHT / 2 - 40), (float)(RECT_WINDOW_WIDTH / 2 + 40), (float)(RECT_WINDOW_HEIGHT / 2 + 40) };
	bool	IsReady = false;
	bool	CanGoNextScene = false;
	StageElement Stage = StageElement::Null;
};

struct PhaseData
{
	uint8	PlayerIndex = 0;
	int		InputKey;
};

struct BattleData
{
	uint8		PlayerIndex = 0;
	Vector2		PosCenter = { 0.f, };
	FRECT		RectBody = { 0.f, };
	bool		IsCollide = false;
	bool		IsFieldEnd = false;
};

struct NetworkEnemyData
{
	enum class AttackType : uint8 {
		NONE,
		MELEE,
		RANGE,
	};

	enum class Status : uint8 {
		NONE,
		CREATE,
		MOVE,
		ATTACK,
	};

	AttackType	AttackType = AttackType::NONE;
	Status		Status = Status::NONE;
	uint32		ID = 0;
	Vector2		Pos = { 0.f, 0.f };
	int			SpriteRow = 0;
	bool		IsCollide = false;
};

struct SceneData
{
	uint8	PlayerIndex = 0;
	uint8	MainPlayerIndex = 0;
	uint8	Scene = (uint8)Scene::Intro;
	int		Record = 0;
	Type	AirPokemon = Type::Empty;
	Type	LandPokemon = Type::Empty;
};

struct EndProcessing
{
	uint8	PlayerIndex = 0;
};

// 소켓 함수 오류 출력 후 종료
static void ErrorQuit(const char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(char *)&lpMsgBuf, 0, NULL);
	MessageBoxA(NULL, (const char *)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

// 소켓 함수 오류 출력
static void ErrorDisplay(const char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(char *)&lpMsgBuf, 0, NULL);
	printf("[%s] %s\n", msg, (char *)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

// 소켓 함수 오류 출력
static void ErrorDisplay(int errcode)
{
	LPVOID lpMsgBuf;
	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, errcode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(char *)&lpMsgBuf, 0, NULL);
	printf("[오류] %s\n", (char *)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

static bool ErrorCheck(int retVal, int type)
{
	// recv error check
	if (type == 0) {
		if (retVal == SOCKET_ERROR) {
			ErrorDisplay("recv()"); return false;
		}
		else if (false == retVal)
			return false;
	}
	// send error check
	else if (type == 1) {
		if (retVal == SOCKET_ERROR) {
			ErrorDisplay("send()"); return false;
		}
		else if (false == retVal)
			return false;
	}
	return true;
}

class Data {
public:
	// 패킷 타입에 따라 반환
	template<typename T>
	static inline DataType GetDataType()
	{
		if (std::is_same_v<T, IntroData>)
			return DataType::INTRO_DATA;
		else if (std::is_same_v<T, TownData>)
			return DataType::TOWN_DATA;
		else if (std::is_same_v<T, StageData>)
			return DataType::STAGE_DATA;
		else if (std::is_same_v<T, PhaseData>)
			return DataType::PHASE_DATA;
		else if (std::is_same_v<T, BattleData>)
			return DataType::BATTLE_DATA;
		else if (std::is_same_v<T, NetworkEnemyData>)
			return DataType::ENEMY_OBJECT_DATA;
		else if (std::is_same_v<T, SceneData>)
			return DataType::SCENE_DATA;
		else if (std::is_same_v<T, EndProcessing>)
			return DataType::END_PROCESSING;
		else
			return DataType::NONE_DATA;
	}

	template<typename T>
	static bool SendData(const SOCKET& clientSock, const T& data)
	{
		// 패킷 송신
		int retVal = send(clientSock, (char*)&data, sizeof(T), 0);
		if (false == ErrorCheck(retVal, 1))
			return false;

		return true;
	}

	template<typename T>
	static bool SendDataAndType(const SOCKET& clientSock, const T& data)
	{
		DataType dataType = GetDataType<T>();

		// 패킷 타입 송신
		int retVal = send(clientSock, (char*)&dataType, sizeof(dataType), 0);
		if (false == ErrorCheck(retVal, 1))
			return false;

		// 패킷 송신
		retVal = send(clientSock, (char*)&data, sizeof(T), 0);
		if (false == ErrorCheck(retVal, 1))
			return false;

		return true;
	}

	template<typename T>
	static bool RecvData(SOCKET& clientSock, T& data)
	{
		// 패킷 수신
		int retVal = recv(clientSock, (char*)&data, sizeof(T), 0);
		if (false == ErrorCheck(retVal, 0))
			return false;

		return true;
	}

	static DataType RecvType(SOCKET& clientSock)
	{
		int retVal;
		DataType dataType;
		ZeroMemory(&dataType, sizeof(dataType));

		retVal = recv(clientSock, (char*)&dataType, sizeof(dataType), 0);
		if (false == ErrorCheck(retVal, 0))
			return DataType::NONE_DATA;

		return dataType;
	}
};

class NetworkPlayerData
{
public:
	NetworkPlayerData() {}
	NetworkPlayerData(SOCKET& sock, uint8 threadId) { mSock = sock, mThreadId = threadId; }
	virtual ~NetworkPlayerData() {}

public:
	SOCKET		mSock;
	uint8		mThreadId;

	SceneData		mSceneData;
	IntroData		mIntroData;
	TownData		mTownData;
	StageData		mStageData;
	PhaseData		mPhaseData;
	BattleData		mBattleData;
	EndProcessing	mEndProcessing;
};
