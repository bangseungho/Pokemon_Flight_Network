#include "..\Utils.h"
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <string>
#include <thread>
#include <chrono>
#include <algorithm>

#include "Timer.h"
#include "Physics.h"
using namespace std;

#define MAX_BUFSIZE     1024

// 서버에서 가지고 있는 플레이어들의 전역 데이터
static unordered_map<uint8, NetworkPlayerData> sPlayers;
CRITICAL_SECTION cs;

void ProcessTimer()
{
	//while (true) {
	//	GET_SINGLE(Timer)->Update();
	//	this_thread::sleep_for(chrono::milliseconds(16)); // 대략 60fps로 맞추기
	//}
}

void ProcessClient(ThreadSocket sock)
{
	// ThreadSocket = Socket + threadId 
	ThreadSocket threadSocket = sock;
	SOCKET clientSock = threadSocket.Sock;
	uint8 threadId = threadSocket.Id;

	// 플레이어 배열에 해당 클라이언트 추가
	sPlayers[threadId] = NetworkPlayerData{ clientSock, threadId };

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
			for (const auto& player : sPlayers) {
				Data::SendDataAndType<EndProcessing>(player.second.mSock, EndProcessing{ threadId });
			}
			break;
		}
#pragma region SceneData
		else if (dataType == DataType::SCENE_DATA) {
			// 전역 플레이어 배열에서 클라이언트 threadId를 통해 자신의 플레이어에 접근
			auto& data = sPlayers[threadId].mSceneData;
			Data::RecvData<SceneData>(clientSock, data);
			data.PlayerIndex = static_cast<uint8>(threadId);

			// 모든 클라이언트들에게 자신의 정보를 담은 패킷 송신
			uint8 minRecord = 9;
			for (const auto& player : sPlayers) {
				if (minRecord > player.second.mSceneData.Record) {
					minRecord = player.second.mSceneData.Record;
					mainPlayerIndex = player.second.mThreadId;
				}

				data.MainPlayerIndex = mainPlayerIndex;

				// 현재 클라이언트의 씬 정보를 모든 클라이언트로 송신한다.
				Data::SendDataAndType<SceneData>(player.second.mSock, data);
				Data::SendDataAndType<SceneData>(clientSock, player.second.mSceneData);
				Data::SendDataAndType<TownData>(clientSock, player.second.mTownData);
			}

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
			auto& data = sPlayers[threadId].mIntroData;
			Data::RecvData<IntroData>(clientSock, data);
			data.PlayerIndex = static_cast<uint8>(threadId);

			EnterCriticalSection(&cs);
			for (const auto& player : sPlayers) {
				if (player.second.mThreadId == threadId)
					continue;

				// 현재 클라이언트의 인트로 정보를 모든 클라이언트로 송신한다.
				Data::SendDataAndType<IntroData>(player.second.mSock, data);

				// 모든 클라이언트의 인트로 정보를 현재 클라이언트로 송신한다.
				Data::SendDataAndType<IntroData>(clientSock, player.second.mIntroData);
			}
			LeaveCriticalSection(&cs);
		}
#pragma endregion
#pragma region Town
		else if (dataType == DataType::TOWN_DATA) {
			auto& data = sPlayers[threadId].mTownData;
			Data::RecvData<TownData>(clientSock, data);

			data.PlayerIndex = static_cast<uint8>(threadId);

			EnterCriticalSection(&cs);
			bool allReady = all_of(sPlayers.begin(), sPlayers.end(), [](const auto& a) {
					return a.second.mTownData.IsReady == 1;
				});
			data.IsReady = allReady;

			for (const auto& player : sPlayers) {
				Data::SendDataAndType<TownData>(player.second.mSock, data);
			}
			LeaveCriticalSection(&cs);
//#ifdef _DEBUG
//			cout << "CLIENT_NUMBER: " << static_cast<uint32>(threadId)
//				 << ", ISREADY: " << data.IsReady 
//				 << ", POSX: " << data.PlayerData.Pos.x 
//				 << ", POSY: " << data.PlayerData.Pos.y << endl;
//#endif 
		}
#pragma endregion
#pragma region Stage
		else if (dataType == DataType::STAGE_DATA) {
			auto& data = sPlayers[threadId].mStageData;
			Data::RecvData<StageData>(clientSock, data);

#ifdef _DEBUG
			cout << "CLIENT_NUMBER: " << static_cast<uint32>(threadId) << ", AIR: " << (uint32)data.AirPokemon << ", LAND: " << (uint32)data.LandPokemon << endl;
#endif
			bool allPlayerReady = true;
			for (const auto& player : sPlayers) {
				if (player.second.mStageData.IsReady == false) {
					allPlayerReady = false;
					break;
				}
			}

			// 모든 플레이어가 준비 완료 되었다면 현재 플레이어와 모든 플레이어와 정보 공유
			if (allPlayerReady) {
				for (const auto& player : sPlayers) {
					if (player.second.mThreadId == threadId)
						continue;

					// 현재 클라이언트의 인트로 정보를 모든 클라이언트로 송신한다.
					Data::SendDataAndType<StageData>(player.second.mSock, data);

					// 모든 클라이언트의 인트로 정보를 현재 클라이언트로 송신한다.
					Data::SendDataAndType<StageData>(clientSock, player.second.mStageData);
				}
			}

			if (mainPlayerIndex != threadId)
				continue;

			if (data.InputKey == 0)
				continue;

			// 메인 플레이어 기준 화면 동시 송출
			for (const auto& player : sPlayers) {
				Data::SendDataAndType<StageData>(player.second.mSock, sPlayers[mainPlayerIndex].mStageData);
			}
		}
#pragma endregion
#pragma region Phase
		else if (dataType == DataType::PHASE_DATA) {
			auto& data = sPlayers[threadId].mPhaseData;
			Data::RecvData<PhaseData>(clientSock, data);
#ifdef _DEBUG
			cout << "ISREADY: " << data.IsReady << endl;
#endif
		}
#pragma endregion
#pragma region Battle
		else if (dataType == DataType::BATTLE_DATA) {
			auto& data = sPlayers[threadId].mBattleData;
			Data::RecvData<BattleData>(clientSock, data);

			//for (const auto& player : sPlayers) {
			//	if (player.mThreadId == threadId)
			//		continue;

			//	SendData<BattleData>(clientSock, player.mBattleData);
			//}
#ifdef _DEBUG
			cout << "ISCOLLIDER: " << data.IsCollider 
				 << ", POSX: " << data.PosX 
				 << ", POSY: " << data.PosY << endl;
#endif
		}
#pragma endregion
	}

	sPlayers.erase(threadId);
	closesocket(clientSock);

#ifdef _DEBUG
	cout << "[" << static_cast<uint32>(threadId)
		<< "번 클라이언트 종료] IP: " << inet_ntoa(clientaddr.sin_addr)
		<< ", PORT: " << ntohs(clientaddr.sin_port) << endl << endl;
#endif

	return;
}

int main(int argc, char* argv[])
{
#pragma region Init
	int retval;
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return 1;
	InitializeCriticalSection(&cs);
	GET_SINGLE(Timer)->Init();
	GET_SINGLE(Timer)->Start();
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

	thread processTimerThread{ ProcessTimer };
	vector<thread> processClientThread;
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
		processClientThread.emplace_back(ProcessClient, clientSock);
	}
#pragma endregion
#pragma region Close
	for (auto& clientThread : processClientThread) clientThread.join();

	DeleteCriticalSection(&cs);
	closesocket(listen_sock);
	WSACleanup();
#pragma endregion
}
