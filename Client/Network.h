#pragma once

class Network
{
	SINGLETON(Network)

public:
	void Init(std::string ipAddr);
	void Connect();
	void DisConnect();
	void Close();

	void SendIntroData(const IntroData& data = {});
	void SendTownData(const TownData& data = {});
	void SendStageData(const StageData& data = {});

	IntroData& GetIntroData() { return mRecvIntroData; }
	TownData& GetTownData() { return mRecvTownData; }
	StageData& GetStageData() { return mRecvStageData; }

private:
	SOCKET mClientSock;
	SOCKADDR_IN mServerAddr;

	HANDLE mRecvThread;

	IntroData mRecvIntroData;
	TownData mRecvTownData;
	StageData mRecvStageData;
};

