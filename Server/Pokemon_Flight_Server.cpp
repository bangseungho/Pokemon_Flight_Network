﻿#include "..\Utils.h"
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <string>

#include "ServerUtils.h"

using namespace std;

#define MAX_BUFSIZE     1024

// 서버에서 가지고 있는 플레이어들의 전역 데이터
static unordered_map<uint8, PlayerData> sPlayers;
CRITICAL_SECTION cs;
uint8 sPlayerCount{};

DWORD WINAPI ProcessClient(LPVOID sock)
{
	// ThreadSocket = Socket + threadId 
	ThreadSocket* threadSocket = reinterpret_cast<ThreadSocket*>(sock);
	SOCKET clientSock = threadSocket->Sock;
	uint8 threadId = threadSocket->Id;

	// 플레이어 배열에 해당 클라이언트 추가
	sPlayers[threadId] = PlayerData{ clientSock, threadId };

	// 클라이언트 정보 가져오기
	SOCKADDR_IN clientaddr;
	int addrlen;
	addrlen = sizeof(clientaddr);
	getpeername(clientSock, (SOCKADDR*)&clientaddr, &addrlen);

	while (1) {
		// 총 데이터에서 앞 4바이트 정수값만 recv하여 데이터 타입을 받아온다.
		DataType dataType;
		dataType = RecvDataType(clientSock);
		if (dataType == DataType::NONE_DATA)
			break;

		// 클라이언트 종료했을 경우 모든 클라이언트에게 자신의 종료 신호를 송신한다.
		if (dataType == DataType::END_PROCESSING) {
			for (const auto& player : sPlayers) {
				SendData<EndProcessing>(player.second.mSock, EndProcessing{ threadId });
			}
			break;
		}

#pragma region Intro
		if (dataType == DataType::INTRO_DATA) {
			// 전역 플레이어 배열에서 클라이언트 threadId를 통해 자신의 플레이어에 접근
			auto& data = sPlayers[threadId].mIntroData;
			RecvData<IntroData>(clientSock, data);

			// 모든 클라이언트들에게 패킷 송신
			for (const auto& player : sPlayers) {
				// 플레이어가 자신이라면 패킷을 전송하지 않는다.
				if (player.second.mThreadId == threadId)
					continue;

				IntroData data{ static_cast<uint8>(sPlayers.size()), player.second.mThreadId };
				SendData<IntroData>(player.second.mSock, data);
			}
		}
#pragma endregion
#pragma region Town
		if (dataType == DataType::TOWN_DATA) {
			auto& data = sPlayers[threadId].mTownData;
			RecvData<TownData>(clientSock, data);
			data.PlayerIndex = static_cast<uint8>(threadId);

			for (const auto& player : sPlayers) {
				if (player.second.mThreadId == threadId)
					continue;

				SendData<TownData>(player.second.mSock, data);
			}

			cout << "ISREADY: " << data.IsReady << ", POSX: " << data.PlayerData.Pos.x << ", POSY: " << data.PlayerData.Pos.y << endl;
		}
#pragma endregion
#pragma region Stage
		if (dataType == DataType::STAGE_DATA) {
			auto& data = sPlayers[threadId].mStageData;
			RecvData<StageData>(clientSock, data);

			SendData<StageData>(clientSock, data);

			//for (const auto& player : sPlayers) {
			//	if (player.mThreadId == threadId)
			//		continue;

			//	SendData<StageData>(clientSock, player.mStageData);
			//}

			cout << "RECORD: " << data.Record << endl;
		}
#pragma endregion
#pragma region Phase
		if (dataType == DataType::PHASE_DATA) {
			auto& data = sPlayers[threadId].mPhaseData;
			RecvData<PhaseData>(clientSock, data);

			//for (const auto& player : sPlayers) {
			//	if (player.mThreadId == threadId)
			//		continue;

			//	SendData<PhaseData>(clientSock, player.mPhaseData);
			//}

			cout << "ISREADY: " << data.IsReady << endl;
		}
#pragma endregion
#pragma region Battle
		if (dataType == DataType::BATTLE_DATA) {
			auto& data = sPlayers[threadId].mBattleData;
			RecvData<BattleData>(clientSock, data);

			//for (const auto& player : sPlayers) {
			//	if (player.mThreadId == threadId)
			//		continue;

			//	SendData<BattleData>(clientSock, player.mBattleData);
			//}

			cout << "ISCOLLIDER: " << data.IsCollider << ", POSX: " << data.PosX << ", POSY: " << data.PosY << endl;
		}
#pragma endregion
	}

	sPlayers.erase(threadId);
	
	closesocket(clientSock);
	cout << "[클라이언트 종료] IP: " << inet_ntoa(clientaddr.sin_addr) << ", PORT: " << ntohs(clientaddr.sin_port) << endl << endl;
	return 0;
}

int main(int argc, char* argv[])
{
#pragma region Init
	int retval;
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return 1;
	InitializeCriticalSection(&cs);
#pragma endregion

#pragma region Socket
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) ErrorQuit("socket()");
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
	if (retval == SOCKET_ERROR) ErrorQuit("bind()");
#pragma endregion

#pragma region Listen
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) ErrorQuit("listen()");
#pragma endregion
#pragma region Accept
	ThreadSocket clientSock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	HANDLE hThread;

	while (1) {
		addrlen = sizeof(clientaddr);
		clientSock.Sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
		if (clientSock.Sock == INVALID_SOCKET) break;
		clientSock.Id = sPlayerCount++;

		char addr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));

		cout << "[클라이언트 접속] IP: " << inet_ntoa(clientaddr.sin_addr) << ", PORT: " << ntohs(clientaddr.sin_port) << endl << endl;

		hThread = CreateThread(NULL, 0, ProcessClient, &clientSock, 0, NULL);

		if (hThread == NULL) { closesocket(clientSock.Sock); }
		else { CloseHandle(hThread); }
	}
#pragma endregion
#pragma region Close
	DeleteCriticalSection(&cs);
	closesocket(listen_sock);
	WSACleanup();
#pragma endregion
}
