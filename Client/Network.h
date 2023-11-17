#pragma once
#include <thread>

using namespace std;

class Network
{
	SINGLETON(Network)

public:
	void Init(string ipAddr);
	void Connect();
	void DisConnect();
	void Close();

	void Receiver();
	void SendIntroData(const IntroData& data = {});
	void SendTownData(const TownData& data = {});
	void SendStageData(const StageData& data = {});

	IntroData& GetIntroData() { return mRecvIntroData; }
	vector<TownData>& GetTownData() { return mRecvTownData; }
	StageData& GetStageData() { return mRecvStageData; }

private:
	SOCKET mClientSock;
	SOCKADDR_IN mServerAddr;
	thread mRecvThread;

	IntroData mRecvIntroData;
	vector<TownData> mRecvTownData;
	StageData mRecvStageData;
};

