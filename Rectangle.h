#pragma once

#include <list>

enum ColliderWall{ None, Top, Bottom, Left, Right, Inside };
//enum ObjectType { Platform, Agent, Collectable, Enemy };

class Rectangle
{
protected:
	float xMin;
	float xMax;
	float yMin;
	float yMax;
	//ObjectType objectType;

public:
	Rectangle();
	Rectangle(float xMin, float xMax, float yMin, float yMax);
	void moveX(float xChange);
	void moveY(float yChange);
	void setPosition(float xMin, float xMax, float yMin, float yMax);
	ColliderWall collision(Rectangle collider, float *collideValue);
	std::list<float> getVerticesData();
	void getCenter(float* xVal, float* yVal);
	float getXCenter();
	float getYCenter();
	float getXMin();
	float getXMax();
	float getYMin();
	float getYMax();
};

