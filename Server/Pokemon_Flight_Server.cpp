#include "..\Utils.h"
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <algorithm>

#include "Timer.h"
#include "Battle.h"

#define MAX_BUFSIZE     1024

// 서버에서 가지고 있는 플레이어들의 전역 데이터
unordered_map<uint8, NetworkPlayerData> sPlayerMap;
std::mutex sPlayersMutex;
HANDLE hAllPlayerBattleSceneEvent;

void ProcessClient(ThreadSocket sock)
{
	// ThreadSocket = Socket + threadId 
	ThreadSocket threadSocket = sock;
	SOCKET clientSock = threadSocket.Sock;
	uint8 threadId = threadSocket.Id;

	// 플레이어 배열에 해당 클라이언트 추가
	sPlayerMap[threadId] = NetworkPlayerData{ clientSock, threadId };

	// 클라이언트와 연결이 되었다면 클라이언트에게 자신의 인덱스를 송신
	if (!Data::SendData(clientSock, threadId))
		return;

	int mainPlayerIndex = 0;

	// 클라이언트 정보 가져오기
	SOCKADDR_IN clientaddr;
	int addrlen;
	addrlen = sizeof(clientaddr);
	getpeername(clientSock, (SOCKADDR*)&clientaddr, &addrlen);

	while (1) {
		// 총 데이터에서 앞 4바이트 정수값만 recv하여 데이터 타입을 받아온다.
		DataType dataType;
		dataType = Data::RecvType(clientSock);
		if (dataType == DataType::NONE_DATA)
			continue;

		// 클라이언트 종료했을 경우 모든 클라이언트에게 자신의 종료 신호를 송신한다.
		if (dataType == DataType::END_PROCESSING) {
			for (const auto& player : sPlayerMap) {
				Data::SendDataAndType<EndProcessing>(player.second.mSock, EndProcessing{ threadId });
			}
			break;
		}
#pragma region SceneData
		else if (dataType == DataType::SCENE_DATA) {
			// 전역 플레이어 배열에서 클라이언트 threadId를 통해 자신의 플레이어에 접근
			auto& data = sPlayerMap[threadId].mSceneData;
			std::lock_guard<std::mutex> lock(sPlayersMutex);
			Data::RecvData<SceneData>(clientSock, data);
			data.PlayerIndex = static_cast<uint8>(threadId);

			// 모든 클라이언트들에게 자신의 정보를 담은 패킷 송신
			uint8 minRecord = 99;
			for (auto& player : sPlayerMap) {
				if (minRecord > player.second.mSceneData.Record) {
					minRecord = player.second.mSceneData.Record;
					mainPlayerIndex = player.second.mThreadId;
				}
			}

			for (auto& player : sPlayerMap) {
				player.second.mSceneData.MainPlayerIndex = mainPlayerIndex;

				if (threadId == player.first)
					continue;

				// 모든 플레이어에게 자신의 데이터 송신
				Data::SendDataAndType<SceneData>(player.second.mSock, sPlayerMap[threadId].mSceneData);
				Data::SendDataAndType<TownData>(player.second.mSock, sPlayerMap[threadId].mTownData);
				Data::SendDataAndType<StageData>(player.second.mSock, sPlayerMap[threadId].mStageData);

				// 자신에게 모든 플레이어 데이터 송신
				Data::SendDataAndType<SceneData>(clientSock, player.second.mSceneData);
				Data::SendDataAndType<TownData>(clientSock, player.second.mTownData);
				Data::SendDataAndType<StageData>(clientSock, player.second.mStageData);
			}

			// 모든 플레이어들이 배틀 화면에 있을 경우에만 배틀 프로세스를 진행한다.
			bool isAllPlayerBattleScene = all_of(sPlayerMap.begin(), sPlayerMap.end(), [](const auto& a) {
				return a.second.mSceneData.Scene == (uint8)Scene::Battle; });

			if (isAllPlayerBattleScene)
				SetEvent(hAllPlayerBattleSceneEvent);

#ifdef _DEBUG
			string sceneStr;
			switch (data.Scene)
			{
			case 0:
				sceneStr = "INTRO";
				break;
			case 1:
				sceneStr = "TOWN";
				break;
			case 2:
				sceneStr = "STAGE";
				break;
			case 3:
				sceneStr = "PHASE";
				break;
			case 4:
				sceneStr = "BATTLE";
				break;
			default:
				sceneStr = "NONE";
				break;
			}
			cout << "CLIENT_NUMBER: " << static_cast<uint32>(threadId) 
				 << ", SCENE: " << sceneStr << endl;
#endif 

		}
#pragma endregion
#pragma region Intro
		else if (dataType == DataType::INTRO_DATA) {
			auto& data = sPlayerMap[threadId].mIntroData;
			std::lock_guard<std::mutex> lock(sPlayersMutex);

			Data::RecvData<IntroData>(clientSock, data);
			data.PlayerIndex = static_cast<uint8>(threadId);

			for (const auto& player : sPlayerMap) {
				// 현재 클라이언트의 인트로 정보를 모든 클라이언트로 송신한다.
				Data::SendDataAndType<IntroData>(player.second.mSock, data);

				if (threadId == player.first)
					continue;

				// 모든 클라이언트의 인트로 정보를 현재 클라이언트로 송신한다.
				Data::SendDataAndType<IntroData>(clientSock, player.second.mIntroData);
			}
		}
#pragma endregion
#pragma region Town
		else if (dataType == DataType::TOWN_DATA) {
			auto& data = sPlayerMap[threadId].mTownData;
			std::lock_guard<std::mutex> lock(sPlayersMutex);

			Data::RecvData<TownData>(clientSock, data);
			data.PlayerIndex = static_cast<uint8>(threadId);

			bool allReady = all_of(sPlayerMap.begin(), sPlayerMap.end(), [](const auto& a) {
					return a.second.mTownData.IsReady == 1;
				});
			data.CanGoNextScene = allReady;

			for (const auto& player : sPlayerMap) {
				Data::SendDataAndType<TownData>(player.second.mSock, data);
			}
		}
#pragma endregion
#pragma region Stage
		else if (dataType == DataType::STAGE_DATA) {
			auto& data = sPlayerMap[threadId].mStageData;
			std::lock_guard<std::mutex> lock(sPlayersMutex);

			Data::RecvData<StageData>(clientSock, data);

			// 메인 플레이어일 경우에만 스테이지 데이터를 다른 플레이어에게 송신
			if (threadId != mainPlayerIndex)
				continue;

			// 모든 플레이어의 준비 상태 확인
			bool allReady = all_of(sPlayerMap.begin(), sPlayerMap.end(), [](const auto& a) {
				return a.second.mStageData.IsReady == 1; });

			// 모든 플레이어가 준비되었다면 메인 플레이어의 다음 씬 플래그를 true로 설정
			if (allReady)
				sPlayerMap[mainPlayerIndex].mStageData.CanGoNextScene = true;

			// 메인 플레이어의 화면 동시 송출
			for (const auto& player : sPlayerMap) {
				Data::SendDataAndType<StageData>(player.second.mSock, sPlayerMap[mainPlayerIndex].mStageData);
			}

			for ( auto& player : sPlayerMap) {
				player.second.mStageData.InputKey = 0;
			}
		}
#pragma endregion
#pragma region Phase
		else if (dataType == DataType::PHASE_DATA) {
			auto& data = sPlayerMap[threadId].mPhaseData;
			std::lock_guard<std::mutex> lock(sPlayersMutex);

			Data::RecvData<PhaseData>(clientSock, data);

			for (const auto& player : sPlayerMap) {
				Data::SendDataAndType<PhaseData>(player.second.mSock, sPlayerMap[mainPlayerIndex].mPhaseData);
			}

			for (auto& player : sPlayerMap) {
				player.second.mPhaseData.InputKey = 0;
			}
		}
#pragma endregion
#pragma region Battle
		else if (dataType == DataType::BATTLE_DATA) {
			auto& data = sPlayerMap[threadId].mBattleData;
			Data::RecvData<BattleData>(clientSock, data);
		}
#pragma endregion
	}

	sPlayerMap.erase(threadId);
	closesocket(clientSock);

#ifdef _DEBUG
	cout << "[" << static_cast<uint32>(threadId)
		<< "번 클라이언트 종료] IP: " << inet_ntoa(clientaddr.sin_addr)
		<< ", PORT: " << ntohs(clientaddr.sin_port) << endl << endl;
#endif

	return;
}

