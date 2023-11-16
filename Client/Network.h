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
	void SendStageData(const StageData& data = {});
	void SendTownData(const TownData& data = {});

private:
	SOCKET mClientSock;
	SOCKADDR_IN mServerAddr;
};

