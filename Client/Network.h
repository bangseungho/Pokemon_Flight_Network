#pragma once
#include <thread>

using namespace std;

class Network
{
	SINGLETON(Network)

public:
	Network();
	~Network();

public:
	void ClientReceiver();

public:
	void Init(string ipAddr);
	void Connect();

public:
	template<typename T>
	void SendDataAndType(const T& data = {});

public:
	uint8 GetClientIndex() const { return mClientIndex; }
	uint8 GetMainPlayerIndex() const { return mMainPlayerIndex; }
	bool IsConnected() const { return mConnected; }
	SOCKET& GetSocket() { return mClientSock; }
	mutex& GetMemberMapMutex() { return mMemberMapMutex; }
	unordered_map<uint8, NetworkPlayerData>& GetMemberMap() { return mRecvMemberMap; }
	unordered_map<uint8, NetworkEnemyData>& GetEnemyMap() { return mRecvEnemyMap; }

private:
	bool				mConnected;
	uint8				mClientIndex;
	uint8				mMainPlayerIndex;
	thread				mRecvClientThread;
	SOCKET				mClientSock;
	SOCKADDR_IN			mServerAddr;
	mutex				mMemberMapMutex;

	unordered_map<uint8, NetworkPlayerData> mRecvMemberMap;
	unordered_map<uint8, NetworkEnemyData> mRecvEnemyMap;
};

#define GET_MEMBER_MAP GET_SINGLE(Network)->GetMemberMap()
#define GET_ENEMY_VEC GET_SINGLE(Network)->GetEnemyMap()
#define MEMBER_MAP(index) GET_SINGLE(Network)->GetMemberMap()[index]
#define MY_INDEX GET_SINGLE(Network)->GetClientIndex()
#define MP_INDEX GET_SINGLE(Network)->GetMainPlayerIndex()

template<typename T>
inline void Network::SendDataAndType(const T& data)
{
	int retVal = Data::SendDataAndType(mClientSock, data);

#ifdef _DEBUG
	std::lock_guard<std::mutex> lock(mMemberMapMutex);
	string dataTypeStr;
	DataType dataType = Data::GetDataType<T>();
	if (retVal) {
		cout << "[ Successed Send ";
		switch (dataType)
		{
		case DataType::NONE_DATA:
			dataTypeStr = "NONE_DATA";
			break;
		case DataType::INTRO_DATA:
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE);
			dataTypeStr = "INTRO_DATA";
			break;
		case DataType::TOWN_DATA:
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
			dataTypeStr = "TOWN_DATA";
			break;
		case DataType::STAGE_DATA:
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);
			dataTypeStr = "STAGE_DATA";
			break;
		case DataType::PHASE_DATA:
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_BLUE);
			dataTypeStr = "PHASE_DATA";
			break;
		case DataType::BATTLE_DATA:
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN);
			dataTypeStr = "BATTLE_DATA";
			break;
		case DataType::SCENE_DATA:
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_GREEN);
			dataTypeStr = "SCENE_DATA";
			break;
		case DataType::END_PROCESSING:
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
			dataTypeStr = "END_PROCESSING";
			break;
		default:
			dataTypeStr = "NONE_DATA";
			break;
		}
		cout << dataTypeStr;
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
		cout << " Data - (" << sizeof(T) << " Byte) ]" << endl;
	}
	else
		cout << "[ Failed Send Data ]" << endl;
#endif 
}