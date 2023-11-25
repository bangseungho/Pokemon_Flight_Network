#include "stdafx.h"
#include "Network.h"

DECLARE_SINGLE(Network);

Network::Network()
{
	mConnected = false;
	mRecvMemberMap.reserve(10);
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
			EndProcessing recvData;
			ZeroMemory(&recvData, sizeof(EndProcessing));

			// 패킷 수신
			Data::RecvData<EndProcessing>(mClientSock, recvData);

			// 종료 클라이언트가 자신이라면 스레드를 종료한다.
			if (mClientIndex == recvData.PlayerIndex)
				break;

			// 멤버 맵에 해당 키 값이 있는 경우만 멤버 맵에서 해당 멤버 제거
			auto findIt = mRecvMemberMap.find(recvData.PlayerIndex);
			if (findIt != mRecvMemberMap.end())
				mRecvMemberMap.erase(findIt);

#ifdef _DEBUG
			cout << "[" << static_cast<uint32>(recvData.PlayerIndex) << "번 플레이어 게임 종료]" << endl;
#endif 
		}
#pragma endregion
#pragma region SceneData
		else if (dataType == DataType::SCENE_DATA) {
			// 패킷을 수신할 임시 객체
			SceneData recvData;
			ZeroMemory(&recvData, sizeof(SceneData));

			// 패킷 수신
			Data::RecvData<SceneData>(mClientSock, recvData);

			// 멤버 맵에 해당 키 값이 있는 경우만 멤버 맵에 데이터 이동
			auto findIt = mRecvMemberMap.find(recvData.PlayerIndex);
			if (findIt != mRecvMemberMap.end())
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

#ifdef _DEBUG
			cout << endl << "[	" << inet_ntoa(mServerAddr.sin_addr) << " 서버 플레이어 목록	]" << endl;
			for (const auto& member : mRecvMemberMap) {
				cout << "[	    " << static_cast<uint32>(recvData.PlayerIndex) << "번 플레이어 접속중		]" << Endl;
			}
#endif 
		}
#pragma endregion
#pragma region Town
		else if (dataType == DataType::TOWN_DATA) {
			// 패킷을 수신할 임시 객체
			TownData recvData;
			ZeroMemory(&recvData, sizeof(TownData));

			// 패킷 수신
			Data::RecvData<TownData>(mClientSock, recvData);

			// 멤버 맵에 해당 키 값이 있는 경우만 멤버 맵에 데이터 이동
			auto findIt = mRecvMemberMap.find(recvData.PlayerIndex);
			if (findIt != mRecvMemberMap.end())
				mRecvMemberMap[recvData.PlayerIndex].mTownData = move(recvData);
		}

#pragma region stage
		else if (dataType == DataType::STAGE_DATA) {
			// 패킷을 수신할 임시 객체
			StageData recvData;
			ZeroMemory(&recvData, sizeof(StageData));

			// 패킷 수신
			Data::RecvData<StageData>(mClientSock, recvData);

			// 멤버 맵에 해당 키 값이 있는 경우만 멤버 맵에 데이터 이동
			auto findIt = mRecvMemberMap.find(recvData.PlayerIndex);
			if (findIt != mRecvMemberMap.end())
				mRecvMemberMap[recvData.PlayerIndex].mStageData = move(recvData);
		}
#pragma endregion

#pragma region Stage
		else if (dataType == DataType::STAGE_DATA) {
			// 패킷을 수신할 임시 객체
			StageData recvData;
			ZeroMemory(&recvData, sizeof(StageData));

			// 패킷 수신
			Data::RecvData<StageData>(mClientSock, recvData);
			// 멤버 맵에 해당 키 값이 있는 경우만 멤버 맵에 데이터 이동
			auto findIt = mRecvMemberMap.find(recvData.PlayerIndex);
			if (findIt != mRecvMemberMap.end())
				mRecvMemberMap[recvData.PlayerIndex].mStageData= move(recvData);

		}
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


