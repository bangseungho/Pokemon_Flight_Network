#include "..\Utils.h"
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <string>

#include "ServerUtils.h"

//fmod 기기에 설치해야함
using namespace std;

#define MAX_BUFSIZE     1024

// 서버에서 가지고 있는 플레이어들의 전역 데이터
static unordered_map<uint8, NetworkPlayerData> sPlayers;
CRITICAL_SECTION cs;
//EnterCriticalSection(&cs);
//LeaveCriticalSection(&cs);
uint8 sPlayerCount{};

CharacterSelection selection;


DWORD WINAPI ProcessClient(LPVOID sock)
{
	
	
	// ThreadSocket = Socket + threadId 
	ThreadSocket* threadSocket = reinterpret_cast<ThreadSocket*>(sock);
	SOCKET clientSock = threadSocket->Sock;
	uint8 threadId = threadSocket->Id;

	// 플레이어 배열에 해당 클라이언트 추가
	sPlayers[threadId] = NetworkPlayerData{ clientSock, threadId };

	// 클라이언트와 연결이 되었다면 클라이언트에게 자신의 인덱스를 송신
	if (!Data::SendData(clientSock, threadId))
		return 0;

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
			break;

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
			for (const auto& player : sPlayers) {
				// 플레이어가 자신이라면 패킷을 전송하지 않는다.
				if (player.second.mThreadId == threadId)
					continue;

				// 현재 클라이언트의 씬 정보를 모든 클라이언트로 송신한다.
				Data::SendDataAndType<SceneData>(player.second.mSock, data);

				// 모든 클라이언트의 씬, 타운 정보를 현재 클라이언트로 송신한다.
				// 씬 패킷 송신에서 타운 패킷까지 송신하는 이유는 타운에서는 다른 씬과는 다르게
				// 멤버 플레이어들의 씬 위치마다 해당 클라이언트에게 보이거나 보이지 말아야 한다.
				// 또한 키보드 입력을 눌렀을 때만 타운 패킷을 송신하기 때문에 씬을 타운으로 전환 후
				// 한 번도 키보드를 누르지 않은 경우 멤버 플레이어가 보이지 않거나 이전 위치에 있다.
				// 따라서 새로운 씬이 로드 될 때마다 새로운 위치를 전송하는 것이다.
				// 이후 배틀에서도 멤버 플레이어들의 배틀 패킷을 한 번 보내야 한다.
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

			for (const auto& player : sPlayers) {
				if (player.second.mThreadId == threadId)
					continue;

				// 현재 클라이언트의 인트로 정보를 모든 클라이언트로 송신한다.
				Data::SendDataAndType<IntroData>(player.second.mSock, data);

				// 모든 클라이언트의 인트로 정보를 현재 클라이언트로 송신한다.
				Data::SendDataAndType<IntroData>(clientSock, player.second.mIntroData);
			}
		}
#pragma endregion
#pragma region Town
		else if (dataType == DataType::TOWN_DATA) {
			auto& data = sPlayers[threadId].mTownData;
			Data::RecvData<TownData>(clientSock, data);
			data.PlayerIndex = static_cast<uint8>(threadId);

			//EnterCriticalSection(&cs);
			for (const auto& player : sPlayers) {
				if (player.second.mThreadId == threadId)
					continue;

				Data::SendDataAndType<TownData>(player.second.mSock, data);
			}
			//LeaveCriticalSection(&cs);

#ifdef _DEBUG
			cout << "CLIENT_NUMBER: " << static_cast<uint32>(threadId) 
				 << ", ISREADY: " << data.IsReady 
				 << ", POSX: " << data.PlayerData.Pos.x 
				 << ", POSY: " << data.PlayerData.Pos.y << endl;
#endif 
		}
#pragma endregion
#pragma region Stage
		if (dataType == DataType::STAGE_DATA) {
			auto& data = sPlayers[threadId].mStageData;
			Data::RecvData<StageData>(clientSock, data);

			
				for (const auto& player : sPlayers) {

					if (player.second.mThreadId == threadId) {
						

						if (selection.player1 == 3) { // 1착
							data.PlayerData.PickApproved = true;
							selection.player1 = data.PlayerData.PickedCharacter;
							Data::SendDataAndType<StageData>(player.second.mSock, data);
						}
						else if (selection.player1 == data.PlayerData.PickedCharacter) { // 겹침
							data.PlayerData.PickApproved = false;
							Data::SendDataAndType<StageData>(player.second.mSock, data);
						
						}
						else if (selection.player1 != data.PlayerData.PickedCharacter) { //안겹침
							data.PlayerData.PickApproved = true;
							selection.player2 = data.PlayerData.PickedCharacter;
							Data::SendDataAndType<StageData>(player.second.mSock, data);
						}

					}


				}
			

			
			


			//for (const auto& player : sPlayers) {
			//	if (player.mThreadId == threadId)
			//		continue;

			//	SendData<StageData>(clientSock, player.mStageData);
			//}
			Data::SendDataAndType<StageData>(clientSock, data);

#ifdef _DEBUG
			cout << "RECORD: " << data.Record << endl;
			cout << "pick1: " << selection.player1 << "pick2: " << selection.player2 << endl;
#endif
		}
#pragma endregion
#pragma region Phase
		if (dataType == DataType::PHASE_DATA) {
			auto& data = sPlayers[threadId].mPhaseData;
			Data::RecvData<PhaseData>(clientSock, data);

			//for (const auto& player : sPlayers) {
			//	if (player.mThreadId == threadId)
			//		continue;

			//	SendData<PhaseData>(clientSock, player.mPhaseData);
			//}

#ifdef _DEBUG
			cout << "ISREADY: " << data.IsReady << endl;
#endif
		}
#pragma endregion
#pragma region Battle
		if (dataType == DataType::BATTLE_DATA) {
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

#ifdef _DEBUG
		cout << "[" << static_cast<uint32>(clientSock.Id) 
		     << "번 클라이언트 접속] IP: " << inet_ntoa(clientaddr.sin_addr) 
			 << ", PORT: " << ntohs(clientaddr.sin_port) << endl << endl;
#endif 

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
