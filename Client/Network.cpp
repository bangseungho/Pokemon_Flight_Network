#include "stdafx.h"
#include "Network.h"

DECLARE_SINGLE(Network);

void Network::Init(std::string ipAddr)
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) 
		ErrorQuit("init()");

	ZeroMemory(&mServerAddr, sizeof(SOCKADDR_IN));
	mServerAddr.sin_family = AF_INET;
	mServerAddr.sin_port = htons(SERVERPORT);
	inet_pton(AF_INET, ipAddr.c_str(), &mServerAddr.sin_addr);
}

void Network::Connect()
{
	mClientSock = socket(AF_INET, SOCK_STREAM, 0);
	if (mClientSock == INVALID_SOCKET)
		ErrorQuit("socket()");

	int retVal;
	retVal = connect(mClientSock, (SOCKADDR*)&mServerAddr, sizeof(mServerAddr));
	if (retVal == SOCKET_ERROR) ErrorQuit("connect()");

	// 클라이언트 자신의 소켓에 대한 정보 얻기
	SOCKADDR_IN localAddr;
	int localAddrLength = sizeof(localAddr);
	getsockname(mClientSock, (SOCKADDR*)&localAddr, &localAddrLength);

#ifdef _DEBUG
	cout << "[ Connect to Server! ]" << endl;
	cout << "[ Server - (IP: " << inet_ntoa(mServerAddr.sin_addr) << ", ";
	cout << "PORT: " << SERVERPORT << ") ]" << endl;
	cout << "[ Client - (IP: " << inet_ntoa(localAddr.sin_addr) << ", ";
	cout << "PORT: " << ntohs(localAddr.sin_port) << ") ]" << Endl;
#endif 

	GET_SINGLE(Network)->SendIntroData();
}

void Network::DisConnect()
{
	closesocket(mClientSock);

#ifdef _DEBUG
	cout << "[ DisConnect to Server! ]" << Endl;
#endif 
}

void Network::Close()
{
	closesocket(mClientSock);
	WSACleanup();

#ifdef _DEBUG
	cout << "[ Close the Socket! ]" << Endl;
#endif 
}

void Network::SendIntroData(const IntroData& data)
{
	int retVal = SendData(mClientSock, data);

#ifdef _DEBUG
	if (retVal)
		cout << "[ Send IntroData - (" << sizeof(IntroData) << " Byte) ]" << Endl;
	else
		cout << "[ Failed Send IntroData! ]" << Endl;
#endif 
}

void Network::SendTownData(const TownData& data)
{
	int retVal = SendData(mClientSock, data);

#ifdef _DEBUG
	if (retVal)
		cout << "[ Send TownData - (" << sizeof(TownData) << " Byte) ]" << Endl;
	else
		cout << "[ Failed Send TownData! ]" << Endl;
#endif 
}

void Network::SendStageData(const StageData& data)
{
	int retVal = SendData(mClientSock, data);

#ifdef _DEBUG
	if (retVal)
		cout << "[ Send StageData - (" << sizeof(StageData) << " Byte) ]" << Endl;
	else
		cout << "[ Failed Send StageData! ]" << Endl;
#endif 
}
