#pragma once

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
	void Invalidata();

private:
	unique_ptr<EnemyController> mEnemyController;
};

