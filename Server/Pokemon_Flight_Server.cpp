#include "..\Utils.h"
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "ServerUtils.h"

using namespace std;

#define SERVERPORT		9000
#define MAX_BUFSIZE     1024

CRITICAL_SECTION gPlayerCS;
static vector<PlayerData> sPlayers;

DWORD WINAPI ProcessClient(LPVOID sock)
{
	ThreadSocket* threadSocket = reinterpret_cast<ThreadSocket*>(sock);
	SOCKET clientSock = threadSocket->Sock;
	uint8 threadId = threadSocket->Id;

	sPlayers.emplace_back(clientSock, threadId);

	SOCKADDR_IN clientaddr;
	int addrlen;

	addrlen = sizeof(clientaddr);
	getpeername(clientSock, (SOCKADDR*)&clientaddr, &addrlen);

	while (1) {
		DataType dataType;
		dataType = RecvDataType(clientSock);
		if (dataType == DataType::NONE_DATA)
			break;

#pragma region Intro
		if (dataType == DataType::INTRO_DATA) {
			auto& data = sPlayers[threadId].mIntroData;
			RecvData<IntroData>(clientSock, data);

			for (const auto& player : sPlayers) {
				if (player.mThreadId == threadId)
					continue;
				
				SendData<IntroData>(clientSock, player.mIntroData);
			}

			cout << "ID: " << data.Id << ", PASSWORD: " << data.Password << endl;
		}
#pragma endregion
#pragma region Town
		if (dataType == DataType::TOWN_DATA) {
			auto& data = sPlayers[threadId].mTownData;
			RecvData<TownData>(clientSock, data);

			for (const auto& player : sPlayers) {
				if (player.mThreadId == threadId)
					continue;

				SendData<TownData>(clientSock, player.mTownData);
			}

			cout << "ISREADY: " << data.IsReady << ", POSX: " << data.PosX << ", POSY: " << data.PosY << endl;
		}
#pragma endregion
#pragma region Stage
		if (dataType == DataType::STAGE_DATA) {
			auto& data = sPlayers[threadId].mStageData;
			RecvData<StageData>(clientSock, data);

			for (const auto& player : sPlayers) {
				if (player.mThreadId == threadId)
					continue;

				SendData<StageData>(clientSock, player.mStageData);
			}

			cout << "RECORD: " << data.Record << endl;
		}
#pragma endregion
#pragma region Phase
		if (dataType == DataType::PHASE_DATA) {
			auto& data = sPlayers[threadId].mPhaseData;
			RecvData<PhaseData>(clientSock, data);

			for (const auto& player : sPlayers) {
				if (player.mThreadId == threadId)
					continue;

				SendData<PhaseData>(clientSock, player.mPhaseData);
			}

			cout << "ISREADY: " << data.IsReady << endl;
		}
#pragma endregion
#pragma region Battle
		if (dataType == DataType::BATTLE_DATA) {
			auto& data = sPlayers[threadId].mBattleData;
			RecvData<BattleData>(clientSock, data);

			for (const auto& player : sPlayers) {
				if (player.mThreadId == threadId)
					continue;

				SendData<BattleData>(clientSock, player.mBattleData);
			}

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
#pragma region Init
	int retval;
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return 1;
#pragma endregion

#pragma region Socket
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) err_quit("socket()");
#pragma endregion

#pragma region SockAddr
	SOCKADDR_IN serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
#pragma endregion

#pragma region Bind
	retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");
#pragma endregion

#pragma region Listen
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit("listen()");
#pragma endregion
#pragma region Accept
	ThreadSocket clientSock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	HANDLE hThread;

	InitializeCriticalSection(&gPlayerCS);

	int id{};
	while (1) {
		addrlen = sizeof(clientaddr);
		clientSock.Sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
		if (clientSock.Sock == INVALID_SOCKET) break;
		clientSock.Id = id++;

		char addr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));

		cout << "[클라이언트 접속] IP: " << inet_ntoa(clientaddr.sin_addr) << ", PORT: " << ntohs(clientaddr.sin_port) << endl;

		hThread = CreateThread(NULL, 0, ProcessClient, &clientSock, 0, NULL);

		if (hThread == NULL) { closesocket(clientSock.Sock); }
		else { CloseHandle(hThread); }
	}
#pragma endregion
#pragma region Close
	DeleteCriticalSection(&gPlayerCS);
	closesocket(listen_sock);
	WSACleanup();
#pragma endregion
}
