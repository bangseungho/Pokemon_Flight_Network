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
	void Init(string ipAddr);
	void Connect();

	void Receiver();

	template<typename T>
	void SendDataToTemplate(const T& data = {});

	void SendIntroData(const IntroData& data = {});
	void SendTownData(const TownData& data = {});
	void SendStageData(const StageData& data = {});

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
inline void Network::SendDataToTemplate(const T& data)
{
	int retVal = Data::SendDataAndType(mClientSock, data);
#ifdef _DEBUG
	if (retVal)
		cout << "[ Send TypelessData - (" << sizeof(T) << " Byte) ]" << Endl;
	else
		cout << "[ Failed Send TypelessData! ]" << Endl;
#endif 
}