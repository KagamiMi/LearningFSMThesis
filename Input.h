#pragma once
#include <iostream>

class Input
{
	
private:
	bool overHealthLimit;
	bool enemyVisible;
	bool pointVisible;
	bool platformVisible;

public:
	Input(bool overHealthLimit, bool enemyVisible, bool pointVisible, bool platformVisible);
	bool getOverHealthLimit();
	bool getEnemyVisible();
	bool getPointVisible();
	bool getPlatformVisible();
	bool operator==(Input input);
	friend std::ostream& operator<<(std::ostream& os, const Input& input);
};


