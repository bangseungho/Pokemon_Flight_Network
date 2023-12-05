#pragma once

class EnemyController;
class Battle
{
public:
	Battle();
	~Battle();

public:
	void Init(NetworkPlayerData* playerMap);
	void Update(float elapsedTime);

private:
	void CreateEnemys(float elapsedTime);

private:
	NetworkPlayerData* mPlayerMap;
	unique_ptr<EnemyController> mEnemyController;
	
};

