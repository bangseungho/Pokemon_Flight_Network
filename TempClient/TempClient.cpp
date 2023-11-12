#include "..\Utils.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <string>

#include "ClientUtils.h"

char* SERVERIP = (char*)"127.0.0.1";
#define SERVERPORT 9000
#define BUFSIZE    1024

using namespace std;

int main(int argc, char* argv[])
{
	int retVal;
#pragma region Init
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return 1;
#pragma endregion
#pragma region Socket
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");
#pragma endregion
#pragma region SockAddr
	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	inet_pton(AF_INET, SERVERIP, &serveraddr.sin_addr);
	serveraddr.sin_port = htons(SERVERPORT);
#pragma endregion
#pragma region Connect
	retVal = connect(sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if (retVal == SOCKET_ERROR) err_quit("connect()");
#pragma endregion
#pragma region SendRecvData
	IntroData introData = {
		DataType::INTRO_DATA,
		1111,
		1234,
	};
	
	TownData townData = {
		DataType::TOWN_DATA,
		1.5f,
		3.5f,
		true
	};

	StageData stageData = {
		DataType::STAGE_DATA,
		3
	};

	PhaseData phaseData = {
		DataType::PHASE_DATA,
		true
	};

	BattleData battleData = {
		DataType::BATTLE_DATA,
		100.f,
		200.f,
		false
	};

	while (true) {
		int input;
		cin >> input;

		switch (input)
		{
		case static_cast<int>(DataType::INTRO_DATA):
		{
			int len = sizeof(introData);
			retVal = send(sock, (char*)&introData, len, 0);
			if (ErrorCheck(retVal, 1))
				break;
		}
		break;
		case static_cast<int>(DataType::TOWN_DATA):
		{
			int len = sizeof(townData);
			retVal = send(sock, (char*)&townData, len, 0);
			if (ErrorCheck(retVal, 1))
				break;
		}
		break;
		case static_cast<int>(DataType::STAGE_DATA):
		{
			int len = sizeof(stageData);
			retVal = send(sock, (char*)&stageData, len, 0);
			if (ErrorCheck(retVal, 1))
				break;
		}
		break;
		case static_cast<int>(DataType::PHASE_DATA):
		{
			int len = sizeof(phaseData);
			retVal = send(sock, (char*)&phaseData, len, 0);
			if (ErrorCheck(retVal, 1))
				break;
		}
		break;
		case static_cast<int>(DataType::BATTLE_DATA):
		{
			int len = sizeof(battleData);
			retVal = send(sock, (char*)&battleData, len, 0);
			if (ErrorCheck(retVal, 1))
				break;
		}
		break;
		}
	}


		

#pragma endregion
#pragma region Close
	closesocket(sock);
	WSACleanup();
#pragma endregion
}