void ProcessBattle()
{
	while (true) {
		// 모든 플레이어가 배틀 씬에 입장할 때까지 기다린다.
		WaitForSingleObject(hAllPlayerBattleSceneEvent, INFINITE);
	
		Battle battle;
		battle.Init();
		while (true) {
			GET_SINGLE(Timer)->Update();
			battle.Update(DELTA_TIME);
		}
	}
}

int main(int argc, char* argv[])
{
#pragma region Init
	int retval;
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return 1;
	GET_SINGLE(Timer)->Init();
	GET_SINGLE(Timer)->Start();
	hAllPlayerBattleSceneEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
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

	thread logicThread{ ProcessBattle };
	vector<thread> clientThread;
	uint8 sPlayerCount{};

	while (1) {
		addrlen = sizeof(clientaddr);
		clientSock.Sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
		if (clientSock.Sock == INVALID_SOCKET) break;
		clientSock.Id = sPlayerCount++;

		char addr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));

#ifdef _DEBUG
		cout << "[" << static_cast<uint32>(clientSock.Id) 
		     << "번 클라이언트 접속] IP: " << inet_ntoa(clientaddr.sin_addr) 
			 << ", PORT: " << ntohs(clientaddr.sin_port) << endl << endl;
#endif 
		clientThread.emplace_back(ProcessClient, clientSock);
	}
#pragma endregion
#pragma region Close
	for (auto& clientThread : clientThread) clientThread.join();
	logicThread.join();
	closesocket(listen_sock);
	WSACleanup();
#pragma endregion
}
