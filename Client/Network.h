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
	mutex& GetEnemyMapMutex() { return mEnemyMapMutex; }
	mutex& GetBulletMutex() { return mBulletMutex; }
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
	mutex				mEnemyMapMutex;
	mutex				mBulletMutex;

	unordered_map<uint8, NetworkPlayerData> mRecvMemberMap;
	unordered_map<uint8, NetworkEnemyData> mRecvEnemyMap;
};

#define GET_MEMBER_MAP GET_SINGLE(Network)->GetMemberMap()
#define GET_ENEMY_MAP GET_SINGLE(Network)->GetEnemyMap()
#define MEMBER_MAP(index) GET_SINGLE(Network)->GetMemberMap()[index]
#define ENEMY_MAP(index) GET_SINGLE(Network)->GetEnemyMap()[index]
#define MY_INDEX GET_SINGLE(Network)->GetClientIndex()
#define MP_INDEX GET_SINGLE(Network)->GetMainPlayerIndex()

template<typename T>
inline void Network::SendDataAndType(const T& data)
{
	int retVal = Data::SendDataAndType(mClientSock, data);
}