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
			// 종료 클라이언트 인덱스를 수신
			EndProcessing recvData;

			// 패킷 수신
			Data::RecvData<EndProcessing>(mClientSock, recvData);

			// 종료 클라이언트가 자신이라면 스레드를 종료한다.
			if (mClientIndex == recvData.PlayerIndex)
				break;

			// 멤버 맵에 해당 키 값이 있는 경우만 멤버 맵에서 해당 멤버 제거
			auto findIt = mRecvMemberMap.find(recvData.PlayerIndex);
			if (findIt != mRecvMemberMap.end())
				mRecvMemberMap.erase(findIt);

//#ifdef _DEBUG
//			cout << "[" << static_cast<uint32>(recvData.PlayerIndex) << "번 플레이어 게임 종료]" << endl;
//#endif 
		}
#pragma endregion
#pragma region SceneData
		else if (dataType == DataType::SCENE_DATA) {
			// 패킷을 수신할 임시 객체
			SceneData recvData;

			// 패킷 수신
			Data::RecvData<SceneData>(mClientSock, recvData);

			// 메인 플레이어 인덱스
			mMainPlayerIndex = recvData.MainPlayerIndex;

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

			// 패킷 수신
			Data::RecvData<TownData>(mClientSock, recvData);

			// 멤버 맵에 해당 키 값이 있는 경우만 멤버 맵에 데이터 이동
			auto findIt = mRecvMemberMap.find(recvData.PlayerIndex);
			if (findIt != mRecvMemberMap.end())
				mRecvMemberMap[recvData.PlayerIndex].mTownData = move(recvData);
		}
#pragma endregion
#pragma region Stage
		else if (dataType == DataType::STAGE_DATA) {
			// 패킷을 수신할 임시 객체
			StageData recvData;

			// 패킷 수신
			Data::RecvData<StageData>(mClientSock, recvData);

			// 멤버의 패킷이라면 맴버 맵에 넣어줌
			auto findIt = mRecvMemberMap.find(recvData.PlayerIndex);
			if (findIt != mRecvMemberMap.end())
				mRecvMemberMap[recvData.PlayerIndex].mStageData = move(recvData);
		}
#pragma endregion
#pragma region Phase
		else if (dataType == DataType::PHASE_DATA) {
			// 패킷을 수신할 임시 객체
			PhaseData recvData;

			// 패킷 수신
			Data::RecvData<PhaseData>(mClientSock, recvData);

			// 멤버의 패킷이라면 맴버 맵에 넣어줌
			auto findIt = mRecvMemberMap.find(recvData.PlayerIndex);
			if (findIt != mRecvMemberMap.end())
				mRecvMemberMap[recvData.PlayerIndex].mPhaseData = move(recvData);
		}
#pragma endregion
#pragma region Battle
		else if (dataType == DataType::BATTLE_DATA) {
			// 패킷을 수신할 임시 객체
			BattleData recvData;

			// 패킷 수신
			Data::RecvData<BattleData>(mClientSock, recvData);

			// 멤버의 패킷이라면 맴버 맵에 넣어줌
			auto findIt = mRecvMemberMap.find(recvData.PlayerIndex);
			if (findIt != mRecvMemberMap.end())
				mRecvMemberMap[recvData.PlayerIndex].mBattleData = move(recvData);

			//cout << mRecvMemberMap[MY_INDEX].mBattleData.PosCenter.x << endl;
		}
#pragma endregion
#pragma region Enemy
		else if (dataType == DataType::ENEMY_OBJECT_DATA) {
			// 패킷을 수신할 임시 객체
			NetworkEnemyData recvData;
			// 패킷 수신
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
			// 패킷을 수신할 임시 객체
			NetworkBulletData recvData;
			// 패킷 수신
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
			default:
				break;
			}
		}
		else if (dataType == DataType::GAME_DATA) {
			// 패킷을 수신할 임시 객체
			NetworkGameData recvData;
			// 패킷 수신
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

	// 네이글 알고리즘 해제
	int delayZeroOpt = 1;
	setsockopt(mClientSock, IPPROTO_TCP, TCP_NODELAY, (const char*)&delayZeroOpt, sizeof(delayZeroOpt));

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
	mRecvClientThread = thread(&Network::ClientReceiver, this);
}