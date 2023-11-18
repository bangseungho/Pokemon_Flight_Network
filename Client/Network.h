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

	template<typename T>
	void SendTypelessData(const T& data = {});

	void Receiver();
	void SendIntroData(const IntroData& data = {});
	void SendTownData(const TownData& data = {});
	void SendStageData(const StageData& data = {});

	IntroData& GetIntroData() { return mRecvIntroData; }
	unordered_map<uint8, TownData>& GetTownData() { return mRecvTownData; }
	StageData& GetStageData() { return mRecvStageData; }
	
	uint8 GetClientIndex() const { return mRecvIntroData.PlayerIndex; }

private:
	bool mConnected;
	SOCKET mClientSock;
	SOCKADDR_IN mServerAddr;
	thread mRecvThread;

	IntroData mRecvIntroData;
	unordered_map<uint8, TownData> mRecvTownData; // 파트너 플레이어
	StageData mRecvStageData;
};

template<typename T>
inline void Network::SendTypelessData(const T& data)
{
	int retVal = SendData(mClientSock, data);

#ifdef _DEBUG
	if (retVal)
		cout << "[ Send TypelessData - (" << sizeof(T) << " Byte) ]" << Endl;
	else
		cout << "[ Failed Send TypelessData! ]" << Endl;
#endif 
}
