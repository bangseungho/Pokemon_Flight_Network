#include "stdafx.h"
#include "Network.h"

DECLARE_SINGLE(Network);

Network::Network()
{
	mConnected = false;
}

Network::~Network()
{
	if (mRecvThread.joinable())
		mRecvThread.join();

	closesocket(mClientSock);
	WSACleanup();

#ifdef _DEBUG
	cout << "[ Close the Socket! ]" << Endl;
#endif 
}

void Network::Receiver()
{
	while (1) {
		DataType dataType;
		dataType = Data::RecvType(mClientSock);

#pragma region EndProcessing
		if (dataType == DataType::END_PROCESSING) {
			// 종료 클라이언트 인덱스를 수신
			EndProcessing endProcessing;
			ZeroMemory(&endProcessing, sizeof(EndProcessing));

			// 패킷 수신
			Data::RecvData<EndProcessing>(mClientSock, endProcessing);

			// 종료 클라이언트 인덱스가 자신이라면 종료
			if (mClientIndex == endProcessing.PlayerIndex)
				break;

			// 자신이 아니라면 멤버 맵에서 해당 클라이언트 제거
			mRecvMemberMap.erase(endProcessing.PlayerIndex);
		}
#pragma endregion
#pragma region SceneData
		else if (dataType == DataType::SCENE_DATA) {
			// 패킷을 수신할 임시 객체
			SceneData recvData;
			ZeroMemory(&recvData, sizeof(SceneData));

			// 패킷 수신
			Data::RecvData<SceneData>(mClientSock, recvData);

			// 멤버 맵에 임시 객체 이동
			mRecvMemberMap[recvData.PlayerIndex].mSceneData = move(recvData);
		}
#pragma endregion
#pragma region Intro
		else if (dataType == DataType::INTRO_DATA) {
			// 패킷을 수신할 임시 객체
			IntroData recvData;
			ZeroMemory(&recvData, sizeof(IntroData));

			// 패킷 수신
			Data::RecvData<IntroData>(mClientSock, recvData);

			// 새로운 멤버 생성
			mRecvMemberMap[recvData.PlayerIndex].mIntroData = move(recvData);
		}
#pragma endregion
#pragma region Town
		else if (dataType == DataType::TOWN_DATA) {
			// 패킷을 수신할 임시 객체
			TownData recvData;
			ZeroMemory(&recvData, sizeof(TownData));

			// 패킷 수신
			Data::RecvData<TownData>(mClientSock, recvData);

			// 멤버 맵에 임시 객체 이동
			mRecvMemberMap[recvData.PlayerIndex].mTownData = move(recvData);
		}
#pragma endregion
	}
}

void Network::Init(string ipAddr)
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
	if (true == mConnected) {
#ifdef _DEBUG
		cout << "[ Already Connected! ]" << Endl;
#endif 
		return;
	}

	mClientSock = socket(AF_INET, SOCK_STREAM, 0);
	if (mClientSock == INVALID_SOCKET)
		ErrorQuit("socket()");

	int retVal;
	retVal = connect(mClientSock, (SOCKADDR*)&mServerAddr, sizeof(mServerAddr));
	if (retVal == SOCKET_ERROR) ErrorQuit("connect()");
	else mConnected = true;

	// 클라이언트 자신의 소켓에 대한 정보 얻기
	SOCKADDR_IN localAddr;
	int localAddrLength = sizeof(localAddr);
	getsockname(mClientSock, (SOCKADDR*)&localAddr, &localAddrLength);

	// 첫 연결에서 자신의 클라이언트 인덱스를 수신한다.
	Data::RecvData(mClientSock, mClientIndex);

#ifdef _DEBUG
	cout << "[ Connect to Server! ]" << endl;
	cout << "[ Server - (IP: " << inet_ntoa(mServerAddr.sin_addr) << ", ";
	cout << "PORT: " << SERVERPORT << ") ]" << endl;
	cout << "[ Client - (IP: " << inet_ntoa(localAddr.sin_addr) << ", ";
	cout << "PORT: " << ntohs(localAddr.sin_port) << ", ";
	cout << "CLIENT_NUMBER: " << static_cast<uint32>(mClientIndex) << ") ]" << Endl;
#endif 
	
	// 자신의 클라이언트 인덱스를 수신 받았다면 다시 모든 클라이언트에게 자신의 인덱스를 송신한다.
	Network::SendDataAndType(IntroData{ mClientIndex });

	// Recv 스레드 생성
	mRecvThread = thread(&Network::Receiver, this);
}