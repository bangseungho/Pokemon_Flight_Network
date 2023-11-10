#include "..\..\Common.h"
#include<Windows.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#define SERVERPORT		9000
#define MAX_BUFSIZE     1024

using namespace std;

void SetCursorPosition(int x, int y, int clientName)
{
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD cursorPos;
	cursorPos.X = x;
	cursorPos.Y = clientName * 4 + y;
	SetConsoleCursorPosition(console, cursorPos);
}

class FileData
{
public:
	FileData() { mClientNum = ClientNum++; }
	virtual ~FileData() { }

public:
	void RecvFileName(SOCKET& clientSock)
	{
		int retVal;
		int len;

		retVal = recv(clientSock, (char*)&len, sizeof(int), MSG_WAITALL);
		if (retVal == SOCKET_ERROR) err_display("recv()");

		mFileName.resize(len);

		retVal = recv(clientSock, (char*)mFileName.c_str(), len, MSG_WAITALL);
		if (retVal == SOCKET_ERROR) err_display("recv()");

		mFile.open(mFileName, ios::binary);
	}

	void RecvFileSize(SOCKET& clientSock)
	{
		int retVal;
		int len;

		retVal = recv(clientSock, (char*)&len, sizeof(int), MSG_WAITALL);
		if (retVal == SOCKET_ERROR) err_display("recv()");

		mSFileSize.resize(len);

		retVal = recv(clientSock, (char*)mSFileSize.c_str(), len, MSG_WAITALL);
		if (retVal == SOCKET_ERROR) err_display("recv()");

		mFileSize = atoi(mSFileSize.c_str());
	}

	void RecvFileData(SOCKET& clientSock)
	{
		int retVal;

		int remainFileSize = mFileSize;
		int bufSize;
		while (1) {
			if (remainFileSize >= MAX_BUFSIZE)
				bufSize = MAX_BUFSIZE;
			else {
				bufSize = remainFileSize;
				mCompleteRecvData = true;
			}

			string buf;
			buf.resize(bufSize);

			retVal = recv(clientSock, (char*)buf.c_str(), bufSize, MSG_WAITALL);
			if (retVal == SOCKET_ERROR or retVal == 0) break;

			mFile.write((char*)buf.c_str(), bufSize);

			remainFileSize -= bufSize;
			mCurrFileSize += bufSize;
			PrintRecvData();
		}
	}

	void PrintRecvData() const
	{
		SetCursorPosition(0, 2, mClientNum);
		if (!mCompleteRecvData)
			cout << "[" << mClientNum << "번 클라이언트" << "] " << "[수신중..] - [파일 이름] : " << mFileName << " " << "[파일 전송률] : " << mCurrFileSize / (float)mFileSize * 100 << "% / 100% \r";
		if (mCompleteRecvData)
			cout << "[" << mClientNum << "번 클라이언트" << "] " << "[파일 수신 완료]- [파일 이름] : " << mFileName << " " << "[파일 전송률] : " << mCurrFileSize / (float)mFileSize * 100 << "% / 100% \r";
	}

	int GetClientNum() const
	{
		return mClientNum;
	}

private:
	ofstream mFile;
	string mFileName;
	string mSFileSize;
	size_t mFileSize;

	bool mCompleteRecvData = false;
	double mCurrFileSize = 0.f;

	int mClientNum;
	static int ClientNum;
};

int FileData::ClientNum = 0;

DWORD WINAPI ProcessClient(LPVOID sock)
{
	SOCKET clientSock = (SOCKET)sock;

	struct sockaddr_in clientaddr;
	char addr[INET_ADDRSTRLEN];
	int addrlen;

	addrlen = sizeof(clientaddr);
	getpeername(clientSock, (struct sockaddr*)&clientaddr, &addrlen);
	inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));

	FileData fileData;
	fileData.RecvFileName(clientSock);
	fileData.RecvFileSize(clientSock);
	fileData.RecvFileData(clientSock);

	closesocket(clientSock);
	SetCursorPosition(0, 3, fileData.GetClientNum());
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

		SetCursorPosition(0, 0, clientName);
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
