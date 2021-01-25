#pragma once
#include <iostream>

class SimpleInput
{
public:
	bool lowHealth;
	bool endOfPlatformOnRight;
	bool endOfPlatformOnLeft;
	bool enemyOnLeft;
	bool enemyOnRight;
	bool finishOnLeft;
	bool finishOnRight;
	bool platformAboveOnRight;
	bool platformAboveOnLeft;
	bool pointOnLeft;
	bool pointOnRight;
	//int inputsCount = 11;
public:
	SimpleInput(bool lowHealth, bool endOfPlatformOnRight, bool endOfPlatformOnLeft, bool enemyOnLeft, bool enemyOnRight, bool finishOnLeft, 
		bool finishOnRight, bool platformAboveOnRight, bool platformAboveOnLeft, bool pointOnLeft, bool pointOnRight);
	SimpleInput();
	bool operator==(SimpleInput input);
	friend std::ostream& operator<<(std::ostream& os, const SimpleInput& input);
};

