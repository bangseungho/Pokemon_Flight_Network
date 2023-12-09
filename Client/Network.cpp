#include "stdafx.h"
#include "Network.h"
#include "scene.h"
#include "enemy.h"
#include "player.h"
#include "interface.h"

DECLARE_SINGLE(Network);
extern SceneManager* sceneManager;
extern EnemyController* enemies;
extern GUIManager* gui;
extern atomic<bool> isEndBattle;

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

//#ifdef _DEBUG
//			cout << "[" << static_cast<uint32>(recvData.PlayerIndex) << "�� �÷��̾� ���� ����]" << endl;
//#endif 
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
#pragma region Battle
		else if (dataType == DataType::BATTLE_DATA) {
			// ��Ŷ�� ������ �ӽ� ��ü
			BattleData recvData;

			// ��Ŷ ����
			Data::RecvData<BattleData>(mClientSock, recvData);

			// ����� ��Ŷ�̶�� �ɹ� �ʿ� �־���
			auto findIt = mRecvMemberMap.find(recvData.PlayerIndex);
			if (findIt != mRecvMemberMap.end())
				mRecvMemberMap[recvData.PlayerIndex].mBattleData = move(recvData);

			//cout << mRecvMemberMap[MY_INDEX].mBattleData.PosCenter.x << endl;
		}
#pragma endregion
#pragma region Enemy
		else if (dataType == DataType::ENEMY_OBJECT_DATA) {
			// ��Ŷ�� ������ �ӽ� ��ü
			NetworkEnemyData recvData;
			// ��Ŷ ����
			Data::RecvData<NetworkEnemyData>(mClientSock, recvData);

			switch (recvData.AttackType)
			{
			case NetworkEnemyData::AttackType::MELEE:
				enemies->CreateRecvMelee(recvData);
				break;
			case NetworkEnemyData::AttackType::RANGE:
				enemies->CreateRecvRange(recvData);
				break;
			case NetworkEnemyData::AttackType::DEATH:
				enemies->Pop(recvData.TargetIndex);
				break;
			default:
				break;
			}
		}
#pragma endregion
#pragma region Bullet
		else if (dataType == DataType::BULLET_DATA) {
			// ��Ŷ�� ������ �ӽ� ��ü
			NetworkBulletData recvData;
			// ��Ŷ ����
			Data::RecvData<NetworkBulletData>(mClientSock, recvData);

			if (isEndBattle.load() == true)
				continue;

			switch (recvData.Status)
			{
			case NetworkBulletData::Status::CREATE:
				sceneManager->GetMemberMap()[recvData.PlayerIndex]->Shot();
				break;
			case NetworkBulletData::Status::IDENTITY:
				sceneManager->GetMemberMap()[recvData.PlayerIndex]->ActiveSkill(Skill::Identity);
				break;
			case NetworkBulletData::Status::SECTOR:
				sceneManager->GetMemberMap()[recvData.PlayerIndex]->ActiveSkill(Skill::Sector);
				break;
			case NetworkBulletData::Status::CIRCLE:
				sceneManager->GetMemberMap()[recvData.PlayerIndex]->ActiveSkill(Skill::Circle);
				break;
			case NetworkBulletData::Status::E_CREATE:
				enemies->AttackBasedOnIndex(recvData.PlayerIndex);
				break;
			default:
				break;
			}
		}
		else if (dataType == DataType::GAME_DATA) {
			// ��Ŷ�� ������ �ӽ� ��ü
			NetworkGameData recvData;
			// ��Ŷ ����
			Data::RecvData<NetworkGameData>(mClientSock, recvData);

			if (recvData.IsEndBattleProcess == true) {
				sceneManager->StartLoading(sceneManager->GetHwnd());

				for (auto& member : mRecvMemberMap) {
					member.second.mBattleData.Clear();
				}
			}
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

	// ���̱� �˰����� ����
	int delayZeroOpt = 1;
	setsockopt(mClientSock, IPPROTO_TCP, TCP_NODELAY, (const char*)&delayZeroOpt, sizeof(delayZeroOpt));

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