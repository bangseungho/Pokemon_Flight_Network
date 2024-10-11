#pragma once

class Player;
class EnemyController;

class Battle
{
public:
	Battle();
	~Battle();

public:
	void Init();
	void Update(float elapsedTime);

private:
	vector<shared_ptr<Player>>	mPlayerVec;
	shared_ptr<EnemyController> mEnemyController;
};

