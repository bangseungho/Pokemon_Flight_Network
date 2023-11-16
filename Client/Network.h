#pragma once

class Network
{
	SINGLETON(Network)

public:
	void Init(std::string ipAddr);
	void Connect();
	void DisConnect();
	void Close();

	void SendIntroData(const IntroData& data);

private:
	SOCKET mClientSock;
	SOCKADDR_IN mServerAddr;
};

