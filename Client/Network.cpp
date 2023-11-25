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
			// ���� Ŭ���̾�Ʈ �ε����� ����
			EndProcessing recvData;
			ZeroMemory(&recvData, sizeof(EndProcessing));

			// ��Ŷ ����
			Data::RecvData<EndProcessing>(mClientSock, recvData);

			// ���� Ŭ���̾�Ʈ�� �ڽ��̶�� �����带 �����Ѵ�.
			if (mClientIndex == recvData.PlayerIndex)
				break;

			// ��� �ʿ� �ش� Ű ���� �ִ� ��츸 ��� �ʿ��� �ش� ��� ����
			auto findIt = mRecvMemberMap.find(recvData.PlayerIndex);
			if (findIt != mRecvMemberMap.end())
				mRecvMemberMap.erase(findIt);

#ifdef _DEBUG
			cout << "[" << static_cast<uint32>(recvData.PlayerIndex) << "�� �÷��̾� ���� ����]" << endl;
#endif 
		}
#pragma endregion
#pragma region SceneData
		else if (dataType == DataType::SCENE_DATA) {
			// ��Ŷ�� ������ �ӽ� ��ü
			SceneData recvData;
			ZeroMemory(&recvData, sizeof(SceneData));

			// ��Ŷ ����
			Data::RecvData<SceneData>(mClientSock, recvData);

			// ��� �ʿ� �ش� Ű ���� �ִ� ��츸 ��� �ʿ� ������ �̵�
			auto findIt = mRecvMemberMap.find(recvData.PlayerIndex);
			if (findIt != mRecvMemberMap.end())
				mRecvMemberMap[recvData.PlayerIndex].mSceneData = move(recvData);
		}
#pragma endregion
#pragma region Intro
		else if (dataType == DataType::INTRO_DATA) {
			// ��Ŷ�� ������ �ӽ� ��ü
			IntroData recvData;
			ZeroMemory(&recvData, sizeof(IntroData));

			// ��Ŷ ����
			Data::RecvData<IntroData>(mClientSock, recvData);

			// ���ο� ��� ����
			mRecvMemberMap[recvData.PlayerIndex].mIntroData = move(recvData);

#ifdef _DEBUG
			cout << endl << "[	" << inet_ntoa(mServerAddr.sin_addr) << " ���� �÷��̾� ���	]" << endl;
			for (const auto& member : mRecvMemberMap) {
				cout << "[	    " << static_cast<uint32>(recvData.PlayerIndex) << "�� �÷��̾� ������		]" << Endl;
			}
#endif 
		}
#pragma endregion
#pragma region Town
		else if (dataType == DataType::TOWN_DATA) {
			// ��Ŷ�� ������ �ӽ� ��ü
			TownData recvData;
			ZeroMemory(&recvData, sizeof(TownData));

			// ��Ŷ ����
			Data::RecvData<TownData>(mClientSock, recvData);

			// ��� �ʿ� �ش� Ű ���� �ִ� ��츸 ��� �ʿ� ������ �̵�
			auto findIt = mRecvMemberMap.find(recvData.PlayerIndex);
			if (findIt != mRecvMemberMap.end())
				mRecvMemberMap[recvData.PlayerIndex].mTownData = move(recvData);
		}

#pragma region stage
		else if (dataType == DataType::STAGE_DATA) {
			// ��Ŷ�� ������ �ӽ� ��ü
			StageData recvData;
			ZeroMemory(&recvData, sizeof(StageData));

			// ��Ŷ ����
			Data::RecvData<StageData>(mClientSock, recvData);

			// ��� �ʿ� �ش� Ű ���� �ִ� ��츸 ��� �ʿ� ������ �̵�
			auto findIt = mRecvMemberMap.find(recvData.PlayerIndex);
			if (findIt != mRecvMemberMap.end())
				mRecvMemberMap[recvData.PlayerIndex].mStageData = move(recvData);
		}
#pragma endregion

#pragma region Stage
		else if (dataType == DataType::STAGE_DATA) {
			// ��Ŷ�� ������ �ӽ� ��ü
			StageData recvData;
			ZeroMemory(&recvData, sizeof(StageData));

			// ��Ŷ ����
			Data::RecvData<StageData>(mClientSock, recvData);
			// ��� �ʿ� �ش� Ű ���� �ִ� ��츸 ��� �ʿ� ������ �̵�
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

	// Ŭ���̾�Ʈ �ڽ��� ���Ͽ� ���� ���� ���
	SOCKADDR_IN localAddr;
	int localAddrLength = sizeof(localAddr);
	getsockname(mClientSock, (SOCKADDR*)&localAddr, &localAddrLength);

	// ù ���ῡ�� �ڽ��� Ŭ���̾�Ʈ �ε����� �����Ѵ�.
	Data::RecvData(mClientSock, mClientIndex);

#ifdef _DEBUG
	cout << "[ Connect to Server! ]" << endl;
	cout << "[ Server - (IP: " << inet_ntoa(mServerAddr.sin_addr) << ", ";
	cout << "PORT: " << SERVERPORT << ") ]" << endl;
	cout << "[ Client - (IP: " << inet_ntoa(localAddr.sin_addr) << ", ";
	cout << "PORT: " << ntohs(localAddr.sin_port) << ", ";
	cout << "CLIENT_NUMBER: " << static_cast<uint32>(mClientIndex) << ") ]" << Endl;
#endif 
	
	// �ڽ��� Ŭ���̾�Ʈ �ε����� ���� �޾Ҵٸ� �ٽ� ��� Ŭ���̾�Ʈ���� �ڽ��� �ε����� �۽��Ѵ�.
	Network::SendDataAndType(IntroData{ mClientIndex });

	// Recv ������ ����
	mRecvThread = thread(&Network::Receiver, this);
}


