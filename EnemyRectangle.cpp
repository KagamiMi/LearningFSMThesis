#include "EnemyRectangle.h"


EnemyRectangle::EnemyRectangle()
{
}

EnemyRectangle::EnemyRectangle(float xMin, float xMax, float yMin, float yMax, float xMinMoveLimit, float xMaxMoveLimit):
	Rectangle(xMin,xMax,yMin,yMax)
{
	float xCenter = xMin + (xMax - xMin)/2.0f;
	xMinLimit = xCenter + xMinMoveLimit;
	xMaxLimit = xCenter + xMaxMoveLimit;
}

void EnemyRectangle::moveX(float xChange)
{
	if (moveLeft)
	{
		xMin -= xChange;
		xMax -= xChange;

		if (xMin <= xMinLimit)
		{
			float align = xMinLimit - xMin;
			xMin += align;
			xMax += align;

			moveLeft = false;
			this->moveX(align);
		}
	}
	else
	{
		xMin += xChange;
		xMax += xChange;

		if (xMax >= xMaxLimit)
		{
			float align = xMax - xMaxLimit;
			xMin -= align;
			xMax -= align;

			moveLeft = true;
			this->moveX(align);
		}
	}
}

bool EnemyRectangle::getCollided()
{
	return collided;
}

void EnemyRectangle::setCollided(bool collided)
{
	this->collided = collided;
}
