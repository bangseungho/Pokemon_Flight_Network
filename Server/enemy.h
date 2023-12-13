#pragma once
#include "object.h"

class EnemyController {
private:
	uint32 mAccId = 0;

	int createDelay_Melee = 0;
	int createDelay_Range = 0;
	int delay_Melee = 0;
	int delay_Range = 0;
	int createAmount_Melee = 0;
	int createAmount_Range = 0;

public:
	EnemyController();
	~EnemyController();
	void CreateCheckMelee();
	void CreateCheckRange();
};