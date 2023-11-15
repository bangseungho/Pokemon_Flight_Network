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

DWORD WINAPI RecvData(LPVOID sock)
{
	ThreadSocket* threadSocket = reinterpret_cast<ThreadSocket*>(sock);
	SOCKET clientSock = threadSocket->Sock;

	SOCKADDR_IN clientaddr;
	int addrlen;

	addrlen = sizeof(clientaddr);
	getpeername(clientSock, (SOCKADDR*)&clientaddr, &addrlen);

	while (1) {
		DataType dataType;
		dataType = RecvDataType(clientSock);

#pragma region Intro
		if (dataType == DataType::INTRO_DATA) {
			IntroData data;
			RecvData<IntroData>(clientSock, data);
			cout << "ID: " << data.Id << ", PASSWORD: " << data.Password << endl;
		}
#pragma endregion
#pragma region Town
		if (dataType == DataType::TOWN_DATA) {
			TownData data;
			RecvData<TownData>(clientSock, data);
			cout << "ISREADY: " << data.IsReady << ", POSX: " << data.PosX << ", POSY: " << data.PosY << endl;
		}
#pragma endregion
#pragma region Stage
		if (dataType == DataType::STAGE_DATA) {
			StageData data;
			RecvData<StageData>(clientSock, data);
			cout << "RECORD: " << data.Record << endl;
		}
#pragma endregion
#pragma region Phase
		if (dataType == DataType::PHASE_DATA) {
			PhaseData data;
			RecvData<PhaseData>(clientSock, data);
			cout << "ISREADY: " << data.IsReady << endl;
		}
#pragma endregion
#pragma region Battle
		if (dataType == DataType::BATTLE_DATA) {
			BattleData data;
			RecvData<BattleData>(clientSock, data);
			cout << "ISCOLLIDER: " << data.IsCollider << ", POSX: " << data.PosX << ", POSY: " << data.PosY << endl;
		}
#pragma endregion
	}

	closesocket(clientSock);
	cout << "[클라이언트 종료] IP: " << inet_ntoa(clientaddr.sin_addr) << ", PORT: " << ntohs(clientaddr.sin_port) << endl;
	return 0;
}

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
		0000,
		0000,
	};
	
	TownData townData = {
		0.0f,
		0.0f,
		false
	};

	StageData stageData = {
		0
	};

	PhaseData phaseData = {
		false
	};

	BattleData battleData = {
		0.f,
		0.f,
		false
	};

	HANDLE hThread;
	hThread = CreateThread(NULL, 0, RecvData, &sock, 0, NULL);

	while (true) {
		int input;
		cin >> input;

		switch (input)
		{
		case static_cast<int>(DataType::INTRO_DATA):
		{
			SendData<IntroData>(sock, introData);
		}
		break;
		case static_cast<int>(DataType::TOWN_DATA):
		{
			SendData<TownData>(sock, townData);
		}
		break;
		case static_cast<int>(DataType::STAGE_DATA):
		{
			SendData<StageData>(sock, stageData);
		}
		break;
		case static_cast<int>(DataType::PHASE_DATA):
		{
			SendData<PhaseData>(sock, phaseData);
		}
		break;
		case static_cast<int>(DataType::BATTLE_DATA):
		{
			SendData<BattleData>(sock, battleData);
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
