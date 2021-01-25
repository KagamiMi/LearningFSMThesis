#include "Input.h"

Input::Input(bool overHealthLimit, bool enemyVisible, bool pointVisible, bool platformVisible):
	overHealthLimit(overHealthLimit),enemyVisible(enemyVisible),pointVisible(pointVisible),platformVisible(platformVisible)
{
}

bool Input::getOverHealthLimit()
{
	return overHealthLimit;
}

bool Input::getEnemyVisible()
{
	return enemyVisible;
}

bool Input::getPointVisible()
{
	return pointVisible;
}

bool Input::getPlatformVisible()
{
	return platformVisible;
}

bool Input::operator==(Input input)
{
	return overHealthLimit == input.overHealthLimit && enemyVisible == input.enemyVisible 
		&& pointVisible == input.pointVisible && platformVisible == input.platformVisible;
}

std::ostream& operator<<(std::ostream& os, const Input& input)
{
	os << input.overHealthLimit << input.enemyVisible << input.pointVisible << input.platformVisible;
	return os;
}
