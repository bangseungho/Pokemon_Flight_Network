#include "..\Utils.h"
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "ServerUtils.h"

#define SERVERPORT		9000
#define MAX_BUFSIZE     1024

using namespace std;

DWORD WINAPI ProcessClient(LPVOID sock)
{
	SOCKET clientSock = (SOCKET)sock;

	struct sockaddr_in clientaddr;
	char addr[INET_ADDRSTRLEN];
	int addrlen;

	addrlen = sizeof(clientaddr);
	getpeername(clientSock, (struct sockaddr*)&clientaddr, &addrlen);
	inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));

	while (1) {
		DataType dataType;
		dataType = RecvDataType(clientSock);
		if (dataType == DataType::NONE_DATA)
			break;

#pragma region INTRO
		if (dataType == DataType::INTRO_DATA) {
			IntroData data;
			recv(clientSock, (char*)&data, sizeof(IntroData), 0);

			cout << "ID: " << data.Id << ", PASSWORD: " << data.Password << endl;
		}
#pragma endregion
#pragma region TOWN
		if (dataType == DataType::TOWN_DATA) {
			TownData data;
			recv(clientSock, (char*)&data, sizeof(TownData), 0);

			cout << "ISREADY: " << data.IsReady << ", POSX: " << data.PosX << ", POSY: " << data.PosY << endl;
		}
#pragma endregion
#pragma region STAGE
		if (dataType == DataType::STAGE_DATA) {
			StageData data;
			recv(clientSock, (char*)&data, sizeof(StageData), 0);

			cout << "RECORD: " << data.Record << endl;
		}
#pragma endregion
#pragma region PHASE
		if (dataType == DataType::PHASE_DATA) {
			PhaseData data;
			recv(clientSock, (char*)&data, sizeof(PhaseData), 0);

			cout << "ISREADY: " << data.IsReady << endl;
		}
#pragma endregion
#pragma region BATTLE
		if (dataType == DataType::BATTLE_DATA) {
			BattleData data;
			recv(clientSock, (char*)&data, sizeof(BattleData), 0);

			cout << "ISCOLLIDER: " << data.IsCollider << ", POSX: " << data.PosX << ", POSY: " << data.PosY << endl;
		}
#pragma endregion
	}

	closesocket(clientSock);
	printf("[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n", addr, ntohs(clientaddr.sin_port));
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
	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
#pragma endregion

#pragma region Bind
	retval = bind(listen_sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");
#pragma endregion

#pragma region Listen
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit("listen()");
#pragma endregion

	SOCKET clientSock;
	struct sockaddr_in clientaddr;
	int addrlen;
	HANDLE hThread;
	int clientName{};
	system("cls");
	while (1) {
		addrlen = sizeof(clientaddr);
		clientSock = accept(listen_sock, (struct sockaddr*)&clientaddr, &addrlen);
		if (clientSock == INVALID_SOCKET) break;

		char addr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));

		printf("\n[TCP 서버] [%d]클라이언트 접속: IP 주소=%s, 포트 번호=%d\n", clientName, addr, ntohs(clientaddr.sin_port));
		clientName++;

		hThread = CreateThread(NULL, 0, ProcessClient, (LPVOID)clientSock, 0, NULL);
		if (hThread == NULL) { closesocket(clientSock); }
		else { CloseHandle(hThread); }
	}

#pragma region Close
	closesocket(listen_sock);
	WSACleanup();
#pragma endregion
}
