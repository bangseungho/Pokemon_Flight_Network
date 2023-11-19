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
	void Receiver();

public:
	void Init(string ipAddr);
	void Connect();

public:
	template<typename T>
	void SendDataAndType(const T& data = {});

public:
	uint8 GetClientIndex() const { return mClientIndex; }
	bool IsConnected() const { return mConnected; }
	SOCKET& GetSocket() { return mClientSock; }
	unordered_map<uint8, NetworkPlayerData>& GetMemberMap() { return mRecvMemberMap; }

private:
	bool			mConnected;
	uint8			mClientIndex;
	thread			mRecvThread;
	SOCKET			mClientSock;
	SOCKADDR_IN		mServerAddr;

	unordered_map<uint8, NetworkPlayerData> mRecvMemberMap;
};

template<typename T>
inline void Network::SendDataAndType(const T& data)
{
	int retVal = Data::SendDataAndType(mClientSock, data);

#ifdef _DEBUG
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