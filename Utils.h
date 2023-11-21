#define _CRT_SECURE_NO_WARNINGS 
#define _WINSOCK_DEPRECATED_NO_WARNINGS 

#define SERVERPORT		9000

#define TPLAYER_IMAGESIZE_X 64
#define TPLAYER_IMAGESIZE_Y 64

#include <winsock2.h> 
#include <ws2tcpip.h> 
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <iostream>

#pragma comment(lib, "ws2_32") 

using int8 = __int8;
using int16 = __int16;
using int32 = __int32;
using int64 = __int64;
using uint8 = unsigned __int8;
using uint16 = unsigned __int16;
using uint32 = unsigned __int32;
using uint64 = unsigned __int64;

enum class DataType : int
{
	NONE_DATA,
	INTRO_DATA,
	TOWN_DATA,
	STAGE_DATA,
	PHASE_DATA,
	BATTLE_DATA,
	SCENE_DATA,
	END_PROCESSING,
};

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
		POINT	Pos;
		RECT	RectDraw;
		RECT	RectImage = { 0, 0, TPLAYER_IMAGESIZE_X, TPLAYER_IMAGESIZE_Y };
	};

	uint8			PlayerIndex = 0;
	TownPlayerData	PlayerData;
	bool			IsReady;
};

struct StageData
{
	uint8	PlayerIndex = 0;
	bool	SelectSuccess;
	int		Record;
};

struct PhaseData
{
	uint8	PlayerIndex = 0;
	bool	IsReady;
};

struct BattleData
{
	uint8	PlayerIndex = 0;
	float	PosX;
	float	PosY;
	bool	IsCollider;
};

struct SceneData
{
	uint8	PlayerIndex = 0;
	uint8	Scene = 0;
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

	IntroData		mIntroData;
	TownData		mTownData;
	StageData		mStageData;
	PhaseData		mPhaseData;
	BattleData		mBattleData;
	SceneData		mSceneData;
	EndProcessing	mEndProcessing;
};