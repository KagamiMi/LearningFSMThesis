#include "SimpleInput.h"



SimpleInput::SimpleInput(bool lowHealth, bool endOfPlatformOnRight, bool endOfPlatformOnLeft, bool enemyOnLeft, bool enemyOnRight,
	bool finishOnLeft, bool finishOnRight, bool platformAboveOnRight, bool platformAboveOnLeft, bool pointOnLeft, bool pointOnRight):
	lowHealth(lowHealth),endOfPlatformOnRight(endOfPlatformOnRight),endOfPlatformOnLeft(endOfPlatformOnLeft),enemyOnLeft(enemyOnLeft), enemyOnRight(enemyOnRight),
	finishOnLeft(finishOnLeft),finishOnRight(finishOnRight),platformAboveOnRight(platformAboveOnRight), platformAboveOnLeft(platformAboveOnLeft),
	pointOnLeft(pointOnLeft),pointOnRight(pointOnRight)
{
}

SimpleInput::SimpleInput() :
	lowHealth(0), endOfPlatformOnRight(0), endOfPlatformOnLeft(0), enemyOnLeft(0), enemyOnRight(0),
	finishOnLeft(0), finishOnRight(0), platformAboveOnRight(0), platformAboveOnLeft(0),
	pointOnLeft(0), pointOnRight(0)
{

}

bool SimpleInput::operator==(SimpleInput input)
{
	return lowHealth == input.lowHealth && endOfPlatformOnRight == input.endOfPlatformOnRight && endOfPlatformOnLeft == input.endOfPlatformOnLeft
		&& enemyOnLeft == input.enemyOnLeft && enemyOnRight == input.enemyOnRight && finishOnLeft == input.finishOnLeft && finishOnRight == input.finishOnRight
		&& platformAboveOnRight == input.platformAboveOnRight && platformAboveOnLeft == input.platformAboveOnLeft && pointOnLeft == input.pointOnLeft
		&& pointOnRight == input.pointOnRight;
}

std::ostream& operator<<(std::ostream& os, const SimpleInput& input)
{
	os << input.lowHealth << input.endOfPlatformOnRight << input.endOfPlatformOnLeft << input.enemyOnLeft << input.enemyOnRight
		<< input.finishOnLeft << input.finishOnRight << input.platformAboveOnRight << input.platformAboveOnLeft << input.pointOnLeft << input.pointOnRight;
	return os;
}