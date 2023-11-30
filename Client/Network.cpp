#include "stdafx.h"
#include "Network.h"
#include "scene.h"

DECLARE_SINGLE(Network);
extern SceneManager* sceneManager;

Network::Network()
{
	auto rectWindow = sceneManager->GetRectWindow();

	mConnected = false;
	mRecvMemberMap.reserve(10);
}

Network::~Network()
{
	if (mRecvClientThread.joinable())
		mRecvClientThread.join();

	closesocket(mClientSock);
	WSACleanup();

#ifdef _DEBUG
	cout << "[ Close the Socket! ]" << Endl;
#endif 
}

void Network::ClientReceiver()
{
	while (1) {
		DataType dataType;
		dataType = Data::RecvType(mClientSock);

#pragma region EndProcessing
		if (dataType == DataType::END_PROCESSING) {
			// ���� Ŭ���̾�Ʈ �ε����� ����
			EndProcessing recvData;

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

			// ��Ŷ ����
			Data::RecvData<SceneData>(mClientSock, recvData);

			// ���� �÷��̾� �ε���
			mMainPlayerIndex = recvData.MainPlayerIndex;

			// ��� �ʿ� �ش� Ű ���� �ִ� ��츸 ��� �ʿ� ������ �̵�
			auto findIt = mRecvMemberMap.find(recvData.PlayerIndex);
			if (findIt != mRecvMemberMap.end())
				mRecvMemberMap[recvData.PlayerIndex].mSceneData = move(recvData);

#ifdef _DEBUG
			std::lock_guard<std::mutex> lock(mMemberMapMutex);
			for (const auto& member : mRecvMemberMap) {
				if (mClientIndex == member.second.mSceneData.PlayerIndex)
					continue;

				string airPokemonStr;
				switch (member.second.mSceneData.AirPokemon)
				{
				case Type::Empty:
					airPokemonStr = "EMPTY";
					break;
				case Type::Fire:
					airPokemonStr = "MOLTRES";
					break;
				case Type::Elec:
					airPokemonStr = "THUNDER";
					break;
				case Type::Water:
					airPokemonStr = "ARTICUNO";
					break;
				}

				string landPokemonStr;
				switch (member.second.mSceneData.LandPokemon)
				{
				case Type::Empty:
					landPokemonStr = "EMPTY";
					break;
				case Type::Fire:
					landPokemonStr = "CHARMANDER";
					break;
				case Type::Elec:
					landPokemonStr = "PIKACHU";
					break;
				case Type::Water:
					landPokemonStr = "SQUIRTLE";
					break;
				}

				cout << "[" << (uint32)member.first << "�� �÷��̾�] - (Scene: " << (uint32)member.second.mSceneData.Scene << ", " 
					<< "AIR: " << airPokemonStr << ", LAND: " << landPokemonStr << ")" << endl;
			}
#endif 
		}
#pragma endregion
#pragma region Intro
		else if (dataType == DataType::INTRO_DATA) {
			// ��Ŷ�� ������ �ӽ� ��ü
			IntroData recvData;

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

			// ��Ŷ ����
			Data::RecvData<TownData>(mClientSock, recvData);

			// ��� �ʿ� �ش� Ű ���� �ִ� ��츸 ��� �ʿ� ������ �̵�
			auto findIt = mRecvMemberMap.find(recvData.PlayerIndex);
			if (findIt != mRecvMemberMap.end())
				mRecvMemberMap[recvData.PlayerIndex].mTownData = move(recvData);
		}
#pragma endregion
#pragma region Stage
		else if (dataType == DataType::STAGE_DATA) {
			// ��Ŷ�� ������ �ӽ� ��ü
			StageData recvData;

			// ��Ŷ ����
			Data::RecvData<StageData>(mClientSock, recvData);

			// ����� ��Ŷ�̶�� �ɹ� �ʿ� �־���
			auto findIt = mRecvMemberMap.find(recvData.PlayerIndex);
			if (findIt != mRecvMemberMap.end())
				mRecvMemberMap[recvData.PlayerIndex].mStageData = move(recvData);
		}
#pragma endregion
#pragma region Phase
		else if (dataType == DataType::PHASE_DATA) {
			// ��Ŷ�� ������ �ӽ� ��ü
			PhaseData recvData;

			// ��Ŷ ����
			Data::RecvData<PhaseData>(mClientSock, recvData);

			// ����� ��Ŷ�̶�� �ɹ� �ʿ� �־���
			auto findIt = mRecvMemberMap.find(recvData.PlayerIndex);
			if (findIt != mRecvMemberMap.end())
				mRecvMemberMap[recvData.PlayerIndex].mPhaseData = move(recvData);
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

	//// ���̱� �˰��� ����
	//int delayZeroOpt = 1;
	//setsockopt(mClientSock, IPPROTO_TCP, TCP_NODELAY, (const char*)&delayZeroOpt, sizeof(delayZeroOpt));

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
	mRecvClientThread = thread(&Network::ClientReceiver, this);
}