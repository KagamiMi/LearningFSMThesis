#pragma once
#include "Rectangle.h"
class EnemyRectangle :
	public Rectangle
{
private:
	float xMaxLimit;
	float xMinLimit;
	bool moveLeft = true;
	bool collided = false;
public:
	EnemyRectangle();
	EnemyRectangle(float xMin, float xMax, float yMin, float yMax, float xMinMoveLimit, float xMaxMoveLimit);
	void moveX(float xChange);
	bool getCollided();
	void setCollided(bool collided);
};

