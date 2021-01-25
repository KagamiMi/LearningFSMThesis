#include "Record.h"

Record::Record(int points, int health, int enemies, bool finished, int steps, float distanceX, float distanceY):
	points(points), health(health), enemies(enemies), finished(finished), steps(steps), distanceX(distanceX), distanceY(distanceY)
{
}

int Record::getPoints()
{
	return points;
}

int Record::getHealth()
{
	return health;
}

int Record::getEnemies()
{
	return enemies;
}

bool Record::getFinished()
{
	return finished;
}

float Record::getSteps()
{
	return steps;
}

float Record::getDistanceX()
{
	return distanceX;
}


float Record::getDistanceY()
{
	return distanceY;
}