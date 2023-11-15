#define _CRT_SECURE_NO_WARNINGS 
#define _WINSOCK_DEPRECATED_NO_WARNINGS 

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
};

struct ThreadSocket
{
	SOCKET	Sock;
	uint8	Id;
};

struct IntroData
{
	int Id;
	int Password;
};

struct TownData
{
	float	PosX;
	float	PosY;
	bool	IsReady;
};

struct StageData
{
	int Record;
};

struct PhaseData
{
	bool IsReady;
};

struct BattleData
{
	float	PosX;
	float	PosY;
	bool	IsCollider;
};

// 소켓 함수 오류 출력 후 종료
void err_quit(const char *msg)
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
void err_display(const char *msg)
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
void err_display(int errcode)
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

bool ErrorCheck(int retVal, int type)
{
	// recv error check
	if (type == 0) {
		if (retVal == SOCKET_ERROR) {
			err_display("recv()"); return false;
		}
		else if (false == retVal)
			return false;
	}
	// send error check
	else if (type == 1) {
		if (retVal == SOCKET_ERROR) {
			err_display("send()"); return false;
		}
		else if (false == retVal)
			return false;
	}
	return true;
}

DataType RecvDataType(SOCKET& clientSock) 
{
	int retVal;
	DataType dataType;
	ZeroMemory(&dataType, sizeof(dataType));

	retVal = recv(clientSock, (char*)&dataType, sizeof(dataType), 0);
	if (false == ErrorCheck(retVal, 0))
		return DataType::NONE_DATA;

	return dataType;
}

// 패킷 타입에 따라 반환
template<typename T>
inline DataType GetDataType()
{
	if (std::is_same_v<T, IntroData>)
		return DataType::INTRO_DATA;
	else if (std::is_same_v<T, TownData>)
		return DataType::TOWN_DATA;
	else if (std::is_same_v<T, StageData>)
		return DataType::STAGE_DATA;
	else if (std::is_same_v<T, PhaseData>)
		return DataType::PHASE_DATA;
	else if (std::is_convertible_v<T, BattleData>)
		return DataType::BATTLE_DATA;
	else
		return DataType::NONE_DATA;
}

template<typename T>
bool SendData(SOCKET& clientSock, const T& data) 
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
bool RecvData(SOCKET& clientSock, T& data)
{
	// 패킷 수신
	int retVal = recv(clientSock, (char*)&data, sizeof(T), 0);
	if (false == ErrorCheck(retVal, 0))
		return false;

	return true;
}